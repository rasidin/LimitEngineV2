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
enum class RenderPass : char {
    PrePass = 0,
    BasePass,
    TranslucencyPass,
    NumOfRenderPass
};

static constexpr char* RenderPassNames[] = {
    "PrePass",
    "BasePass",
    "TranslucencyPass"
};

enum class BackgroundImageType : char {
    None = 0,
    Fullscreen,
    Longlat,
    
    Num,
};

enum class ResourceState : int {
    Common                      = 0,
    VertexAndConstantBuffer     = 1 << 0,
    IndexBuffer                 = 1 << 1,
    RenderTarget                = 1 << 2,
    UnorderedAccess             = 1 << 3,
    DepthWrite                  = 1 << 4,
    DepthRead                   = 1 << 5,
    NonPixelShaderResource      = 1 << 6,
    PixelShaderResource         = 1 << 7,
    StreamOut                   = 1 << 8,
    IndirectArgument            = 1 << 9,
    CopyDest                    = 1 <<10,
    CopySource                  = 1 <<11,
    ResolveDest                 = 1 <<12,
    ResolveSource               = 1 <<13,
    GenericRead                 = 1 <<14,
    Present                     = 1 <<15
};

extern "C++" {
    inline ResourceState operator | (ResourceState a, ResourceState b) { return ResourceState(static_cast<int>(a) | static_cast<int>(b)); }
    inline ResourceState operator & (ResourceState a, ResourceState b) { return ResourceState(static_cast<int>(a) & static_cast<int>(b)); }
}

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
