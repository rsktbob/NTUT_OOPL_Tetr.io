#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"
#include "mygame.h"
#include <iostream>

using namespace game_framework;

/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
/////////////////////////////////////////////////////////////////////////////

CAudio* music;

bool fire_animation = true;

int speed = 4;

bool click_check(UINT nChar, CPoint point, CMovingBitmap character)
{
	if (point.x >= character.GetLeft() && point.x <= character.GetLeft() + character.GetWidth()
		&& point.y >= character.GetTop() && point.y <= character.GetTop() + character.GetHeight()
		&& nChar == VK_LBUTTON)
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
	cube.LoadBitmapByString({ "resources/cube_black.bmp", "resources/cube_red.bmp", "resources/cube_blue.bmp", "resources/cube_yellow.bmp",  "resources/cube_green.bmp",
		"resources/cube_purple.bmp", "resources/cube_orange.bmp",  "resources/cube_cyan.bmp" }); // 0 black, 1 red, 2 blue, 3 yellow, 4 green, 5 purple, 6 orange, 7 cyan
	cube.LoadBitmapByString({ "resources/cube_transparent.bmp" }, RGB(255, 255, 255)); // 8 transparent
	return cube;
}

void display_game(CMovingBitmap cube[][10], CMovingBitmap cube_next[][4],  CMovingBitmap cube_place)
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
					music->Play(6);
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
				music->Play(6);
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
	}
	else if (phase == 4)
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
	}
}

void CGameStateRun::OnInit()  								// 遊戲的初值及圖形設定
{
	phase = 1;
	sub_phase = 1;
	id = 0;

	music = CAudio::Instance();
	music->Load(0, "resources/Arial_City.wav");
	music->Load(1, "resources/To_The_Limit.wav");
	music->Load(2, "resources/The_Great_Eastern_Expedition.wav");
	music->Load(3, "resources/Morning_Sun_Kamoking.wav");
	music->Load(4, "resources/In_Sorrow_And_Pains_Mirera.wav");
	music->Load(5, "resources/Piercing_Wind.wav");
	music->Load(6, "resources/Touch_Menu.wav");
	music->Load(7, "resources/Click_Menu.wav");
	music->Load(8, "resources/Back_Menu.wav");
	music->Load(9, "resources/Touch_Check_Menu.wav");
	music->Load(10, "resources/Click_Check_Menu.wav");
	music->Load(11, "resources/Touch_Option_Menu.wav");
	music->Play(0, true);

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

	touch_option_menu_first = true;
	touch_option_menu_selected = false;

}

void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
}

void CGameStateRun::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
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
			music->Play(8);
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
				music->Play(7);
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
				music->Play(8);
				tittle.SetFrameIndexOfBitmap(1);
				back.SetFrameIndexOfBitmap(0);
				start[id].SetAnimation(60, true);
				game_mode.SetFrameIndexOfBitmap(0);
				phase = 2;;
			}
			if (click_check(nFlags, point, start[0]))
			{
				music->Play(7);
				music->Stop(0);
				music->Play(rand() % 6, true);
				background.SetFrameIndexOfBitmap(rand() % 6);
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
				music->Play(8);
				tittle.SetFrameIndexOfBitmap(1);
				back.SetFrameIndexOfBitmap(0);
				start[id].SetAnimation(60, true);
				game_mode.SetFrameIndexOfBitmap(0);
				phase = 2;
				sub_phase = 1;
			}
			if (click_check(nFlags, point, start[1]))
			{
				music->Play(7);
				music->Stop(0);
				music->Play(rand() % 6, true);
				background.SetFrameIndexOfBitmap(rand() % 6);
				sub_phase = 2;
			}
			for (int i = 0; i < 4; i++)
			{
				if (click_check(nFlags, point, blitz_menu_check[i]))
				{
					music->Play(10);
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
			if (click_check(nFlags, point, start[2]))
			{
				music->Play(7);
				music->Stop(0);
				music->Play(rand() % 6, true);
				background.SetFrameIndexOfBitmap(rand() % 6);
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
