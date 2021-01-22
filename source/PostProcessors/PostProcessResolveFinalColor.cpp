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
@file PostProcessResolveFinalColor.cpp
@brief Post processor for Resolving final color
@author minseob
**********************************************************************/
#include "PostProcessors/PostProcessResolveFinalColor.h"

#include "Managers/DrawManager.h"
#include "Managers/Draw2DManager.h"
#include "Managers/SceneManager.h"
#include "Managers/ShaderManager.h"
#include "Managers/RenderTargetPoolManager.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/Shader.h"
#include "Renderer/PipelineStateDescriptor.h"

#include "Shaders/ResolveSceneColorSRGB.ps.h"

namespace LimitEngine {
void PostProcessResolveFinalColor::Init(const InitializeOptions &Options)
{
    switch (Options.OutputColorSpace)
    {
    case InitializeOptions::ColorSpace::sRGB: {
        mResolveShader = LE_ShaderManager.GetShader<ResolveSceneColorSRGB_PS>();
        mColorSpace = ColorSpace::sRGB;
    } break;
    case InitializeOptions::ColorSpace::PQ: {
        //mResolveShader = LE_ShaderManager.GetShader("ResolveSceneColorPQ");
        mColorSpace = ColorSpace::PQ;
    } break;
    }
    mPipelineState = new PipelineState();
    mConstantBuffer = new ConstantBuffer();
}

void PostProcessResolveFinalColor::Process(PostProcessContext &Context, VectorArray<PooledRenderTarget> &RenderTargets)
{
    if (!mResolveShader.IsValid()) return;

    FrameBufferTextureRefPtr framebuffer = LE_DrawManager.GetFrameBufferTexture();

    if (mPipelineState.IsValid() && mPipelineState->IsValid() == false) {
        PipelineStateDescriptor psdesc;
        psdesc.SetRenderTargetBlendEnabled(0, false);
        psdesc.SetRenderTargetFormat(0, framebuffer.Get());
        psdesc.SetDepthEnabled(false);
        psdesc.SetDepthFunc(RendererFlag::TestFlags::Always);
        psdesc.SetStencilEnabled(false);
        psdesc.Shaders[static_cast<int>(Shader::Type::Pixel)] = mResolveShader;

        LE_Draw2DManager.BuildPipelineState(psdesc);
        psdesc.Finalize();

        mPipelineState->Init(psdesc);
    }

    DrawCommand::SetViewport(LEMath::IntRect(0, 0, framebuffer->GetSize().X(), framebuffer->GetSize().Y()));
    DrawCommand::SetScissorRect(LEMath::IntRect(0, 0, framebuffer->GetSize().X(), framebuffer->GetSize().Y()));
    DrawCommand::SetPipelineState(mPipelineState.Get());
    //DrawCommand::SetConstantBuffer(0, mConstantBuffer.Get());

    DrawCommand::ResourceBarrier(framebuffer.Get(), ResourceState::RenderTarget);
    DrawCommand::SetRenderTarget(0, framebuffer.Get(), nullptr);
    //DrawCommand::BindTexture(0, RenderTargets[0]);

    //CameraRefPtr MainCamera = LE_SceneManager.GetCamera();
    //if (MainCamera.IsValid()) {
    //    int EVOffsetParam = mResolveShader->GetUniformLocation("EVOffset");
    //    if (EVOffsetParam >= 0) {
    //        DrawCommand::SetShaderUniformFloat1(mResolveShader.Get(), mResolveCB.Get(), EVOffsetParam, MainCamera->GetExposure());
    //    }
    //}

    LE_Draw2DManager.DrawScreen();
}
} // LimitEngine