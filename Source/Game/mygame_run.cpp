#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"
#include "mygame.h"

using namespace game_framework;

/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
/////////////////////////////////////////////////////////////////////////////

CAudio* music;

bool fire_animation = true;

int speed = 4;

bool click_check(UINT nFlags, CPoint point, CMovingBitmap character)
{
	if (point.x >= character.GetLeft() && point.x <= character.GetLeft() + character.GetWidth()
		&& point.y >= character.GetTop() && point.y <= character.GetTop() + character.GetHeight()
		&& nFlags == VK_LBUTTON)
	{
		return true;
	}
	return false;
}

bool touch_check(CPoint point, CMovingBitmap character)
{
	if (point.x >= character.GetLeft() && point.x <= character.GetLeft() + character.GetWidth()
		&& point.y >= character.GetTop() && point.y <= character.GetTop() + character.GetHeight())
	{
		return true;
	}
	return false;
}

void touch_option_menu(CPoint point, bool& touch_option_menu_selected)
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

CMovingBitmap Cube()
{
	CMovingBitmap cube;
	cube.LoadBitmapByString({ "resources/cube_black.bmp", "resources/cube_light_blue.bmp", "resources/cube_yellow.bmp", "resources/cube_purple.bmp",  "resources/cube_green.bmp",
		"resources/cube_red.bmp", "resources/cube_blue.bmp",  "resources/cube_orange.bmp" });
	cube.LoadBitmapByString({ "resources/cube_transparent.bmp" }, RGB(255, 255, 255));
	return cube;
}

void display_game(vector<vector<CMovingBitmap>>& cube, vector<vector<CMovingBitmap>>& cube_next,  CMovingBitmap cube_place)
{

	cube_place.ShowBitmap();

	for (int i = 0; i < 22; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			cube[i][j].ShowBitmap();
		}
	}

	for (int i = 0; i < 14; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cube_next[i][j].ShowBitmap();
		}
	}
}

bool game_over_animation(vector<vector<CMovingBitmap>>& cube, vector<vector<CMovingBitmap>>& cube_next, CMovingBitmap &cube_place, vector<CMovingBitmap> &fire, bool& fire_animation)
{
	if (fire_animation)
	{
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
			for (int i = 0; i < 22; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					cube[i][j].SetTopLeft(cube[i][j].GetLeft(), cube[i][j].GetTop() + speed);
				}
			}
			for (int i = 0; i < 14; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					cube_next[i][j].SetTopLeft(cube_next[i][j].GetLeft(), cube_next[i][j].GetTop() + speed);
				}
			}
			cube_place.SetTopLeft(cube_place.GetLeft(), cube_place.GetTop() + speed);
			if (speed < 20)
			{
				speed += 4;
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				fire[i].SetFrameIndexOfBitmap(0);
			}
			return true;
		}
	}
	return false;
}

void game_init(vector<vector<CMovingBitmap>>& cube, vector<vector<CMovingBitmap>>& cube_next, CMovingBitmap &cube_place, vector<CMovingBitmap> &fire, bool& fire_animation)
{
	fire_animation = true;
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
			cube_next[i][j] = Cube();
			cube_next[i][j].SetTopLeft(1154 + j * 32, 270 + i * 32);
			cube_next[i][j].SetFrameIndexOfBitmap(2);
		}
	}
	cube_place.SetTopLeft(618, 224);
}

