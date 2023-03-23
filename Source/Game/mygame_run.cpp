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

CMovingBitmap Cube()
{
	CMovingBitmap cube;
	cube.LoadBitmapByString({ "resources/cube_black.bmp", "resources/cube_red.bmp", "resources/cube_blue.bmp", "resources/cube_yellow.bmp",  "resources/cube_green.bmp",
		"resources/cube_purple.bmp", "resources/cube_orange.bmp",  "resources/cube_cyan.bmp" }); // 0 black, 1 red, 2 blue, 3 yellow, 4 green, 5 purple, 6 orange, 7 cyan
	cube.LoadBitmapByString({ "resources/cube_transparent.bmp" }, RGB(255, 255, 255)); // 8 transparent
	return cube;
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
				second_menu[i].SetTopLeft(second_menu[i].GetLeft() - 10, 100 + i * 140);
			}
			else if (second_menu[i].GetLeft() < 375 && !second_menu_selected[i])
			{
				second_menu[i].SetTopLeft(second_menu[i].GetLeft() + 10, 100 + i * 140);
			}
		}
	}
	if (phase >= 2)
	{
		if (back.GetLeft() < 0 && back_selected)
		{
			back.SetTopLeft(back.GetLeft() + 10, 80);
		}
		else if (back.GetLeft() > -40 && !back_selected)
		{
			back.SetTopLeft(back.GetLeft() - 10, 80);
		}
	}
}

