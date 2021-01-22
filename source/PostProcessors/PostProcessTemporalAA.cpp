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

#include "Renderer/DrawCommand.h"
#include "Managers/Draw2DManager.h"
#include "Managers/ShaderManager.h"
#include "Managers/RenderTargetPoolManager.h"

namespace LimitEngine {
void PostProcessTemporalAA::Init(const InitializeOptions &Options)
{
    //mTemporalAAShader = LE_ShaderManager.GetShader("TemporalAA");
    //mShaderParameterPosition_SceneColor = 0; // mTemporalAAShader->GetTextureLocation("SceneColor");
    //mShaderParameterPosition_SceneColorHistory = 1; // mTemporalAAShader->GetTextureLocation("SceneColorHistory");
}

void PostProcessTemporalAA::Process(PostProcessContext &Context, VectorArray<PooledRenderTarget> &RenderTargets)
{
    return;
    //if (!mTemporalAAShader.IsValid()) return;

    //PooledRenderTarget Output = LE_RenderTargetPoolManager.GetRenderTarget(Context.SceneColor.GetDesc());
    //DrawCommand::ResourceBarrier(Output.Get(), ResourceState::RenderTarget);
    //DrawCommand::SetRenderTarget(0, Output.Get(), nullptr);
    //DrawCommand::BindTexture(mShaderParameterPosition_SceneColor, RenderTargets[0]);
    //if (mHistorySceneColor.Get())
    //    DrawCommand::BindTexture(mShaderParameterPosition_SceneColorHistory, mHistorySceneColor);
    //else
    //    DrawCommand::BindTexture(mShaderParameterPosition_SceneColorHistory, RenderTargets[0]);
    //DrawCommand::SetDepthFunc(RendererFlag::TestFlags::Always);
    //DrawCommand::SetBlendFunc(0, RendererFlag::BlendFlags::Source);

    //LE_Draw2DManager.DrawScreen(mTemporalAAShader.Get());

    //DrawCommand::ResourceBarrier(Output.Get(), ResourceState::Common);

    //mHistorySceneColor = Output;
    //RenderTargets[0] = Output;
}
} // LimitEngine