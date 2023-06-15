#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <optional>
#include <string>
#include "tetris_game.h"
#include "button.h"

using namespace std;

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
		void game_almost_over_animation();
		bool game_over_animation();
		bool game_success_animation();
		void game_level_up_animation();
		void game_exit_animation();
		void game_clear_lines_animation(int scene);
		bool game_start_animation(GameType gametype);
		void display_game();
		void display_lines(unsigned lines_total);
		void display_play_passed_time();
		void display_play_remaining_time();
		void display_on_button_score();
		void display_on_right_score();
		void display_on_button_level();
		void display_on_left_level();
		void display_reciprocal_animation();
		void display_lines_graph(unsigned int lines_total);
		void display_play_total_time();
		void display_play_real_time(COLORREF color);
		void display_play_total_score(COLORREF color);
		void display_custom_control_menu();
		void display_finish();
		void display_clear_lines_animation();
		void display_game_start_animation();
		void game_init();
		void game_update(Event event);
		void game_natural_decline();
		void game_control();
		void game_record_current_time();
		void game_record_current_score();
		void game_model(GameType gametype);
		void fail_game_menu_move();
		void fail_game_menu_click(UINT nFlags, CPoint point);
		void fail_game_menu_touch(CPoint point);
		void change_background_music(AUDIO_ID new_background_music, bool is_cycled);
		void change_scene(int new_phase, int new_sub_phase, AUDIO_ID new_background_music, bool is_cycled);

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

		vector<Button> second_menu = vector<Button>(4);					// 二級菜單
		vector<bool> second_menu_selected = { false, false, false, false };

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
		CMovingBitmap zen_mode;

		vector<CMovingBitmap> custom_menu = vector<CMovingBitmap>(2);
		CMovingBitmap custom_control_menu;
		vector<CMovingBitmap> custom_end_menu = vector<CMovingBitmap>(3);

		vector<CMovingBitmap> board_width_control = vector<CMovingBitmap>(2);
		vector<bool> board_width_control_selected = { false, false };
		vector<CMovingBitmap> board_height_control = vector<CMovingBitmap>(2);
		vector<bool> board_height_control_selected = { false, false };

		vector<CMovingBitmap> again = vector<CMovingBitmap>(3);
		vector<CMovingBitmap> start = vector<CMovingBitmap>(4);
		vector<CMovingBitmap> fire = vector<CMovingBitmap>(4);
		CMovingBitmap level_up_scene;
		CMovingBitmap exit_scene;

		int canvas_height = 22;
		int canvas_width = 10;

		int board_height = 20;
		int board_width = 10;

		int straight_var = 0;
		int horizontal_var = 0;

		TetrisGame tetris_game = TetrisGame(22, 10);
		TetrisGame save_tetris_game = TetrisGame(22, 10);
		CMovingBitmap cube;
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
		vector<CMovingBitmap> lines_graph;
		CMovingBitmap lines_graph_body;
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

		CMovingBitmap single_scene;
		CMovingBitmap double_scene;
		CMovingBitmap triple_scene;
		CMovingBitmap quad_scene;

		int current_lines;

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
		string start_animation_font;
		bool test_mode = false;
		int clear_lines_fourtyl = 40;
		int clear_lines_zen = 20;
		int clear_lines_custom = 150;
		int remaing_time_blitz = 120000;
		vector<bool> is_played_start_animation_effect;

		Button play_url;
		Button tetra_channel_url;
		Button about_url;
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
