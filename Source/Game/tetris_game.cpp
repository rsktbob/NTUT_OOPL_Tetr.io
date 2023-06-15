#include "stdafx.h"
#include "tetris_game.h"
#include "trimino.h"
#pragma once

namespace game_framework {
	TetrisGame::TetrisGame(int height, int width)
	{
		canvas_height = height;
		canvas_width = width;
		canvas = Canvas(canvas_height, vector<Color>(canvas_width));
		random_color_array = produce_seven_color_not_repeative();
		active_tromino = Tromino::according_color_tromino(random_color_array.front(), canvas_width);
		predict_tromino_landing_position();
		render_predict_tromino_to_canvas();
		random_color_array.pop();
		for (unsigned i = 0; i < 5; i++)
		{
			next_tromino_array.emplace_back(Tromino::according_color_tromino(random_color_array.front(), canvas_width));
			random_color_array.pop();
		}
		render_next_tromino_array_to_place_canvas();
	}

	void TetrisGame::event_handler(Event event)
	{
		// Generate a new tromino if active_tromino is null
		if (!this->active_tromino.has_value())
		{
			score += per_round_score;
			per_round_score = 0;
			if (hold_tromino.has_value())
			{
				hold_tromino->color = hold_tromino->according_matrix_return_color();
				render_hold_tromino_to_hold_canvas();
			}
			this->active_tromino.emplace(next_tromino_array.front());
			predict_tromino_landing_position();
			next_tromino_array.pop_front();
			if (random_color_array.empty())
			{
				random_color_array = produce_seven_color_not_repeative();
			}
			next_tromino_array.emplace_back(Tromino::according_color_tromino(random_color_array.front(), canvas_width));
			random_color_array.pop();
			render_next_tromino_array_to_place_canvas();
			hold_once_per_round = true;
		}

		remove_active_tromino_from_canvas();

		// Move/drop/rotate/fall tromino based on event
		if (event == Event::left)
		{
			move_active_tromino_left();
			predict_tromino_landing_position();
		}
		else if (event == Event::right)
		{
			move_active_tromino_right();
			predict_tromino_landing_position();
		}
		else if (event == Event::rotate)
		{
			rotate_active_tromino();
			predict_tromino_landing_position();
		}
		else if (event == Event::down)
		{
			while (!is_active_tromino_reached_bottom())
			{
				active_tromino->y += 1;
				per_round_score += 2;
			}
		}
		else if (event == Event::tick)
		{
			active_tromino->y += 1;
			per_round_score += 1;
		}
		else if (event == Event::hold)
		{
			hold_active_tromino();
			render_hold_tromino_to_hold_canvas();
			predict_tromino_landing_position();
		}

		// Render active_tromino before proceeding as it may be reset later on

		render_active_tromino_to_canvas();

		if (is_active_tromino_reached_bottom())
		{

			remove_and_prepend_rows();

			// Set active_tromino to null and generate a new tromino on the next process
			active_tromino.reset();

			predict_tromino.reset();
			render_predict_tromino_to_canvas();

			// Only check for game over if active_tromino reached bottom
			game_over = is_game_over();
		}
	}

	void TetrisGame::remove_predict_tromino_from_canvas()
	{
		for (unsigned row_index = 0; row_index < predict_tromino->matrix.size(); row_index++)
		{
			for (unsigned column_index = 0; column_index < predict_tromino->matrix[row_index].size(); column_index++)
			{
				int y = predict_tromino->y + row_index;
				if (y >= 0)
				{
					if (predict_tromino->matrix[row_index][column_index] == 1)
					{
						canvas[y][predict_tromino->x + column_index] = Color::black;
					}
				}
			}
		}
	}

	void TetrisGame::remove_active_tromino_from_canvas()
	{
		for (unsigned row_index = 0; row_index < active_tromino->matrix.size(); row_index++)
		{
			for (unsigned column_index = 0; column_index < active_tromino->matrix[row_index].size(); column_index++)
			{
				int y = active_tromino->y + row_index;
				if (y >= 0 && active_tromino->matrix[row_index][column_index] == 1)
				{
					canvas[y][active_tromino->x + column_index] = Color::black;
				}
			}
		}
	}

	bool TetrisGame::is_horizontal_side_clear(Event direction)
	{
		int active_tromino_right = active_tromino->x + (active_tromino->width() - 1);

		if (direction == Event::left ? (active_tromino->x == 0) : (active_tromino_right == canvas_width - 1))
		{
			return false;
		}

		int column_index_to_check = direction == Event::left ? active_tromino->x - 1 : active_tromino_right + 1;

		for (int row_index = 0; row_index < active_tromino->height(); row_index++)
		{
			// `auto` doesn't work as active_tromino->y maybe negative
			int y = active_tromino->y + row_index;

			if (y > 0 && canvas[y][column_index_to_check] != Color::black)
			{
				return false;
			}
		}

		return true;
	}

