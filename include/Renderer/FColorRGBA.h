/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_fColorRGBA.h
 @brief fColorRGBA Class
 @author minseob (https://github.com/rasidin)
 -----------------------------------------------------------
 History:
 - 2012/6/17 Created by minseob
 ***********************************************************/

#ifndef _LE_FCOLORRGBA_H_
#define _LE_FCOLORRGBA_H_

#include <string.h>

namespace LimitEngine {
class fColorRGBA
{public:
	union 
	{
		struct { float r, g, b, a; };
		float m[4];
	};

	fColorRGBA() { ::memset(m, 0, 4 * sizeof(float)); }
	fColorRGBA(float red, float green, float blue, float alpha) 
	{
		m[0] = red;
		m[1] = green;
		m[2] = blue;
		m[3] = alpha;
	}
    fColorRGBA(float c)
    {
        m[0] = m[1] = m[2] = m[3] = c;
    }
    static fColorRGBA ONE() { return fColorRGBA(1.0f); }
    static fColorRGBA ZERO() { return fColorRGBA(0.0f); }
};
}

#endif // _LE_FCOLORRGBA_H_