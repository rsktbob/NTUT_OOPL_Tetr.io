#include "stdafx.h"
#include "button.h"
#pragma once

namespace game_framework {
	Button::Button() : _x_beign(0), _x_end(0), _direction(Direction::left) {};
	Button::Button(int x_begin, int x_end, Direction direction) : _x_beign(x_begin), _x_end(x_end), _direction(direction) {}

	bool Button::check_touch(CPoint point)
	{
		if (point.x >= this->GetLeft() && point.x <= this->GetLeft() + this->GetWidth()
			&& point.y >= this->GetTop() && point.y <= this->GetTop() + this->GetHeight())
		{
			return true;
		}
		return false;
	}

	void  Button::change_state(CPoint point) {
		if (_speed == 0)
		{
			if (check_touch(point) && _is_touched_first)
			{
				this->SetFrameIndexOfBitmap(1);
				_is_touched_first = false;
			}
			else if (!check_touch(point) && !_is_touched_first)
			{
				this->SetFrameIndexOfBitmap(0);
				_is_touched_first = true;
			}
		}
	}

	bool  Button::check_click(UINT nFlags, CPoint point) {
		if (point.x >= this->GetLeft() && point.x <= this->GetLeft() + this->GetWidth()
			&& point.y >= this->GetTop() && point.y <= this->GetTop() + this->GetHeight()
			&& nFlags == VK_LBUTTON)
		{
			return true;
		}
		return false;
	}
}

