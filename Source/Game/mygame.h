#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <map>

using namespace std;

const int CANVAS_WIDTH = 10;
const int CANVAS_HEIGHT = 22;
const int PREVIEW_ROW_COUNT = 2;

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
		AUDIO_DING,				// 0
		AUDIO_LAKE,				// 1
		AUDIO_NTUT				// 2
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
		transparent
	};

	enum Event
	{
		left,
		right,
		down,
		rotate,
		tick
	};

	struct GameState
	{
		vector<vector<Color>> canvas;
		bool game_over;
		int score;
	};

	class Tromino
	{
	public:
		int x = 3;
		int y;
		Color color;
		TrominoMatrix matrix;

		Tromino(Color color, TrominoMatrix matrix)
			: color(color), matrix(matrix)
		{
			y = -height() + 1;
		}

		static Tromino random_tromino()
		{
			Color color;
			TrominoMatrix matrix;

			int tromino_type_count = 7;
			int random_number = rand() % (7 + 1);

			// https://tetris.wiki/Tetromino#The_basic_tetrominoes
			if (random_number % tromino_type_count == 0)
			{
				color = Color::blue;
				matrix = {
					{1, 1, 1, 1},
				};
			}
			else if (random_number % tromino_type_count == 1)
			{
				color = Color::yellow;
				matrix = {
					{1, 1},
					{1, 1},
				};
			}
			else if (random_number % tromino_type_count == 2)
			{
				color = Color::purple;
				matrix = {
					{0, 1, 0},
					{1, 1, 1},
				};
			}
			else if (random_number % tromino_type_count == 2)
			{
				color = Color::green;
				matrix = {
					{0, 1, 1},
					{1, 1, 0},
				};
			}
			else if (random_number % tromino_type_count == 2)
			{
				color = Color::red;
				matrix = {
					{1, 1, 0},
					{0, 1, 1},
				};
			}
			else if (random_number % tromino_type_count == 2)
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
			return this->matrix[0].size();
		}

		int height() const
		{
			return this->matrix.size();
		}
	};

	class TetrisGame
	{
	public:
		optional<Tromino> active_tromino;
		vector<vector<Color>> canvas = vector<vector<Color>>(CANVAS_HEIGHT, vector<Color>(CANVAS_WIDTH));

		GameState event_handler(Event event)
		{
			// Generate a new tromino if active_tromino is null
			if (!this->active_tromino.has_value())
				this->active_tromino.emplace(Tromino::random_tromino());

			// Reset canvas
			for (const auto &row : this->active_tromino->matrix)
			{
				for (size_t i = 0; i < row.size(); ++i)
				{
					int y = this->active_tromino->y + (&row - &this->active_tromino->matrix[0]);
					if (y >= 0)
					{
						this->canvas[y][this->active_tromino->x + i] = Color::black;
					}
				}
			}

			// Move/rotate tromino based on user input
			if (event == Event::left)
			{
				if (this->active_tromino->x > 0)
				{
					this->active_tromino->x -= 1;
				}
			}
			if (event == Event::right)
			{
				if (this->active_tromino->x + this->active_tromino->width() < CANVAS_WIDTH)
				{
					this->active_tromino->x += 1;
				}
			}
			if (event == Event::down)
			{
				while (true)
				{
					event_handler(Event::tick);
					if (!this->active_tromino.has_value())
						return { this->canvas,false };
				}
			}
			if (event == Event::rotate)
			{
				TrominoMatrix new_matrix(this->active_tromino->width(), vector<char>(this->active_tromino->height(), 0));
				for (const auto &row : this->active_tromino->matrix)
				{
					for (size_t i = 0; i < row.size(); ++i)
					{
						new_matrix[i][this->active_tromino->height() - 1 - (&row - &this->active_tromino->matrix[0])] = row[i];
					}
				}
				this->active_tromino->matrix = new_matrix;
			}
			if (event == Event::tick)
			{
				this->active_tromino->y += 1;
			}

			// Render active_tromino to canvas
			for (unsigned row_index = 0; row_index < this->active_tromino->matrix.size(); row_index++)
			{
				auto row = this->active_tromino->matrix[row_index];
				for (unsigned column_index = 0; column_index < row.size(); column_index++)
				{
					auto cell = row[column_index];

					int y = this->active_tromino->y + row_index;
					int x = this->active_tromino->x + column_index;
					// y maybe negative
					if (cell == 1 && y >= 0)
					{
						this->canvas[y][x] = this->active_tromino->color;
					}
				}
			}

			// Remove filled rows and prepend empty rows
			for (unsigned row_index = 0; row_index < this->canvas.size(); row_index++)
			{
				vector<Color> row = this->canvas[row_index];
				if (all_of(row.begin(), row.end(), [](Color color)
				{ return color != Color::black; }))
				{
					this->canvas.erase(this->canvas.begin() + row_index);
					this->canvas.insert(this->canvas.begin(), vector<Color>(CANVAS_WIDTH, Color::black));
				}
			}

			// If active_tromino reaches the bottom
			if (this->reached_bottom())
			{
				// Set active_tromino to null and generate a new tromino on the next process
				this->active_tromino.reset();

				// Check if game is over
				// The first and secord row is transparent and for preview purposes only, thus not counted as a row
				if (
					find_if(
						this->canvas[PREVIEW_ROW_COUNT + 1].begin(),
						this->canvas[PREVIEW_ROW_COUNT + 1].end(),
						[](Color color) { return color != Color::black; }) != this->canvas[1].end())
				{
					return { this->canvas,true };
				}
			}

			return { this->canvas,false };
		}

		bool reached_bottom()
		{
			if (this->active_tromino->y + this->active_tromino->height() >= CANVAS_HEIGHT)
			{
				return true;
			}

			// Loop from left to right
			for (signed column_index = 0; column_index < this->active_tromino->width(); column_index++)
			{
				// Loop from bottom to top
				for (int row_index = this->active_tromino->height() - 1; row_index >= 0; row_index--)
				{
					if (this->active_tromino->matrix[row_index][column_index] == 1)
					{
						// There is something in the canvas below the bottommost `1` cell
						if (
							this->active_tromino->y + row_index + 1 >= 0 &&
							this->canvas[this->active_tromino->y + row_index + 1][this->active_tromino->x + column_index] != Color::black)
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
	protected:
		void OnMove();									// 移動遊戲元素
		void OnShow();									// 顯示這個狀態的遊戲畫面
	private:
		int phase;
		int sub_phase;
		int id;
		bool fire_animation;
		
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

		vector<CMovingBitmap> blitz_menu = vector<CMovingBitmap>(4);
		vector<CMovingBitmap> blitz_menu_check = vector<CMovingBitmap>(4);

		vector<CMovingBitmap> zen_menu = vector<CMovingBitmap>(2);

		vector<CMovingBitmap> start = vector<CMovingBitmap>(4);

		vector<vector<CMovingBitmap>> cube = vector<vector<CMovingBitmap>>(22, vector<CMovingBitmap>(10));
		CMovingBitmap cube_place;
		vector<vector<CMovingBitmap>> cube_next = vector<vector<CMovingBitmap>>(14, vector<CMovingBitmap>(4));

		vector<CMovingBitmap> fire = vector<CMovingBitmap>(4);

		TetrisGame tetris_game;
		int game_action_next_time;
		int game_time_interval;
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
}
