/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  Definitions
@brief Definitions for renderer
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

namespace LimitEngine {
struct RendererFlag {
    enum class PrimitiveTypes
    {
        POINTLIST = 0,
        LINELIST,
        LINESTRIP,
        TRIANGLELIST,
        TRIANGLESTRIP,
        TRIANGLEFAN,
    };
    enum class EnabledFlags
    {
        DEPTH_TEST = 1 << 0,
        DEPTH_WRITE = 1 << 1,
        ALPHABLEND = 1 << 2,
        STENCIL = 1 << 3,

        ALPHABLEND_RT0 = ALPHABLEND | (1 << 8),
        ALPHABLEND_RT1 = ALPHABLEND | (1 << 9),
        ALPHABLEND_RT2 = ALPHABLEND | (1 << 10),
        ALPHABLEND_RT3 = ALPHABLEND | (1 << 11),
        ALPHABLEND_RT4 = ALPHABLEND | (1 << 12),
        ALPHABLEND_RT5 = ALPHABLEND | (1 << 13),
        ALPHABLEND_RT6 = ALPHABLEND | (1 << 14),
        ALPHABLEND_RT7 = ALPHABLEND | (1 << 15),
    };
    // Culling
    enum class Culling
    {
        None = 0,
        ClockWise,
        CounterClockWise,
        Max,
    };

    enum class TestFlags
    {
        NEVER = 0,
        LESS,            //! a< b
        EQUAL,           //! a==b
        LEQUAL,          //! a<=b
        GREATER,         //! a> b
        NOTEQUAL,        //! a!=b
        GEQUAL,          //! a>=b
        ALWAYS,

        COUNT,
    };

    enum class BlendFlags
    {
        SOURCE = 0,      // NoBlend just source to target : output = src
        ALPHABLEND,      // AlphaBlend (Normal alpha blend) : output = src * srcAlpha + dst * (1 - srcAlpha)
        ADD,             // Additional : output = src * srcAlpha + dst
    };

    enum class CreateBufferFlags
    {
        CPU_READABLE = 1<<0, // Readble by CPU
        CPU_WRITABLE = 1<<1, // Writable by CPU
    };
};
}
