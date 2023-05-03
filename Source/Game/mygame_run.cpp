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

void TetrisGame::remove_and_prepend_rows() {
	for (unsigned row_index = 0; row_index < canvas.size(); row_index++) {
		vector<Color> row = canvas[row_index];
		if (all_of(row.begin(), row.end(), [](Color color) { return color != Color::black; })) {
			music->Play(AUDIO_ID::Cube_Clear);
			lines += 1;
			score += 150;
			canvas.erase(canvas.begin() + row_index);
			canvas.insert(canvas.begin(), vector<Color>(CANVAS_WIDTH, Color::black));
		}
	}
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

CMovingBitmap CGameStateRun::Cube()
{
	CMovingBitmap cube;
	cube.LoadBitmapByString({ "resources/cube_black.bmp", "resources/cube_light_blue.bmp", "resources/cube_yellow.bmp", "resources/cube_purple.bmp",  "resources/cube_green.bmp",
		"resources/cube_red.bmp", "resources/cube_blue.bmp",  "resources/cube_orange.bmp", "resources/cube_grey.bmp" });
	cube.LoadBitmapByString({ "resources/cube_transparent.bmp" }, RGB(255, 255, 255));
	return cube;
}

void CGameStateRun::display_game()
{
	cube_place.ShowBitmap();

	for (int i = 0; i < CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < CANVAS_WIDTH; j++)
		{
			cube[i][j].ShowBitmap();
		}
	}

	for (int i = 0; i < PLACE_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < PLACE_CUBE_CANVAS_WIDTH; j++)
		{
			cube_next[i][j].ShowBitmap();
		}
	}

	for (int i = 0; i < HOLD_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < HOLD_CUBE_CANVAS_WIDTH; j++)
		{
			cube_hold[i][j].ShowBitmap();
		}
	}
}

void CGameStateRun::display_lines()
{
	if (!game_over)
	{
		game_lines_displacement = game_lines < 10 ? 0 : 20;
		sprintf(lines_display, "%d", game_lines);

		CDC *pDC = CDDraw::GetBackCDC();

		CTextDraw::ChangeFontLog(pDC, 22, "微軟正黑體", RGB(255, 255, 255), 50);
		CTextDraw::Print(pDC, 710, 715, "LINES");
		CTextDraw::ChangeFontLog(pDC, 43, "微軟正黑體", RGB(255, 255, 255), 200);
		CTextDraw::Print(pDC, 707 - game_lines_displacement, 740, lines_display);
		CTextDraw::ChangeFontLog(pDC, 28, "微軟正黑體", RGB(255, 255, 255), 200);
		CTextDraw::Print(pDC, 733, 754, "/40");

		CDDraw::ReleaseBackCDC();
	}
}


void CGameStateRun::display_time()
{
	if (!game_over)
	{
		game_current_time = clock() - game_init_time;
		game_minutes = game_current_time / 60000;
		game_seconds = (game_current_time / 1000) % 60;
		game_milliseconds = game_current_time % 1000;
		sprintf(time_display_front, "%d:%02d", game_minutes, game_seconds);
		sprintf(time_display_back, ".%03d", game_milliseconds);

		CDC *pDC = CDDraw::GetBackCDC();

		CTextDraw::ChangeFontLog(pDC, 22, "微軟正黑體", RGB(255, 255, 255), 50);
		CTextDraw::Print(pDC, 718, 800, "TIME");
		CTextDraw::ChangeFontLog(pDC, 43, "微軟正黑體", RGB(255, 255, 255), 50);
		CTextDraw::Print(pDC, 630, 825, time_display_front);
		CTextDraw::ChangeFontLog(pDC, 28, "微軟正黑體", RGB(255, 255, 255), 50);
		CTextDraw::Print(pDC, 715, 838, time_display_back);

		CDDraw::ReleaseBackCDC();
	}
}

