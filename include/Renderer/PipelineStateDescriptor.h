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
@file PipelineStateDescriptor.h
@brief Descriptor for PipelineState
@author minseob
**********************************************************************/
#ifndef LIMITENGINEV2_PIPELINESTATEDESCRIPTOR_H_
#define LIMITENGINEV2_PIPELINESTATEDESCRIPTOR_H_

#include <LERenderer>

#include "Core/Util.h"
#include "Core/Hash.h"
#include "Core/MemoryAllocator.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
struct PipelineStateDescriptor {
    static constexpr int MaxRenderTargetsNum = 8;
    static constexpr int MaxInputElementsNum = 16;

    ShaderRefPtr Shaders[static_cast<int>(Shader::Type::Num)];

    struct {
        bool AlphaToCoverageEnabled = false;
        bool IndependentBlendEnabled = false;
        struct {
            bool BlendEnabled = false;
            RendererFlag::BlendValues SrcBlend = RendererFlag::BlendValues::SrcAlpha;
            RendererFlag::BlendValues DestBlend = RendererFlag::BlendValues::InvSrcAlpha;
            RendererFlag::BlendOperators BlendOp = RendererFlag::BlendOperators::Add;
            RendererFlag::BlendValues SrcAlpha = RendererFlag::BlendValues::One;
            RendererFlag::BlendValues DestAlpha = RendererFlag::BlendValues::InvSrcAlpha;
            RendererFlag::BlendOperators BlendOpAlpha = RendererFlag::BlendOperators::Add;
        } RenderTargetBlendDescriptor[MaxRenderTargetsNum];
    } BlendDescriptor;
    inline void SetAlphaToCoverageEnabled(bool enabled) { BlendDescriptor.AlphaToCoverageEnabled = enabled; }
    inline void SetIndependentBlendEnabled(bool enabled) { BlendDescriptor.IndependentBlendEnabled = enabled; }
    inline void SetRenderTargetBlendEnabled(uint32 rtidx, bool enabled) { BlendDescriptor.RenderTargetBlendDescriptor[rtidx].BlendEnabled = enabled; }
    inline void SetRenderTargetBlendOperator(uint32 rtidx,
        const RendererFlag::BlendValues& src, const RendererFlag::BlendValues& dest, const RendererFlag::BlendOperators& op,
        const RendererFlag::BlendValues& srcalpha, const RendererFlag::BlendValues& destalpha, const RendererFlag::BlendOperators& opalpha)
    {
        BlendDescriptor.RenderTargetBlendDescriptor[rtidx] = {
            BlendDescriptor.RenderTargetBlendDescriptor[rtidx].BlendEnabled, src, dest, op, srcalpha, destalpha, opalpha
        };
    }

    struct {
        RendererFlag::FillMode FillMode = RendererFlag::FillMode::Solid;
        RendererFlag::CullMode CullMode = RendererFlag::CullMode::Back;
        RendererFlag::Culling  Culling = RendererFlag::Culling::CounterClockWise;
    } RasterizerDescriptor;
    inline void SetFillMode(const RendererFlag::FillMode& mode)  { RasterizerDescriptor.FillMode = mode; }
    inline void SetCullMode(const RendererFlag::CullMode& mode)  { RasterizerDescriptor.CullMode = mode; }
    inline void SetCulling(const RendererFlag::Culling& culling) { RasterizerDescriptor.Culling = culling; }

    uint32 RenderTargetsNum = 0u;
    RendererFlag::BufferFormat RenderTargetFormats[MaxRenderTargetsNum] = {
        RendererFlag::BufferFormat::Unknown,
        RendererFlag::BufferFormat::Unknown,
        RendererFlag::BufferFormat::Unknown,
        RendererFlag::BufferFormat::Unknown,
        RendererFlag::BufferFormat::Unknown,
        RendererFlag::BufferFormat::Unknown,
        RendererFlag::BufferFormat::Unknown,
        RendererFlag::BufferFormat::Unknown
    };
    RendererFlag::BufferFormat DepthTargetFormat = RendererFlag::BufferFormat::Unknown;
    inline void SetRenderTargetFormat(uint32 rtnum, TextureInterface *fbtex)
    {
        RenderTargetsNum = MAX((rtnum + 1), RenderTargetsNum);
        RenderTargetFormats[rtnum] = fbtex->GetFormat();
    }
    inline void SetDepthStencilTarget(TextureInterface* dstex)
    {
        if (dstex == nullptr) {
            DepthTargetFormat = RendererFlag::BufferFormat::Unknown;
        }
        else {
            DepthTargetFormat = dstex->GetFormat();
        }
    }

