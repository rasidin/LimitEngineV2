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
@file  PostProcessAmbientOcclusion.cpp
@brief PostProcessAmbientOcclusion Class
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#include "PostProcessors/PostProcessAmbientOcclusion.h"

#include "Renderer/DrawCommand.h"
#include "Renderer/SamplerState.h"
#include "Managers/Draw2DManager.h"
#include "Managers/SceneManager.h"
#include "Managers/ShaderManager.h"
#include "Managers/RenderTargetPoolManager.h"

namespace LimitEngine {
void PostProcessAmbientOcclusion::Init(const InitializeOptions &Options)
{
    mAmbientOcclusionShader = LE_ShaderManager.GetShader("AmbientOcclusion");
    if (mAmbientOcclusionShader.IsValid())
    {
        //mAmbientOcclusionCB = new ConstantBuffer();
        //mAmbientOcclusionCB->Create(mAmbientOcclusionShader.Get());

        //mAmbientOcclusionShader_Parameters.Position_ViewMatrix = mAmbientOcclusionShader->GetUniformLocation("ViewMatrix");
        //mAmbientOcclusionShader_Parameters.Position_ViewProjMatrix = mAmbientOcclusionShader->GetUniformLocation("ViewProjectionMatrix");
        //mAmbientOcclusionShader_Parameters.Position_UVtoViewParameter = mAmbientOcclusionShader->GetUniformLocation("UVtoViewParameter");
        //mAmbientOcclusionShader_Parameters.Position_PerspectiveProjectionParameters = mAmbientOcclusionShader->GetUniformLocation("PerspectiveProjectionParameters");
        //mAmbientOcclusionShader_Parameters.Position_AOParameters = mAmbientOcclusionShader->GetUniformLocation("AOParameters");
        //mAmbientOcclusionShader_Parameters.Position_AOResolutionParameters = mAmbientOcclusionShader->GetUniformLocation("AOResolutionParameters");
        //mAmbientOcclusionShader_Parameters.Position_FrameIndexContext = mAmbientOcclusionShader->GetUniformLocation("FrameIndexContext");
        //mAmbientOcclusionShader_Parameters.Position_BlueNoiseContext = mAmbientOcclusionShader->GetUniformLocation("BlueNoiseContext");

        //mAmbientOcclusionShader_Parameters.Position_SceneDepthSampler = mAmbientOcclusionShader->GetTextureLocation("SceneDepthSampler");
        //mAmbientOcclusionShader_Parameters.Position_SceneNormalSampler = mAmbientOcclusionShader->GetTextureLocation("SceneNormalSampler");
        //mAmbientOcclusionShader_Parameters.Position_BlueNoiseSampler = mAmbientOcclusionShader->GetTextureLocation("BlueNoiseSampler");
        //mAmbientOcclusionShader_Parameters.Position_SceneDepthTexture = mAmbientOcclusionShader->GetTextureLocation("SceneDepthTexture");
        //mAmbientOcclusionShader_Parameters.Position_SceneNormalTexture = mAmbientOcclusionShader->GetTextureLocation("SceneNormalTexture");
        //mAmbientOcclusionShader_Parameters.Position_BlueNoiseTexture = mAmbientOcclusionShader->GetTextureLocation("BlueNoiseTexture");
    }

    mAmbientOcclusionBlurShader = LE_ShaderManager.GetShader("AmbientOcclusionBlur");
    if (mAmbientOcclusionBlurShader.IsValid())
    {
        //mAmbientOcclusionBlurCB = new ConstantBuffer();
        //mAmbientOcclusionBlurCB->Create(mAmbientOcclusionBlurShader.Get());

        //mAmbientOcclusionBlurShader_Parameters.Position_AmbientOcclusionTexture = mAmbientOcclusionBlurShader->GetTextureLocation("AmbientOcclusionTexture");
        //mAmbientOcclusionBlurShader_Parameters.Position_AmbientOcclusionSampler = mAmbientOcclusionBlurShader->GetTextureLocation("AmbientOcclusionSampler");
        //mAmbientOcclusionBlurShader_Parameters.Position_BlurParameters = mAmbientOcclusionBlurShader->GetUniformLocation("BlurParameters");
    }
}

bool PostProcessAmbientOcclusion::processAmbientOcclusion(PostProcessContext &Context, VectorArray<PooledRenderTarget> &RenderTargets)
{
    //RenderTargetDesc AmbientOcclusionBufferDesc(
    //    Context.SceneColor.GetDesc().Size,
    //    1,
    //    TEXTURE_COLOR_FORMAT_G16R16F
    //);
    //PooledRenderTarget AmbientOcclusionBuffer = LE_RenderTargetPoolManager.GetRenderTarget(AmbientOcclusionBufferDesc);
    //if (mAmbientOcclusionShader.IsValid()) {
    //    PooledRenderTarget Output = LE_RenderTargetPoolManager.GetRenderTarget(AmbientOcclusionBufferDesc);
    //    DrawCommand::ResourceBarrier(Output.Get(), ResourceState::RenderTarget);
    //    DrawCommand::SetRenderTarget(0, Output.Get(), nullptr);

    //    DrawCommand::SetShaderUniformMatrix4(
    //        mAmbientOcclusionShader.Get(),
    //        mAmbientOcclusionCB.Get(),
    //        mAmbientOcclusionShader_Parameters.Position_ViewMatrix,
    //        sizeof(LEMath::FloatMatrix4x4),
    //        (float*)&Context.RenderStateContext->GetViewMatrix()
    //    );
    //    DrawCommand::SetShaderUniformMatrix4(
    //        mAmbientOcclusionShader.Get(),
    //        mAmbientOcclusionCB.Get(),
    //        mAmbientOcclusionShader_Parameters.Position_ViewProjMatrix,
    //        sizeof(LEMath::FloatMatrix4x4),
    //        (float*)&Context.RenderStateContext->GetViewProjMatrix()
    //    );
    //    DrawCommand::SetShaderUniformFloat4(
    //        mAmbientOcclusionShader.Get(),
    //        mAmbientOcclusionCB.Get(),
    //        mAmbientOcclusionShader_Parameters.Position_UVtoViewParameter,
    //        LE_SceneManager.GetUVtoViewParameter());
    //    DrawCommand::SetShaderUniformFloat4(
    //        mAmbientOcclusionShader.Get(),
    //        mAmbientOcclusionCB.Get(),
    //        mAmbientOcclusionShader_Parameters.Position_PerspectiveProjectionParameters,
    //        LE_SceneManager.GetPerspectiveProjectionParameters());
    //    DrawCommand::SetShaderUniformFloat4(
    //        mAmbientOcclusionShader.Get(),
    //        mAmbientOcclusionCB.Get(),
    //        mAmbientOcclusionShader_Parameters.Position_AOParameters,
    //        LEMath::FloatVector4(1.0f, 1.0f * 1.0f, 1.0f / 1.0f, 0.4f));
    //    DrawCommand::SetShaderUniformFloat4(
    //        mAmbientOcclusionShader.Get(),
    //        mAmbientOcclusionCB.Get(),
    //        mAmbientOcclusionShader_Parameters.Position_AOResolutionParameters,
    //        LEMath::FloatVector4((float)AmbientOcclusionBufferDesc.Size.Width(), (float)AmbientOcclusionBufferDesc.Size.Height(), 1.0f / AmbientOcclusionBufferDesc.Size.Width(), 1.0f / AmbientOcclusionBufferDesc.Size.Height()));
    //    DrawCommand::SetShaderUniformInt4(
    //        mAmbientOcclusionShader.Get(),
    //        mAmbientOcclusionCB.Get(),
    //        mAmbientOcclusionShader_Parameters.Position_FrameIndexContext,
    //        Context.FrameIndexContext
    //    );
    //    DrawCommand::SetShaderUniformFloat4(
    //        mAmbientOcclusionShader.Get(),
    //        mAmbientOcclusionCB.Get(),
    //        mAmbientOcclusionShader_Parameters.Position_BlueNoiseContext,
    //        Context.BlueNoiseContext);
    //    DrawCommand::BindSampler(mAmbientOcclusionShader_Parameters.Position_BlueNoiseSampler,
    //        SamplerState::Get(SamplerStateDesc(
    //            SamplerStateFilter::MIN_MAG_MIP_POINT,
    //            SamplerStateAddressMode::Wrap,
    //            SamplerStateAddressMode::Wrap,
    //            SamplerStateAddressMode::Wrap
    //        )));
    //    DrawCommand::BindTexture(mAmbientOcclusionShader_Parameters.Position_BlueNoiseTexture, Context.BlueNoiseTexture.Get());
    //    DrawCommand::BindSampler(mAmbientOcclusionShader_Parameters.Position_SceneDepthSampler,
    //        SamplerState::Get(SamplerStateDesc(
    //            SamplerStateFilter::MIN_MAG_MIP_POINT,
    //            SamplerStateAddressMode::Wrap,
    //            SamplerStateAddressMode::Wrap,
    //            SamplerStateAddressMode::Wrap
    //        )));
    //    DrawCommand::BindTexture(mAmbientOcclusionShader_Parameters.Position_SceneDepthTexture, Context.SceneDepth);
    //    DrawCommand::BindSampler(mAmbientOcclusionShader_Parameters.Position_SceneNormalSampler,
    //        SamplerState::Get(SamplerStateDesc(
    //            SamplerStateFilter::MIN_MAG_MIP_LINEAR,
    //            SamplerStateAddressMode::Wrap,
    //            SamplerStateAddressMode::Wrap,
    //            SamplerStateAddressMode::Wrap
    //        )));
    //    DrawCommand::BindTexture(mAmbientOcclusionShader_Parameters.Position_SceneNormalTexture, Context.SceneNormal);

    //    LE_Draw2DManager.DrawScreen(mAmbientOcclusionShader.Get(), mAmbientOcclusionCB.Get());

    //    DrawCommand::ResourceBarrier(Output.Get(), ResourceState::Common);
    //    RenderTargets[0] = Output;

    //    return true;
    //}
    return false;
}

bool PostProcessAmbientOcclusion::processAmbientOcclusionBlur(PostProcessContext &Context, VectorArray<PooledRenderTarget> &RenderTargets, const LEMath::FloatVector2 &UVDirection)
{
    //RenderTargetDesc AmbientOcclusionBlurBufferDesc(
    //    Context.SceneColor.GetDesc().Size,
    //    1,
    //    TEXTURE_COLOR_FORMAT_G16R16F
    //);
    //if (mAmbientOcclusionBlurShader.IsValid()) {
    //    PooledRenderTarget Output = LE_RenderTargetPoolManager.GetRenderTarget(AmbientOcclusionBlurBufferDesc);
    //    DrawCommand::ResourceBarrier(Output.Get(), ResourceState::RenderTarget);
    //    DrawCommand::SetRenderTarget(0, Output.Get(), nullptr);

    //    DrawCommand::SetShaderUniformFloat4(
    //        mAmbientOcclusionBlurShader.Get(),
    //        mAmbientOcclusionBlurCB.Get(),
    //        mAmbientOcclusionBlurShader_Parameters.Position_BlurParameters,
    //        LEMath::FloatVector4(UVDirection.X(), UVDirection.Y(), 1.0f / 25.0f * 2.78f, 0.1f));
    //    DrawCommand::BindSampler(mAmbientOcclusionBlurShader_Parameters.Position_AmbientOcclusionSampler,
    //        SamplerState::Get(SamplerStateDesc(
    //            SamplerStateFilter::MIN_MAG_MIP_POINT,
    //            SamplerStateAddressMode::Clamp,
    //            SamplerStateAddressMode::Clamp,
    //            SamplerStateAddressMode::Clamp
    //        )));
    //    DrawCommand::BindTexture(mAmbientOcclusionBlurShader_Parameters.Position_AmbientOcclusionTexture, RenderTargets[0]);

    //    LE_Draw2DManager.DrawScreen(mAmbientOcclusionBlurShader.Get(), mAmbientOcclusionBlurCB.Get());

    //    DrawCommand::ResourceBarrier(Output.Get(), ResourceState::Common);
    //    RenderTargets[0] = Output;
    //    return true;
    //}
    return true;
}

void PostProcessAmbientOcclusion::Process(PostProcessContext &Context, VectorArray<PooledRenderTarget> &RenderTargets)
{
    //if (processAmbientOcclusion(Context, RenderTargets)) {
    //    LEMath::IntSize aoResultSize = RenderTargets[0].Get()->GetSize();
    //    // BlurX
    //    if (processAmbientOcclusionBlur(Context, RenderTargets, LEMath::FloatVector2(1.0f / aoResultSize.X(), 0.0f))) {
    //        // BlurY
    //        if (processAmbientOcclusionBlur(Context, RenderTargets, LEMath::FloatVector2(0.0f, 1.0f / aoResultSize.Y()))) {
    //        }
    //    }
    //}
}
}