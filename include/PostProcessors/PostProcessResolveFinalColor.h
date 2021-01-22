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
@file PostProcessResolveFinalColor.h
@brief Post processor for Resolving final color (to screen, last frame buffer)
@author minseob
**********************************************************************/
#ifndef LIMITENGINEV2_POSTPROCESS_POSTPROCESSRESOLVEFINALCOLOR_H_
#define LIMITENGINEV2_POSTPROCESS_POSTPROCESSRESOLVEFINALCOLOR_H_

#include <LERenderer>
#include "PostProcessor.h"

#include "Core/ReferenceCountedPointer.h"
#include "Containers/VectorArray.h"
#include "Renderer/Shader.h"
#include "Renderer/PipelineState.h"

namespace LimitEngine {
class PostProcessResolveFinalColor : public PostProcessor
{
public:
    enum class ColorSpace : uint8 {
        sRGB = 0,
        PQ,
        Linear,
        Num
    };

public:
    PostProcessResolveFinalColor() {}
    virtual ~PostProcessResolveFinalColor() {}

    // Interface at PostProcessor
    virtual void Init(const InitializeOptions &Options) override;
    virtual void Process(PostProcessContext &Context, VectorArray<PooledRenderTarget> &RenderTargets) override;

private:
    ColorSpace mColorSpace;

    ShaderRefPtr mResolveShader;
    ConstantBufferRefPtr mConstantBuffer = nullptr;
    PipelineStateRefPtr mPipelineState = nullptr;
};
} // LimitEngine
#endif // LIMITENGINEV2_POSTPROCESS_POSTPROCESSRESOLVEFINALCOLOR_H_