    struct StencilFaceOperator {
        RendererFlag::StencilOperators FailOperator = RendererFlag::StencilOperators::Keep;
        RendererFlag::StencilOperators DepthFailOperator = RendererFlag::StencilOperators::Keep;
        RendererFlag::StencilOperators PassOperator = RendererFlag::StencilOperators::Replace;
        RendererFlag::TestFlags StencilFunc = RendererFlag::TestFlags::Always;
    };
    struct {
        bool DepthEnabled = false;
        RendererFlag::DepthWriteMask DepthWriteMask = RendererFlag::DepthWriteMask::All;
        RendererFlag::TestFlags DepthFunc = RendererFlag::TestFlags::Never;
        bool StencilEnabled = false;
        uint8 StencilReadMask = 0u;
        uint8 StencilWriteMask = 0u;
        StencilFaceOperator StencilFrontFace;
        StencilFaceOperator StencilBackFace;
    } DepthStencilDescriptor;
    inline void SetDepthEnabled(bool enabled) { DepthStencilDescriptor.DepthEnabled = enabled; }
    inline void SetDepthWriteMask(const RendererFlag::DepthWriteMask& mask) { DepthStencilDescriptor.DepthWriteMask = mask; }
    inline void SetDepthFunc(const RendererFlag::TestFlags& func) { DepthStencilDescriptor.DepthFunc = func; }
    inline void SetStencilEnabled(bool enabled) { DepthStencilDescriptor.StencilEnabled = enabled; }
    inline void SetStencilReadMask(uint8 mask) { DepthStencilDescriptor.StencilReadMask = mask; }
    inline void SetStencilWriteMask(uint8 mask) { DepthStencilDescriptor.StencilWriteMask = mask; }
    inline void SetStencilFrontFace(const RendererFlag::StencilOperators &fail, const RendererFlag::StencilOperators &depthfail, const RendererFlag::StencilOperators &pass, const RendererFlag::TestFlags &func) {
        DepthStencilDescriptor.StencilFrontFace.FailOperator = fail;
        DepthStencilDescriptor.StencilFrontFace.DepthFailOperator = depthfail;
        DepthStencilDescriptor.StencilFrontFace.PassOperator = pass;
        DepthStencilDescriptor.StencilFrontFace.StencilFunc = func;
    }
    inline void SetStencilBackFace(const RendererFlag::StencilOperators& fail, const RendererFlag::StencilOperators& depthfail, const RendererFlag::StencilOperators& pass, const RendererFlag::TestFlags& func) {
        DepthStencilDescriptor.StencilBackFace.FailOperator = fail;
        DepthStencilDescriptor.StencilBackFace.DepthFailOperator = depthfail;
        DepthStencilDescriptor.StencilBackFace.PassOperator = pass;
        DepthStencilDescriptor.StencilBackFace.StencilFunc = func;
    }

    RendererFlag::PrimitiveTopologyType Topology = RendererFlag::PrimitiveTopologyType::Triangle;
    inline void SetPrimitiveTopologyType(const RendererFlag::PrimitiveTopologyType& type) { Topology = type; }

    struct {
        const char* SemanticName;
        uint32 SemanticIndex;
        RendererFlag::BufferFormat Format;
        uint32 InputSlot;
        uint32 AlignedByteOffset;
        RendererFlag::InputClassification InputSlotClass;
        uint32 InstanceDataStepRate;
    } InputElementDescriptors[MaxInputElementsNum];
    uint32 InputElementCount;

    uint64 Hash = 0ull;

    void ComputeHash() {
        const size_t ThisClassSizeAligned = GetSizeAlign(sizeof(PipelineStateDescriptor), 16ull);
        void *HashSource = (void*)MemoryAllocator::Alloc(ThisClassSizeAligned);
        uint8* HashSourcePtr = (uint8*)HashSource;
        ::memset(HashSource, 0, ThisClassSizeAligned);
        ::memcpy(HashSourcePtr, Shaders, sizeof(Shaders));
        HashSourcePtr += sizeof(Shaders);
        ::memcpy(HashSourcePtr, &BlendDescriptor, sizeof(BlendDescriptor));
        HashSourcePtr += sizeof(BlendDescriptor);
        ::memcpy(HashSourcePtr, &RasterizerDescriptor, sizeof(RasterizerDescriptor));
        HashSourcePtr += sizeof(RasterizerDescriptor);
        ::memcpy(HashSourcePtr, &DepthStencilDescriptor, sizeof(DepthStencilDescriptor));
        HashSourcePtr += sizeof(DepthStencilDescriptor);
        ::memcpy(HashSourcePtr, &Topology, sizeof(Topology));
        HashSourcePtr += sizeof(Topology);
        ::memcpy(HashSourcePtr, InputElementDescriptors, sizeof(InputElementDescriptors));

        Hash = Hash::GenerateHash(static_cast<uint64*>(HashSource), ThisClassSizeAligned);
        MemoryAllocator::Free(HashSource);
    }

    void Finalize() {
        ComputeHash();
    }

    bool operator == (const PipelineStateDescriptor& desc) {
        LEASSERT(Hash && desc.Hash);
        return Hash == desc.Hash;
    }
};
}

#endif //  LIMITENGINEV2_PIPELINESTATEDESCRIPTOR_H_