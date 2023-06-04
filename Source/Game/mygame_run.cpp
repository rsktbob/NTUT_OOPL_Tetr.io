#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"
#include "mygame.h"
#include <ctime>
#include <algorithm>
#include <string>

using namespace game_framework;

CAudio* music = CAudio::Instance();

/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
/////////////////////////////////////////////////////////////////////////////



//Button----------------------------------------------------------------
Button::Button(AUDIO_ID touch_audio_effect, AUDIO_ID click_audio_effect, AUDIO_ID next_background_music) : CMovingBitmap(),
_touch_audio_effect(touch_audio_effect),
_click_audio_effect(click_audio_effect),
_next_background_music(next_background_music) {}

bool Button::is_clicked(UINT nFlags, CPoint point, CMovingBitmap character)
{
	if (is_touched(point, character) && nFlags == VK_LBUTTON)
	{
		return true;
	}
	return false;
}

bool Button::is_touched(CPoint point, CMovingBitmap character)
{
	if (point.x >= character.GetLeft() && point.x <= character.GetLeft() + character.GetWidth() && point.y >= character.GetTop() && point.y <= character.GetTop() + character.GetHeight())
	{
		_is_touched_first = false;
		return true;
	}
	_is_touched_first = true;
	return false;
}

//---------------------------------------------------------------------

void TetrisGame::remove_and_prepend_rows() {
	int per_round_lines = 0;
	for (unsigned row_index = 0; row_index < canvas.size(); row_index++) {
		vector<Color> row = canvas[row_index];
		if (all_of(row.begin(), row.end(), [](Color color) { return color != Color::black; })) {
			music->Play(AUDIO_ID::Cube_Clear);
			per_round_lines += 1;
			canvas.erase(canvas.begin() + row_index);
			canvas.insert(canvas.begin(), vector<Color>(canvas_width, Color::black));
		}
	}
	lines += per_round_lines;
	per_round_score += cleared_lines_to_get_score[per_round_lines];
}

bool CGameStateRun::click_check(UINT nFlags, CPoint point, CMovingBitmap character)
{
	if (point.x >= character.GetLeft() && point.x <= character.GetLeft() + character.GetWidth()
		&& point.y >= character.GetTop() && point.y <= character.GetTop() + character.GetHeight()
		&& nFlags == VK_LBUTTON)
	{
		return true;
	}
	return false;
}

bool CGameStateRun::touch_check(CPoint point, CMovingBitmap character)
{
	if (point.x >= character.GetLeft() && point.x <= character.GetLeft() + character.GetWidth()
		&& point.y >= character.GetTop() && point.y <= character.GetTop() + character.GetHeight())
	{
		return true;
	}
	return false;
}

void CGameStateRun::touch_option_menu(CPoint point)
{
	if (point.x >= 270 && point.x <= 1655)
	{
		if (point.y >= 455 && point.y <= 485)
		{
			touch_option_menu_selected = true;
		}
		else if (point.y >= 491 && point.y <= 521)
		{
			touch_option_menu_selected = true;
		}
		else if (point.y >= 527 && point.y <= 557)
		{
			touch_option_menu_selected = true;
		}
		else if (point.y >= 563 && point.y <= 593)
		{
			touch_option_menu_selected = true;
		}
		else
		{
			touch_option_menu_selected = false;
		}
	}
	else
	{
		touch_option_menu_selected = false;
	}
}

void CGameStateRun::set_canvas(int height, int width)
{
	canvas_height = height + 2;
	canvas_width = width;
}

CMovingBitmap CGameStateRun::Cube()
{
	CMovingBitmap cube;
	cube.LoadBitmapByString({ "resources/cube_black.bmp", "resources/cube_light_blue.bmp", "resources/cube_yellow.bmp", "resources/cube_purple.bmp",  "resources/cube_green.bmp",
		"resources/cube_red.bmp", "resources/cube_blue.bmp",  "resources/cube_orange.bmp", "resources/cube_grey.bmp" });
	cube.LoadBitmapByString({ "resources/cube_transparent.bmp" }, RGB(255, 255, 255));
	cube.LoadBitmapByString({ "resources/cube_translucent.bmp" });
	return cube;
}


void CGameStateRun::display_game()
{
	cube_next_border.ShowBitmap();
	cube_hold_border.ShowBitmap();
	for_each(left_cube_border.begin(), left_cube_border.end(), [](CMovingBitmap &cube) { cube.ShowBitmap(); });
	for_each(right_cube_border.begin(), right_cube_border.end(), [](CMovingBitmap &cube) { cube.ShowBitmap(); });
	for_each(bottom_cube_border.begin(), bottom_cube_border.end(), [](CMovingBitmap &cube) { cube.ShowBitmap(); });
	for (int i = 0; i < (int)cubes.size(); i++)
	{
		for (int j = 0; j < (int)cubes[0].size(); j++)
		{
			cubes[i][j].ShowBitmap();
		}
	}

	for (int i = 0; i < NEXT_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < NEXT_CUBE_CANVAS_WIDTH; j++)
		{
			next_cubes[i][j].ShowBitmap();
		}
	}

	for (int i = 0; i < HOLD_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < HOLD_CUBE_CANVAS_WIDTH; j++)
		{
			hold_cubes[i][j].ShowBitmap();
		}
	}
}

void CGameStateRun::display_lines(unsigned lines_total)
{
	lines_displacement_front = (to_string(tetris_game.lines).size() - 1) * 20 - (2 - to_string(lines_total).size()) * 15;
	lines_displacement_back = (2 - to_string(lines_total).size()) * 15;
	sprintf(lines_display_front, "%d", tetris_game.lines);
	sprintf(lines_display_back, "/%d", lines_total);

	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 22, "微軟正黑體", font_color, 50);
	CTextDraw::Print(pDC, 710 + horizontal_var, 715 - straight_var + font_decline_distance, "LINES");
	CTextDraw::ChangeFontLog(pDC, 43, "微軟正黑體", font_color, 200);
	CTextDraw::Print(pDC, 707 + horizontal_var - lines_displacement_front, 740 - straight_var + font_decline_distance, lines_display_front);
	CTextDraw::ChangeFontLog(pDC, 28, "微軟正黑體", font_color, 200);
	CTextDraw::Print(pDC, 733 + horizontal_var + lines_displacement_back, 754 - straight_var + font_decline_distance, lines_display_back);

	CDDraw::ReleaseBackCDC();
}


void CGameStateRun::display_play_passed_time()
{
	if (!tetris_game.game_success)
	{
		game_current_time = clock() - tetris_game.init_time;
		game_minutes = game_current_time / 60000;
		game_seconds = (game_current_time / 1000) % 60;
		game_milliseconds = game_current_time % 1000;
	}
	sprintf(time_display_front, "%d:%02d", game_minutes, game_seconds);
	sprintf(time_display_back, ".%03d", game_milliseconds);

	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 22, "微軟正黑體", font_color, 50);
	CTextDraw::Print(pDC, 718 + horizontal_var, 800 - straight_var + font_decline_distance, "TIME");
	CTextDraw::ChangeFontLog(pDC, 43, "微軟正黑體", font_color, 50);
	CTextDraw::Print(pDC, 630 + horizontal_var, 825 - straight_var + font_decline_distance, time_display_front);
	CTextDraw::ChangeFontLog(pDC, 28, "微軟正黑體", font_color, 50);
	CTextDraw::Print(pDC, 715 + horizontal_var, 838 - straight_var + font_decline_distance, time_display_back);

	CDDraw::ReleaseBackCDC();
}

void CGameStateRun::display_play_remaining_time()
{
	game_remaining_time = 120000 - (clock() - tetris_game.init_time) >= 0 ? 120000 - (clock() - tetris_game.init_time) : 0;
	game_minutes = game_remaining_time / 60000;
	game_seconds = (game_remaining_time / 1000) % 60;
	game_milliseconds = game_remaining_time % 1000;
	sprintf(time_display_front, "%d:%02d", game_minutes, game_seconds);
	sprintf(time_display_back, ".%03d", game_milliseconds);

	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 22, "微軟正黑體", font_color, 50);
	CTextDraw::Print(pDC, 718, 800 + font_decline_distance, "TIME");
	CTextDraw::ChangeFontLog(pDC, 43, "微軟正黑體", font_color, 50);
	CTextDraw::Print(pDC, 630, 825 + font_decline_distance, time_display_front);
	CTextDraw::ChangeFontLog(pDC, 28, "微軟正黑體", font_color, 50);
	CTextDraw::Print(pDC, 715, 838 + font_decline_distance, time_display_back);

	CDDraw::ReleaseBackCDC();
}

void CGameStateRun::display_on_button_score()
{
	score_display = to_string(tetris_game.score);
	for (int i = score_display.length() - 3; i >= 1; i -= 3)
	{
		score_display.insert(i, ",");
	}
	score_displacement = (score_display.length() * 12) / 2;

	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 22, "微軟正黑體", RGB(255, 255, 255), 50);
	CTextDraw::Print(pDC, 948 - score_displacement, 875, score_display);

	CDDraw::ReleaseBackCDC();
}

void CGameStateRun::display_on_right_score()
{
	score_display = to_string(tetris_game.score);
	for (int i = score_display.length() - 3; i >= 1; i -= 3)
	{
		score_display.insert(i, ",");
	}

	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 22, "微軟正黑體", font_color, 50);
	CTextDraw::Print(pDC, 1145 - horizontal_var, 750 + straight_var + font_decline_distance, "SCORE");

	CTextDraw::ChangeFontLog(pDC, 43, "微軟正黑體", font_color, 50);
	CTextDraw::Print(pDC, 1145 - horizontal_var, 775 + straight_var + font_decline_distance, score_display);

	CDDraw::ReleaseBackCDC();
}


void CGameStateRun::display_on_button_level()
{
	sprintf(level_display, "%d", tetris_game.level);
	level_displacement = (strlen(level_display) * 41) / 2;

	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 45, "微軟正黑體", RGB(255, 255, 255), 50);
	CTextDraw::Print(pDC, 955 - level_displacement, 900, level_display);

	CDDraw::ReleaseBackCDC();
}

void CGameStateRun::display_on_left_level()
{
	sprintf(level_display, "%d", tetris_game.level);
	level_displacement = (to_string(tetris_game.level).size() - 1) * 20;

	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 22, "微軟正黑體", font_color, 50);
	CTextDraw::Print(pDC, 707 + horizontal_var, 630 - straight_var + font_decline_distance, "LEVEL");

	CTextDraw::ChangeFontLog(pDC, 43, "微軟正黑體", font_color, 200);
	CTextDraw::Print(pDC, 755 + horizontal_var - level_displacement, 655 - straight_var + font_decline_distance, level_display);

	CDDraw::ReleaseBackCDC();
}

void CGameStateRun::display_lines_graph(unsigned int lines_total)
{
	int length = (int)lines_graph.size();
	int lines = (int)round(tetris_game.lines * ((double)length / lines_total)) > length ? length : (int)round(tetris_game.lines * ((double)length / lines_total));
	for (int i = 0; i < lines; i++)
	{
		if (i < lines - 1)
		{
			lines_graph[i].ShowBitmap();
		}
		else
		{
			lines_graph_top.SetTopLeft(lines_graph[i].GetLeft(), lines_graph[i].GetTop());
			lines_graph_top.ShowBitmap();
		}
	}
}

void CGameStateRun::display_play_total_time()
{
	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 120, "微軟正黑體", RGB(244, 193, 155), 50);
	CTextDraw::Print(pDC, 765 - end_time_displacement, 180, end_time_display_front);
	CTextDraw::ChangeFontLog(pDC, 85, "微軟正黑體", RGB(244, 193, 155), 50);
	CTextDraw::Print(pDC, 996 + end_time_displacement, 210, end_time_display_back);

	CDDraw::ReleaseBackCDC();
}