void canvas_update(vector<vector<CMovingBitmap>>& cube, vector<vector<Color>> &canvas)
{
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
			if (game_next_decline_time <= clock())
			{
				auto game_state = tetris_game.event_handler(Event::tick);
				vector<vector<Color>> canvas = game_state.canvas;
				canvas_update(cube, canvas);
				game_next_decline_time = clock() + game_decline_time_interval;
			}
			if (game_next_move_time <= clock())
			{
				if (left_key_down)
				{
					auto game_state = tetris_game.event_handler(Event::left);
					vector<vector<Color>> canvas = game_state.canvas;
					canvas_update(cube, canvas);
				}
				if (right_key_down)
				{
					auto game_state = tetris_game.event_handler(Event::right);
					vector<vector<Color>> canvas = game_state.canvas;
					canvas_update(cube, canvas);
				}
				if (down_key_down)
				{
					auto game_state = tetris_game.event_handler(Event::tick);
					vector<vector<Color>> canvas = game_state.canvas;
					canvas_update(cube, canvas);
				}
				game_next_move_time = clock() + game_move_time_interval;
			}
		}
	}
	else if (phase == 4)
	{
		if (sub_phase == 1)
		{
			if (touch_option_menu_selected)
			{
				if (touch_option_menu_first)
				{
					music->Play(AUDIO_ID::Touch_Option_Menu);
					touch_option_menu_first = false;
				}
			}
			else
			{
				touch_option_menu_first = true;
			}
		}
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
	music->Play(AUDIO_ID::Arial_City, true);

	first_menu.LoadBitmapByString({ "resources/first_menu.bmp" });
	first_menu.SetTopLeft(420, 340);

	join.LoadBitmapByString({ "resources/join.bmp", "resources/join_done.bmp" });
	join.SetTopLeft(420, 670);

	tittle.LoadBitmapA({ "resources/tittle.bmp", "resources/tittle2.bmp", "resources/40l_tittle.bmp", "resources/blitz_tittle.bmp", "resources/zen_tittle.bmp" });
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

	for (int i = 0; i < 22; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			cube[i][j] = Cube();
			cube[i][j].SetTopLeft(788 + j * 32, 160 + i * 32);
			if (i < 2)
			{
				cube[i][j].SetFrameIndexOfBitmap(8);
			}
		}
	}

	fourtyl_menu[0].LoadBitmapByString({ "resources/40l_information.bmp" });
	fourtyl_menu[0].SetTopLeft(270, 100);

	fourtyl_menu[1].LoadBitmapByString({ "resources/40l_music.bmp" });
	fourtyl_menu[1].SetTopLeft(270, 284);

	fourtyl_menu[2].LoadBitmapByString({ "resources/40l_options.bmp" });
	fourtyl_menu[2].SetTopLeft(270, 388);

	fourtyl_menu[3].LoadBitmapByString({ "resources/40l_advanced.bmp" });
	fourtyl_menu[3].SetTopLeft(270, 638);

	for (int i = 0; i < 4; i++)
	{
		fourtyl_menu_check[i].LoadBitmapByString({ "resources/check_transparent.bmp" }, RGB(255, 255, 255));
		fourtyl_menu_check[i].LoadBitmapByString({ "resources/40l_check.bmp" });
		fourtyl_menu_check[i].SetTopLeft(290, 457 + i * 36);
	}

	start[0].LoadBitmapByString({ "resources/40l_start_1.bmp", "resources/40l_start_2.bmp", "resources/40l_start_3.bmp", "resources/40l_start_4.bmp", "resources/40l_start_5.bmp", 
		"resources/40l_start_6.bmp", "resources/40l_start_7.bmp", "resources/40l_start_8.bmp", "resources/40l_start_7.bmp", "resources/40l_start_6.bmp", "resources/40l_start_5.bmp", "resources/40l_start_4.bmp", "resources/40l_start_3.bmp", "resources/40l_start_2.bmp", });
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
		"resources/blitz_start_6.bmp", "resources/blitz_start_7.bmp", "resources/blitz_start_8.bmp", "resources/blitz_start_7.bmp", "resources/blitz_start_6.bmp", "resources/blitz_start_5.bmp","resources/blitz_start_4.bmp", "resources/blitz_start_3.bmp", "resources/blitz_start_2.bmp" });
	start[1].SetTopLeft(1391, 290);

	zen_menu[0].LoadBitmapByString({ "resources/zen_information.bmp" });
	zen_menu[0].SetTopLeft(270, 100);

	zen_menu[1].LoadBitmapByString({ "resources/zen_blank.bmp" });
	zen_menu[1].SetTopLeft(270, 284);

	start[2].LoadBitmapByString({ "resources/zen_start_1.bmp", "resources/zen_start_2.bmp", "resources/zen_start_3.bmp", "resources/zen_start_4.bmp", "resources/zen_start_5.bmp",
		"resources/zen_start_6.bmp", "resources/zen_start_7.bmp", "resources/zen_start_8.bmp", "resources/zen_start_7.bmp", "resources/zen_start_6.bmp", "resources/zen_start_5.bmp","resources/zen_start_4.bmp", "resources/zen_start_3.bmp", "resources/zen_start_2.bmp" });
	start[2].SetTopLeft(1395, 285);
	
	cube_place.LoadBitmapByString({ "resources/cube_place.bmp", "resources/cube_place_game_over.bmp" }, RGB(0, 0, 255));
	cube_place.SetTopLeft(618, 224);

	for (int i = 0; i < 14; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cube_next[i][j] = Cube();
			cube_next[i][j].SetTopLeft(1154 + j * 32, 270 + i * 32);
			cube_next[i][j].SetFrameIndexOfBitmap(2);
		}
	}

	fire[0].LoadBitmapByString({ "resources/fire_1_lt.bmp", "resources/fire_2_lt.bmp", "resources/fire_3_lt.bmp", "resources/fire_4_lt.bmp", "resources/fire_5_lt.bmp",
		"resources/fire_6_lt.bmp", "resources/fire_7_lt.bmp", "resources/fire_8_lt.bmp", "resources/fire_9_lt.bmp", "resources/fire_10_lt.bmp", "resources/fire_11_lt.bmp",
		"resources/fire_12_lt.bmp", "resources/fire_13_lt.bmp", "resources/fire_14_lt.bmp", "resources/fire_15_lt.bmp", "resources/fire_16_lt.bmp", "resources/fire_17_lt.bmp",
		"resources/fire_18_lt.bmp", "resources/fire_19_lt.bmp", "resources/fire_20_lt.bmp", "resources/fire_21_lt.bmp", "resources/fire_22_lt.bmp", "resources/fire_23_lt.bmp",
		"resources/fire_24_lt.bmp", "resources/fire_25_lt.bmp", "resources/fire_26_lt.bmp", "resources/fire_27_lt.bmp", "resources/fire_28_lt.bmp", "resources/fire_29_lt.bmp",
		"resources/fire_30_lt.bmp", "resources/fire_31_lt.bmp", "resources/fire_32_lt.bmp", "resources/fire_33_lt.bmp", "resources/fire_34_lt.bmp", "resources/fire_35_lt.bmp",
		"resources/fire_36_lt.bmp", "resources/fire_37_lt.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[0].SetFrameIndexOfBitmap(37);
	fire[0].SetAnimation(16, true);
	fire[0].SetTopLeft(670, 78);

	fire[1].LoadBitmapByString({ "resources/fire_1_rt.bmp", "resources/fire_2_rt.bmp", "resources/fire_3_rt.bmp", "resources/fire_4_rt.bmp", "resources/fire_5_rt.bmp",
	"resources/fire_6_rt.bmp", "resources/fire_7_rt.bmp", "resources/fire_8_rt.bmp", "resources/fire_9_rt.bmp", "resources/fire_10_rt.bmp", "resources/fire_11_lt.bmp",
	"resources/fire_12_rt.bmp", "resources/fire_13_rt.bmp", "resources/fire_14_rt.bmp", "resources/fire_15_rt.bmp", "resources/fire_16_rt.bmp", "resources/fire_17_rt.bmp",
	"resources/fire_18_rt.bmp", "resources/fire_19_rt.bmp", "resources/fire_20_rt.bmp", "resources/fire_21_rt.bmp", "resources/fire_22_rt.bmp", "resources/fire_23_lt.bmp",
	"resources/fire_24_rt.bmp", "resources/fire_25_rt.bmp", "resources/fire_26_rt.bmp", "resources/fire_27_rt.bmp", "resources/fire_28_rt.bmp", "resources/fire_29_lt.bmp",
	"resources/fire_30_rt.bmp", "resources/fire_31_rt.bmp", "resources/fire_32_rt.bmp", "resources/fire_33_rt.bmp", "resources/fire_34_rt.bmp", "resources/fire_35_lt.bmp",
	"resources/fire_36_rt.bmp", "resources/fire_37_rt.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[1].SetFrameIndexOfBitmap(37);
	fire[1].SetAnimation(16, true);
	fire[1].SetTopLeft(1050, 78);

	fire[2].LoadBitmapByString({ "resources/fire_1_lb.bmp", "resources/fire_2_lb.bmp", "resources/fire_3_lb.bmp", "resources/fire_4_lb.bmp", "resources/fire_5_lb.bmp",
	"resources/fire_6_lb.bmp", "resources/fire_7_lb.bmp", "resources/fire_8_lb.bmp", "resources/fire_9_lb.bmp", "resources/fire_10_lb.bmp", "resources/fire_11_lb.bmp",
	"resources/fire_12_lb.bmp", "resources/fire_13_lb.bmp", "resources/fire_14_lb.bmp", "resources/fire_15_lb.bmp", "resources/fire_16_lb.bmp", "resources/fire_17_lb.bmp",
	"resources/fire_18_lb.bmp", "resources/fire_19_lb.bmp", "resources/fire_20_lb.bmp", "resources/fire_21_lb.bmp", "resources/fire_22_lb.bmp", "resources/fire_23_lb.bmp",
	"resources/fire_24_lb.bmp", "resources/fire_25_lb.bmp", "resources/fire_26_lb.bmp", "resources/fire_27_lb.bmp", "resources/fire_28_lb.bmp", "resources/fire_29_lb.bmp",
	"resources/fire_30_lb.bmp", "resources/fire_31_lb.bmp", "resources/fire_32_lb.bmp", "resources/fire_33_lb.bmp", "resources/fire_34_lb.bmp", "resources/fire_35_lb.bmp",
	"resources/fire_36_lb.bmp", "resources/fire_37_lb.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[2].SetFrameIndexOfBitmap(37);
	fire[2].SetAnimation(16, true);
	fire[2].SetTopLeft(670, 870);

	fire[3].LoadBitmapByString({ "resources/fire_1_rb.bmp", "resources/fire_2_rb.bmp", "resources/fire_3_rb.bmp", "resources/fire_4_rb.bmp", "resources/fire_5_rb.bmp",
	"resources/fire_6_rb.bmp", "resources/fire_7_rb.bmp", "resources/fire_8_rb.bmp", "resources/fire_9_rb.bmp", "resources/fire_10_rb.bmp", "resources/fire_11_rb.bmp",
	"resources/fire_12_rb.bmp", "resources/fire_13_rb.bmp", "resources/fire_14_rb.bmp", "resources/fire_15_rb.bmp", "resources/fire_16_rb.bmp", "resources/fire_17_rb.bmp",
	"resources/fire_18_rb.bmp", "resources/fire_19_rb.bmp", "resources/fire_20_rb.bmp", "resources/fire_21_rb.bmp", "resources/fire_22_rb.bmp", "resources/fire_23_rb.bmp",
	"resources/fire_24_rb.bmp", "resources/fire_25_rb.bmp", "resources/fire_26_rb.bmp", "resources/fire_27_rb.bmp", "resources/fire_28_rb.bmp", "resources/fire_29_rb.bmp",
	"resources/fire_30_rb.bmp", "resources/fire_31_rb.bmp", "resources/fire_32_rb.bmp", "resources/fire_33_rb.bmp", "resources/fire_34_rb.bmp", "resources/fire_35_rb.bmp",
	"resources/fire_36_rb.bmp", "resources/fire_37_rb.bmp", "resources/fire_transparent.bmp" }, RGB(0, 0, 0));

	fire[3].SetFrameIndexOfBitmap(37);
	fire[3].SetAnimation(16, true);
	fire[3].SetTopLeft(1050, 870);
	fire_animation = true;

	touch_option_menu_first = true;
	touch_option_menu_selected = false;

	game_decline_time_interval = 1500;
	game_move_time_interval = 100;
	left_key_down = false;
	right_key_down = false;
	down_key_down = false;
}

void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (phase == 3)
	{
		if (sub_phase == 2)
		{
			if (nChar == VK_UP)
			{
				auto game_state = tetris_game.event_handler(Event::rotate);
				vector<vector<Color>> canvas = game_state.canvas;
				canvas_update(cube, canvas);
			}
			else if (nChar == VK_DOWN)
			{
				game_next_move_time = clock();
				down_key_down = true;
			}
			else if (nChar == VK_LEFT)
			{
				game_next_move_time = clock();
				left_key_down = true;
				right_key_down = false;
			}
			else if (nChar == VK_RIGHT)
			{
				game_next_move_time = clock();
				left_key_down = false;
				right_key_down = true;
			}
			else if (nChar == VK_SPACE)
			{
				auto game_state = tetris_game.event_handler(Event::down);
				vector<vector<Color>> canvas = game_state.canvas;
				canvas_update(cube, canvas);
			}
		}
	}
}

void CGameStateRun::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (phase == 3)
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
				phase = 2;;
			}
			if (click_check(nFlags, point, start[0]))
			{
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(AUDIO_ID::Arial_City);
				music->Play(rand() % 6, true);
				background.SetFrameIndexOfBitmap(rand() % 6);
				fire_animation = true;
				game_next_decline_time = clock();
				game_next_move_time = clock();
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
				sub_phase = 1;
			}
			if (click_check(nFlags, point, start[1]))
			{
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(AUDIO_ID::Arial_City);
				music->Play(rand() % 6, true);
				background.SetFrameIndexOfBitmap(rand() % 6);
				game_next_decline_time = clock();
				game_next_move_time = clock();
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
				music->Play(AUDIO_ID::Click_Menu);
				music->Stop(AUDIO_ID::Arial_City);
				music->Play(rand() % 6, true);
				background.SetFrameIndexOfBitmap(rand() % 6);
				game_next_decline_time = clock();
				game_next_move_time = clock();
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
			touch_option_menu(point, touch_option_menu_selected);
		}
	}
	if (phase == 4)
	{
		if (sub_phase == 1)
		{
			touch_option_menu(point, touch_option_menu_selected);
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
	static int end_time = 0;
	if (phase == 1)
	{
		tittle.ShowBitmap();

		first_menu.ShowBitmap();
		join.ShowBitmap();

		osk.ShowBitmap();

		if (sub_phase == 2)
		{
			sub_phase += 1;
			end_time = clock() + 300;
		}
		else if (sub_phase == 3)
		{
			if (clock() > end_time)
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

			display_game(cube, cube_next, cube_place);

			fire[0].ShowBitmap();
			fire[1].ShowBitmap();
			fire[2].ShowBitmap();
			fire[3].ShowBitmap();
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

			display_game(cube, cube_next,  cube_place);
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
		else if (sub_phase == 2)
		{
			background.ShowBitmap();

			display_game(cube, cube_next, cube_place);
		}
	}
	else if (phase == 6)
	{
		background.ShowBitmap();

		tittle.ShowBitmap();
	}
}
