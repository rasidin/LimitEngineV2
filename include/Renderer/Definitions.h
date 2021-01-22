/*********************************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file Definitions.h
@brief Definitions for Renderer
@author minseob
**********************************************************************/
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

    enum class BufferFormat
    {
        Unknown = 0,
        R32G32B32A32_Typeless,
        R32G32B32A32_Float,
        R32G32B32A32_UInt,
        R32G32B32A32_SInt,
        R32G32B32_Typeless,
        R32G32B32_Float,
        R32G32B32_UInt,
        R32G32B32_SInt,
        R16G16B16A16_Typeless,
        R16G16B16A16_Float,
        R16G16B16A16_UNorm,
        R16G16B16A16_UInt,
        R16G16B16A16_SNorm,
        R16G16B16A16_SInt,
        R32G32_Typeless,
        R32G32_Float,
        R32G32_UInt,
        R32G32_SInt,
        R10G10B10A2_Typeless,
        R10G10B10A2_UNorm,
        R8G8B8A8_Typeless,
        R8G8B8A8_UNorm,
        R8G8B8A8_UNorm_SRGB,
        R8G8B8A8_UInt,
        R8G8B8A8_SNorm,
        R8G8B8A8_SInt,
        R16G16_Typeless,
        R16G16_Float,
        R16G16_UNorm,
        R16G16_UInt,
        R16G16_SNorm,
        R16G16_SInt,
        R32_Float,
        D32_Float,
        R32_UInt,
        R32_SInt,
        D24_UNorm_S8_UInt,
        R16_Typeless,
        R16_Float,
        D16_UNorm,
        R16_UNorm,
        R16_SInt,
        R8_Typeless,
        R8_UNorm,
        R8_UInt,
        R8_SNorm,
        R8_SInt,
    };

    enum class InputClassification
    {
        PerVertexData,
        PerInstanceData
    };

    enum class FillMode
    {
        WireFrame = 0,
        Solid,
    };

    enum class CullMode
    {
        None = 0,
        Front, 
        Back
    };

    // Culling
    enum class Culling
    {
        ClockWise,
        CounterClockWise,
        Max,
    };

    enum class DepthWriteMask
    {
        Zero = 0,
        All,
    };

    enum class TestFlags
    {
        Never = 0,
        Less,            //! a< b
        Equal,           //! a==b
        LEqual,          //! a<=b
        Greate,         //! a> b
        NotEqual,        //! a!=b
        GEqual,          //! a>=b
        Always,

        COUNT,
    };

    enum class StencilOperators
    {
        Keep = 0,
        Zero,
        Replace,
        IncreaseSaturate,
        DecreaseSaturate,
        Invert,
        Increase,
        Decrease
    };

    enum class BlendValues
    {
        Zero = 0,
        One,
        SrcColor,
        InvSrcColor,
        SrcAlpha,
        InvSrcAlpha,
        DestAlpha,
        InvDestAlpha,
        DestColor,
        InvDestColor,
    };

    enum class BlendOperators
    {
        Add = 0,
        Subtract,
        RevSubtract,
        Min,
        Max
    };

    enum class BlendFlags
    {
        SOURCE = 0,      // NoBlend just source to target : output = src
        ALPHABLEND,      // AlphaBlend (Normal alpha blend) : output = src * srcAlpha + dst * (1 - srcAlpha)
        ADD,             // Additional : output = src * srcAlpha + dst
    };

    enum class PrimitiveTopologyType
    {
        Undefined,
        Point,
        Line,
        Triangle,
        Patch
    };

    enum class CreateBufferFlags
    {
        CPU_READABLE = 1<<0, // Readble by CPU
        CPU_WRITABLE = 1<<1, // Writable by CPU
    };
};
}