void CGameStateRun::display_score()
{
	score_display = to_string(game_score);
	for (int i = score_display.length() - 3; i >= 1; i-=3)
	{
		score_display.insert(i, ",");
	}
	game_score_displacement = (score_display.length() * 12) / 2;

	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 22, "微軟正黑體", RGB(255, 255, 255), 50);
	CTextDraw::Print(pDC, 948 - game_score_displacement, 875, score_display);

	CDDraw::ReleaseBackCDC();
}

void CGameStateRun::display_level()
{
	sprintf(level_display, "%d", game_level);
	game_level_displacement = (strlen(level_display) * 41) / 2;

	CDC *pDC = CDDraw::GetBackCDC();

	CTextDraw::ChangeFontLog(pDC, 45, "微軟正黑體", RGB(255, 255, 255), 50);
	CTextDraw::Print(pDC, 955 - game_level_displacement, 900, level_display);

	CDDraw::ReleaseBackCDC();
}

void CGameStateRun::display_lines_graph()
{
	for (int i = 0; i < game_lines % 20; i++)
	{
		if (i < game_lines % 20 - 1)
		{
			lines_graph_body[i].ShowBitmap();
		}
		else
		{
			int y = lines_graph_body[i].GetTop();
			lines_graph_top.SetTopLeft(1114, y);
			lines_graph_top.ShowBitmap();
		}
	}
}

bool  CGameStateRun::game_over_animation()
{
	cube_place.SetFrameIndexOfBitmap(1);
	if (fire_animation)
	{
		music->Play(AUDIO_ID::Game_Over);
		for (int i = 0; i < 4; i++)
		{
			fire[i].ToggleAnimation();
		}
		fire_animation = false;
	}
	if (fire[0].GetFrameIndexOfBitmap() == 37)
	{
		if (cube[0][0].GetTop() < 1080)
		{
			for (int i = 0; i < CANVAS_HEIGHT; i++)
			{
				for (int j = 0; j < CANVAS_WIDTH; j++)
				{
					cube[i][j].SetTopLeft(cube[i][j].GetLeft(), cube[i][j].GetTop() + 20);
				}
			}
			for (int i = 0; i < PLACE_CUBE_CANVAS_HEIGHT; i++)
			{
				for (int j = 0; j < PLACE_CUBE_CANVAS_WIDTH; j++)
				{
					cube_next[i][j].SetTopLeft(cube_next[i][j].GetLeft(), cube_next[i][j].GetTop() + 20);
				}
			}
			for (int i = 0; i < HOLD_CUBE_CANVAS_HEIGHT; i++)
			{
				for (int j = 0; j < HOLD_CUBE_CANVAS_WIDTH; j++)
				{
					cube_hold[i][j].SetTopLeft(cube_hold[i][j].GetLeft(), cube_hold[i][j].GetTop() + 20);
				}
			}
			cube_place.SetTopLeft(cube_place.GetLeft(), cube_place.GetTop() + 20);
		}
		else
		{
			return false;
		}
	}
	return true;
}

void CGameStateRun::game_level_up_animation()
{
	if (level_up_animation)
	{
		level_up.SetFrameIndexOfBitmap(0);
		music->Pause();
		music->Play(AUDIO_ID::Level_Up);
		level_up.ToggleAnimation();
		level_up_animation = false;
	}
	if (level_up.GetFrameIndexOfBitmap() == 124)
	{
		music->Resume();
		level_up_animation = true;
		game_level = game_lines / 20 + 1;
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
			music->Stop(audio_id);
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
	game_over = false;
	fire_animation = true;
	level_up_animation = true;
	background.SetFrameIndexOfBitmap(rand() % 6);
	cube_place.SetFrameIndexOfBitmap(0);
	tetris_game = TetrisGame();
	game_init_time = clock();
	game_next_decline_time = clock();
	game_next_move_time = clock();
	for (int i = 0; i < 22; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			cube[i][j].SetTopLeft(788 + j * 32, 160 + i * 32);
		}
	}
	for (int i = 0; i < 14; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cube_next[i][j].SetTopLeft(1154 + j * 32, 270 + i * 32);
		}
	}
	for (int i = 0; i < HOLD_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < HOLD_CUBE_CANVAS_WIDTH; j++)
		{
			cube_hold[i][j].SetTopLeft(638 + j * 32, 267 + i * 32);
		}
	}
	for_each(fire.begin(), fire.end(), [](CMovingBitmap fire)
	{
		fire.SetFrameIndexOfBitmap(0);
	});
	cube_place.SetTopLeft(618, 224);
}