void CGameStateRun::display_play_real_time(COLORREF color)
{
	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 30, "微軟正黑體", color, 50);
	CTextDraw::Print(pDC, 385, 964, real_time_display);

	CDDraw::ReleaseBackCDC();
}

void CGameStateRun::display_play_total_score(COLORREF color)
{
	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 120, "微軟正黑體", color, 50);
	CTextDraw::Print(pDC, 931 - end_score_displacement, 180, end_score_display);

	CDDraw::ReleaseBackCDC();
}

void  CGameStateRun::display_reciprocal_animation()
{
	if (120000 - (clock() - tetris_game.init_time) <= 10000 && 120000 - (clock() - tetris_game.init_time) > 0 && !tetris_game.game_over)
	{
		string reciprocal_num = to_string(120000 - (clock() - tetris_game.init_time) < 0 ? 0 : (int)ceil((120000 - (double)(clock() - tetris_game.init_time)) / 1000));
		unsigned reciprocal_num_displacement = reciprocal_num.length() * 35;
		CDC *pDC = CDDraw::GetBackCDC();

		CTextDraw::ChangeFontLog(pDC, 120, "微軟正黑體", RGB(247, 193, 4), 50);
		CTextDraw::Print(pDC, 950 - reciprocal_num_displacement, 485, reciprocal_num);

		CDDraw::ReleaseBackCDC();
	}
}

void CGameStateRun::display_custom_control_menu()
{
	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 18, "微軟正黑體", RGB(228, 228, 228), 50);
	CTextDraw::Print(pDC, 302, 325, "BOARD WIDTH");
	CTextDraw::Print(pDC, 450, 325, to_string(board_width));
	CTextDraw::Print(pDC, 302, 358, "BOARD HEOGHT");
	CTextDraw::Print(pDC, 473, 358, to_string(board_height));

	CDDraw::ReleaseBackCDC();

	for (int i = 0; i < 2; i++)
	{
		board_width_control[i].ShowBitmap();
		board_height_control[i].ShowBitmap();
	}
}

void CGameStateRun::display_finish()
{
	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 80, "微軟正黑體", RGB(238, 255, 26), 50);
	CTextDraw::Print(pDC, 841, 480, "Finish");

	CDDraw::ReleaseBackCDC();
}

void CGameStateRun::display_clear_lines_animation()
{
	single_scene.ShowBitmap();
	double_scene.ShowBitmap();
	triple_scene.ShowBitmap();
	quad_scene.ShowBitmap();
}

bool CGameStateRun::game_over_animation()
{
	if (fire_animation_check)
	{
		music->Play(AUDIO_ID::Game_Over);
		for_each(left_cube_border.begin(), left_cube_border.end(), [](CMovingBitmap &boder) { boder.SetFrameIndexOfBitmap(1);  });
		for_each(right_cube_border.begin(), right_cube_border.end(), [](CMovingBitmap &boder) { boder.SetFrameIndexOfBitmap(1);  });
		for_each(bottom_cube_border.begin(), bottom_cube_border.end(), [](CMovingBitmap &boder) { boder.SetFrameIndexOfBitmap(1);  });
		cube_hold_border.SetFrameIndexOfBitmap(1);
		cube_next_border.SetFrameIndexOfBitmap(1);
		font_color = RGB(255, 51, 0);
		for (int i = 0; i < 4; i++)
		{
			fire[i].ToggleAnimation();
		}
		fire_animation_check = false;
	}
	if (fire[0].GetFrameIndexOfBitmap() == 37)
	{
		if (cubes[0][0].GetTop() < 1080)
		{
			font_decline_distance += 30;
			for (int i = 0; i < canvas_height; i++)
			{
				for (int j = 0; j < canvas_width; j++)
				{
					cubes[i][j].SetTopLeft(cubes[i][j].GetLeft(), cubes[i][j].GetTop() + 30);
				}
			}
			for (int i = 0; i < NEXT_CUBE_CANVAS_HEIGHT; i++)
			{
				for (int j = 0; j < NEXT_CUBE_CANVAS_WIDTH; j++)
				{
					next_cubes[i][j].SetTopLeft(next_cubes[i][j].GetLeft(), next_cubes[i][j].GetTop() + 30);
				}
			}
			for (int i = 0; i < HOLD_CUBE_CANVAS_HEIGHT; i++)
			{
				for (int j = 0; j < HOLD_CUBE_CANVAS_WIDTH; j++)
				{
					hold_cubes[i][j].SetTopLeft(hold_cubes[i][j].GetLeft(), hold_cubes[i][j].GetTop() + 30);
				}
			}
			for (unsigned i = 0; i < lines_graph.size(); i++) { lines_graph[i].SetTopLeft(lines_graph[i].GetLeft(), lines_graph[i].GetTop() + 30); }
			for (unsigned i = 0; i < left_cube_border.size(); i++) 
			{ 
				left_cube_border[i].SetTopLeft(left_cube_border[i].GetLeft(), left_cube_border[i].GetTop() + 30);
				right_cube_border[i].SetTopLeft(right_cube_border[i].GetLeft(), right_cube_border[i].GetTop() + 30);
			}
			for (unsigned i = 0; i < bottom_cube_border.size(); i++)
			{
				bottom_cube_border[i].SetTopLeft(bottom_cube_border[i].GetLeft(), bottom_cube_border[i].GetTop() + 30);
			}
			cube_hold_border.SetTopLeft(cube_hold_border.GetLeft(), cube_hold_border.GetTop() + 30);
			cube_next_border.SetTopLeft(cube_next_border.GetLeft(), cube_next_border.GetTop() + 30);
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool CGameStateRun::game_success_animation()
{
	static int record_current_time = 0;
	if (finish_animation_check) {
		music->Stop(background_music);
		music->Play(AUDIO_ID::Game_Finish);
		game_record_current_time();
		record_current_time = clock();
		finish_animation_check = false;
	}
	else if (record_current_time < clock() - 2000) {
		return false;
	}
	return true;
}

void CGameStateRun::game_level_up_animation()
{
	if (level_up_animation_check)
	{
		level_up_scene.SetFrameIndexOfBitmap(0);
		music->Pause();
		music->Play(AUDIO_ID::Level_Up);
		level_up_scene.ToggleAnimation();
		level_up_animation_check = false;
	}
	if (level_up_scene.GetFrameIndexOfBitmap() == 124)
	{
		music->Resume();
		tetris_game.level += 1;
		tetris_game.lines -= 20;
		level_up_animation_check = true;
	}
}

void CGameStateRun::game_exit_animation()
{
	if (exit_check)
	{
		if (exit_scene.GetFrameIndexOfBitmap() < 31)
		{
			exit_scene.SetFrameIndexOfBitmap(exit_scene.GetFrameIndexOfBitmap() + 1);
			if (exit_scene.GetFrameIndexOfBitmap() == 3 || exit_scene.GetFrameIndexOfBitmap() == 11 || exit_scene.GetFrameIndexOfBitmap() == 18)
			{
				music->Play(AUDIO_ID::Exit_Process_Game);
			}
			if (exit_scene.GetFrameIndexOfBitmap() == 31)
			{
				music->Play(AUDIO_ID::Exit_Game);
			}
		}
		else
		{
			tittle.SetFrameIndexOfBitmap(1);
			game_mode.SetFrameIndexOfBitmap(0);
			music->Stop(background_music);
			music->Play(AUDIO_ID::Arial_City, true);
			phase = 2;
		}
	}
	else
	{
		if (exit_scene.GetFrameIndexOfBitmap() > 0)
		{
			exit_scene.SetFrameIndexOfBitmap(exit_scene.GetFrameIndexOfBitmap() - 1);
		}
		else
		{
			exit_scene.SetFrameIndexOfBitmap(31);
		}
	}
}

void CGameStateRun::game_init()
{
	left_key_down = false;
	right_key_down = false;
	down_key_down = false;
	exit_check = false;
	fire_animation_check = true;
	level_up_animation_check = true;
	finish_animation_check = true;
	current_lines = 0;
	decline_distance = 0;
	font_color = RGB(255, 255, 255);
	font_decline_distance = 0;
	game_decline_time_interval = 1000;
	background.SetFrameIndexOfBitmap(rand() % 6);
	tetris_game = TetrisGame(canvas_height, canvas_width);
	game_remaining_time = 120000;
	game_current_time = clock();
	tetris_game.init_time = clock();
	game_next_decline_time = clock();
	game_next_move_time = clock();
	cubes = vector<vector<CMovingBitmap>>(canvas_height, vector<CMovingBitmap>(canvas_width, Cube()));
	lines_graph = vector<CMovingBitmap>((canvas_height - 2) * 2, lines_graph_body);
	straight_var = (22 - canvas_height) * 16;
	horizontal_var = (10 - canvas_width) * 16;
	int cubes_position_x = 788 + horizontal_var;
	int cubes_position_y = 160 + straight_var;
	int next_cubes_position_x = 1154 - horizontal_var;
	int next_cubes_position_y = 270 + straight_var;
	int hold_cubes_position_x = 638 + horizontal_var;
	int hold_cubes_position_y = 267 + straight_var;
	int left_cubes_border_position_x = 785 + horizontal_var;
	int left_cubes_border_position_y = 224 + straight_var;
	int right_cubes_border_position_x = 1108 - horizontal_var;
	int right_cubes_border_position_y = 224 + straight_var;
	int bottom_cubes_border_position_x = 785 + horizontal_var;
	int bottom_cubes_border_position_y = 864 - straight_var;
	int lines_graph_position_x = 1114 - horizontal_var;
	int lines_graph_position_y = 847 - straight_var;
	left_cube_border = vector<CMovingBitmap>(canvas_height - 2, cube_staight_border);
	right_cube_border = vector<CMovingBitmap>(canvas_height - 2, lines_graph_border);
	bottom_cube_border = vector<CMovingBitmap>(canvas_width + 1, cube_horizontal_border);
	cube_hold_border.SetFrameIndexOfBitmap(0);
	cube_next_border.SetFrameIndexOfBitmap(0);
	fire[0].SetTopLeft(670 + horizontal_var, 78 + straight_var);
	fire[1].SetTopLeft(1050 - horizontal_var, 78 + straight_var);
	fire[2].SetTopLeft(670 + horizontal_var, 870 - straight_var);
	fire[3].SetTopLeft(1050 - horizontal_var, 870 - straight_var);
	single_scene.SetTopLeft(520 + horizontal_var, 375 + straight_var);
	single_scene.SetFrameIndexOfBitmap(12);
	double_scene.SetTopLeft(450 + horizontal_var, 395 + straight_var);
	double_scene.SetFrameIndexOfBitmap(12);
	triple_scene.SetTopLeft(480 + horizontal_var, 388 + straight_var);
	triple_scene.SetFrameIndexOfBitmap(12);
	quad_scene.SetTopLeft(553 + horizontal_var, 383 + straight_var);
	quad_scene.SetFrameIndexOfBitmap(12);
	for (int i = 0; i < (int)cubes.size(); i++)
	{
		for (int j = 0; j < (int)cubes[0].size(); j++)
		{
			cubes[i][j].SetTopLeft(cubes_position_x + j * 32, cubes_position_y + i * 32);
		}
	}
	for (int i = 0; i < 14; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			next_cubes[i][j].SetTopLeft(next_cubes_position_x + j * 32, next_cubes_position_y + i * 32);
		}
	}
	for (int i = 0; i < HOLD_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < HOLD_CUBE_CANVAS_WIDTH; j++)
		{
			hold_cubes[i][j].SetTopLeft(hold_cubes_position_x + j * 32, hold_cubes_position_y + i * 32);
		}
	}
	for (int i = 0; i < (int)left_cube_border.size(); i++)
	{
		left_cube_border[i].SetTopLeft(left_cubes_border_position_x, left_cubes_border_position_y + i * 32);
		right_cube_border[i].SetTopLeft(right_cubes_border_position_x, right_cubes_border_position_y + i  * 32);
	}
	for (int i = 0; i < (int)bottom_cube_border.size(); i++)
	{
		bottom_cube_border[i].SetTopLeft(bottom_cubes_border_position_x + i * 32, bottom_cubes_border_position_y);
	}
	for (unsigned i = 0; i < lines_graph.size(); i++)
	{
		lines_graph[i].SetTopLeft(lines_graph_position_x, lines_graph_position_y - i * 16);
	}
	cube_hold_border.SetTopLeft(cubes_position_x - 168, cubes_position_y + 64);
	cube_next_border.SetTopLeft(cubes_position_x + canvas_width * 32 + 29, cubes_position_y + 64);
	for_each(fire.begin(), fire.end(), [](CMovingBitmap fire) { fire.SetFrameIndexOfBitmap(0); });
}

void CGameStateRun::game_update(Event event)
{
	if (tetris_game.game_over) return;

	tetris_game.event_handler(event);
	Canvas canvas = tetris_game.canvas;
	for (int i = 0; i < canvas_height; i++)
	{
		for (int j = 0; j < canvas_width; j++)
		{
			if (i < PREVIEW_ROW_COUNT && canvas[i][j] == Color::black)
			{
				cubes[i][j].SetFrameIndexOfBitmap(Color::transparent);
			}
			else
			{
				cubes[i][j].SetFrameIndexOfBitmap(canvas[i][j]);
			}
		}
	}

	Canvas place_canvas = tetris_game.next_canvas;
	for (int i = 0; i < NEXT_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < NEXT_CUBE_CANVAS_WIDTH; j++)
		{
			next_cubes[i][j].SetFrameIndexOfBitmap(place_canvas[i][j]);
		}
	}

	Canvas hold_canvas = tetris_game.hold_canvas;
	for (int i = 0; i < HOLD_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < HOLD_CUBE_CANVAS_WIDTH; j++)
		{
			hold_cubes[i][j].SetFrameIndexOfBitmap(hold_canvas[i][j]);
		}
	}
}