void CGameStateRun::OnInit()  								// 遊戲的初值及圖形設定
{
	phase = 1;
	sub_phase = 1;
	id = 0;
	srand((unsigned)time(NULL));

	first_menu.LoadBitmapByString({ "resources/first_menu.bmp" });
	first_menu.SetTopLeft(420, 340);

	join.LoadBitmapByString({ "resources/join.bmp", "resources/join_done.bmp" });
	join.SetTopLeft(420, 670);

	tittle.LoadBitmapA({ "resources/tittle.bmp", "resources/tittle2.bmp", "resources/40l_tittle.bmp", "resources/blitz_tittle.bmp", "resources/zen_tittle.bmp" });
	tittle.SetTopLeft(0, 0);

	osk.LoadBitmapByString({ "resources/osk.bmp" });
	osk.SetTopLeft(0, 930);

	background.LoadBitmapByString({ "resources/background1.bmp",  "resources/background2.bmp", "resources/background3.bmp", "resources/background4.bmp",  "resources/background5.bmp", "resources/background6.bmp" });
	background.SetFrameIndexOfBitmap(rand() % 6);
	background.SetTopLeft(0, 0);

	second_menu[0].LoadBitmapByString({ "resources/40l.bmp", "resources/40l_selected.bmp" });
	second_menu[0].SetTopLeft(375, 100);

	second_menu[1].LoadBitmapByString({ "resources/blitz.bmp", "resources/blitz_selected.bmp" });
	second_menu[1].SetTopLeft(375, 240);

	second_menu[2].LoadBitmapByString({ "resources/zen.bmp", "resources/zen_selected.bmp" });
	second_menu[2].SetTopLeft(375, 380);

	second_menu[3].LoadBitmapByString({ "resources/custom.bmp", "resources/custom_selected.bmp" });
	second_menu[3].SetTopLeft(375, 520);

	back.LoadBitmapByString({ "resources/back.bmp", "resources/back_selected.bmp" });
	back.SetTopLeft(-40, 80);

	game_mode.LoadBitmapByString({ "resources/game_mode.bmp", "resources/40l_press_start.bmp",  "resources/blitz_press_start.bmp",  "resources/zen_press_start.bmp" });
	game_mode.SetTopLeft(0, 940);

	for (int i = 0; i < 22; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			cube[i][j] = Cube();
			cube[i][j].SetTopLeft(800 + j * 32, 160 + i * 32);
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

	start[0].LoadBitmapByString({ "resources/40l_start.bmp", "resources/40l_start_dark.bmp" });
	start[0].SetTopLeft(1391, 288);

	blitz_menu[0].LoadBitmapByString({ "resources/blitz_information.bmp" });
	blitz_menu[0].SetTopLeft(270, 100);

	blitz_menu[1].LoadBitmapByString({ "resources/blitz_blank.bmp" });
	blitz_menu[1].SetTopLeft(270, 284);

	blitz_menu[2].LoadBitmapByString({ "resources/blitz_options.bmp" });
	blitz_menu[2].SetTopLeft(270, 388);

	blitz_menu[3].LoadBitmapByString({ "resources/blitz_advanced.bmp" });
	blitz_menu[3].SetTopLeft(270, 638);

	start[1].LoadBitmapByString({ "resources/blitz_start.bmp", "resources/blitz_start_dark.bmp" });
	start[1].SetTopLeft(1391, 290);

	zen_menu[0].LoadBitmapByString({ "resources/zen_information.bmp" });
	zen_menu[0].SetTopLeft(270, 100);

	zen_menu[1].LoadBitmapByString({ "resources/zen_blank.bmp" });
	zen_menu[1].SetTopLeft(270, 284);

	start[2].LoadBitmapByString({ "resources/zen_start.bmp", "resources/zen_start_dark.bmp" });
	start[2].SetTopLeft(1395, 285);

	cube_hold.LoadBitmapByString({ "resources/cube_hold.bmp" }, RGB(255, 0, 0));
	cube_hold.SetTopLeft(628, 224);
	
	cube_place.LoadBitmapByString({ "resources/cube_place.bmp" }, RGB(255, 0, 0));
	cube_place.SetTopLeft(1124, 224);

	cube_boundary[0].LoadEmptyBitmap(640, 4);
	cube_boundary[0].SetTopLeft(796, 224);
	
	cube_boundary[1].LoadEmptyBitmap(4, 351);
	cube_boundary[1].SetTopLeft(796, 864);

	cube_boundary[2].LoadEmptyBitmap(640, 4);
	cube_boundary[2].SetTopLeft(1120, 224);
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
			tittle.SetFrameIndexOfBitmap(0);
			back.SetFrameIndexOfBitmap(0);
			phase = 1;
			sub_phase = 1;
		}
		for (int i = 0; i < 4; i++)
		{
			if (click_check(nFlags, point, second_menu[i]))
			{
				background.SetFrameIndexOfBitmap(rand() % 6);
				tittle.SetFrameIndexOfBitmap(i + 2);
				game_mode.SetFrameIndexOfBitmap(i + 1);
				id = i;
				start[i].SetAnimation(200, false);
				phase = 3 + i;
				sub_phase = 1;
			}
		}
	}
	else if (phase >= 3)
	{
		if (click_check(nFlags, point, back))
		{
			tittle.SetFrameIndexOfBitmap(1);
			back.SetFrameIndexOfBitmap(0);
			start[id].SetAnimation(200, true);
			game_mode.SetFrameIndexOfBitmap(0);
			phase = 2;
			sub_phase = 1;
		}
		for (int i = 0; i < 3; i++)
		{
			if (click_check(nFlags, point, start[i]))
			{
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

			start[0].ShowBitmap();
		}
		else if (sub_phase == 2)
		{
			background.ShowBitmap();

			for (int i = 0; i < 22; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					cube[i][j].ShowBitmap();
				}
			}

			cube_hold.ShowBitmap();
			cube_place.ShowBitmap();

			for (int i = 0; i < 3; i++)
			{
				cube_boundary[i].ShowBitmap();
			}
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

			start[1].ShowBitmap();
		}
		else if (sub_phase == 2)
		{
			background.ShowBitmap();

			for (int i = 0; i < 22; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					cube[i][j].ShowBitmap();
				}
			}

			cube_hold.ShowBitmap();
			cube_place.ShowBitmap();

			for (int i = 0; i < 3; i++)
			{
				cube_boundary[i].ShowBitmap();
			}
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

			for (int i = 0; i < 22; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					cube[i][j].ShowBitmap();
				}
			}

			cube_hold.ShowBitmap();
			cube_place.ShowBitmap();

			for (int i = 0; i < 3; i++)
			{
				cube_boundary[i].ShowBitmap();
			}
		}
	}
	else if (phase == 6)
	{
		background.ShowBitmap();

		tittle.ShowBitmap();
	}
}
