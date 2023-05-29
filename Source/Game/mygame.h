#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <deque>
#include <ctime>
#include <queue>

using namespace std;

const int CANVAS_HEIGHT = 22;
const int CANVAS_WIDTH = 10;

const int PREVIEW_ROW_COUNT = 2;

const int NEXT_CUBE_CANVAS_WIDTH = 4;
const int NEXT_CUBE_CANVAS_HEIGHT = 14;

const int HOLD_CUBE_CANVAS_WIDTH = 4;
const int HOLD_CUBE_CANVAS_HEIGHT = 2;

using TrominoMatrix = vector<vector<char>>;


/*
 * mygame.h: 本檔案儲遊戲本身的class的interface
 * Copyright (C) 2002-2008 Woei-Kae Chen <wkc@csie.ntut.edu.tw>
 *
 * This file is part of game, a free game development framework for windows.
 *
 * game is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * game is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	 2004-03-02 V4.0
 *      1. Add CGameStateInit, CGameStateRun, and CGameStateOver to
 *         demonstrate the use of states.
 *   2005-09-13
 *      Rewrite the codes for CBall and CEraser.
 *   2005-09-20 V4.2Beta1.
 *   2005-09-29 V4.2Beta2.
 *   2006-02-08 V4.2
 *      1. Rename OnInitialUpdate() -> OnInit().
 *      2. Replace AUDIO_CANYON as AUDIO_NTUT.
 *      3. Add help bitmap to CGameStateRun.
 *   2006-09-09 V4.3
 *      1. Rename Move() and Show() as OnMove and OnShow() to emphasize that they are
 *         event driven.
 *   2008-02-15 V4.4
 *      1. Add namespace game_framework.
 *      2. Replace the demonstration of animation as a new bouncing ball.
 *      3. Use ShowInitProgress(percent) to display loading progress.
*/


namespace game_framework {
	/////////////////////////////////////////////////////////////////////////////
	// Constants
	/////////////////////////////////////////////////////////////////////////////

	enum AUDIO_ID
	{ // 定義各種音效的編號
		Arial_City,
		To_The_Limit,
		The_Great_Eastern_Expedition,
		Morning_Sun_Kamoking,
		In_Sorrow_And_Pains_Mirera,
		Piercing_Wind,
		Success_Story_Akiko_Shioyama,
		Hyper_Velocity,
		Philosophy,
		Touch_Menu,
		Click_Menu,
		Back_Menu,
		Touch_Check_Menu,
		Click_Check_Menu,
		Touch_Option_Menu,
		Cube_Rotate,
		Cube_Horizontal_Move,
		Cube_Decline_Move,
		Cube_Switch,
		Cube_Touch_Bottom,
		Cube_Clear,
		Cube_Full_Clear,
		Game_Over,
		Game_Finish,
		Game_Over_Menu,
		Level_Up,
		Exit_Process_Game,
		Exit_Game,
		Random,
		None
	};

	enum Color
	{
		black,
		light_blue,
		yellow,
		purple,
		green,
		red,
		blue,
		orange,
		grey,
		transparent,
		translucent
	};

	using Canvas = vector<vector<Color>>;

	enum class Event
	{
		left,
		right,
		down,
		rotate,
		tick,
		hold
	};

	enum class HorizontalDirection
	{
		left,
		right
	};

	enum class GameType
	{
		fourtyl,
		blitz,
		zen,
		custom
	};

	class Button : public CMovingBitmap
	{
	public:
		bool _is_touched_first = true;
		AUDIO_ID _touch_audio_effect;
		AUDIO_ID _click_audio_effect;
		AUDIO_ID _next_background_music;

		Button(AUDIO_ID touch_audio_effect, AUDIO_ID click_audio_effect, AUDIO_ID next_background_music);

		bool is_clicked(UINT nFlags, CPoint point, CMovingBitmap character);
		bool is_touched(CPoint point, CMovingBitmap character);
	};

	class Tromino
	{
	public:
		int x;
		int y;
		Color color;
		TrominoMatrix matrix;

		Tromino(Color color, TrominoMatrix matrix, int canvas_width) : color(color), matrix(matrix)
		{
			x = (canvas_width - width()) / 2;
			y = -height() + 1;
		}

		static Tromino according_color_tromino(Color color, int canvas_width)
		{
			TrominoMatrix matrix;

			// https://stackoverflow.com/a/47588671
			// https://tetris.wiki/Tetromino#The_basic_tetrominoes
			switch (color)
			{
			case Color::light_blue:
				matrix = {
					{1, 1, 1, 1},
				};
				break;

			case Color::yellow:
				matrix = {
					{1, 1},
					{1, 1},
				};
				break;

			case Color::purple:
				matrix = {
					{0, 1, 0},
					{1, 1, 1},
				};
				break;

			case Color::green:
				matrix = {
					{0, 1, 1},
					{1, 1, 0},
				};
				break;

			case Color::red:
				matrix = {
					{1, 1, 0},
					{0, 1, 1},
				};
				break;

			case Color::blue:
				matrix = {
					{1, 0, 0},
					{1, 1, 1},
				};
				break;

			case Color::orange:
				matrix = {
					{0, 0, 1},
					{1, 1, 1},
				};
			}

			return Tromino(color, matrix, canvas_width);
		}