void  CGameStateRun::game_natural_decline()
{
	game_update(Event::tick);
	game_next_decline_time = clock() + game_decline_time_interval;
}

void CGameStateRun::game_control()
{
	if (left_key_down)
	{
		music->Play(AUDIO_ID::Cube_Horizontal_Move);
		game_update(Event::left);
	}
	if (right_key_down)
	{
		music->Play(AUDIO_ID::Cube_Horizontal_Move);
		game_update(Event::right);
	}
	if (down_key_down)
	{
		music->Play(AUDIO_ID::Cube_Decline_Move);
		game_update(Event::tick);
	}
	game_next_move_time = clock() + game_move_time_interval;
	game_move_time_interval = 55;
}

void CGameStateRun::game_record_current_time()
{
	now = time(0);
	tm *localtm = localtime(&now);

	int real_time_year = localtm->tm_year + 1900;
	int real_time_month = localtm->tm_mon + 1;
	int real_time_day = localtm->tm_mday;

	int real_time_hour = localtm->tm_hour;
	int real_time_min = localtm->tm_min;
	int real_time_sec = localtm->tm_sec;

	end_time_displacement = game_minutes >= 10 ? 35 : 0;

	sprintf(end_time_display_front, "%d:%02d", game_minutes, game_seconds);
	sprintf(end_time_display_back, ".%03d", game_milliseconds);
	sprintf(real_time_display, "%d/%d/%02d  %d:%02d:%02d", real_time_year, real_time_month, real_time_day, real_time_hour, real_time_min, real_time_sec);
}

void CGameStateRun::game_record_current_score()
{
	end_score_display = to_string(tetris_game.score);
	for (int i = end_score_display.length() - 3; i >= 1; i -= 3)
	{
		end_score_display.insert(i, ",");
	}
	end_score_displacement = (end_score_display.length() - 1) * 35;
}

void CGameStateRun::game_model(GameType gametype)
{
	if (gametype == GameType::fourtyl)
	{
		if (sub_phase == 1)
		{
			if (touch_option_menu_selected)
			{
				if (touch_option_menu_first)
				{
					music->Play(AUDIO_ID::Touch_Check_Menu);
					touch_option_menu_first = false;
				}
			}
			else
			{
				touch_option_menu_first = true;
			}
		}
		else if (sub_phase == 2)
		{
			if (!tetris_game.game_over && !tetris_game.game_success)
			{
				if (game_next_decline_time <= clock())
				{
					game_natural_decline();
				}
				if (game_next_move_time <= clock())
				{
					game_control();
				}
				if (tetris_game.lines >= 40)
				{
					tetris_game.game_success = true;
				}
				if (exit_check || exit_scene.GetFrameIndexOfBitmap() != 31)
				{
					game_exit_animation();
				}
				if (current_lines < tetris_game.lines)
				{
					game_clear_lines_animation(tetris_game.lines - current_lines);
					current_lines = tetris_game.lines;
				}
			}
			else if (tetris_game.game_over)
			{
				if (!game_over_animation())
				{
					music->Stop(background_music);
					music->Play(AUDIO_ID::Game_Over_Menu);
					sub_phase = 3;
				}
			}
			else {
				if (!game_success_animation())
				{
					background_music = AUDIO_ID::Success_Story_Akiko_Shioyama;
					music->Play(background_music, true);
					tittle.SetFrameIndexOfBitmap(6);
					again[0].SetAnimation(60, false);
					sub_phase = 4;
				}
			}
		}
		else if (sub_phase == 3)
		{
			fail_game_menu_move();
		}
		else if (sub_phase == 4)
		{
			if (back.GetLeft() < 0 && back_selected)
			{
				if (touch_menu_check_first)
				{
					music->Play(AUDIO_ID::Touch_Menu);
					touch_menu_check_first = false;
				}
				back.SetTopLeft(back.GetLeft() + 10, 80);
			}
			else if (back.GetLeft() > -40 && !back_selected)
			{
				touch_menu_check_first = true;
				back.SetTopLeft(back.GetLeft() - 10, 80);
			}
		}
	}
	else if (gametype == GameType::blitz)
	{
		if (sub_phase == 1)
		{
			if (touch_option_menu_selected)
			{
				if (touch_option_menu_first)
				{
					music->Play(AUDIO_ID::Touch_Check_Menu);
					touch_option_menu_first = false;
				}
			}
			else
			{
				touch_option_menu_first = true;
			}
		}
		else if (sub_phase == 2)
		{
			if (!tetris_game.game_over && !tetris_game.game_success)
			{
				if (game_next_decline_time <= clock())
				{
					game_natural_decline();
				}
				if (game_next_move_time <= clock())
				{
					game_control();
				}
				if (exit_check || exit_scene.GetFrameIndexOfBitmap() != 31)
				{
					game_exit_animation();
				}
				if (tetris_game.lines >= blitz_level_to_lines[tetris_game.level])
				{
					tetris_game.lines -= blitz_level_to_lines[tetris_game.level];
					tetris_game.level += 1;
					game_decline_time_interval = blitz_level_to_speed[tetris_game.level];
				}
				if (game_remaining_time == 0)
				{
					tetris_game.game_success = true;
				}
				if (current_lines < tetris_game.lines)
				{
					game_clear_lines_animation(tetris_game.lines - current_lines);
					current_lines = tetris_game.lines;
				}
			}
			else if (tetris_game.game_over)
			{
				if (!game_over_animation())
				{
					music->Stop(background_music);
					music->Play(AUDIO_ID::Game_Over_Menu);
					sub_phase = 3;
				}
			}
			else {
				if (!game_success_animation())
				{
					background_music = AUDIO_ID::Philosophy;
					music->Play(background_music, true);
					game_record_current_time();
					game_record_current_score();
					tittle.SetFrameIndexOfBitmap(7);
					again[1].SetAnimation(60, false);
					sub_phase = 4;
				}
			}
		}
		else if (sub_phase == 3)
		{
			fail_game_menu_move();
		}
		else if (sub_phase == 4)
		{
			if (back.GetLeft() < 0 && back_selected)
			{
				if (touch_menu_check_first)
				{
					music->Play(AUDIO_ID::Touch_Menu);
					touch_menu_check_first = false;
				}
				back.SetTopLeft(back.GetLeft() + 10, 80);
			}
			else if (back.GetLeft() > -40 && !back_selected)
			{
				touch_menu_check_first = true;
				back.SetTopLeft(back.GetLeft() - 10, 80);
			}
		}
	}
	else if (gametype == GameType::zen)
	{
		if (sub_phase == 2)
		{
			if (!tetris_game.game_over)
			{
				if (game_next_decline_time <= clock())
				{
					game_natural_decline();
				}
				if (game_next_move_time <= clock())
				{
					game_control();
				}
				if (exit_check || exit_scene.GetFrameIndexOfBitmap() != 31)
				{
					game_exit_animation();
					save_tetris_game = tetris_game;
				}
				if (current_lines < tetris_game.lines)
				{
					game_clear_lines_animation(tetris_game.lines - current_lines);
					current_lines = tetris_game.lines;
				}
			}
			else
			{
				music->Play(AUDIO_ID::Cube_Full_Clear);
				sub_phase = 3;
				record_current_time = clock() + 2000;
			}
			if (tetris_game.lines >= 20)
			{
				game_level_up_animation();
			}
		}
		else if (sub_phase == 3)
		{
			if (record_current_time < clock())
			{
				tetris_game.clear_all_canvas();
				tetris_game.game_over = false;
				sub_phase = 2;
			}
		}
	}
	else if (gametype == GameType::custom)
	{
		if (sub_phase == 2)
		{
			if (!tetris_game.game_over)
			{
				if (game_next_decline_time <= clock())
				{
					game_natural_decline();
				}
				if (game_next_move_time <= clock())
				{
					game_control();
				}
				if (exit_check || exit_scene.GetFrameIndexOfBitmap() != 31)
				{
					game_exit_animation();
				}
				if (current_lines < tetris_game.lines)
				{
					game_clear_lines_animation(tetris_game.lines - current_lines);
					current_lines = tetris_game.lines;
				}
			}
			else if (tetris_game.game_over)
			{
				if (!game_over_animation())
				{
					music->Stop(background_music);
					music->Play(AUDIO_ID::Game_Over_Menu);
					music->Play(AUDIO_ID::Classy_Cat, true);
					background_music = AUDIO_ID::Classy_Cat;
					again[2].SetAnimation(60, false);
					game_record_current_time();
					game_record_current_score();
					sub_phase = 3;
				}
			}
		}
		else if (sub_phase == 3)
		{
			if (back.GetLeft() < 0 && back_selected)
			{
				if (touch_menu_check_first)
				{
					music->Play(AUDIO_ID::Touch_Menu);
					touch_menu_check_first = false;
				}
				back.SetTopLeft(back.GetLeft() + 10, 80);
			}
			else if (back.GetLeft() > -40 && !back_selected)
			{
				touch_menu_check_first = true;
				back.SetTopLeft(back.GetLeft() - 10, 80);
			}
		}
	}
}