	void TetrisGame::move_active_tromino_left()
	{
		if (is_horizontal_side_clear(Event::left))
			active_tromino->x -= 1;
	}

	void TetrisGame::move_active_tromino_right()
	{
		if (is_horizontal_side_clear(Event::right))
			active_tromino->x += 1;
	}

	void TetrisGame::rotate_active_tromino()
	{
		TrominoMatrix new_matrix(active_tromino->width(), vector<char>(active_tromino->height(), 0));

		for (unsigned row_index = 0; row_index < active_tromino->matrix.size(); row_index++)
		{
			for (unsigned column_index = 0; column_index < active_tromino->matrix[row_index].size(); column_index++)
			{
				new_matrix[column_index][active_tromino->height() - 1 - row_index] = active_tromino->matrix[row_index][column_index];
			}
		}

		active_tromino->matrix = new_matrix;

		if (active_tromino->x + (active_tromino->width() - 1) >= canvas_width)
		{
			active_tromino->x = canvas_width - active_tromino->width();
		}
	}

	void TetrisGame::hold_active_tromino()
	{
		if (hold_once_per_round)
		{
			if (hold_tromino.has_value())
			{
				optional<Tromino> temporary_tromino = active_tromino;
				active_tromino = hold_tromino;
				hold_tromino = Tromino(Color::grey, temporary_tromino->according_color_return_matrix(), canvas_width);
			}
			else
			{
				hold_tromino = Tromino(Color::grey, active_tromino->according_color_return_matrix(), canvas_width);
				active_tromino = next_tromino_array.front();
				next_tromino_array.pop_front();
				if (random_color_array.empty())
				{
					random_color_array = produce_seven_color_not_repeative();
				}
				next_tromino_array.emplace_back(Tromino::according_color_tromino(random_color_array.front(), canvas_width));
				random_color_array.pop();
			}
			hold_once_per_round = false;
		}
	}

	bool TetrisGame::is_active_tromino_reached_bottom()
	{
		if (active_tromino->y + active_tromino->height() >= canvas_height)
		{
			return true;
		}

		// Loop from left to right
		for (int column_index = 0; column_index < active_tromino->width(); column_index++)
		{
			// Loop from bottom to top
			for (int row_index = active_tromino->height() - 1; row_index >= 0; row_index--)
			{
				if (active_tromino->matrix[row_index][column_index] == 1)
				{
					int y = active_tromino->y + row_index + 1;

					// There is something in the canvas below the bottommost `1` cell
					if (y >= 0 && canvas[y][active_tromino->x + column_index] != Color::black && canvas[y][active_tromino->x + column_index] != Color::translucent)
					{
						return true;
					}

					// Continue to the next column after checking the first cell with `1`
					break;
				}
			}
		}

		return false;
	}

	bool TetrisGame::is_predict_tromino_reached_bottom()
	{
		if (predict_tromino->y + predict_tromino->height() >= canvas_height)
		{
			return true;
		}

		// Loop from left to right
		for (int column_index = 0; column_index < predict_tromino->width(); column_index++)
		{
			// Loop from bottom to top
			for (int row_index = predict_tromino->height() - 1; row_index >= 0; row_index--)
			{
				if (predict_tromino->matrix[row_index][column_index] == 1)
				{
					int y = predict_tromino->y + row_index + 1;

					// There is something in the canvas below the bottommost `1` cell
					if (y >= 0 && canvas[y][predict_tromino->x + column_index] != Color::black)
					{
						return true;
					}

					// Continue to the next column after checking the first cell with `1`
					break;
				}
			}
		}

		return false;
	}

	void TetrisGame::render_predict_tromino_to_canvas()
	{
		for (unsigned row_index = 0; row_index < predict_tromino->matrix.size(); row_index++)
		{
			auto row = predict_tromino->matrix[row_index];
			for (unsigned column_index = 0; column_index < row.size(); column_index++)
			{
				auto cell = row[column_index];
				unsigned y = predict_tromino->y + row_index;
				unsigned x = predict_tromino->x + column_index;
				// y maybe negative
				if (cell == 1 && y >= 0 && y < canvas.size())
				{
					canvas[y][x] = predict_tromino->color;
				}
			}
		}
	}

	void TetrisGame::render_active_tromino_to_canvas()
	{
		for (unsigned row_index = 0; row_index < active_tromino->matrix.size(); row_index++)
		{
			auto row = active_tromino->matrix[row_index];
			for (unsigned column_index = 0; column_index < row.size(); column_index++)
			{
				auto cell = row[column_index];
				unsigned y = active_tromino->y + row_index;
				unsigned x = active_tromino->x + column_index;
				// y maybe negative
				if (cell == 1 && y >= 0 && y < canvas.size())
				{
					canvas[y][x] = active_tromino->color;
				}
			}
		}
	}

