#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#pragma once
#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"
#include <vector>
#include <deque>
#include <ctime>
#include <queue>
#include <optional>
#include "music.h"


namespace game_framework {
	const int CANVAS_HEIGHT = 22;
	const int CANVAS_WIDTH = 10;

	const int PREVIEW_ROW_COUNT = 2;

	const int NEXT_CUBE_CANVAS_WIDTH = 4;
	const int NEXT_CUBE_CANVAS_HEIGHT = 14;

	const int HOLD_CUBE_CANVAS_WIDTH = 4;
	const int HOLD_CUBE_CANVAS_HEIGHT = 2;

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
		translucent,
		error
	};

	enum class Event
	{
		left,
		right,
		down,
		rotate,
		tick,
		hold
	};

	enum class GameType
	{
		fourtyl,
		blitz,
		zen,
		custom
	};

	using TrominoMatrix = vector<vector<char>>;
	using Canvas = vector<vector<Color>>;

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
		bool game_start = false;
		bool game_almost_over = false;
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

		TetrisGame(int height, int width);

		void event_handler(Event event);
		void remove_predict_tromino_from_canvas();
		void remove_active_tromino_from_canvas();
		bool is_horizontal_side_clear(Event direction);
		void move_active_tromino_left();
		void move_active_tromino_right();
		void rotate_active_tromino();
		void hold_active_tromino();
		bool is_active_tromino_reached_bottom();
		bool is_predict_tromino_reached_bottom();
		void render_predict_tromino_to_canvas();
		void render_active_tromino_to_canvas();
		void render_next_tromino_array_to_place_canvas();
		void render_hold_tromino_to_hold_canvas();
		void predict_tromino_landing_position();

		// Remove filled rows, prepend empty rows, and update scores in the process
		void remove_and_prepend_rows();
		bool is_game_over();
		void clear_all_canvas();
		queue<Color> produce_seven_color_not_repeative();
		bool check_game_almost_over();
	};
}

#endif