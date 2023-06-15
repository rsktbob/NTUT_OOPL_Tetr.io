#ifndef MUSIC_H
#define MUSIC_H

#pragma once
#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"

namespace game_framework {
	enum AUDIO_ID
	{
		Arial_City,
		To_The_Limit,
		The_Great_Eastern_Expedition,
		Morning_Sun_Kamoking,
		In_Sorrow_And_Pains_Mirera,
		Piercing_Wind,
		Success_Story_Akiko_Shioyama,
		Hyper_Velocity,
		Philosophy,
		Classy_Cat,
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
		Game_Start_Count,
		Random,
		None
	};

	static CAudio *music = CAudio::Instance();
}

#endif