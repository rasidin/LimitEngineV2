/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Draw.h
 @brief Base Draw (Rect, Line...)
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/25 Created by minseob
 ***********************************************************/

#ifndef _LE_DRAW_H_
#define _LE_DRAW_H_

#include <LE_Math>
#include <LE_Graphics>

#include "LE_Singleton.h"

namespace LimitEngine {
class Draw
{
public:
	Draw();
	virtual ~Draw();

	static void DrawPoint(const fPoint &pos, const cColorRGBA &color);
	static void DrawLine(const fPoint &start, const fPoint &end, const cColorRGBA &color);
	static void DrawBox(const fRect &rect, float rotation, const cColorRGBA &color, Texture *texture = NULL, Shader *shader = NULL);
    static void DrawBox(const fRect &rect, float rotation, const cColorRGBA &leftTopColor, const cColorRGBA &rightTopColor, const cColorRGBA &leftBottomColor, const cColorRGBA &rightBottomColor);
    static void DrawScreen(Shader *shader = NULL);
}; // Draw
}

#endif // _LE_DRAW_H_