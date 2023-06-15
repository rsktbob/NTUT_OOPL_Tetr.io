#ifndef BUTTON_H
#define BUTTON_H

#pragma once
#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"
#include "music.h"

namespace game_framework {

	enum Direction
	{
		left,
		right
	};

	class Button : public CMovingBitmap
	{
	public:
		int _x_beign;
		int _x_end;
		int _speed;
		Direction _direction;

		Button();
		Button(int x_begin, int x_end, Direction direction);
		bool _is_touched = false;
		bool _is_touched_first = true;
		bool check_touch(CPoint point);
		void change_state(CPoint point);
		bool check_click(UINT nFlags, CPoint point);
	};
}

#endif