void CGameStateRun::game_clear_lines_animation(int scene)
{
	if (scene == 1)
	{
		single_scene.SetFrameIndexOfBitmap(0);
		single_scene.ToggleAnimation();
	}
	if (scene == 2)
	{
		double_scene.SetFrameIndexOfBitmap(0);
		double_scene.ToggleAnimation();
	}
	if (scene == 3)
	{
		triple_scene.SetFrameIndexOfBitmap(0);
		triple_scene.ToggleAnimation();
	}
	if (scene == 4)
	{
		quad_scene.SetFrameIndexOfBitmap(0);
		quad_scene.ToggleAnimation();
	}
}

void CGameStateRun::fail_game_menu_move()
{
	if (retry.GetLeft() <= 381)
	{
		if (retry.GetLeft() > 301 && retry_selected)
		{
			if (touch_menu_check_first)
			{
				music->Play(AUDIO_ID::Touch_Menu);
				touch_menu_check_first = false;
			}
			retry.SetTopLeft(retry.GetLeft() - 10, 80);
		}
		else if (retry.GetLeft() < 381 && !retry_selected)
		{
			touch_menu_check_first = true;
			retry.SetTopLeft(retry.GetLeft() + 10, 80);
		}

		if (back_to_tittle.GetLeft() > 301 && back_to_tittle_selected)
		{
			if (touch_menu_check_first)
			{
				music->Play(AUDIO_ID::Touch_Menu);
				touch_menu_check_first = false;
			}
			back_to_tittle.SetTopLeft(back_to_tittle.GetLeft() - 10, 200);
		}
		else if (back_to_tittle.GetLeft() < 381 && !back_to_tittle_selected)
		{
			touch_menu_check_first = true;
			back_to_tittle.SetTopLeft(back_to_tittle.GetLeft() + 10, 200);
		}
	}
	else
	{
		retry.SetTopLeft(retry.GetLeft() - 50, 80);
		back_to_tittle.SetTopLeft(back_to_tittle.GetLeft() - 50, 200);
	}
}

void CGameStateRun::fail_game_menu_click(UINT nFlags, CPoint point, GameType gametype)
{
	if (click_check(nFlags, point, retry))
	{
		music->Play(AUDIO_ID::Back_Menu);
		music->Stop(AUDIO_ID::Arial_City);
		background_music = gametype == GameType::blitz ? AUDIO_ID::Hyper_Velocity : rand() % 6;
		music->Play(background_music, gametype == GameType::blitz ? false : true);
		game_init();
		retry.SetTopLeft(1921, 80);
		back_to_tittle.SetTopLeft(1921, 200);
		sub_phase = 2;
	}
	if (click_check(nFlags, point, back_to_tittle))
	{
		music->Play(AUDIO_ID::Back_Menu);
		music->Play(AUDIO_ID::Arial_City, true);
		retry.SetTopLeft(1921, 80);
		back_to_tittle.SetTopLeft(1921, 200);
		sub_phase = 1;
	}
}

void CGameStateRun::fail_game_menu_touch(CPoint point)
{
	if (touch_check(point, retry))
	{
		retry.SetFrameIndexOfBitmap(1);
		retry_selected = true;
	}
	else
	{
		retry.SetFrameIndexOfBitmap(0);
		retry_selected = false;
	}
	if (touch_check(point, back_to_tittle))
	{
		back_to_tittle.SetFrameIndexOfBitmap(1);
		back_to_tittle_selected = true;
	}
	else
	{
		back_to_tittle.SetFrameIndexOfBitmap(0);
		back_to_tittle_selected = false;
	}
}

void CGameStateRun::change_background_music(AUDIO_ID new_background_music, bool is_cycled)
{
	music->Stop(background_music);
	music->Play(new_background_music, is_cycled);
	background_music = new_background_music;
}

void CGameStateRun::change_scene(int new_phase, int new_sub_phase, AUDIO_ID new_background_music = AUDIO_ID::None, bool is_cycled = false)
{
	phase = new_phase;
	sub_phase = new_phase;
	if (new_background_music != AUDIO_ID::None)
	{
		change_background_music(new_background_music, is_cycled);
	}
}

CGameStateRun::CGameStateRun(CGame *g) : CGameState(g)
{
}

CGameStateRun::~CGameStateRun()
{
}

void CGameStateRun::OnBeginState()
{
}

void CGameStateRun::OnMove()							// 移動遊戲元素
{
	if (phase == 2)
	{
		for (int i = 0; i < 4; i++)
		{
			if (second_menu[i].GetLeft() > 305 && second_menu_selected[i])
			{
				if (touch_menu_check_first)
				{
					music->Play(AUDIO_ID::Touch_Menu);
					touch_menu_check_first = false;
				}
				second_menu[i].SetTopLeft(second_menu[i].GetLeft() - 10, 100 + i * 140);
			}
			else if (second_menu[i].GetLeft() < 375 && !second_menu_selected[i])
			{
				touch_menu_check_first = true;
				second_menu[i].SetTopLeft(second_menu[i].GetLeft() + 10, 100 + i * 140);
			}
		}
	}
	if (phase >= 2 && sub_phase == 1)
	{
		if (back.GetLeft() < 0 && back_selected)
		{
			if (touch_menu_check_first)
			{
				music->Play(AUDIO_ID::Touch_Menu);
				touch_menu_check_first = false;
			}
			back.SetTopLeft(back.GetLeft() + 10, 80);
		}
		else if (back.GetLeft() > -40 && !back_selected)
		{
			touch_menu_check_first = true;
			back.SetTopLeft(back.GetLeft() - 10, 80);
		}
	}
	if (phase == 3)
	{
		game_model(GameType::fourtyl);
	}
	else if (phase == 4)
	{
		game_model(GameType::blitz);
	}
	else if (phase == 5)
	{
		game_model(GameType::zen);
	}
	else if (phase == 6)
	{
		game_model(GameType::custom);
	}
}