		int width() const
		{
			return matrix[0].size();
		}

		int height() const
		{
			return matrix.size();
		}

		Color according_matrix_return_color()
		{
			if (matrix == TrominoMatrix({
							  {1, 1, 1, 1},
				}))
			{
				return Color::light_blue;
			}
			else if (matrix == TrominoMatrix({
								   {1, 1},
								   {1, 1},
				}))
			{
				return Color::yellow;
			}
			else if (matrix == TrominoMatrix({
								   {0, 1, 0},
								   {1, 1, 1},
				}))
			{
				return Color::purple;
			}
			else if (matrix == TrominoMatrix({
								   {0, 1, 1},
								   {1, 1, 0},
				}))
			{
				return Color::green;
			}
			else if (matrix == TrominoMatrix({
								   {1, 1, 0},
								   {0, 1, 1},
				}))
			{
				return Color::red;
			}
			else if (matrix == TrominoMatrix({
								   {1, 0, 0},
								   {1, 1, 1},
				}))
			{
				return Color::blue;
			}
			else
			{
				return Color::orange;
			}
		}

		TrominoMatrix according_color_return_matrix()
		{
			switch (color)
			{
			case Color::light_blue:
				return {
					{1, 1, 1, 1},
				};
			case Color::yellow:
				return { {1, 1},
						{1, 1} };
			case Color::purple:
				return { {0, 1, 0},
						{1, 1, 1} };
			case Color::green:
				return { {0, 1, 1},
						{1, 1, 0} };
			case Color::red:
				return { {1, 1, 0},
						{0, 1, 1} };
			case Color::blue:
				return { {1, 0, 0},
						{1, 1, 1} };
			default:
				return { {0, 0, 1},
						{1, 1, 1} };
			}
		}
	};

	class TetrisGame
	{
	public:
		int lines = 0;
		int score = 0;
		int level = 1;
		int init_time = 0;
		int per_round_score = 0;
		bool game_over = false;
		bool game_success = false;
		int canvas_width = 10;
		int canvas_height = 22;
		map<int, int> cleared_lines_to_get_score = { {1, 100}, {2, 300}, {3, 500}, {4, 800} };

		optional<Tromino> active_tromino;
		optional<Tromino> predict_tromino;
		deque<Tromino> next_tromino_array;
		optional<Tromino> hold_tromino;
		queue<Color> random_color_array;
		Canvas canvas;
		Canvas next_canvas = Canvas(NEXT_CUBE_CANVAS_HEIGHT, vector<Color>(NEXT_CUBE_CANVAS_WIDTH));
		Canvas hold_canvas = Canvas(HOLD_CUBE_CANVAS_HEIGHT, vector<Color>(HOLD_CUBE_CANVAS_WIDTH, Color::transparent));
		bool hold_once_per_round = true;

		TetrisGame(int height, int width)
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

		void event_handler(Event event)
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

		void remove_predict_tromino_from_canvas()
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

		void remove_active_tromino_from_canvas()
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

