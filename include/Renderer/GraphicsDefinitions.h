/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2012
-----------------------------------------------------------
@file  LE_Graphics
@brief Graphics common header
@author minseob (leeminseob@outlook.com)
-----------------------------------------------------------
History:
- 2016/10/31 Created by minseob
***********************************************************/
#pragma once
#ifndef _LE_GRAPHICSDEFINITIONS_H_
#define _LE_GRAPHICSDEFINITIONS_H_

namespace LimitEngine {
struct GraphicsFlag {
    enum PrimitiveTypes
    {
        PT_POINTLIST = 0,
        PT_LINELIST,
        PT_LINESTRIP,
        PT_TRIANGLELIST,
        PT_TRIANGLESTRIP,
        PT_TRIANGLEFAN,
    };
    enum EnabledFlags
    {
        EF_DEPTH_TEST = 1 << 0,
        EF_DEPTH_WRITE = 1 << 1,
        EF_ALPHABLEND = 1 << 2,
        EF_STENCIL = 1 << 3,

        EF_ALPHABLEND_RT0 = EF_ALPHABLEND | (1 << 8),
        EF_ALPHABLEND_RT1 = EF_ALPHABLEND | (1 << 9),
        EF_ALPHABLEND_RT2 = EF_ALPHABLEND | (1 << 10),
        EF_ALPHABLEND_RT3 = EF_ALPHABLEND | (1 << 11),
        EF_ALPHABLEND_RT4 = EF_ALPHABLEND | (1 << 12),
        EF_ALPHABLEND_RT5 = EF_ALPHABLEND | (1 << 13),
        EF_ALPHABLEND_RT6 = EF_ALPHABLEND | (1 << 14),
        EF_ALPHABLEND_RT7 = EF_ALPHABLEND | (1 << 15),
    };

    enum TestFlags
    {
        TF_NEVER = 0,
        TF_LESS,            //! a< b
        TF_EQUAL,           //! a==b
        TF_LEQUAL,          //! a<=b
        TF_GREATER,         //! a> b
        TF_NOTEQUAL,        //! a!=b
        TF_GEQUAL,          //! a>=b
        TF_ALWAYS,

        TF_COUNT,
    };

    enum BlendFlags
    {
        BF_SOURCE = 0,      // NoBlend just source to target : output = src
        BF_ALPHABLEND,      // AlphaBlend (Normal alpha blend) : output = src * srcAlpha + dst * (1 - srcAlpha)
        BF_ADD,             // Additional : output = src * srcAlpha + dst
    };

    enum CreateBufferFlags
    {
        CB_CPU_READABLE = 1<<0, // Readble by CPU
        CB_CPU_WRITABLE = 1<<1, // Writable by CPU
    };
};
}

#endif // _LE_GRAPHICSDEFINITIONS_H_