void CGameStateRun::game_update(Event event)
{
	GameState game_state = tetris_game.event_handler(event);
	Canvas canvas = game_state.canvas;
	game_over = game_state.game_over;
	game_lines = game_state.lines;
	game_score = game_state.score;
	for (int i = 0; i < CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < CANVAS_WIDTH; j++)
		{
			if (i < PREVIEW_ROW_COUNT && canvas[i][j] == Color::black)
			{
				cube[i][j].SetFrameIndexOfBitmap(Color::transparent);
			}
			else
			{
				cube[i][j].SetFrameIndexOfBitmap(canvas[i][j]);
			}
		}
	}

	Canvas place_canvas = game_state.place_canvas;
	for (int i = 0; i < PLACE_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < PLACE_CUBE_CANVAS_WIDTH; j++)
		{
			cube_next[i][j].SetFrameIndexOfBitmap(place_canvas[i][j]);
		}
	}

	Canvas hold_canvas = game_state.hold_canvas;
	for (int i = 0; i < HOLD_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < HOLD_CUBE_CANVAS_WIDTH; j++)
		{
			cube_hold[i][j].SetFrameIndexOfBitmap(hold_canvas[i][j]);
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
	game_move_time_interval = 60;
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

	game_end_time_displacement = game_minutes >= 10 ? 35 : 0;

	sprintf(end_time_display_front, "%d:%02d", game_minutes, game_seconds);
	sprintf(end_time_display_back, ".%03d", game_milliseconds);
	sprintf(real_time_display, "%d/%d/%02d  %d:%02d:%02d", real_time_year, real_time_month, real_time_day, real_time_hour, real_time_min, real_time_sec);
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
					music->Play(11);
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
			if (!game_over)
			{
				if (game_next_decline_time <= clock())
				{
					game_natural_decline();
				}
				if (game_next_move_time <= clock())
				{
					game_control();
				}
				if (game_lines >= 6)
				{
					music->Stop(audio_id);
					game_record_current_time();
					tittle.SetFrameIndexOfBitmap(5);
					fourtyl_again.SetAnimation(60, false);
					sub_phase = 4;
				}
				if (exit_check || exit_scene.GetFrameIndexOfBitmap() != 31)
				{
					game_exit_animation();
				}
			}
			else
			{
				if (!game_over_animation())
				{
					music->Stop(audio_id);
					sub_phase = 3;
				}
			}
		}
		else if (sub_phase == 3)
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
	if (gametype == GameType::blitz)
	{
		if (sub_phase == 1)
		{
			if (touch_option_menu_selected)
			{
				if (touch_option_menu_first)
				{
					music->Play(11);
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
			if (!game_over)
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
			}
			else
			{
				if (!game_over_animation())
				{
					music->Stop(audio_id);
					sub_phase = 3;
				}
			}
		}
		else if (sub_phase == 3)
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
			if (!game_over)
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
			}
			else
			{
				music->Play(AUDIO_ID::Cube_Full_Clear);
				sub_phase = 3;
				record_current_time = clock() + 2000;
			}
			if (game_level < game_lines / 20 + 1)
			{
				game_level_up_animation();
			}
		}
		else if (sub_phase == 3)
		{
			if (record_current_time < clock())
			{
				game_init();
				tetris_game.score = game_score;
				tetris_game.lines = game_lines;
				sub_phase = 2;
			}
		}
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
}

