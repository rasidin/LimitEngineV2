/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ByteColorRGBA.h
 @brief ByteColorRGBA Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/
#pragma once

#include "Core/Common.h"
#include "Core/Util.h"

namespace LimitEngine {
class ByteColorRGBA
{public:
	union
	{
		struct { uint8 r, g, b, a; };
		uint32 m;
	};

    ByteColorRGBA(uint32 c) { m = c; }
    ByteColorRGBA(uint8 R, uint8 G, uint8 B, uint8 A)
	{ r = R; g = G; b = B; a = A; }
    ByteColorRGBA(float R, float G, float B, float A)
    { r = FloatToUInt8(R); g = FloatToUInt8(G); b = FloatToUInt8(B); a = FloatToUInt8(A); }
};
}
