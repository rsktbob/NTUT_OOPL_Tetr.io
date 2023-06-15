#include "stdafx.h"
#include "trimino.h"
#pragma once

namespace game_framework
{
	Tromino::Tromino(Color color, TrominoMatrix matrix, int canvas_width) : color(color), matrix(matrix)
    {
        x = (canvas_width - width()) / 2;
        y = -height() + 1;
    }

    Tromino Tromino::according_color_tromino(Color color, int canvas_width)
    {
        TrominoMatrix matrix;

        // https://stackoverflow.com/a/47588671
        // https://tetris.wiki/Tetromino#The_basic_tetrominoes
        switch (color)
        {
        case Color::light_blue:
            matrix = {
                {1, 1, 1, 1},
            };
            break;

        case Color::yellow:
            matrix = {
                {1, 1},
                {1, 1},
            };
            break;

        case Color::purple:
            matrix = {
                {0, 1, 0},
                {1, 1, 1},
            };
            break;

        case Color::green:
            matrix = {
                {0, 1, 1},
                {1, 1, 0},
            };
            break;

        case Color::red:
            matrix = {
                {1, 1, 0},
                {0, 1, 1},
            };
            break;

        case Color::blue:
            matrix = {
                {1, 0, 0},
                {1, 1, 1},
            };
            break;

        case Color::orange:
            matrix = {
                {0, 0, 1},
                {1, 1, 1},
            };
        }

        return Tromino(color, matrix, canvas_width);
    }

    int Tromino::width() const
    {
        return matrix[0].size();
    }

    int Tromino::height() const
    {
        return matrix.size();
    }

    Color Tromino::according_matrix_return_color()
    {
        if (matrix == TrominoMatrix({
                          {1, 1, 1, 1},
                      }))
        {
            return Color::light_blue;
        }
        else if (matrix == TrominoMatrix({
                               {1, 1},
                               {1, 1},
                           }))
        {
            return Color::yellow;
        }
        else if (matrix == TrominoMatrix({
                               {0, 1, 0},
                               {1, 1, 1},
                           }))
        {
            return Color::purple;
        }
        else if (matrix == TrominoMatrix({
                               {0, 1, 1},
                               {1, 1, 0},
                           }))
        {
            return Color::green;
        }
        else if (matrix == TrominoMatrix({
                               {1, 1, 0},
                               {0, 1, 1},
                           }))
        {
            return Color::red;
        }
        else if (matrix == TrominoMatrix({
                               {1, 0, 0},
                               {1, 1, 1},
                           }))
        {
            return Color::blue;
        }
        else
        {
            return Color::orange;
        }
    }

    TrominoMatrix Tromino::according_color_return_matrix()
    {
        switch (color)
        {
        case Color::light_blue:
            return {
                {1, 1, 1, 1},
            };
        case Color::yellow:
            return {{1, 1},
                    {1, 1}};
        case Color::purple:
            return {{0, 1, 0},
                    {1, 1, 1}};
        case Color::green:
            return {{0, 1, 1},
                    {1, 1, 0}};
        case Color::red:
            return {{1, 1, 0},
                    {0, 1, 1}};
        case Color::blue:
            return {{1, 0, 0},
                    {1, 1, 1}};
        default:
            return {{0, 0, 1},
                    {1, 1, 1}};
        }
    }
}