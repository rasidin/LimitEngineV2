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

#include "Managers/Draw2DManager.h"
#include "Managers/SceneManager.h"
#include "Managers/ShaderManager.h"
#include "Managers/RenderTargetPoolManager.h"
#include "Renderer/DrawCommand.h"

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
}

void PostProcessResolveFinalColor::Process(PostProcessContext &Context, VectorArray<PooledRenderTarget> &RenderTargets)
{
    if (!mResolveShader.IsValid()) return;

    DrawCommand::SetRenderTarget(0, nullptr, nullptr);
    DrawCommand::BindTexture(0, RenderTargets[0]);

    DrawCommand::SetDepthFunc(RendererFlag::TestFlags::ALWAYS);
    DrawCommand::SetBlendFunc(0, RendererFlag::BlendFlags::SOURCE);

    CameraRefPtr MainCamera = LE_SceneManager.GetCamera();
    if (MainCamera.IsValid()) {
        int EVOffsetParam = mResolveShader->GetUniformLocation("EVOffset");
        if (EVOffsetParam >= 0) {
            DrawCommand::SetShaderUniformFloat1(mResolveShader.Get(), mResolveCB.Get(), EVOffsetParam, MainCamera->GetExposure());
        }
    }

    LE_Draw2DManager.DrawScreen(mResolveShader.Get(), mResolveCB.Get());
}
} // LimitEngine