void CGameStateRun::OnInit()  								// 遊戲的初值及圖形設定
{
	phase = 1;
	sub_phase = 1;
	id = 0;

	music = CAudio::Instance();
	music->Load(AUDIO_ID::Arial_City, "resources/Arial_City.wav");
	music->Load(AUDIO_ID::To_The_Limit, "resources/To_The_Limit.wav");
	music->Load(AUDIO_ID::The_Great_Eastern_Expedition, "resources/The_Great_Eastern_Expedition.wav");
	music->Load(AUDIO_ID::Morning_Sun_Kamoking, "resources/Morning_Sun_Kamoking.wav");
	music->Load(AUDIO_ID::In_Sorrow_And_Pains_Mirera, "resources/In_Sorrow_And_Pains_Mirera.wav");
	music->Load(AUDIO_ID::Piercing_Wind, "resources/Piercing_Wind.wav");
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
	music->Load(AUDIO_ID::Level_Up, "resources/level_Up.wav");
	music->Load(AUDIO_ID::Exit_Process_Game, "resources/Exit_Process_Game.wav");
	music->Load(AUDIO_ID::Exit_Game, "resources/Exit_Game.wav");

	music->Play(AUDIO_ID::Arial_City, true);

	first_menu.LoadBitmapByString({ "resources/first_menu.bmp" });
	first_menu.SetTopLeft(420, 340);

	join.LoadBitmapByString({ "resources/join.bmp", "resources/join_done.bmp" });
	join.SetTopLeft(420, 670);

	tittle.LoadBitmapA({ "resources/tittle.bmp", "resources/tittle2.bmp", "resources/40l_tittle.bmp", "resources/blitz_tittle.bmp", "resources/zen_tittle.bmp", "resources/40l_result_tittle.bmp"});
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

	game_mode.LoadBitmapByString({ "resources/game_mode.bmp", "resources/40l_press_start.bmp",  "resources/blitz_press_start.bmp",  "resources/zen_press_start.bmp" });
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

	fourtyl_again.LoadBitmapByString({ "resources/40l_again_1.bmp", "resources/40l_again_2.bmp", "resources/40l_again_3.bmp", "resources/40l_again_4.bmp", "resources/40l_again_5.bmp",
		"resources/40l_again_6.bmp", "resources/40l_again_7.bmp", "resources/40l_again_8.bmp", "resources/40l_again_7.bmp", "resources/40l_again_6.bmp", "resources/40l_again_5.bmp",
		"resources/40l_again_4.bmp" , "resources/40l_again_3.bmp" , "resources/40l_again_2.bmp" });
	fourtyl_again.SetTopLeft(1394, 358);

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

	zen_model.LoadBitmapByString({ "resources/zen_model.bmp" }, RGB(0, 0, 255));
	zen_model.SetTopLeft(1800, 0);

	start[2].LoadBitmapByString({ "resources/zen_start_1.bmp", "resources/zen_start_2.bmp", "resources/zen_start_3.bmp", "resources/zen_start_4.bmp", "resources/zen_start_5.bmp",
		"resources/zen_start_6.bmp", "resources/zen_start_7.bmp", "resources/zen_start_8.bmp", "resources/zen_start_7.bmp", "resources/zen_start_6.bmp", "resources/zen_start_5.bmp",
		"resources/zen_start_4.bmp", "resources/zen_start_3.bmp", "resources/zen_start_2.bmp" });
	start[2].SetTopLeft(1395, 285);
	
	cube_place.LoadBitmapByString({ "resources/cube_place.bmp", "resources/cube_place_game_over.bmp" }, RGB(0, 0, 255));
	cube_place.SetTopLeft(618, 224);

	for (unsigned i = 0; i < lines_graph_body.size(); i++)
	{
		lines_graph_body[i].LoadBitmapByString({ "resources/lines_graph_body.bmp" });
		lines_graph_body[i].SetTopLeft(1114, 831-i*32);
	}

	lines_graph_top.LoadBitmapByString({ "resources/lines_graph_top.bmp" });
	lines_graph_top.SetTopLeft(1114, 223);

	for (int i = 0; i < CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < CANVAS_WIDTH; j++)
		{
			cube[i][j] = Cube();
		}
	}

	for (int i = 0; i < PLACE_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < PLACE_CUBE_CANVAS_WIDTH; j++)
		{
			cube_next[i][j] = Cube();
		}
	}

	for (int i = 0; i < HOLD_CUBE_CANVAS_HEIGHT; i++)
	{
		for (int j = 0; j < HOLD_CUBE_CANVAS_WIDTH; j++)
		{
			cube_hold[i][j] = Cube();
		}
	}

	fire[0].LoadBitmapByString({ "resources/fire_1_lt.bmp", "resources/fire_2_lt.bmp", "resources/fire_3_lt.bmp", "resources/fire_4_lt.bmp", "resources/fire_5_lt.bmp",
		"resources/fire_6_lt.bmp", "resources/fire_7_lt.bmp", "resources/fire_8_lt.bmp", "resources/fire_9_lt.bmp", "resources/fire_10_lt.bmp", "resources/fire_11_lt.bmp",
		"resources/fire_12_lt.bmp", "resources/fire_13_lt.bmp", "resources/fire_14_lt.bmp", "resources/fire_15_lt.bmp", "resources/fire_16_lt.bmp", "resources/fire_17_lt.bmp",
		"resources/fire_18_lt.bmp", "resources/fire_19_lt.bmp", "resources/fire_20_lt.bmp", "resources/fire_21_lt.bmp", "resources/fire_22_lt.bmp", "resources/fire_23_lt.bmp",
		"resources/fire_24_lt.bmp", "resources/fire_25_lt.bmp", "resources/fire_26_lt.bmp", "resources/fire_27_lt.bmp", "resources/fire_28_lt.bmp", "resources/fire_29_lt.bmp",
		"resources/fire_30_lt.bmp", "resources/fire_31_lt.bmp", "resources/fire_32_lt.bmp", "resources/fire_33_lt.bmp", "resources/fire_34_lt.bmp", "resources/fire_35_lt.bmp",
		"resources/fire_36_lt.bmp", "resources/fire_37_lt.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[0].SetAnimation(16, true);
	fire[0].SetTopLeft(670, 78);

	fire[1].LoadBitmapByString({ "resources/fire_1_rt.bmp", "resources/fire_2_rt.bmp", "resources/fire_3_rt.bmp", "resources/fire_4_rt.bmp", "resources/fire_5_rt.bmp",
	"resources/fire_6_rt.bmp", "resources/fire_7_rt.bmp", "resources/fire_8_rt.bmp", "resources/fire_9_rt.bmp", "resources/fire_10_rt.bmp", "resources/fire_11_lt.bmp",
	"resources/fire_12_rt.bmp", "resources/fire_13_rt.bmp", "resources/fire_14_rt.bmp", "resources/fire_15_rt.bmp", "resources/fire_16_rt.bmp", "resources/fire_17_rt.bmp",
	"resources/fire_18_rt.bmp", "resources/fire_19_rt.bmp", "resources/fire_20_rt.bmp", "resources/fire_21_rt.bmp", "resources/fire_22_rt.bmp", "resources/fire_23_lt.bmp",
	"resources/fire_24_rt.bmp", "resources/fire_25_rt.bmp", "resources/fire_26_rt.bmp", "resources/fire_27_rt.bmp", "resources/fire_28_rt.bmp", "resources/fire_29_lt.bmp",
	"resources/fire_30_rt.bmp", "resources/fire_31_rt.bmp", "resources/fire_32_rt.bmp", "resources/fire_33_rt.bmp", "resources/fire_34_rt.bmp", "resources/fire_35_lt.bmp",
	"resources/fire_36_rt.bmp", "resources/fire_37_rt.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[1].SetAnimation(16, true);
	fire[1].SetTopLeft(1050, 78);

	fire[2].LoadBitmapByString({ "resources/fire_1_lb.bmp", "resources/fire_2_lb.bmp", "resources/fire_3_lb.bmp", "resources/fire_4_lb.bmp", "resources/fire_5_lb.bmp",
	"resources/fire_6_lb.bmp", "resources/fire_7_lb.bmp", "resources/fire_8_lb.bmp", "resources/fire_9_lb.bmp", "resources/fire_10_lb.bmp", "resources/fire_11_lb.bmp",
	"resources/fire_12_lb.bmp", "resources/fire_13_lb.bmp", "resources/fire_14_lb.bmp", "resources/fire_15_lb.bmp", "resources/fire_16_lb.bmp", "resources/fire_17_lb.bmp",
	"resources/fire_18_lb.bmp", "resources/fire_19_lb.bmp", "resources/fire_20_lb.bmp", "resources/fire_21_lb.bmp", "resources/fire_22_lb.bmp", "resources/fire_23_lb.bmp",
	"resources/fire_24_lb.bmp", "resources/fire_25_lb.bmp", "resources/fire_26_lb.bmp", "resources/fire_27_lb.bmp", "resources/fire_28_lb.bmp", "resources/fire_29_lb.bmp",
	"resources/fire_30_lb.bmp", "resources/fire_31_lb.bmp", "resources/fire_32_lb.bmp", "resources/fire_33_lb.bmp", "resources/fire_34_lb.bmp", "resources/fire_35_lb.bmp",
	"resources/fire_36_lb.bmp", "resources/fire_37_lb.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[2].SetAnimation(16, true);
	fire[2].SetTopLeft(670, 870);

	fire[3].LoadBitmapByString({ "resources/fire_1_rb.bmp", "resources/fire_2_rb.bmp", "resources/fire_3_rb.bmp", "resources/fire_4_rb.bmp", "resources/fire_5_rb.bmp",
	"resources/fire_6_rb.bmp", "resources/fire_7_rb.bmp", "resources/fire_8_rb.bmp", "resources/fire_9_rb.bmp", "resources/fire_10_rb.bmp", "resources/fire_11_rb.bmp",
	"resources/fire_12_rb.bmp", "resources/fire_13_rb.bmp", "resources/fire_14_rb.bmp", "resources/fire_15_rb.bmp", "resources/fire_16_rb.bmp", "resources/fire_17_rb.bmp",
	"resources/fire_18_rb.bmp", "resources/fire_19_rb.bmp", "resources/fire_20_rb.bmp", "resources/fire_21_rb.bmp", "resources/fire_22_rb.bmp", "resources/fire_23_rb.bmp",
	"resources/fire_24_rb.bmp", "resources/fire_25_rb.bmp", "resources/fire_26_rb.bmp", "resources/fire_27_rb.bmp", "resources/fire_28_rb.bmp", "resources/fire_29_rb.bmp",
	"resources/fire_30_rb.bmp", "resources/fire_31_rb.bmp", "resources/fire_32_rb.bmp", "resources/fire_33_rb.bmp", "resources/fire_34_rb.bmp", "resources/fire_35_rb.bmp",
	"resources/fire_36_rb.bmp", "resources/fire_37_rb.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[3].SetAnimation(16, true);
	fire[3].SetTopLeft(1050, 870);
	fire_animation = true;

	level_up.LoadBitmapByString({ "resources/level_up_cutscene_1.bmp", "resources/level_up_cutscene_2.bmp", "resources/level_up_cutscene_3.bmp", "resources/level_up_cutscene_4.bmp",
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
	level_up.LoadBitmapByString({ "resources/level_up_cutscene_transparent.bmp" }, RGB(255, 255, 255));
	level_up.SetFrameIndexOfBitmap(124);
	level_up.SetTopLeft(0, 0);
	level_up.SetAnimation(16, true);

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
	game_over = false;

	retry.LoadBitmapByString({ "resources/retry.bmp", "resources/retry_selected.bmp" });
	retry.SetTopLeft(301, 80);
	retry_selected = false;

	back_to_tittle.LoadBitmapByString({ "resources/back_to_tittle.bmp", "resources/back_to_tittle_selected.bmp" });
	back_to_tittle.SetTopLeft(301, 200);
	back_to_tittle_selected = false;

	fire_animation = true;

	game_score = 0;

	game_level = 1;
}

void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (phase >= 3 )
	{
		if (sub_phase == 2)
		{
			if (!game_over)
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
					game_move_time_interval = 140;
				}
				else if (nChar == VK_LEFT)
				{
					game_next_move_time = clock();
					left_key_down = true;
					right_key_down = false;
					game_move_time_interval = 140;
				}
				else if (nChar == VK_RIGHT)
				{
					game_next_move_time = clock();
					left_key_down = false;
					right_key_down = true;
					game_move_time_interval = 140;
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
				audio_id = rand() % 6;
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(AUDIO_ID::Arial_City);
				music->Play(audio_id, true);
				game_init();
				sub_phase = 2;
			}
			for (int i = 0; i < 4; i++)
			{
				if (click_check(nFlags, point, fourtyl_menu_check[i]))
				{
					music->Play(10);
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
			if (click_check(nFlags, point, retry))
			{
				audio_id = rand() % 6;
				music->Play(AUDIO_ID::Back_Menu);
				music->Stop(AUDIO_ID::Arial_City);
				music->Play(audio_id, true);
				game_init();
				sub_phase = 2;
			}
			if (click_check(nFlags, point, back_to_tittle))
			{
				music->Play(AUDIO_ID::Back_Menu);
				music->Play(AUDIO_ID::Arial_City, true);
				sub_phase = 1;
			}
		}
		else if (sub_phase == 4)
		{
			if (click_check(nFlags, point, fourtyl_again))
			{
				music->Play(AUDIO_ID::Click_Menu);
				music->Play(audio_id, true);
				fourtyl_again.SetAnimation(60, true);
				game_init();
				sub_phase = 2;
			}
			if (click_check(nFlags, point, back))
			{
				music->Play(AUDIO_ID::Back_Menu);
				music->Play(AUDIO_ID::Arial_City, true);
				tittle.SetFrameIndexOfBitmap(1);
				back.SetFrameIndexOfBitmap(0);
				start[id].SetAnimation(60, true);
				game_mode.SetFrameIndexOfBitmap(0);
				phase = 2;
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
				audio_id = rand() % 6;
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(AUDIO_ID::Arial_City);
				music->Play(audio_id, true);
				game_init();
				sub_phase = 2;
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
				audio_id = rand() % 6;
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(AUDIO_ID::Arial_City);
				music->Play(audio_id, true);
				background.SetFrameIndexOfBitmap(rand() % 6);
				game_next_decline_time = clock();
				game_next_move_time = clock();
				game_init();
				tetris_game = save_tetris_game;
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
	}
	if (phase == 4)
	{
		if (sub_phase == 1)
		{
			touch_option_menu(point);
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
			display_lines();
			display_time();
			exit_scene.ShowBitmap();

			if (game_over)
			{
				for_each(fire.begin(), fire.end(), [](CMovingBitmap& fire)
				{
					fire.ShowBitmap();
				});
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

			fourtyl_again.ShowBitmap();

			back.ShowBitmap();

			CDC *pDC = CDDraw::GetBackCDC();

			CTextDraw::ChangeFontLog(pDC, 120, "微軟正黑體", RGB(244, 193, 155), 50);
			CTextDraw::Print(pDC, 765-game_end_time_displacement, 180, end_time_display_front);


			CTextDraw::ChangeFontLog(pDC, 85, "微軟正黑體", RGB(244, 193, 155), 50);
			CTextDraw::Print(pDC, 996+game_end_time_displacement, 210, end_time_display_back);

			CTextDraw::ChangeFontLog(pDC, 30, "微軟正黑體", RGB(169, 117, 83), 50);
			CTextDraw::Print(pDC, 200, 964, real_time_display);

			CDDraw::ReleaseBackCDC();
			
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
		else if (sub_phase >= 2)
		{
			background.ShowBitmap();

			display_game();
			display_lines();
			display_lines_graph();
			exit_scene.ShowBitmap();
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
			zen_model.ShowBitmap();

			display_game();
			display_score();
			display_level();
			display_lines_graph();
			display_lines();
			exit_scene.ShowBitmap();

			level_up.ShowBitmap();
		}
	}
	else if (phase == 6)
	{
		background.ShowBitmap();

		tittle.ShowBitmap();
	}
}
