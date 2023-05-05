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

using namespace std;

const int CANVAS_WIDTH = 10;
const int CANVAS_HEIGHT = 22;
const int PREVIEW_ROW_COUNT = 2;

const int PLACE_CUBE_CANVAS_WIDTH = 4;
const int PLACE_CUBE_CANVAS_HEIGHT = 14;

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

	enum AUDIO_ID {				// 定義各種音效的編號
		Arial_City,
		To_The_Limit,
		The_Great_Eastern_Expedition,
		Morning_Sun_Kamoking,
		In_Sorrow_And_Pains_Mirera,
		Piercing_Wind,
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
		Level_Up,
		Exit_Process_Game,
		Exit_Game
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
		transparent
	};

	using Canvas = vector<vector<Color>>;

	enum class Event {
		left,
		right,
		down,
		rotate,
		tick,
		hold
	};

	enum class HorizontalDirection {
		left,
		right
	};

	struct GameState {
		int lines;
		int score;
		bool game_over;
		Canvas canvas;
		Canvas place_canvas;
		Canvas hold_canvas;
	};

	enum class GameType {
		fourtyl,
		blitz,
		zen
	};

	class Button : public CMovingBitmap
	{
	public:
		bool is_clicked(UINT nFlags, CPoint point, CMovingBitmap character)
		{
			if (is_touched(point, character) && nFlags == VK_LBUTTON)
			{
				return true;
			}
			return false;
		}
		bool is_touched(CPoint point, CMovingBitmap character)
		{
			if (point.x >= character.GetLeft() && point.x <= character.GetLeft() + character.GetWidth()
				&& point.y >= character.GetTop() && point.y <= character.GetTop() + character.GetHeight())
			{
				return true;
			}
			return false;
		}
		Button() : CMovingBitmap()
		{

		}
	};

	class Tromino
	{
	public:
		int x;
		int y;
		Color color;
		TrominoMatrix matrix;

		Tromino(Color color, TrominoMatrix matrix)
			: color(color), matrix(matrix)
		{
			x = (CANVAS_WIDTH - width()) / 2;
			y = -height() + 1;
		}

		static Tromino random_tromino()
		{
			Color color;
			TrominoMatrix matrix;

			int tromino_type_count = 7;
			// https://stackoverflow.com/a/47588671
			int random_number = rand() % tromino_type_count;

			// https://tetris.wiki/Tetromino#The_basic_tetrominoes
			if (random_number == 0)
			{
				color = Color::light_blue;
				matrix = {
					{1, 1, 1, 1},
				};
			}
			else if (random_number == 1)
			{
				color = Color::yellow;
				matrix = {
					{1, 1},
					{1, 1},
				};
			}
			else if (random_number == 2)
			{
				color = Color::purple;
				matrix = {
					{0, 1, 0},
					{1, 1, 1},
				};
			}
			else if (random_number == 3)
			{
				color = Color::green;
				matrix = {
					{0, 1, 1},
					{1, 1, 0},
				};
			}
			else if (random_number == 4)
			{
				color = Color::red;
				matrix = {
					{1, 1, 0},
					{0, 1, 1},
				};
			}
			else if (random_number == 5)
			{
				color = Color::blue;
				matrix = {
					{1, 0, 0},
					{1, 1, 1},
				};
			}
			else
			{
				color = Color::orange;
				matrix = {
					{0, 0, 1},
					{1, 1, 1},
				};
			}

			return Tromino(color, matrix);
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

	class TetrisGame {
	public:
		int lines = 0;
		int score = 0;
		int level = 1;
		int init_time = 0;
		optional<Tromino> active_tromino = Tromino::random_tromino();
		deque<Tromino> next_tromino_array = { Tromino::random_tromino(), Tromino::random_tromino() , Tromino::random_tromino(), Tromino::random_tromino() , Tromino::random_tromino() };
		optional<Tromino> hold_tromino;
		Canvas canvas = Canvas(CANVAS_HEIGHT, vector<Color>(CANVAS_WIDTH));
		Canvas place_canvas = Canvas(PLACE_CUBE_CANVAS_HEIGHT, vector<Color>(PLACE_CUBE_CANVAS_WIDTH));
		Canvas hold_canvas = Canvas(HOLD_CUBE_CANVAS_HEIGHT, vector<Color>(HOLD_CUBE_CANVAS_WIDTH, Color::transparent));

		GameState event_handler(Event event) {
			// Generate a new tromino if active_tromino is null
			if (!this->active_tromino.has_value())
			{
				score += random_score();
				if (hold_tromino.has_value())
				{
					hold_tromino->color = hold_tromino->according_matrix_return_color();
				}
				this->active_tromino.emplace(next_tromino_array.front());
				next_tromino_array.pop_front();
				next_tromino_array.emplace_back(Tromino::random_tromino());
			}

			remove_active_tromino_from_canvas();

			// Move/drop/rotate/fall tromino based on event
			if (event == Event::left) {
				move_active_tromino_left();
			}
			else if (event == Event::right) {
				move_active_tromino_right();
			}
			else if (event == Event::rotate) {
				rotate_active_tromino();
			}
			else if (event == Event::down) {
				while (!is_active_tromino_reached_bottom()) {
					active_tromino->y += 1;
				}
			}
			else if (event == Event::tick) {
				active_tromino->y += 1;
			}
			else if (event == Event::hold) {
				hold_active_tromino();
			}

			// Render active_tromino before proceeding as it may be reset later on
			render_active_tromino_to_canvas();
			render_next_tromino_array_to_place_canvas();
			render_hold_tromino_to_hold_canvas();

			bool game_over = false;

			if (is_active_tromino_reached_bottom()) {

				remove_and_prepend_rows();

				// Set active_tromino to null and generate a new tromino on the next process
				active_tromino.reset();

				// Only check for game over if active_tromino reached bottom
				game_over = is_game_over();
			}

			return { lines, score, game_over, canvas, place_canvas, hold_canvas };
		}

		void remove_active_tromino_from_canvas() {
			for (unsigned row_index = 0; row_index < active_tromino->matrix.size(); row_index++) {
				for (unsigned column_index = 0; column_index < active_tromino->matrix[row_index].size(); column_index++) {
					int y = active_tromino->y + row_index;
					if (y >= 0) canvas[y][active_tromino->x + column_index] = Color::black;
				}
			}
		}

		bool is_horizontal_side_clear(HorizontalDirection direction) {
			int active_tromino_right = active_tromino->x + (active_tromino->width() - 1);

			if (direction == HorizontalDirection::left ? (active_tromino->x == 0) : (active_tromino_right == CANVAS_WIDTH - 1)) {
				return false;
			}

			int column_index_to_check = direction == HorizontalDirection::left ? active_tromino->x - 1 : active_tromino_right + 1;

			for (int row_index = 0; row_index < active_tromino->height(); row_index++) {
				// `auto` doesn't work as active_tromino->y maybe negative
				int y = active_tromino->y + row_index;

				if (y > 0 && canvas[y][column_index_to_check] != Color::black) {
					return false;
				}
			}

			return true;
		}

		void move_active_tromino_left() {
			if (is_horizontal_side_clear(HorizontalDirection::left)) active_tromino->x -= 1;
		}

		void move_active_tromino_right() {
			if (is_horizontal_side_clear(HorizontalDirection::right)) active_tromino->x += 1;
		}

		void rotate_active_tromino() {
			TrominoMatrix new_matrix(active_tromino->width(), vector<char>(active_tromino->height(), 0));

			for (unsigned row_index = 0; row_index < active_tromino->matrix.size(); row_index++) {
				for (unsigned column_index = 0; column_index < active_tromino->matrix[row_index].size(); column_index++) {
					new_matrix[column_index][active_tromino->height() - 1 - row_index] = active_tromino->matrix[row_index][column_index];
				}
			}

			active_tromino->matrix = new_matrix;

			if (active_tromino->x + (active_tromino->width() - 1) >= CANVAS_WIDTH) {
				active_tromino->x = CANVAS_WIDTH - active_tromino->width();
			}
		}

		void hold_active_tromino() {
			if (hold_tromino.has_value())
			{
				optional<Tromino> temporary_tromino = active_tromino;
				active_tromino = hold_tromino;
				hold_tromino = Tromino(Color::grey, temporary_tromino->according_color_return_matrix());
			}
			else
			{
				hold_tromino = Tromino(Color::grey, active_tromino->according_color_return_matrix());
				active_tromino = next_tromino_array.front();
				next_tromino_array.pop_front();
				next_tromino_array.emplace_back(Tromino::random_tromino());
			}
		}

		bool is_active_tromino_reached_bottom() {
			if (active_tromino->y + active_tromino->height() >= CANVAS_HEIGHT) {
				return true;
			}

			// Loop from left to right
			for (int column_index = 0; column_index < active_tromino->width(); column_index++) {
				// Loop from bottom to top
				for (int row_index = active_tromino->height() - 1; row_index >= 0; row_index--) {
					if (active_tromino->matrix[row_index][column_index] == 1) {
						int y = active_tromino->y + row_index + 1;

						// There is something in the canvas below the bottommost `1` cell
						if (y >= 0 && canvas[y][active_tromino->x + column_index] != Color::black) {
							return true;
						}

						// Continue to the next column after checking the first cell with `1`
						break;
					}
				}
			}

			return false;
		}

		void render_active_tromino_to_canvas() {
			for (unsigned row_index = 0; row_index < active_tromino->matrix.size(); row_index++) {
				auto row = active_tromino->matrix[row_index];
				for (unsigned column_index = 0; column_index < row.size(); column_index++) {
					auto cell = row[column_index];
					unsigned y = active_tromino->y + row_index;
					unsigned x = active_tromino->x + column_index;
					// y maybe negative
					if (cell == 1 && y >= 0 && y < canvas.size()) {
						canvas[y][x] = active_tromino->color;
					}
				}
			}
		}

		void render_next_tromino_array_to_place_canvas() {
			// update place_cube_canvas
			for (unsigned i = 0; i < PLACE_CUBE_CANVAS_HEIGHT; i++)
			{
				for (unsigned j = 0; j < PLACE_CUBE_CANVAS_WIDTH; j++)
				{
					place_canvas[i][j] = Color::transparent;
				}
			}
			for (unsigned i = 0; i < next_tromino_array.size(); i++)
			{
				unsigned next_tromino_height = next_tromino_array[i].height();
				unsigned next_tromino_width = next_tromino_array[i].width();
				int next_tromino_horizontal_position = (PLACE_CUBE_CANVAS_WIDTH - next_tromino_array[i].width()) / 2;
				Color next_tromino_color = next_tromino_array[i].color;
				for (unsigned j = 0; j < next_tromino_height; j++)
				{
					for (unsigned k = 0; k < next_tromino_width; k++)
					{
						this->place_canvas[i * 3 + j][k + next_tromino_horizontal_position] = next_tromino_array[i].matrix[j][k] == 1 ? next_tromino_color : Color::transparent;
					}
				}
			}
		}

		void render_hold_tromino_to_hold_canvas() {
			if (hold_tromino.has_value())
			{
				for (unsigned i = 0; i < HOLD_CUBE_CANVAS_HEIGHT; i++)
				{
					for (unsigned j = 0; j < HOLD_CUBE_CANVAS_WIDTH; j++)
					{
						hold_canvas[i][j] = Color::transparent;
					}
				}
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

		// Remove filled rows, prepend empty rows, and update scores in the process
		void remove_and_prepend_rows();

		bool is_game_over() {
			// The first row (index 0) and the second row (index 1) is transparent and for preview purposes only,
			// thus not counted as a row
			auto topmost_row = canvas[PREVIEW_ROW_COUNT];

			// Check if game is over
			return any_of(
				topmost_row.begin(),
				topmost_row.end(),
				[](Color color) { return color != Color::black; }
			);
		}

		int random_score()
		{
			int max = 80;
			int min = 40;
			return rand() % (max - min + 1) + min;
		}
		
		void clear_all_canvas() {
			canvas = Canvas(CANVAS_HEIGHT, vector<Color>(CANVAS_WIDTH));
			place_canvas = Canvas(PLACE_CUBE_CANVAS_HEIGHT, vector<Color>(PLACE_CUBE_CANVAS_WIDTH));
			hold_canvas = Canvas(HOLD_CUBE_CANVAS_HEIGHT, vector<Color>(HOLD_CUBE_CANVAS_WIDTH, Color::transparent));
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
		bool game_over_animation();
		void game_level_up_animation();
		void game_exit_animation();
		void display_game();
		void display_lines(unsigned lines_total);
		void display_time();
		void display_score();
		void display_level();
		void display_lines_graph(unsigned lines_total);
		void game_init();
		void game_update(Event event);
		void game_natural_decline();
		void game_control();
		void game_record_current_time();
		void game_model(GameType gametype);
		CMovingBitmap Cube();
	protected:
		void OnMove();									// 移動遊戲元素
		void OnShow();									// 顯示這個狀態的遊戲畫面
	private:
		int phase;
		int sub_phase;
		int id;
		bool fire_animation;
		bool level_up_animation;
		
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
		CMovingBitmap fourtyl_again;

		vector<CMovingBitmap> blitz_menu = vector<CMovingBitmap>(4);
		vector<CMovingBitmap> blitz_menu_check = vector<CMovingBitmap>(4);

		vector<CMovingBitmap> zen_menu = vector<CMovingBitmap>(2);
		CMovingBitmap zen_model;

		vector<CMovingBitmap> start = vector<CMovingBitmap>(4);

		vector<CMovingBitmap> fire = vector<CMovingBitmap>(4);
		CMovingBitmap level_up_scene;
		CMovingBitmap exit_scene;

		TetrisGame tetris_game;
		TetrisGame save_tetris_game;
		vector<vector<CMovingBitmap>> cube = vector<vector<CMovingBitmap>>(CANVAS_HEIGHT, vector<CMovingBitmap>(CANVAS_WIDTH));
		vector<vector<CMovingBitmap>> cube_next = vector<vector<CMovingBitmap>>(PLACE_CUBE_CANVAS_HEIGHT, vector<CMovingBitmap>(PLACE_CUBE_CANVAS_WIDTH));
		vector<vector<CMovingBitmap>> cube_hold = vector<vector<CMovingBitmap>>(HOLD_CUBE_CANVAS_HEIGHT, vector<CMovingBitmap>(HOLD_CUBE_CANVAS_WIDTH));
		CMovingBitmap cube_place;
		vector<CMovingBitmap> lines_graph_body = vector<CMovingBitmap>(20);
		CMovingBitmap lines_graph_top;
		int audio_id;
		int record_current_time;
		int game_next_decline_time;
		int game_decline_time_interval;
		int game_next_move_time;
		int game_move_time_interval;
		bool left_key_down;
		bool right_key_down;
		bool down_key_down;
		bool exit_check;
		bool game_over;

		CMovingBitmap retry;
		bool retry_selected;
		CMovingBitmap back_to_tittle;
		bool back_to_tittle_selected;

		unsigned game_current_time;
		unsigned game_minutes;
		unsigned game_seconds;
		unsigned game_milliseconds;

		unsigned lines_displacement_front;
		unsigned lines_displacement_back;
		unsigned end_time_displacement;
		unsigned score_displacement;
		unsigned level_displacement;

		unsigned real_time;

		char lines_display_front[6] = {};
		char lines_display_back[6] = {};
		char time_display_front[10] = {};
		char time_display_back[5] = {};
		char end_time_display_front[10] = {};
		char end_time_display_back[5] = {};
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
