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
@file PostProcessTemporalAA.cpp
@brief Post processor for TemporalAA
@author minseob
**********************************************************************/
#include "PostProcessors/PostProcessTemporalAA.h"

#include "Core/MemoryAllocator.h"

#include "Renderer/DrawCommand.h"
#include "Renderer/PipelineState.h"
#include "Managers/Draw2DManager.h"
#include "Managers/ShaderManager.h"
#include "Managers/RenderTargetPoolManager.h"

#include "Shaders/TemporalAA.ps.h"

namespace LimitEngine {
void PostProcessTemporalAA::Init(const InitializeOptions &Options)
{
    mTemporalAAShader = LE_ShaderManager.GetShader<TemporalAA_PS>();
    mPipelineState = new PipelineState();
}

void PostProcessTemporalAA::Process(PostProcessContext &Context, VectorArray<PooledRenderTarget> &RenderTargets)
{
    if (!mTemporalAAShader.IsValid()) return;

    PooledRenderTarget Output = LE_RenderTargetPoolManager.GetRenderTarget(Context.SceneColor.GetDesc());
    DrawCommand::ResourceBarrier(Output.Get(), ResourceState::RenderTarget);

    if (mPipelineState.IsValid() && mPipelineState->IsValid() == false) {
        PipelineStateDescriptor desc;
        desc.SetRenderTargetBlendEnabled(0, false);
        desc.SetRenderTargetFormat(0, Output.Get());
        desc.SetDepthStencilTarget(nullptr);
        desc.SetDepthEnabled(false);
        desc.SetDepthFunc(RendererFlag::TestFlags::Always);
        desc.SetStencilEnabled(false);
        desc.Shaders[static_cast<int>(Shader::Type::Pixel)] = mTemporalAAShader;

        LE_Draw2DManager.BuildPipelineState(desc);
        desc.Finalize();

        mPipelineState->Init(desc);
    }

    DrawCommand::SetViewport(LEMath::IntRect(0, 0, Output.Get()->GetSize().X(), Output.Get()->GetSize().Y()));
    DrawCommand::SetScissorRect(LEMath::IntRect(0, 0, Output.Get()->GetSize().X(), Output.Get()->GetSize().Y()));
    DrawCommand::SetPipelineState(mPipelineState.Get());

    DrawCommand::SetRenderTarget(0, Output.Get(), nullptr);
    DrawCommand::BindTexture(0, RenderTargets[0]);
    if (mHistorySceneColor.Get())
        DrawCommand::BindTexture(1, mHistorySceneColor);
    else
        DrawCommand::BindTexture(1, RenderTargets[0]);
    DrawCommand::BindSampler(0,
        SamplerState::Get({
                SamplerStateFilter::MIN_MAG_MIP_LINEAR,
                SamplerStateAddressMode::Wrap,
                SamplerStateAddressMode::Wrap,
                SamplerStateAddressMode::Wrap,
                0.0f,
                0,
                RendererFlag::TestFlags::Always,
                {0.0f, 0.0f, 0.0f, 0.0f},
                0.0f,
                0.0f
            }));
    DrawCommand::BindSampler(1,
        SamplerState::Get({
                SamplerStateFilter::MIN_MAG_MIP_LINEAR,
                SamplerStateAddressMode::Wrap,
                SamplerStateAddressMode::Wrap,
                SamplerStateAddressMode::Wrap,
                0.0f,
                0,
                RendererFlag::TestFlags::Always,
                {0.0f, 0.0f, 0.0f, 0.0f},
                0.0f,
                0.0f
            }));

    LE_Draw2DManager.DrawScreen();

    DrawCommand::ResourceBarrier(Output.Get(), ResourceState::GenericRead);

    mHistorySceneColor = Output;
    RenderTargets[0] = Output;
}
} // LimitEngine