void CGameStateRun::OnInit()  								// 遊戲的初值及圖形設定
{
	phase = 1;
	sub_phase = 1;
	background_music = AUDIO_ID::Arial_City;
	id = 0;

	music = CAudio::Instance();
	music->Load(AUDIO_ID::Arial_City, "resources/Arial_City.wav");
	music->Load(AUDIO_ID::To_The_Limit, "resources/To_The_Limit.wav");
	music->Load(AUDIO_ID::The_Great_Eastern_Expedition, "resources/The_Great_Eastern_Expedition.wav");
	music->Load(AUDIO_ID::Morning_Sun_Kamoking, "resources/Morning_Sun_Kamoking.wav");
	music->Load(AUDIO_ID::In_Sorrow_And_Pains_Mirera, "resources/In_Sorrow_And_Pains_Mirera.wav");
	music->Load(AUDIO_ID::Piercing_Wind, "resources/Piercing_Wind.wav");
	music->Load(AUDIO_ID::Success_Story_Akiko_Shioyama, "resources/Success_Story_Akiko_Shioyama.wav");
	music->Load(AUDIO_ID::Hyper_Velocity, "resources/Hyper_Velocity.wav");
	music->Load(AUDIO_ID::Philosophy, "resources/Philosophy.wav");
	music->Load(AUDIO_ID::Classy_Cat, "resources/Classy_Cat.wav");
	music->Load(AUDIO_ID::Touch_Menu, "resources/Touch_Menu.wav");
	music->Load(AUDIO_ID::Click_Menu, "resources/Click_Menu.wav");
	music->Load(AUDIO_ID::Back_Menu, "resources/Back_Menu.wav");
	music->Load(AUDIO_ID::Touch_Check_Menu, "resources/Touch_Check_Menu.wav");
	music->Load(AUDIO_ID::Click_Check_Menu, "resources/Click_Check_Menu.wav");
	music->Load(AUDIO_ID::Touch_Option_Menu, "resources/Touch_Option_Menu.wav");
	music->Load(AUDIO_ID::Cube_Rotate, "resources/Cube_Rotate.wav");
	music->Load(AUDIO_ID::Cube_Horizontal_Move, "resources/Cube_Horizontal_Move.wav");
	music->Load(AUDIO_ID::Cube_Decline_Move, "resources/Cube_Decline_Move.wav");
	music->Load(AUDIO_ID::Cube_Switch, "resources/Cube_Switch.wav");
	music->Load(AUDIO_ID::Cube_Touch_Bottom, "resources/Cube_Touch_Bottom.wav");
	music->Load(AUDIO_ID::Cube_Clear, "resources/Cube_Clear.wav");
	music->Load(AUDIO_ID::Cube_Full_Clear, "resources/Cube_Full_Clear.wav");
	music->Load(AUDIO_ID::Game_Over, "resources/Game_Over.wav");
	music->Load(AUDIO_ID::Game_Finish, "resources/Game_Finish.wav");
	music->Load(AUDIO_ID::Game_Over_Menu, "resources/Game_Over_Menu.wav");
	music->Load(AUDIO_ID::Level_Up, "resources/level_Up.wav");
	music->Load(AUDIO_ID::Exit_Process_Game, "resources/Exit_Process_Game.wav");
	music->Load(AUDIO_ID::Exit_Game, "resources/Exit_Game.wav");

	change_background_music(AUDIO_ID::Arial_City, true);

	first_menu.LoadBitmapByString({ "resources/first_menu.bmp" });
	first_menu.SetTopLeft(420, 340);

	join.LoadBitmapByString({ "resources/join.bmp", "resources/join_done.bmp" });
	join.SetTopLeft(420, 670);

	tittle.LoadBitmapA({ "resources/tittle.bmp", "resources/tittle2.bmp", "resources/40l_tittle.bmp", "resources/blitz_tittle.bmp", "resources/zen_tittle.bmp", "resources/custom_tittle.bmp", "resources/40l_result_tittle.bmp", "resources/blitz_result_tittle.bmp" });
	tittle.SetTopLeft(0, 0);

	osk.LoadBitmapByString({ "resources/osk.bmp" });
	osk.SetTopLeft(0, 930);

	background.LoadBitmapByString({ "resources/background1.bmp",  "resources/background2.bmp", "resources/background3.bmp", "resources/background4.bmp",  "resources/background5.bmp", "resources/background6.bmp" });
	background.SetTopLeft(0, 0);

	second_menu[0].LoadBitmapByString({ "resources/40l.bmp", "resources/40l_selected.bmp" });
	background.SetFrameIndexOfBitmap(rand() % 6);
	second_menu[0].SetTopLeft(375, 100);

	second_menu[1].LoadBitmapByString({ "resources/blitz.bmp", "resources/blitz_selected.bmp" });
	second_menu[1].SetTopLeft(375, 240);

	second_menu[2].LoadBitmapByString({ "resources/zen.bmp", "resources/zen_selected.bmp" });
	second_menu[2].SetTopLeft(375, 380);

	second_menu[3].LoadBitmapByString({ "resources/custom.bmp", "resources/custom_selected.bmp" });
	second_menu[3].SetTopLeft(375, 520);

	back.LoadBitmapByString({ "resources/back.bmp", "resources/back_selected.bmp" });
	back.SetTopLeft(-40, 80);

	logo.LoadBitmapByString({ "resources/logo.bmp" }, RGB(255, 255, 255));
	logo.SetTopLeft(0, 865);

	game_mode.LoadBitmapByString({ "resources/game_mode.bmp", "resources/40l_press_start.bmp",  "resources/blitz_press_start.bmp",  "resources/zen_press_start.bmp", "resources/custom_press_start.bmp" });
	game_mode.SetTopLeft(0, 940);

	fourtyl_menu[0].LoadBitmapByString({ "resources/40l_information.bmp" });
	fourtyl_menu[0].SetTopLeft(270, 100);

	fourtyl_menu[1].LoadBitmapByString({ "resources/40l_music.bmp" });
	fourtyl_menu[1].SetTopLeft(270, 284);

	fourtyl_menu[2].LoadBitmapByString({ "resources/40l_options.bmp" });
	fourtyl_menu[2].SetTopLeft(270, 388);

	fourtyl_menu[3].LoadBitmapByString({ "resources/40l_advanced.bmp" });
	fourtyl_menu[3].SetTopLeft(270, 638);

	fourtyl_end_menu[0].LoadBitmapByString({ "resources/40l_final_time.bmp" });
	fourtyl_end_menu[0].SetTopLeft(270, 100);

	fourtyl_end_menu[1].LoadBitmapByString({ "resources/40l_music.bmp" });
	fourtyl_end_menu[1].SetTopLeft(270, 358);

	fourtyl_end_menu[2].LoadBitmapByString({ "resources/40l_model.bmp" });
	fourtyl_end_menu[2].SetTopLeft(0, 960);

	again[0].LoadBitmapByString({ "resources/40l_again_1.bmp", "resources/40l_again_2.bmp", "resources/40l_again_3.bmp", "resources/40l_again_4.bmp", "resources/40l_again_5.bmp",
		"resources/40l_again_6.bmp", "resources/40l_again_7.bmp", "resources/40l_again_8.bmp", "resources/40l_again_7.bmp", "resources/40l_again_6.bmp", "resources/40l_again_5.bmp",
		"resources/40l_again_4.bmp" , "resources/40l_again_3.bmp" , "resources/40l_again_2.bmp" });
	again[0].SetTopLeft(1394, 358);

	again[1].LoadBitmapByString({ "resources/blitz_again_1.bmp", "resources/blitz_again_2.bmp", "resources/blitz_again_3.bmp", "resources/blitz_again_4.bmp", "resources/blitz_again_5.bmp",
		"resources/blitz_again_6.bmp", "resources/blitz_again_7.bmp", "resources/blitz_again_8.bmp", "resources/blitz_again_7.bmp", "resources/blitz_again_6.bmp", "resources/blitz_again_5.bmp",
		"resources/blitz_again_4.bmp" , "resources/blitz_again_3.bmp" , "resources/blitz_again_2.bmp" });
	again[1].SetTopLeft(1394, 357);

	again[2].LoadBitmapByString({ "resources/custom_again_1.bmp", "resources/custom_again_2.bmp", "resources/custom_again_3.bmp", "resources/custom_again_4.bmp", "resources/custom_again_5.bmp",
	"resources/custom_again_6.bmp", "resources/custom_again_7.bmp", "resources/custom_again_8.bmp", "resources/custom_again_7.bmp", "resources/custom_again_6.bmp", "resources/custom_again_5.bmp",
	"resources/custom_again_4.bmp" , "resources/custom_again_3.bmp" , "resources/custom_again_2.bmp" });
	again[2].SetTopLeft(1394, 357);

	for (int i = 0; i < 4; i++)
	{
		fourtyl_menu_check[i].LoadBitmapByString({ "resources/check_transparent.bmp" }, RGB(255, 255, 255));
		fourtyl_menu_check[i].LoadBitmapByString({ "resources/40l_check.bmp" });
		fourtyl_menu_check[i].SetTopLeft(290, 457 + i * 36);
	}

	start[0].LoadBitmapByString({ "resources/40l_start_1.bmp", "resources/40l_start_2.bmp", "resources/40l_start_3.bmp", "resources/40l_start_4.bmp", "resources/40l_start_5.bmp",
		"resources/40l_start_6.bmp", "resources/40l_start_7.bmp", "resources/40l_start_8.bmp", "resources/40l_start_7.bmp", "resources/40l_start_6.bmp", "resources/40l_start_5.bmp",
		"resources/40l_start_4.bmp", "resources/40l_start_3.bmp", "resources/40l_start_2.bmp", });
	start[0].SetTopLeft(1391, 288);

	blitz_menu[0].LoadBitmapByString({ "resources/blitz_information.bmp" });
	blitz_menu[0].SetTopLeft(270, 100);

	blitz_menu[1].LoadBitmapByString({ "resources/blitz_blank.bmp" });
	blitz_menu[1].SetTopLeft(270, 284);

	blitz_menu[2].LoadBitmapByString({ "resources/blitz_options.bmp" });
	blitz_menu[2].SetTopLeft(270, 388);

	blitz_menu[3].LoadBitmapByString({ "resources/blitz_advanced.bmp" });
	blitz_menu[3].SetTopLeft(270, 638);

	blitz_end_menu[0].LoadBitmapByString({ "resources/blitz_final_score.bmp" });
	blitz_end_menu[0].SetTopLeft(270, 100);

	blitz_end_menu[1].LoadBitmapByString({ "resources/blitz_pure.bmp" });
	blitz_end_menu[1].SetTopLeft(270, 358);

	blitz_end_menu[2].LoadBitmapByString({ "resources/blitz_model.bmp" });
	blitz_end_menu[2].SetTopLeft(0, 957);

	for (int i = 0; i < 4; i++)
	{
		blitz_menu_check[i].LoadBitmapByString({ "resources/check_transparent.bmp" }, RGB(255, 255, 255));
		blitz_menu_check[i].LoadBitmapByString({ "resources/blitz_check.bmp" });
		blitz_menu_check[i].SetTopLeft(290, 457 + i * 36);
	}

	start[1].LoadBitmapByString({ "resources/blitz_start_1.bmp", "resources/blitz_start_2.bmp", "resources/blitz_start_3.bmp", "resources/blitz_start_4.bmp", "resources/blitz_start_5.bmp",
		"resources/blitz_start_6.bmp", "resources/blitz_start_7.bmp", "resources/blitz_start_8.bmp", "resources/blitz_start_7.bmp", "resources/blitz_start_6.bmp", "resources/blitz_start_5.bmp",
		"resources/blitz_start_4.bmp", "resources/blitz_start_3.bmp", "resources/blitz_start_2.bmp" });
	start[1].SetTopLeft(1391, 290);

	zen_menu[0].LoadBitmapByString({ "resources/zen_information.bmp" });
	zen_menu[0].SetTopLeft(270, 100);

	zen_menu[1].LoadBitmapByString({ "resources/zen_blank.bmp" });
	zen_menu[1].SetTopLeft(270, 284);

	zen_mode.LoadBitmapByString({ "resources/zen_mode.bmp" }, RGB(0, 0, 255));
	zen_mode.SetTopLeft(1800, 0);

	start[2].LoadBitmapByString({ "resources/zen_start_1.bmp", "resources/zen_start_2.bmp", "resources/zen_start_3.bmp", "resources/zen_start_4.bmp", "resources/zen_start_5.bmp",
		"resources/zen_start_6.bmp", "resources/zen_start_7.bmp", "resources/zen_start_8.bmp", "resources/zen_start_7.bmp", "resources/zen_start_6.bmp", "resources/zen_start_5.bmp",
		"resources/zen_start_4.bmp", "resources/zen_start_3.bmp", "resources/zen_start_2.bmp" });
	start[2].SetTopLeft(1395, 285);

	custom_menu[0].LoadBitmapByString({ "resources/custom_information.bmp" });
	custom_menu[0].SetTopLeft(270, 100);

	custom_menu[1].LoadBitmapByString({ "resources/custom_music.bmp" });
	custom_menu[1].SetTopLeft(270, 236);

	custom_control_menu.LoadBitmapByString({ "resources/board.bmp" });
	custom_control_menu.SetTopLeft(270, 321);

	custom_end_menu[0].LoadBitmapByString({ "resources/custom_final_score.bmp" });
	custom_end_menu[0].SetTopLeft(270, 100);

	custom_end_menu[1].LoadBitmapByString({ "resources/custom_music.bmp" });
	custom_end_menu[1].SetTopLeft(270, 358);

	custom_end_menu[2].LoadBitmapByString({ "resources/custom_model.bmp" });
	custom_end_menu[2].SetTopLeft(0, 957);

	board_width_control[0].LoadBitmapByString({ "resources/up_1.bmp", "resources/up_2.bmp" });
	board_width_control[0].SetTopLeft(1608, 326);

	board_width_control[1].LoadBitmapByString({ "resources/down_1.bmp", "resources/down_2.bmp" });
	board_width_control[1].SetTopLeft(1608, 335);

	board_height_control[0].LoadBitmapByString({ "resources/up_1.bmp", "resources/up_2.bmp" });
	board_height_control[0].SetTopLeft(1608, 361);

	board_height_control[1].LoadBitmapByString({ "resources/down_1.bmp", "resources/down_2.bmp" });
	board_height_control[1].SetTopLeft(1608, 370);


	start[3].LoadBitmapByString({ "resources/custom_start_1.bmp", "resources/custom_start_2.bmp", "resources/custom_start_3.bmp", "resources/custom_start_4.bmp", "resources/custom_start_5.bmp",
	"resources/custom_start_6.bmp", "resources/custom_start_7.bmp", "resources/custom_start_8.bmp", "resources/custom_start_7.bmp", "resources/custom_start_6.bmp", "resources/custom_start_5.bmp",
	"resources/custom_start_4.bmp", "resources/custom_start_3.bmp", "resources/custom_start_2.bmp" });
	start[3].SetTopLeft(1394, 237);

	cube_hold_border.LoadBitmapByString({ "resources/cube_hold_border.bmp",  "resources/cube_hold_border_red.bmp"}, RGB(0, 0, 255));
	cube_next_border.LoadBitmapByString({ "resources/cube_next_border.bmp", "resources/cube_next_border_red.bmp" }, RGB(0, 0, 255));

	cube_staight_border.LoadBitmapByString({ "resources/cube_straight_border.bmp", "resources/cube_straight_border_red.bmp" });
	cube_horizontal_border.LoadBitmapByString({ "resources/cube_horizontal_border.bmp", "resources/cube_horizontal_border_red.bmp"});
	lines_graph_border.LoadBitmapByString({ "resources/lines_graph_border.bmp", "resources/lines_graph_border_red.bmp" });

	lines_graph_body.LoadBitmapByString({ "resources/lines_graph_body.bmp" });

	lines_graph_top.LoadBitmapByString({ "resources/lines_graph_top.bmp" });

	next_cubes = vector<vector<CMovingBitmap>>(NEXT_CUBE_CANVAS_HEIGHT, vector<CMovingBitmap>(NEXT_CUBE_CANVAS_WIDTH, Cube()));
	hold_cubes = vector<vector<CMovingBitmap>>(HOLD_CUBE_CANVAS_HEIGHT, vector<CMovingBitmap>(HOLD_CUBE_CANVAS_WIDTH, Cube()));

	fire[0].LoadBitmapByString({ "resources/fire_1_lt.bmp", "resources/fire_2_lt.bmp", "resources/fire_3_lt.bmp", "resources/fire_4_lt.bmp", "resources/fire_5_lt.bmp",
		"resources/fire_6_lt.bmp", "resources/fire_7_lt.bmp", "resources/fire_8_lt.bmp", "resources/fire_9_lt.bmp", "resources/fire_10_lt.bmp", "resources/fire_11_lt.bmp",
		"resources/fire_12_lt.bmp", "resources/fire_13_lt.bmp", "resources/fire_14_lt.bmp", "resources/fire_15_lt.bmp", "resources/fire_16_lt.bmp", "resources/fire_17_lt.bmp",
		"resources/fire_18_lt.bmp", "resources/fire_19_lt.bmp", "resources/fire_20_lt.bmp", "resources/fire_21_lt.bmp", "resources/fire_22_lt.bmp", "resources/fire_23_lt.bmp",
		"resources/fire_24_lt.bmp", "resources/fire_25_lt.bmp", "resources/fire_26_lt.bmp", "resources/fire_27_lt.bmp", "resources/fire_28_lt.bmp", "resources/fire_29_lt.bmp",
		"resources/fire_30_lt.bmp", "resources/fire_31_lt.bmp", "resources/fire_32_lt.bmp", "resources/fire_33_lt.bmp", "resources/fire_34_lt.bmp", "resources/fire_35_lt.bmp",
		"resources/fire_36_lt.bmp", "resources/fire_37_lt.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[0].SetAnimation(16, true);

	fire[1].LoadBitmapByString({ "resources/fire_1_rt.bmp", "resources/fire_2_rt.bmp", "resources/fire_3_rt.bmp", "resources/fire_4_rt.bmp", "resources/fire_5_rt.bmp",
	"resources/fire_6_rt.bmp", "resources/fire_7_rt.bmp", "resources/fire_8_rt.bmp", "resources/fire_9_rt.bmp", "resources/fire_10_rt.bmp", "resources/fire_11_lt.bmp",
	"resources/fire_12_rt.bmp", "resources/fire_13_rt.bmp", "resources/fire_14_rt.bmp", "resources/fire_15_rt.bmp", "resources/fire_16_rt.bmp", "resources/fire_17_rt.bmp",
	"resources/fire_18_rt.bmp", "resources/fire_19_rt.bmp", "resources/fire_20_rt.bmp", "resources/fire_21_rt.bmp", "resources/fire_22_rt.bmp", "resources/fire_23_lt.bmp",
	"resources/fire_24_rt.bmp", "resources/fire_25_rt.bmp", "resources/fire_26_rt.bmp", "resources/fire_27_rt.bmp", "resources/fire_28_rt.bmp", "resources/fire_29_lt.bmp",
	"resources/fire_30_rt.bmp", "resources/fire_31_rt.bmp", "resources/fire_32_rt.bmp", "resources/fire_33_rt.bmp", "resources/fire_34_rt.bmp", "resources/fire_35_lt.bmp",
	"resources/fire_36_rt.bmp", "resources/fire_37_rt.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[1].SetAnimation(16, true);

	fire[2].LoadBitmapByString({ "resources/fire_1_lb.bmp", "resources/fire_2_lb.bmp", "resources/fire_3_lb.bmp", "resources/fire_4_lb.bmp", "resources/fire_5_lb.bmp",
	"resources/fire_6_lb.bmp", "resources/fire_7_lb.bmp", "resources/fire_8_lb.bmp", "resources/fire_9_lb.bmp", "resources/fire_10_lb.bmp", "resources/fire_11_lb.bmp",
	"resources/fire_12_lb.bmp", "resources/fire_13_lb.bmp", "resources/fire_14_lb.bmp", "resources/fire_15_lb.bmp", "resources/fire_16_lb.bmp", "resources/fire_17_lb.bmp",
	"resources/fire_18_lb.bmp", "resources/fire_19_lb.bmp", "resources/fire_20_lb.bmp", "resources/fire_21_lb.bmp", "resources/fire_22_lb.bmp", "resources/fire_23_lb.bmp",
	"resources/fire_24_lb.bmp", "resources/fire_25_lb.bmp", "resources/fire_26_lb.bmp", "resources/fire_27_lb.bmp", "resources/fire_28_lb.bmp", "resources/fire_29_lb.bmp",
	"resources/fire_30_lb.bmp", "resources/fire_31_lb.bmp", "resources/fire_32_lb.bmp", "resources/fire_33_lb.bmp", "resources/fire_34_lb.bmp", "resources/fire_35_lb.bmp",
	"resources/fire_36_lb.bmp", "resources/fire_37_lb.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[2].SetAnimation(16, true);

	fire[3].LoadBitmapByString({ "resources/fire_1_rb.bmp", "resources/fire_2_rb.bmp", "resources/fire_3_rb.bmp", "resources/fire_4_rb.bmp", "resources/fire_5_rb.bmp",
	"resources/fire_6_rb.bmp", "resources/fire_7_rb.bmp", "resources/fire_8_rb.bmp", "resources/fire_9_rb.bmp", "resources/fire_10_rb.bmp", "resources/fire_11_rb.bmp",
	"resources/fire_12_rb.bmp", "resources/fire_13_rb.bmp", "resources/fire_14_rb.bmp", "resources/fire_15_rb.bmp", "resources/fire_16_rb.bmp", "resources/fire_17_rb.bmp",
	"resources/fire_18_rb.bmp", "resources/fire_19_rb.bmp", "resources/fire_20_rb.bmp", "resources/fire_21_rb.bmp", "resources/fire_22_rb.bmp", "resources/fire_23_rb.bmp",
	"resources/fire_24_rb.bmp", "resources/fire_25_rb.bmp", "resources/fire_26_rb.bmp", "resources/fire_27_rb.bmp", "resources/fire_28_rb.bmp", "resources/fire_29_rb.bmp",
	"resources/fire_30_rb.bmp", "resources/fire_31_rb.bmp", "resources/fire_32_rb.bmp", "resources/fire_33_rb.bmp", "resources/fire_34_rb.bmp", "resources/fire_35_rb.bmp",
	"resources/fire_36_rb.bmp", "resources/fire_37_rb.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[3].SetAnimation(16, true);

	fire_animation_check = true;

	level_up_scene.LoadBitmapByString({ "resources/level_up_cutscene_1.bmp", "resources/level_up_cutscene_2.bmp", "resources/level_up_cutscene_3.bmp", "resources/level_up_cutscene_4.bmp",
	"resources/level_up_cutscene_5.bmp", "resources/level_up_cutscene_6.bmp", "resources/level_up_cutscene_7.bmp", "resources/level_up_cutscene_8.bmp",
	"resources/level_up_cutscene_9.bmp", "resources/level_up_cutscene_10.bmp", "resources/level_up_cutscene_11.bmp", "resources/level_up_cutscene_12.bmp",
	"resources/level_up_cutscene_13.bmp", "resources/level_up_cutscene_14.bmp", "resources/level_up_cutscene_15.bmp", "resources/level_up_cutscene_16.bmp",
	"resources/level_up_cutscene_17.bmp", "resources/level_up_cutscene_18.bmp", "resources/level_up_cutscene_19.bmp", "resources/level_up_cutscene_20.bmp",
	"resources/level_up_cutscene_21.bmp", "resources/level_up_cutscene_22.bmp", "resources/level_up_cutscene_23.bmp", "resources/level_up_cutscene_24.bmp",
	"resources/level_up_cutscene_25.bmp", "resources/level_up_cutscene_26.bmp", "resources/level_up_cutscene_27.bmp", "resources/level_up_cutscene_28.bmp",
	"resources/level_up_cutscene_29.bmp", "resources/level_up_cutscene_30.bmp", "resources/level_up_cutscene_31.bmp", "resources/level_up_cutscene_32.bmp",
	"resources/level_up_cutscene_33.bmp", "resources/level_up_cutscene_34.bmp", "resources/level_up_cutscene_35.bmp", "resources/level_up_cutscene_36.bmp",
	"resources/level_up_cutscene_37.bmp", "resources/level_up_cutscene_38.bmp", "resources/level_up_cutscene_39.bmp", "resources/level_up_cutscene_40.bmp",
	"resources/level_up_cutscene_41.bmp", "resources/level_up_cutscene_42.bmp", "resources/level_up_cutscene_43.bmp", "resources/level_up_cutscene_44.bmp",
	"resources/level_up_cutscene_45.bmp", "resources/level_up_cutscene_46.bmp", "resources/level_up_cutscene_47.bmp", "resources/level_up_cutscene_48.bmp",
	"resources/level_up_cutscene_49.bmp", "resources/level_up_cutscene_50.bmp", "resources/level_up_cutscene_51.bmp", "resources/level_up_cutscene_52.bmp",
	"resources/level_up_cutscene_53.bmp", "resources/level_up_cutscene_54.bmp", "resources/level_up_cutscene_55.bmp", "resources/level_up_cutscene_56.bmp",
	"resources/level_up_cutscene_57.bmp", "resources/level_up_cutscene_58.bmp", "resources/level_up_cutscene_59.bmp", "resources/level_up_cutscene_60.bmp",
	"resources/level_up_cutscene_61.bmp", "resources/level_up_cutscene_62.bmp", "resources/level_up_cutscene_63.bmp", "resources/level_up_cutscene_64.bmp",
	"resources/level_up_cutscene_65.bmp", "resources/level_up_cutscene_66.bmp", "resources/level_up_cutscene_67.bmp", "resources/level_up_cutscene_68.bmp",
	"resources/level_up_cutscene_69.bmp", "resources/level_up_cutscene_70.bmp", "resources/level_up_cutscene_71.bmp", "resources/level_up_cutscene_72.bmp",
	"resources/level_up_cutscene_73.bmp", "resources/level_up_cutscene_74.bmp", "resources/level_up_cutscene_75.bmp", "resources/level_up_cutscene_76.bmp",
	"resources/level_up_cutscene_77.bmp", "resources/level_up_cutscene_78.bmp", "resources/level_up_cutscene_79.bmp", "resources/level_up_cutscene_80.bmp",
	"resources/level_up_cutscene_81.bmp", "resources/level_up_cutscene_82.bmp", "resources/level_up_cutscene_83.bmp", "resources/level_up_cutscene_84.bmp",
	"resources/level_up_cutscene_85.bmp", "resources/level_up_cutscene_86.bmp", "resources/level_up_cutscene_87.bmp", "resources/level_up_cutscene_88.bmp",
	"resources/level_up_cutscene_89.bmp", "resources/level_up_cutscene_90.bmp", "resources/level_up_cutscene_91.bmp", "resources/level_up_cutscene_92.bmp",
	"resources/level_up_cutscene_93.bmp", "resources/level_up_cutscene_94.bmp", "resources/level_up_cutscene_95.bmp", "resources/level_up_cutscene_96.bmp",
	"resources/level_up_cutscene_97.bmp", "resources/level_up_cutscene_98.bmp", "resources/level_up_cutscene_99.bmp", "resources/level_up_cutscene_100.bmp",
	"resources/level_up_cutscene_101.bmp", "resources/level_up_cutscene_102.bmp", "resources/level_up_cutscene_103.bmp", "resources/level_up_cutscene_104.bmp",
	"resources/level_up_cutscene_105.bmp", "resources/level_up_cutscene_106.bmp", "resources/level_up_cutscene_107.bmp", "resources/level_up_cutscene_108.bmp",
	"resources/level_up_cutscene_109.bmp", "resources/level_up_cutscene_110.bmp", "resources/level_up_cutscene_111.bmp", "resources/level_up_cutscene_112.bmp",
	"resources/level_up_cutscene_113.bmp", "resources/level_up_cutscene_114.bmp", "resources/level_up_cutscene_115.bmp", "resources/level_up_cutscene_116.bmp",
	"resources/level_up_cutscene_117.bmp", "resources/level_up_cutscene_118.bmp", "resources/level_up_cutscene_119.bmp", "resources/level_up_cutscene_120.bmp",
	"resources/level_up_cutscene_121.bmp", "resources/level_up_cutscene_122.bmp", "resources/level_up_cutscene_123.bmp", "resources/level_up_cutscene_124.bmp", });
	level_up_scene.LoadBitmapByString({ "resources/level_up_cutscene_transparent.bmp" }, RGB(255, 255, 255));
	level_up_scene.SetFrameIndexOfBitmap(124);
	level_up_scene.SetTopLeft(0, 0);
	level_up_scene.SetAnimation(16, true);

	exit_scene.LoadBitmapByString({ "resources/exit_animation_1.bmp", "resources/exit_animation_2.bmp", "resources/exit_animation_3.bmp", "resources/exit_animation_4.bmp",
	"resources/exit_animation_5.bmp", "resources/exit_animation_6.bmp", "resources/exit_animation_7.bmp", "resources/exit_animation_8.bmp",
	"resources/exit_animation_9.bmp", "resources/exit_animation_10.bmp", "resources/exit_animation_11.bmp", "resources/exit_animation_12.bmp",
	"resources/exit_animation_13.bmp", "resources/exit_animation_14.bmp", "resources/exit_animation_15.bmp", "resources/exit_animation_16.bmp",
	"resources/exit_animation_17.bmp", "resources/exit_animation_18.bmp", "resources/exit_animation_19.bmp", "resources/exit_animation_20.bmp",
	"resources/exit_animation_21.bmp", "resources/exit_animation_22.bmp", "resources/exit_animation_23.bmp", "resources/exit_animation_24.bmp",
	"resources/exit_animation_25.bmp", "resources/exit_animation_26.bmp", "resources/exit_animation_27.bmp", "resources/exit_animation_28.bmp",
	"resources/exit_animation_29.bmp", "resources/exit_animation_30.bmp", "resources/exit_animation_31.bmp", "resources/exit_animation_transparent.bmp" }, RGB(0, 0, 0));

	exit_scene.SetTopLeft(10, 910);
	exit_scene.SetFrameIndexOfBitmap(31);

	touch_option_menu_first = true;
	touch_option_menu_selected = false;

	game_decline_time_interval = 1000;
	game_move_time_interval = 140;
	left_key_down = false;
	right_key_down = false;
	down_key_down = false;

	retry.LoadBitmapByString({ "resources/retry.bmp", "resources/retry_selected.bmp" });
	retry.SetTopLeft(1921, 80);
	retry_selected = false;

	back_to_tittle.LoadBitmapByString({ "resources/back_to_tittle.bmp", "resources/back_to_tittle_selected.bmp" });
	back_to_tittle.SetTopLeft(1921, 200);
	back_to_tittle_selected = false;

	fire_animation_check = true;

	single_scene.LoadBitmapByString({ "resources/single_1.bmp", "resources/single_2.bmp", "resources/single_3.bmp" , "resources/single_4.bmp",
	"resources/single_5.bmp", "resources/single_6.bmp", "resources/single_7.bmp" , "resources/single_8.bmp",  "resources/single_9.bmp",
	"resources/single_10.bmp", "resources/single_11.bmp", "resources/single_12.bmp",  "resources/single_transparent.bmp" }, RGB(0, 0, 0));
	single_scene.SetTopLeft(520, 375);
	single_scene.SetAnimation(60, true);

	double_scene.LoadBitmapByString({ "resources/double_1.bmp", "resources/double_2.bmp", "resources/double_3.bmp" , "resources/double_4.bmp",
	"resources/double_5.bmp", "resources/double_6.bmp", "resources/double_7.bmp" , "resources/double_8.bmp",  "resources/double_9.bmp",
	"resources/double_10.bmp", "resources/double_11.bmp", "resources/double_12.bmp", "resources/double_transparent.bmp"  }, RGB(0, 0, 0));
	double_scene.SetTopLeft(450, 395);
	double_scene.SetAnimation(60, true);

	triple_scene.LoadBitmapByString({ "resources/triple_1.bmp", "resources/triple_2.bmp", "resources/triple_3.bmp" , "resources/triple_4.bmp",
	"resources/triple_5.bmp", "resources/triple_6.bmp", "resources/triple_7.bmp" , "resources/triple_8.bmp",  "resources/triple_9.bmp",
	"resources/triple_10.bmp", "resources/triple_11.bmp", "resources/triple_12.bmp", "resources/triple_transparent.bmp"  }, RGB(0, 0, 0));
	triple_scene.SetTopLeft(480, 388);
	triple_scene.SetAnimation(60, true);

	quad_scene.LoadBitmapByString({ "resources/quad_1.bmp", "resources/quad_2.bmp", "resources/quad_3.bmp" , "resources/quad_4.bmp",
	"resources/quad_5.bmp", "resources/quad_6.bmp", "resources/quad_7.bmp" , "resources/quad_8.bmp",  "resources/quad_9.bmp",
	"resources/quad_10.bmp", "resources/quad_11.bmp", "resources/quad_12.bmp", "resources/quad_transparent.bmp"  }, RGB(0, 0, 0));
	quad_scene.SetTopLeft(548, 383);
	quad_scene.SetAnimation(60, true);

	blitz_level_to_speed = { {1, 1000}, {2, 643} , {3, 404}, {4, 249},
	{5, 111}, {6, 88}, {7, 50}, {8, 28}, {9, 15}, {10, 8}, {11, 4},
	{12, 2}, {13, 1}, {14, 1}, {15, 1} };

	blitz_level_to_lines = { {1, 3}, {2, 5}, {3, 7}, {4, 9}, {5, 11}, {6, 13},{7, 15},
	{8, 17}, {9, 19}, {10, 21}, {11, 24}, {12, 26}, {13, 28}, {14, 30}, {15, 32} };

}

void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (phase >= 3)
	{
		if (sub_phase == 2)
		{
			if (!tetris_game.game_over)
			{
				if (nChar == VK_UP)
				{
					music->Play(AUDIO_ID::Cube_Rotate);
					game_update(Event::rotate);
				}
				else if (nChar == VK_DOWN)
				{
					game_next_move_time = clock();
					down_key_down = true;
					game_move_time_interval = 110;
				}
				else if (nChar == VK_LEFT)
				{
					game_next_move_time = clock();
					left_key_down = true;
					right_key_down = false;
					game_move_time_interval = 110;
				}
				else if (nChar == VK_RIGHT)
				{
					game_next_move_time = clock();
					left_key_down = false;
					right_key_down = true;
					game_move_time_interval = 110;
				}
				else if (nChar == VK_SPACE)
				{
					music->Play(AUDIO_ID::Cube_Touch_Bottom);
					game_update(Event::down);
					game_update(Event::tick);
				}
				else if (nChar == 0x43)
				{
					music->Play(AUDIO_ID::Cube_Switch);
					game_update(Event::hold);
				}
				else if (nChar == VK_ESCAPE)
				{
					exit_scene.SetFrameIndexOfBitmap(0);
					exit_check = true;
				}
			}
		}
	}
}

void CGameStateRun::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (phase >= 3)
	{
		if (sub_phase == 2)
		{
			if (nChar == VK_LEFT)
			{
				left_key_down = false;
			}
			else if (nChar == VK_RIGHT)
			{
				right_key_down = false;
			}
			else if (nChar == VK_DOWN)
			{
				down_key_down = false;
			}
			else if (nChar == VK_ESCAPE)
			{
				exit_check = false;
			}
		}
	}
}

void CGameStateRun::OnLButtonDown(UINT nFlags, CPoint point)  // 處理滑鼠的動作
{
	if (phase == 1)
	{
		if (click_check(nFlags, point, join))
		{
			join.SetFrameIndexOfBitmap(1);
			sub_phase += 1;
		}
	}
	else if (phase == 2)
	{
		if (click_check(nFlags, point, back))
		{
			music->Play(AUDIO_ID::Back_Menu);
			tittle.SetFrameIndexOfBitmap(0);
			back.SetFrameIndexOfBitmap(0);
			back_selected = false;
			back.SetTopLeft(-40, 80);
			phase = 1;
			sub_phase = 1;
		}
		for (int i = 0; i < 4; i++)
		{
			if (click_check(nFlags, point, second_menu[i]))
			{
				music->Play(AUDIO_ID::Click_Menu);
				tittle.SetFrameIndexOfBitmap(i + 2);
				game_mode.SetFrameIndexOfBitmap(i + 1);
				id = i;
				start[i].SetAnimation(60, false);
				phase = 3 + i;
				sub_phase = 1;
			}
		}
	}
	else if (phase == 3)
	{
		if (sub_phase == 1)
		{
			if (click_check(nFlags, point, back))
			{
				music->Play(AUDIO_ID::Back_Menu);
				tittle.SetFrameIndexOfBitmap(1);
				back.SetFrameIndexOfBitmap(0);
				start[id].SetAnimation(60, true);
				game_mode.SetFrameIndexOfBitmap(0);
				phase = 2;
			}
			if (click_check(nFlags, point, start[0]))
			{
				background_music = rand() % 6;
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(AUDIO_ID::Arial_City);
				music->Play(background_music, true);
				set_canvas(20, 10);
				game_init();
				sub_phase = 2;
			}
			for (int i = 0; i < 4; i++)
			{
				if (click_check(nFlags, point, fourtyl_menu_check[i]))
				{
					music->Play(AUDIO_ID::Click_Check_Menu);
					if (fourtyl_menu_check[i].GetFrameIndexOfBitmap() == 0)
					{
						fourtyl_menu_check[i].SetFrameIndexOfBitmap(1);
					}
					else
					{
						fourtyl_menu_check[i].SetFrameIndexOfBitmap(0);
					}
				}
			}
		}
		else if (sub_phase == 3)
		{
			fail_game_menu_click(nFlags, point, GameType::fourtyl);
		}
		else if (sub_phase == 4)
		{
			if (click_check(nFlags, point, again[0]))
			{
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(background_music);
				background_music = rand() % 6;
				music->Play(background_music, true);
				again[0].SetAnimation(60, true);
				game_init();
				sub_phase = 2;
			}
			if (click_check(nFlags, point, back))
			{
				music->Stop(background_music);
				music->Play(AUDIO_ID::Back_Menu);
				music->Play(AUDIO_ID::Arial_City, true);
				again[0].SetAnimation(60, true);
				tittle.SetFrameIndexOfBitmap(1);
				back.SetFrameIndexOfBitmap(0);
				start[id].SetAnimation(60, true);
				game_mode.SetFrameIndexOfBitmap(0);
				phase = 2;
				sub_phase = 1;
			}
		}
	}
	else if (phase == 4)
	{
		if (sub_phase == 1)
		{
			if (click_check(nFlags, point, back))
			{
				music->Play(AUDIO_ID::Back_Menu);
				tittle.SetFrameIndexOfBitmap(1);
				back.SetFrameIndexOfBitmap(0);
				start[id].SetAnimation(60, true);
				game_mode.SetFrameIndexOfBitmap(0);
				phase = 2;
			}
			if (click_check(nFlags, point, start[1]))
			{
				background_music = AUDIO_ID::Hyper_Velocity;
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(AUDIO_ID::Arial_City);
				music->Play(background_music);
				set_canvas(20, 10);
				game_init();
				sub_phase = 2;
			}
			for (int i = 0; i < 4; i++)
			{
				if (click_check(nFlags, point, blitz_menu_check[i]))
				{
					music->Play(AUDIO_ID::Click_Check_Menu);
					if (blitz_menu_check[i].GetFrameIndexOfBitmap() == 0)
					{
						blitz_menu_check[i].SetFrameIndexOfBitmap(1);
					}
					else
					{
						blitz_menu_check[i].SetFrameIndexOfBitmap(0);
					}
				}
			}
		}
		else if (sub_phase == 3)
		{
			fail_game_menu_click(nFlags, point, GameType::blitz);
		}
		else if (sub_phase == 4)
		{
			if (click_check(nFlags, point, again[1]))
			{
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(background_music);
				background_music = AUDIO_ID::Hyper_Velocity;
				music->Play(background_music);
				again[1].SetAnimation(60, true);
				game_init();
				sub_phase = 2;
			}
			if (click_check(nFlags, point, back))
			{
				music->Stop(background_music);
				music->Play(AUDIO_ID::Back_Menu);
				music->Play(AUDIO_ID::Arial_City, true);
				again[1].SetAnimation(60, true);
				tittle.SetFrameIndexOfBitmap(1);
				back.SetFrameIndexOfBitmap(0);
				start[id].SetAnimation(60, true);
				game_mode.SetFrameIndexOfBitmap(0);
				phase = 2;
				sub_phase = 1;
			}
		}
	}
	else if (phase == 5)
	{
		if (sub_phase == 1)
		{
			if (click_check(nFlags, point, back))
			{
				music->Play(AUDIO_ID::Back_Menu);
				tittle.SetFrameIndexOfBitmap(1);
				back.SetFrameIndexOfBitmap(0);
				start[id].SetAnimation(60, true);
				game_mode.SetFrameIndexOfBitmap(0);
				phase = 2;
				sub_phase = 1;
			}
			if (click_check(nFlags, point, start[2]))
			{
				background_music = rand() % 6;
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(AUDIO_ID::Arial_City);
				music->Play(background_music, true);
				background.SetFrameIndexOfBitmap(rand() % 6);
				game_next_decline_time = clock();
				game_next_move_time = clock();
				set_canvas(20, 10);
				game_init();
				tetris_game = save_tetris_game;
				sub_phase = 2;
			}
		}
	}
	else if (phase == 6)
	{
		if (sub_phase == 1)
		{
			if (click_check(nFlags, point, board_width_control[0]))
			{
				board_width = board_width >= 30 ? board_width : board_width + 1;
			}
			else if (click_check(nFlags, point, board_width_control[1]))
			{
				board_width = board_width <= 6 ? board_width : board_width - 1;
			}
			if (click_check(nFlags, point, board_height_control[0]))
			{
				board_height = board_height >= 30 ? board_height : board_height + 1;
			}
			else if (click_check(nFlags, point, board_height_control[1]))
			{
				board_height = board_height <= 16 ? board_height : board_height - 1;
			}
			if (click_check(nFlags, point, back))
			{
				music->Play(AUDIO_ID::Back_Menu);
				tittle.SetFrameIndexOfBitmap(1);
				back.SetFrameIndexOfBitmap(0);
				start[id].SetAnimation(60, true);
				game_mode.SetFrameIndexOfBitmap(0);
				phase = 2;
			}
			if (click_check(nFlags, point, start[3]))
			{
				background_music = rand() % 6;
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(AUDIO_ID::Arial_City);
				music->Play(background_music, true);
				set_canvas(board_height, board_width);
				game_init();
				sub_phase = 2;
			}
		}
		else if (sub_phase == 3)
		{
			if (click_check(nFlags, point, back))
			{
				music->Stop(background_music);
				music->Play(AUDIO_ID::Back_Menu);
				music->Play(AUDIO_ID::Arial_City, true);
				again[2].SetAnimation(60, true);
				tittle.SetFrameIndexOfBitmap(1);
				back.SetFrameIndexOfBitmap(0);
				start[id].SetAnimation(60, true);
				game_mode.SetFrameIndexOfBitmap(0);
				phase = 2;
				sub_phase = 1;
			}
			if (click_check(nFlags, point, again[2]))
			{
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(background_music);
				background_music = rand() % 6;
				music->Play(background_music, true);
				again[2].SetAnimation(60, true);
				game_init();
				sub_phase = 2;
			}
		}
	}
}

void CGameStateRun::OnLButtonUp(UINT nFlags, CPoint point)	// 處理滑鼠的動作
{
}

void CGameStateRun::OnMouseMove(UINT nFlags, CPoint point)	// 處理滑鼠的動作
{
	if (phase == 2)
	{
		for (int i = 0; i < 4; i++)
		{
			if (touch_check(point, second_menu[i]) && !second_menu_selected[i])
			{
				second_menu[i].SetFrameIndexOfBitmap(1);
				second_menu_selected[i] = true;
			}
			else if (!touch_check(point, second_menu[i]) && second_menu_selected[i])
			{
				second_menu[i].SetFrameIndexOfBitmap(0);
				second_menu_selected[i] = false;
			}
		}
	}
	if (phase >= 2)
	{
		if (touch_check(point, back))
		{
			back.SetFrameIndexOfBitmap(1);
			back_selected = true;
		}
		else
		{
			back.SetFrameIndexOfBitmap(0);
			back_selected = false;
		}
	}
	if (phase == 3)
	{
		if (sub_phase == 1)
		{
			touch_option_menu(point);
		}
		else if (sub_phase == 3)
		{
			fail_game_menu_touch(point);
		}
	}
	if (phase == 4)
	{
		if (sub_phase == 1)
		{
			touch_option_menu(point);
		}
		else if (sub_phase == 3)
		{
			fail_game_menu_touch(point);
		}
	}
	if (phase == 6)
	{
		for (int i = 0; i < 2; i++)
		{
			if (touch_check(point, board_width_control[i]) && !board_width_control_selected[i])
			{
				board_width_control[i].SetFrameIndexOfBitmap(1);
				board_width_control_selected[i] = true;
			}
			else if (!touch_check(point, board_width_control[i]) && board_width_control_selected[i])
			{
				board_width_control[i].SetFrameIndexOfBitmap(0);
				board_width_control_selected[i] = false;
			}
			if (touch_check(point, board_height_control[i]) && !board_height_control_selected[i])
			{
				board_height_control[i].SetFrameIndexOfBitmap(1);
				board_height_control_selected[i] = true;
			}
			else if (!touch_check(point, board_height_control[i]) && board_height_control_selected[i])
			{
				board_height_control[i].SetFrameIndexOfBitmap(0);
				board_height_control_selected[i] = false;
			}
		}
	}
}

void CGameStateRun::OnRButtonDown(UINT nFlags, CPoint point)  // 處理滑鼠的動作
{
}

void CGameStateRun::OnRButtonUp(UINT nFlags, CPoint point)	// 處理滑鼠的動作
{
}

void CGameStateRun::OnShow()
{
	if (phase == 1)
	{
		tittle.ShowBitmap();

		first_menu.ShowBitmap();
		join.ShowBitmap();

		osk.ShowBitmap();

		if (sub_phase == 2)
		{
			sub_phase += 1;
			record_current_time = clock() + 300;
		}
		else if (sub_phase == 3)
		{
			if (clock() > record_current_time)
			{
				join.SetFrameIndexOfBitmap(0);
				tittle.SetFrameIndexOfBitmap(1);
				phase += 1;
				sub_phase = 1;
			}
		}
	}
	else if (phase == 2)
	{
		background.ShowBitmap();
		tittle.ShowBitmap();
		back.ShowBitmap();
		logo.ShowBitmap();
		game_mode.ShowBitmap();
		for (int i = 0; i < 4; i++)
		{
			second_menu[i].ShowBitmap();
		}
	}
	else if (phase == 3)
	{
		if (sub_phase == 1)
		{
			tittle.ShowBitmap();
			game_mode.ShowBitmap();
			back.ShowBitmap();

			for (int i = 0; i < 4; i++)
			{
				fourtyl_menu[i].ShowBitmap();
			}

			for (int i = 0; i < 4; i++)
			{
				fourtyl_menu_check[i].ShowBitmap();
			}

			start[0].ShowBitmap();
		}
		else if (sub_phase == 2)
		{
			background.ShowBitmap();

			display_game();
			display_lines(40);
			display_play_passed_time();
			exit_scene.ShowBitmap();
			display_lines_graph(40);
			display_clear_lines_animation();

			if (tetris_game.game_over)
			{
				for_each(fire.begin(), fire.end(), [](CMovingBitmap& fire)
				{
					fire.ShowBitmap();
				});
			}

			if (tetris_game.game_success) {
				display_finish();
			}
		}
		else if (sub_phase == 3)
		{
			retry.ShowBitmap();
			back_to_tittle.ShowBitmap();
		}
		else if (sub_phase == 4)
		{
			tittle.ShowBitmap();

			for (unsigned i = 0; i < fourtyl_end_menu.size(); i++)
			{
				fourtyl_end_menu[i].ShowBitmap();
			}

			again[0].ShowBitmap();
			back.ShowBitmap();
			display_play_total_time();
			display_play_real_time(RGB(169, 117, 83));
		}
	}
	else if (phase == 4)
	{
		if (sub_phase == 1)
		{
			tittle.ShowBitmap();
			game_mode.ShowBitmap();
			back.ShowBitmap();

			for (int i = 0; i < 4; i++)
			{
				blitz_menu[i].ShowBitmap();
			}

			for (int i = 0; i < 4; i++)
			{
				blitz_menu_check[i].ShowBitmap();
			}

			start[1].ShowBitmap();
		}
		else if (sub_phase == 2)
		{
			background.ShowBitmap();

			display_game();
			display_lines(blitz_level_to_lines[tetris_game.level]);
			display_lines_graph(blitz_level_to_lines[tetris_game.level]);
			display_on_left_level();
			display_play_remaining_time();
			display_on_right_score();
			display_reciprocal_animation();
			exit_scene.ShowBitmap();
			display_clear_lines_animation();

			if (tetris_game.game_over)
			{
				for_each(fire.begin(), fire.end(), [](CMovingBitmap& fire)
				{
					fire.ShowBitmap();
				});
			}


			if (tetris_game.game_success) {
				CDC *pDC = CDDraw::GetBackCDC();

				CTextDraw::ChangeFontLog(pDC, 80, "微軟正黑體", RGB(238, 255, 26), 50);
				CTextDraw::Print(pDC, 841, 480, "Finish");

				CDDraw::ReleaseBackCDC();
			}
		}
		else if (sub_phase == 3)
		{
			retry.ShowBitmap();
			back_to_tittle.ShowBitmap();
		}
		else if (sub_phase == 4)
		{
			tittle.ShowBitmap();

			for (unsigned i = 0; i < fourtyl_end_menu.size(); i++)
			{
				blitz_end_menu[i].ShowBitmap();
			}

			again[1].ShowBitmap();
			back.ShowBitmap();
			display_play_total_score(RGB(243, 197, 197));
			display_play_real_time(RGB(224, 159, 159));
		}
	}
	else if (phase == 5)
	{
		if (sub_phase == 1)
		{
			tittle.ShowBitmap();
			game_mode.ShowBitmap();
			back.ShowBitmap();

			for (int i = 0; i < 2; i++)
			{
				zen_menu[i].ShowBitmap();
			}

			start[2].ShowBitmap();
		}
		else if (sub_phase >= 2)
		{
			background.ShowBitmap();
			zen_mode.ShowBitmap();

			display_game();
			display_on_button_score();
			display_on_button_level();
			display_lines_graph(20);
			exit_scene.ShowBitmap();
			display_clear_lines_animation();

			level_up_scene.ShowBitmap();
		}
	}
	else if (phase == 6)
	{
		if (sub_phase == 1)
		{
			tittle.ShowBitmap();
			game_mode.ShowBitmap();
			back.ShowBitmap();

			for (int i = 0; i < 2; i++)
			{
				custom_menu[i].ShowBitmap();
			}


			custom_control_menu.ShowBitmap();
			display_custom_control_menu();

			start[3].ShowBitmap();
		}
		else if (sub_phase == 2)
		{
			background.ShowBitmap();
			display_game();
			display_lines_graph(150);
			display_lines(150);
			display_on_left_level();
			display_play_passed_time();
			display_on_right_score();
			display_clear_lines_animation();
			if (tetris_game.game_over)
			{
				for_each(fire.begin(), fire.end(), [](CMovingBitmap& fire)
				{
					fire.ShowBitmap();
				});
			}

			exit_scene.ShowBitmap();
		}
		else if (sub_phase == 3)
		{
			tittle.ShowBitmap();
			for (unsigned int i = 0; i < custom_end_menu.size(); i++) { custom_end_menu[i].ShowBitmap(); };
			again[2].ShowBitmap();
			back.ShowBitmap();
			display_play_total_score(RGB(197, 179, 244));
			display_play_real_time(RGB(157, 151, 243));
		}
	}
}