	void TetrisGame::render_next_tromino_array_to_place_canvas()
	{
		// update place_cube_canvas
		next_canvas = vector<vector<Color>>(NEXT_CUBE_CANVAS_HEIGHT, vector<Color>(NEXT_CUBE_CANVAS_WIDTH, Color::transparent));
		for (unsigned i = 0; i < next_tromino_array.size(); i++)
		{
			unsigned next_tromino_height = next_tromino_array[i].height();
			unsigned next_tromino_width = next_tromino_array[i].width();
			int next_tromino_horizontal_position = (NEXT_CUBE_CANVAS_WIDTH - next_tromino_array[i].width()) / 2;
			Color next_tromino_color = next_tromino_array[i].color;
			for (unsigned j = 0; j < next_tromino_height; j++)
			{
				for (unsigned k = 0; k < next_tromino_width; k++)
				{
					this->next_canvas[i * 3 + j][k + next_tromino_horizontal_position] = next_tromino_array[i].matrix[j][k] == 1 ? next_tromino_color : Color::transparent;
				}
			}
		}
	}

	void TetrisGame::render_hold_tromino_to_hold_canvas()
	{
		if (hold_tromino.has_value())
		{
			hold_canvas = vector<vector<Color>>(HOLD_CUBE_CANVAS_HEIGHT, vector<Color>(HOLD_CUBE_CANVAS_WIDTH, Color::transparent));
			int hold_tromino_horizontal_position = (HOLD_CUBE_CANVAS_WIDTH - hold_tromino->width()) / 2;
			for (signed i = 0; i < hold_tromino->height(); i++)
			{
				for (signed j = 0; j < hold_tromino->width(); j++)
				{
					hold_canvas[i][j + hold_tromino_horizontal_position] = hold_tromino->matrix[i][j] == 1 ? hold_tromino->color : Color::transparent;
				}
			}
		}
	}

	void TetrisGame::predict_tromino_landing_position()
	{
		remove_predict_tromino_from_canvas();
		predict_tromino.reset();
		predict_tromino.emplace(active_tromino.value());
		predict_tromino->color = Color::translucent;
		while (!is_predict_tromino_reached_bottom())
		{
			predict_tromino->y += 1;
		}
		render_predict_tromino_to_canvas();
	}

	// Remove filled rows, prepend empty rows, and update scores in the process
	void TetrisGame::remove_and_prepend_rows()
	{
		int per_round_lines = 0;
		for (unsigned row_index = 0; row_index < canvas.size(); row_index++)
		{
			vector<Color> row = canvas[row_index];
			if (all_of(row.begin(), row.end(), [](Color color)
			{ return color != Color::black; }))
			{
				music->Play(AUDIO_ID::Cube_Clear);
				per_round_lines += 1;
				canvas.erase(canvas.begin() + row_index);
				canvas.insert(canvas.begin(), vector<Color>(canvas_width, Color::black));
			}
		}
		if (!check_game_almost_over())
		{
			game_almost_over = true;
		}
		else
		{
			game_almost_over = false;
		}
		lines += per_round_lines;
		per_round_score += cleared_lines_to_get_score[per_round_lines];
	}

	bool TetrisGame::is_game_over()
	{
		// The first row (index 0) and the second row (index 1) is transparent and for preview purposes only,
		// thus not counted as a row
		auto topmost_row = canvas[PREVIEW_ROW_COUNT];

		// Check if game is over
		return any_of(
			topmost_row.begin(),
			topmost_row.end(),
			[](Color color)
		{ return color != Color::black; });
	}

	void TetrisGame::clear_all_canvas()
	{
		canvas = Canvas(canvas_height, vector<Color>(canvas_width));
		next_canvas = Canvas(NEXT_CUBE_CANVAS_HEIGHT, vector<Color>(NEXT_CUBE_CANVAS_WIDTH));
		hold_canvas = Canvas(HOLD_CUBE_CANVAS_HEIGHT, vector<Color>(HOLD_CUBE_CANVAS_WIDTH, Color::transparent));
	}

	queue<Color> TetrisGame::produce_seven_color_not_repeative()
	{
		vector<Color> color_arr = { light_blue, yellow, purple, green, red, blue, orange };
		queue<Color> random_color_arr;
		for (int i = 0; i < 7; i++)
		{
			int random_pos = i + rand() % (7 - i);
			random_color_arr.push(color_arr[random_pos]);
			Color key = color_arr[i];
			color_arr[i] = color_arr[random_pos];
			color_arr[random_pos] = key;
		}
		return random_color_arr;
	}

	bool TetrisGame::check_game_almost_over()
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < canvas_width; j++)
			{
				if (canvas[i][j] > Color::grey)
				{
					return true;
				}
			}
		}
		return false;
	}
}