		bool is_horizontal_side_clear(HorizontalDirection direction)
		{
			int active_tromino_right = active_tromino->x + (active_tromino->width() - 1);

			if (direction == HorizontalDirection::left ? (active_tromino->x == 0) : (active_tromino_right == canvas_width - 1))
			{
				return false;
			}

			int column_index_to_check = direction == HorizontalDirection::left ? active_tromino->x - 1 : active_tromino_right + 1;

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

		void move_active_tromino_left()
		{
			if (is_horizontal_side_clear(HorizontalDirection::left))
				active_tromino->x -= 1;
		}

		void move_active_tromino_right()
		{
			if (is_horizontal_side_clear(HorizontalDirection::right))
				active_tromino->x += 1;
		}

		void rotate_active_tromino()
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

		void hold_active_tromino()
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

		bool is_active_tromino_reached_bottom()
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

		bool is_predict_tromino_reached_bottom()
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

		void render_predict_tromino_to_canvas()
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

		void render_active_tromino_to_canvas()
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

		void render_next_tromino_array_to_place_canvas()
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

		void render_hold_tromino_to_hold_canvas()
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

		void predict_tromino_landing_position()
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
		void remove_and_prepend_rows();

		bool is_game_over()
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

		void clear_all_canvas()
		{
			canvas = Canvas(canvas_height, vector<Color>(canvas_width));
			next_canvas = Canvas(NEXT_CUBE_CANVAS_HEIGHT, vector<Color>(NEXT_CUBE_CANVAS_WIDTH));
			hold_canvas = Canvas(HOLD_CUBE_CANVAS_HEIGHT, vector<Color>(HOLD_CUBE_CANVAS_WIDTH, Color::transparent));
		}

		queue<Color> produce_seven_color_not_repeative()
		{
			vector<Color> color_arr = { light_blue, yellow, purple, green, red, blue, orange };
			queue<Color> random_color_arr;
			for (int i = 0; i < 7; i++)
			{
				int random_pos = i + rand() % (7-i);
				random_color_arr.push(color_arr[random_pos]);
				Color key = color_arr[i];
				color_arr[i] = color_arr[random_pos];
				color_arr[random_pos] = key;
			}
			return random_color_arr;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的遊戲開頭畫面物件
	// 每個Member function的Implementation都要弄懂
	/////////////////////////////////////////////////////////////////////////////

	class CGameStateInit : public CGameState {
	public:
		CGameStateInit(CGame *g);
		void OnInit();  								// 遊戲的初值及圖形設定
		void OnBeginState();							// 設定每次重玩所需的變數
		void OnKeyUp(UINT, UINT, UINT); 				// 處理鍵盤Up的動作
		void OnLButtonDown(UINT nFlags, CPoint point);  // 處理滑鼠的動作
	protected:
		void OnShow();									// 顯示這個狀態的遊戲畫面
	private:
		CMovingBitmap logo;								// csie的logo
	};

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
	// 每個Member function的Implementation都要弄懂
	/////////////////////////////////////////////////////////////////////////////

	class CGameStateRun : public CGameState {
	public:
		CGameStateRun(CGame *g);
		~CGameStateRun();
		void OnBeginState();							// 設定每次重玩所需的變數
		void OnInit();  								// 遊戲的初值及圖形設定
		void OnKeyDown(UINT, UINT, UINT);
		void OnKeyUp(UINT, UINT, UINT);
		void OnLButtonDown(UINT nFlags, CPoint point);  // 處理滑鼠的動作
		void OnLButtonUp(UINT nFlags, CPoint point);	// 處理滑鼠的動作
		void OnMouseMove(UINT nFlags, CPoint point);	// 處理滑鼠的動作 
		void OnRButtonDown(UINT nFlags, CPoint point);  // 處理滑鼠的動作
		void OnRButtonUp(UINT nFlags, CPoint point);	// 處理滑鼠的動作
		bool click_check(UINT nFlags, CPoint point, CMovingBitmap character);
		bool touch_check(CPoint point, CMovingBitmap character);
		void touch_option_menu(CPoint point);
		void set_canvas(int hright, int width);
		bool game_over_animation();
		bool game_success_animation();
		void game_level_up_animation();
		void game_exit_animation();
		void display_game();
		void display_lines(unsigned lines_total);
		void display_play_passed_time();
		void display_play_remaining_time();
		void display_on_button_score();
		void display_on_right_score();
		void display_on_button_level();
		void display_on_left_level();
		void display_reciprocal_animation();
		void display_lines_graph(unsigned lines_total);
		void display_play_total_time();
		void display_play_real_time(COLORREF color);
		void display_play_total_score();
		void display_custom_control_menu();
		void display_finish();
		void game_init();
		void game_update(Event event);
		void game_natural_decline();
		void game_control();
		void game_record_current_time();
		void game_record_current_score();
		void game_model(GameType gametype);
		void fail_game_menu_move();
		void fail_game_menu_click(UINT nFlags, CPoint point, GameType gametype);
		void fail_game_menu_touch(CPoint point);
		void change_background_music(AUDIO_ID new_background_music, bool is_cycled);
		void change_scene(int new_phase, int new_sub_phase, AUDIO_ID new_background_music, bool is_cycled);
		CMovingBitmap Cube();
	protected:
		void OnMove();									// 移動遊戲元素
		void OnShow();									// 顯示這個狀態的遊戲畫面
	private:
		int id;
		int background_music;
		int phase;
		int sub_phase;
		bool fire_animation_check;
		bool level_up_animation_check;
		bool finish_animation_check;
		
		bool touch_menu_check_first;
		bool touch_option_menu_first;
		bool touch_option_menu_selected;

		CMovingBitmap first_menu;						// 一級菜單
		CMovingBitmap join;
		CMovingBitmap tittle;
		CMovingBitmap osk;

		CMovingBitmap background;

		vector<CMovingBitmap> second_menu = vector<CMovingBitmap>(4);					// 二級菜單
		vector<bool> second_menu_selected= { false, false, false, false };

		CMovingBitmap back;
		bool back_selected = false;

		CMovingBitmap logo;

		CMovingBitmap game_mode;

		vector<CMovingBitmap> fourtyl_menu = vector<CMovingBitmap>(4);
		vector<CMovingBitmap> fourtyl_menu_check = vector<CMovingBitmap>(4);
		vector<CMovingBitmap> fourtyl_end_menu = vector<CMovingBitmap>(3);

		vector<CMovingBitmap> blitz_menu = vector<CMovingBitmap>(4);
		vector<CMovingBitmap> blitz_menu_check = vector<CMovingBitmap>(4);
		vector<CMovingBitmap> blitz_end_menu = vector<CMovingBitmap>(3);

		vector<CMovingBitmap> zen_menu = vector<CMovingBitmap>(2);
		CMovingBitmap zen_model;

		vector<CMovingBitmap> custom_menu = vector<CMovingBitmap>(2);
		CMovingBitmap custom_control_menu;
		vector<CMovingBitmap> board_width_control = vector<CMovingBitmap>(2);
		vector<bool> board_width_control_selected = { false, false };
		vector<CMovingBitmap> board_height_control = vector<CMovingBitmap>(2);
		vector<bool> board_height_control_selected = { false, false };

		vector<CMovingBitmap> again = vector<CMovingBitmap>(2);
		vector<CMovingBitmap> start = vector<CMovingBitmap>(4);
		vector<CMovingBitmap> fire = vector<CMovingBitmap>(4);
		CMovingBitmap level_up_scene;
		CMovingBitmap exit_scene;

		int canvas_height = 22;
		int canvas_width = 10;

		int board_height = 20;
		int board_width = 10;

		TetrisGame tetris_game = TetrisGame(22, 10);
		TetrisGame save_tetris_game = TetrisGame(22, 10);
		vector<vector<CMovingBitmap>> cubes;
		vector<vector<CMovingBitmap>> next_cubes = vector<vector<CMovingBitmap>>(NEXT_CUBE_CANVAS_HEIGHT, vector<CMovingBitmap>(NEXT_CUBE_CANVAS_WIDTH));
		vector<vector<CMovingBitmap>> hold_cubes = vector<vector<CMovingBitmap>>(HOLD_CUBE_CANVAS_HEIGHT, vector<CMovingBitmap>(HOLD_CUBE_CANVAS_WIDTH));
		CMovingBitmap cube_next_border;
		CMovingBitmap cube_hold_border;
		CMovingBitmap cube_staight_border;
		CMovingBitmap cube_horizontal_border;
		CMovingBitmap lines_graph_border;
		vector<CMovingBitmap> left_cube_border;
		vector<CMovingBitmap> right_cube_border;
		vector<CMovingBitmap> bottom_cube_border;
		vector<CMovingBitmap> lines_graph_body = vector<CMovingBitmap>(40);
		CMovingBitmap lines_graph_top;
		int record_current_time;
		int game_next_decline_time;
		int game_decline_time_interval;
		int game_next_move_time;
		int game_move_time_interval;
		bool left_key_down;
		bool right_key_down;
		bool down_key_down;
		bool exit_check;
		map<int, int> blitz_level_to_speed;
		map<int, int> blitz_level_to_lines;
		COLORREF font_color;
		int font_decline_distance;

		CMovingBitmap retry;
		bool retry_selected;
		CMovingBitmap back_to_tittle;
		bool back_to_tittle_selected;
		
		unsigned game_remaining_time;
		unsigned game_current_time;
		unsigned game_minutes;
		unsigned game_seconds;
		unsigned game_milliseconds;

		unsigned lines_displacement_front;
		unsigned lines_displacement_back;
		unsigned end_time_displacement;
		unsigned end_score_displacement;
		unsigned score_displacement;
		unsigned level_displacement;

		unsigned decline_distance;

		unsigned real_time;

		char lines_display_front[6] = {};
		char lines_display_back[6] = {};
		char time_display_front[10] = {};
		char time_display_back[5] = {};
		char end_time_display_front[10] = {};
		char end_time_display_back[5] = {};
		string end_score_display;
		char real_time_display[20] = {};
		string score_display;
		char level_display[10] = {};

		time_t now;
	};

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的結束狀態(Game Over)
	// 每個Member function的Implementation都要弄懂
	/////////////////////////////////////////////////////////////////////////////

	class CGameStateOver : public CGameState {
	public:
		CGameStateOver(CGame *g);
		void OnBeginState();							// 設定每次重玩所需的變數
		void OnInit();
	protected:
		void OnMove();									// 移動遊戲元素
		void OnShow();									// 顯示這個狀態的遊戲畫面
	private:
		int counter;	// 倒數之計數器
	};

	class Menu : public CMovingBitmap
	{
		Menu() : CMovingBitmap() {}
	};
}
