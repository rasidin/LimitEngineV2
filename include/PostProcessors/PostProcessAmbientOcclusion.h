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
@file  PostProcessorAmbientOcclusion.h
@brief PostProcessor AmbientOcclusion Class
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

#include <LeRenderer>
#include <LEFloatVector2.h>

#include "PostProcessor.h"

#include "Renderer/Shader.h"

namespace LimitEngine {
class PostProcessAmbientOcclusion : public PostProcessor
{
public:
    PostProcessAmbientOcclusion() {}
    virtual ~PostProcessAmbientOcclusion() {}

    virtual void Init(const InitializeOptions& Options) override;
    virtual void Process(PostProcessContext &Context, VectorArray<PooledRenderTarget> &RenderTargets) override;

private:
    bool processAmbientOcclusion(PostProcessContext &Context, VectorArray<PooledRenderTarget> &RenderTargets);
    bool processAmbientOcclusionBlur(PostProcessContext &Context, VectorArray<PooledRenderTarget> &RenderTargets, const LEMath::FloatVector2 &UVDirection);

private:
    ShaderRefPtr mAmbientOcclusionShader;
    ShaderRefPtr mAmbientOcclusionBlurShader;

    struct AmbientOcclusionShader_Parameters
    {
        int Position_ViewMatrix;
        int Position_ViewProjMatrix;
        int Position_UVtoViewParameter;
        int Position_PerspectiveProjectionParameters;
        int Position_AOParameters;
        int Position_AOResolutionParameters;
        int Position_FrameIndexContext;
        int Position_BlueNoiseContext;

        int Position_SceneDepthSampler;
        int Position_SceneDepthTexture;
        int Position_SceneNormalSampler;
        int Position_SceneNormalTexture;
        int Position_BlueNoiseSampler;
        int Position_BlueNoiseTexture;
    } mAmbientOcclusionShader_Parameters;

    struct AmbientOcclusionBlurShader_Parameters
    {
        int Position_AmbientOcclusionTexture;
        int Position_AmbientOcclusionSampler;
        int Position_BlurParameters;
    } mAmbientOcclusionBlurShader_Parameters;
};
} // LimitEngine