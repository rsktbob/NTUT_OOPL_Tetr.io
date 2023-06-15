#ifndef TROMINO_H
#define TROMINO_H

#pragma once
#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"
#pragma once
#include "music.h"

namespace game_framework {

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
		grey,
		transparent,
		translucent,
		error
	};

	using TrominoMatrix = vector<vector<char>>;

	class Tromino
	{
	public:
		int x;
		int y;
		Color color;
		TrominoMatrix matrix;

		Tromino(Color color, TrominoMatrix matrix, int canvas_width);
		static Tromino according_color_tromino(Color color, int canvas_width);
		int width() const;
		int height() const;
		Color according_matrix_return_color();
		TrominoMatrix according_color_return_matrix();
	};
}
#endif