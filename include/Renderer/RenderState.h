/*******************************************************************
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
@file  RenderState.h
@brief Render state
@author minseob (https://github.com/rasidin)
*********************************************************************/
#ifndef LIMITENGINEV2_RENDERSTATE_H_
#define LIMITENGINEV2_RENDERSTATE_H_

#include <LERenderer>

#include <LEIntVector4.h>
#include <LEFloatVector4.h>
#include <LEFloatMatrix4x4.h>

#include "Core/Object.h"
#include "Core/ReferenceCountedPointer.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/Texture.h"
#include "Renderer/PipelineStateDescriptor.h"
#include "Renderer/SamplerState.h"

#include "Shaders/Standard_prepass.vs.h"
#include "Shaders/Standard_prepass.ps.h"
#include "Shaders/Standard_basepass.vs.h"
#include "Shaders/Standard_basepass.ps.h"

namespace LimitEngine {
class RenderState : public Object<LimitEngineMemoryCategory::Graphics>
{
public:
    struct ShaderDriverForRenderState
    {
        LEMath::IntVector4*     TemporalContext = nullptr;
        LEMath::IntVector4*     FrameIndexContext = nullptr;
        LEMath::FloatVector4*   BlueNoiseContext = nullptr;
        LEMath::FloatVector4*   IBLReflectionInfo = nullptr;
        LEMath::FloatMatrix4x4* ViewMatrix = nullptr;
        LEMath::FloatMatrix4x4* ProjectionMatrix = nullptr;
        LEMath::FloatMatrix4x4* WorldMatrix = nullptr;
        LEMath::FloatMatrix4x4* ViewProjectionMatrix = nullptr;
        LEMath::FloatMatrix4x4* WorldViewProjectionMatrix = nullptr;
        void Setup(const Standard_prepass_VS::ConstantBuffer0 &cb)
        {
            FrameIndexContext = (LEMath::IntVector4*)&cb.FrameIndexContext[0];
            BlueNoiseContext = (LEMath::FloatVector4*)&cb.BlueNoiseContext[0];
            ViewMatrix = (LEMath::FloatMatrix4x4*)&cb.ViewMatrix[0];
            ProjectionMatrix = (LEMath::FloatMatrix4x4*)&cb.ProjectionMatrix[0];
            WorldMatrix = (LEMath::FloatMatrix4x4*)&cb.WorldMatrix[0];
            ViewProjectionMatrix = (LEMath::FloatMatrix4x4*)&cb.ViewProjectionMatrix[0];
            WorldViewProjectionMatrix = (LEMath::FloatMatrix4x4*)&cb.WorldViewProjMatrix[0];
        }
        void Setup(const Standard_basepass_VS::ConstantBuffer0& cb)
        {
            FrameIndexContext = (LEMath::IntVector4*)&cb.FrameIndexContext[0];
            BlueNoiseContext = (LEMath::FloatVector4*)&cb.BlueNoiseContext[0];
            ViewMatrix = (LEMath::FloatMatrix4x4*)&cb.ViewMatrix[0];
            ProjectionMatrix = (LEMath::FloatMatrix4x4*)&cb.ProjectionMatrix[0];
            WorldMatrix = (LEMath::FloatMatrix4x4*)&cb.WorldMatrix[0];
            ViewProjectionMatrix = (LEMath::FloatMatrix4x4*)&cb.ViewProjectionMatrix[0];
            WorldViewProjectionMatrix = (LEMath::FloatMatrix4x4*)&cb.WorldViewProjMatrix[0];
        }
        void Setup(const Standard_basepass_PS::ConstantBuffer0& cb)
        {
            FrameIndexContext = (LEMath::IntVector4*)&cb.FrameIndexContext[0];
            BlueNoiseContext = (LEMath::FloatVector4*)&cb.BlueNoiseContext[0];
            IBLReflectionInfo = (LEMath::FloatVector4*)&cb.IBLReflectionInfo[0];
            ViewMatrix = (LEMath::FloatMatrix4x4*)&cb.ViewMatrix[0];
            ProjectionMatrix = (LEMath::FloatMatrix4x4*)&cb.ProjectionMatrix[0];
            WorldMatrix = (LEMath::FloatMatrix4x4*)&cb.WorldMatrix[0];
            ViewProjectionMatrix = (LEMath::FloatMatrix4x4*)&cb.ViewProjectionMatrix[0];
            WorldViewProjectionMatrix = (LEMath::FloatMatrix4x4*)&cb.WorldViewProjMatrix[0];
        }
    };
    struct TexturePositionForRenderState
    {
        int32 IBLReflectionTexturePosition = -1;
        int32 IBLIrradianceTexturePosition = -1;
        int32 EnvironmentBRDFTexturePosition = -1;
        int32 AmbientOcclusionTexturePosition = -1;
        void Setup(const char **texturenames, uint32 count)
        {
            for (uint32 nameidx = 0; nameidx < count; nameidx++) {
                if (strcmp(texturenames[nameidx], "IBLReflectionTexture") == 0) {
                    IBLReflectionTexturePosition = nameidx;
                }
                else if (strcmp(texturenames[nameidx], "IBLIrradianceTexture") == 0) {
                    IBLIrradianceTexturePosition = nameidx;
                }
                else if (strcmp(texturenames[nameidx], "EnvironmentBRDFTexture") == 0) {
                    EnvironmentBRDFTexturePosition = nameidx;
                }
                else if (strcmp(texturenames[nameidx], "AmbientOcclusionTexture") == 0) {
                    AmbientOcclusionTexturePosition = nameidx;
                }
            }
        }
    };
    struct SamplerPositionForRenderState
    {
        int32 IBLReflectionSamplerPosition = -1;
        int32 IBLIrradianceSamplerPosition = -1;
        int32 EnvironmentBRDFSamplerPosition = -1;
        int32 AmbientOcclusionSamplerPosition = -1;
        void Setup(const char** samplernames, uint32 count)
        {
            for (uint32 nameidx = 0; nameidx < count; nameidx++) {
                if (strcmp(samplernames[nameidx], "IBLReflectionSampler") == 0) {
                    IBLReflectionSamplerPosition = nameidx;
                }
                else if (strcmp(samplernames[nameidx], "IBLIrradianceSampler") == 0) {
                    IBLIrradianceSamplerPosition = nameidx;
                }
                else if (strcmp(samplernames[nameidx], "EnvironmentBRDFSampler") == 0) {
                    EnvironmentBRDFSamplerPosition = nameidx;
                }
                else if (strcmp(samplernames[nameidx], "AmbientOcclusionSampler") == 0) {
                    AmbientOcclusionSamplerPosition = nameidx;
                }
            }
        }
    };

    // ----------------------------------------------------
    // Flags
    // ----------------------------------------------------
    void SetSceneBegan(bool b) { mSceneBegan = b; }
    void SetModelDrawingBegan(bool b) { mModelDrawingBegan = b; }

    bool IsSceneBegan() const { return mSceneBegan; }
    bool IsModelDrawingBegan() const { return mModelDrawingBegan; }

    void SetRenderPass(const RenderPass &InRenderPass) { mRenderPass = InRenderPass; }
    RenderPass GetRenderPass() const { return mRenderPass; }

    void SetTemporalContext(int FrameIndex, int NumTemporalFrames, int SamplesInFrame, int AllSamplesNum)
    {
        mTemporalContext.SetX(FrameIndex);
        mTemporalContext.SetY(NumTemporalFrames);
        mTemporalContext.SetZ(SamplesInFrame);
        mTemporalContext.SetW(AllSamplesNum);
    }
    void SetFrameIndexContext(int FrameIndexMod8, int FrameIndexMod16, int FrameIndexMod32, int FrameIndexMod64)
    {
        mFrameIndexContext.SetX(FrameIndexMod8);
        mFrameIndexContext.SetY(FrameIndexMod16);
        mFrameIndexContext.SetZ(FrameIndexMod32);
        mFrameIndexContext.SetW(FrameIndexMod64);
    }
    const LEMath::IntVector4& GetFrameIndexContext() const { return mFrameIndexContext; }
    const LEMath::IntVector4& GetTemporalContext() const { return mTemporalContext; }

    void SetBlueNoiseContext(const LEMath::FloatVector4 &Context)
    {
        mBlueNoiseContext = Context;
    }
    const LEMath::FloatVector4& GetBlueNoiseContext() const { return mBlueNoiseContext; }

    // ----------------------------------------------------
    // General matrices
    // ----------------------------------------------------
    void SetViewMatrix(         const LEMath::FloatMatrix4x4& view)         { mGeneralMatrices.viewMatrix = view; }
    void SetInvViewMatrix(      const LEMath::FloatMatrix4x4& invView)      { mGeneralMatrices.invViewMatrix = invView; }
    void SetProjMatrix(         const LEMath::FloatMatrix4x4& proj)         { mGeneralMatrices.projMatrix = proj; }
    void SetInvProjMatrix(      const LEMath::FloatMatrix4x4& invProj)      { mGeneralMatrices.invProjMatrix = invProj; }
    void SetViewProjMatrix(     const LEMath::FloatMatrix4x4& viewProj)     { mGeneralMatrices.viewProjMatrix = viewProj; }
    void SetInvViewProjMatrix(  const LEMath::FloatMatrix4x4& invViewProj)  { mGeneralMatrices.invViewProjMatrix = invViewProj; }
    void SetWorldViewProjMatrix(const LEMath::FloatMatrix4x4& wvp)          { mGeneralMatrices.worldViewProjMatrix = wvp; }
    void SetWorldMatrix(        const LEMath::FloatMatrix4x4& world)        { mGeneralMatrices.worldMatrix = world; }

    const LEMath::FloatMatrix4x4& GetViewMatrix() const                     { return mGeneralMatrices.viewMatrix; }
    const LEMath::FloatMatrix4x4& GetInvViewMatrix() const                  { return mGeneralMatrices.invViewMatrix; }
    const LEMath::FloatMatrix4x4& GetProjMatrix() const                     { return mGeneralMatrices.projMatrix; }
    const LEMath::FloatMatrix4x4& GetInvProjMatrix() const                  { return mGeneralMatrices.invProjMatrix; }
    const LEMath::FloatMatrix4x4& GetViewProjMatrix() const                 { return mGeneralMatrices.viewProjMatrix; }
    const LEMath::FloatMatrix4x4& GetInvViewProjMatrix() const              { return mGeneralMatrices.invViewProjMatrix; }
    const LEMath::FloatMatrix4x4& GetWorldViewProjMatrix() const            { return mGeneralMatrices.worldViewProjMatrix; }
    const LEMath::FloatMatrix4x4& GetWorldMatrix() const                    { return mGeneralMatrices.worldMatrix; }

    // ----------------------------------------------------
    // Environment textures
    // ----------------------------------------------------
    void SetIBLReflectionTexture(const TextureRefPtr &ibltex)               { mEnvironmentTextures.iblReflectionTexture = ibltex; }
    TextureRefPtr GetIBLReflectionTexture() const                           { return mEnvironmentTextures.iblReflectionTexture; }
    void SetIBLIrradianceTexture(const TextureRefPtr &ibltex)               { mEnvironmentTextures.iblIrradianceTexture = ibltex; }
    TextureRefPtr GetIBLIrradianceTexture() const                           { return mEnvironmentTextures.iblIrradianceTexture; }
    void SetEnvironmentBRDFTexture(const TextureRefPtr &envBRDFtex)         { mEnvironmentTextures.environmentBRDFTexture = envBRDFtex; }
    TextureRefPtr GetEnvironmentBRDFTexture() const                         { return mEnvironmentTextures.environmentBRDFTexture; }
    void SetBlueNoiseTexture(const TextureRefPtr &blueNoiseTex)             { mBlueNoiseTexture = blueNoiseTex; }
    TextureRefPtr GetAmbientOcclusionTexture() const                        { return mEnvironmentTextures.ambientOcclusionTexture; }
    void SetAmbientOcclusionTexture(const TextureRefPtr &aoTexture)         { mEnvironmentTextures.ambientOcclusionTexture = aoTexture; }
    TextureRefPtr GetBlueNoiseTexture() const                               { return mBlueNoiseTexture; }

    // ----------------------------------------------------
    // Connect to shader
    // ----------------------------------------------------
    void SetToShaderDriver(const ShaderDriverForRenderState &driver) const
    {
        if (driver.TemporalContext)             *driver.TemporalContext = mTemporalContext;
        if (driver.BlueNoiseContext)            *driver.BlueNoiseContext = mBlueNoiseContext;
        if (driver.IBLReflectionInfo && mEnvironmentTextures.iblReflectionTexture)
            *driver.IBLReflectionInfo = LEMath::FloatVector4(
                    log2f((float)mEnvironmentTextures.iblReflectionTexture->GetSize().X()) + 1,
                    0.0f,0.0f,0.0
                );
        if (driver.FrameIndexContext)           *driver.FrameIndexContext = mFrameIndexContext;
        if (driver.ViewMatrix)                  *driver.ViewMatrix = mGeneralMatrices.viewMatrix;
        if (driver.WorldMatrix)                 *driver.WorldMatrix = mGeneralMatrices.worldMatrix;
        if (driver.ProjectionMatrix)            *driver.ProjectionMatrix = mGeneralMatrices.projMatrix;
        if (driver.ViewProjectionMatrix)        *driver.ViewProjectionMatrix = mGeneralMatrices.viewProjMatrix;
        if (driver.WorldViewProjectionMatrix)   *driver.WorldViewProjectionMatrix = mGeneralMatrices.worldViewProjMatrix;
    }
    void SetTextures(const TexturePositionForRenderState& pos) const
    {
        if (pos.IBLReflectionTexturePosition >= 0 && mEnvironmentTextures.iblReflectionTexture.IsValid()) {
            DrawCommand::BindTexture(pos.IBLReflectionTexturePosition, mEnvironmentTextures.iblReflectionTexture.Get());
        }
        if (pos.IBLIrradianceTexturePosition >= 0 && mEnvironmentTextures.iblIrradianceTexture.IsValid()) {
            DrawCommand::BindTexture(pos.IBLIrradianceTexturePosition, mEnvironmentTextures.iblIrradianceTexture.Get());
        }
        if (pos.EnvironmentBRDFTexturePosition >= 0 && mEnvironmentTextures.environmentBRDFTexture.IsValid()) {
            DrawCommand::BindTexture(pos.EnvironmentBRDFTexturePosition, mEnvironmentTextures.environmentBRDFTexture.Get());
        }
        if (pos.AmbientOcclusionTexturePosition >= 0 && mEnvironmentTextures.ambientOcclusionTexture.IsValid()) {
            DrawCommand::BindTexture(pos.AmbientOcclusionTexturePosition, mEnvironmentTextures.ambientOcclusionTexture.Get());
        }
    }
    void SetSamplers(const SamplerPositionForRenderState& pos) const
    {
        if (pos.IBLReflectionSamplerPosition >= 0) {
            DrawCommand::BindSampler(pos.IBLReflectionSamplerPosition, SamplerState::Get({
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
        }
        if (pos.IBLIrradianceSamplerPosition >= 0) {
            DrawCommand::BindSampler(pos.IBLIrradianceSamplerPosition, SamplerState::Get({
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
        }
        if (pos.AmbientOcclusionSamplerPosition >= 0) {
            DrawCommand::BindSampler(pos.AmbientOcclusionSamplerPosition, SamplerState::Get({
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
        }
        if (pos.EnvironmentBRDFSamplerPosition >= 0) {
            DrawCommand::BindSampler(pos.EnvironmentBRDFSamplerPosition, SamplerState::Get({
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
        }
    }

    // ----------------------------------------------------
    // Pipeline state
    // ----------------------------------------------------
    void SetRenderTarget(int rtnum, TextureInterface *color, TextureInterface *depth) {
        if (color) {
            mPipelineStateDescriptor.SetRenderTargetFormat(rtnum, color);
        }
        if (depth) {
            mPipelineStateDescriptor.SetDepthStencilTarget(depth);
        }
    }
    void SetDepthEnabled(bool enabled) { mPipelineStateDescriptor.SetDepthEnabled(enabled); }
    void SetDepthWriteMask(const RendererFlag::DepthWriteMask& mask) { mPipelineStateDescriptor.SetDepthWriteMask(mask); }
    void SetDepthFunc(const RendererFlag::TestFlags& func) { mPipelineStateDescriptor.SetDepthFunc(func); }
    inline const PipelineStateDescriptor& GetPipelineStateDescriptor() const { return mPipelineStateDescriptor; }
private:
    bool mSceneBegan;
    bool mModelDrawingBegan;

    RenderPass mRenderPass;
    PipelineStateDescriptor mPipelineStateDescriptor;

    LEMath::IntVector4 mTemporalContext; // x = FrameIndex, y = NumTemporalFrames, z = Samples in one frame, w = All Samples
    LEMath::IntVector4 mFrameIndexContext; // (mod8, mod16, mod32, mod64)
    LEMath::FloatVector4 mBlueNoiseContext;
    TextureRefPtr mBlueNoiseTexture;

    struct GeneralMatrices {
        LEMath::FloatMatrix4x4  viewMatrix;
        LEMath::FloatMatrix4x4  invViewMatrix;
        LEMath::FloatMatrix4x4  projMatrix;
        LEMath::FloatMatrix4x4  invProjMatrix;
        LEMath::FloatMatrix4x4  viewProjMatrix;
        LEMath::FloatMatrix4x4  invViewProjMatrix;
        LEMath::FloatMatrix4x4  worldMatrix;
        LEMath::FloatMatrix4x4  worldViewProjMatrix;
        GeneralMatrices()
            : viewMatrix()
            , invViewMatrix()
            , projMatrix()
            , invProjMatrix()
            , viewProjMatrix()
            , invViewProjMatrix()
            , worldMatrix()
            , worldViewProjMatrix()
        {}
    } mGeneralMatrices;

    struct EnvironmentTextures {
        TextureRefPtr    iblReflectionTexture;
        TextureRefPtr    iblIrradianceTexture;
        TextureRefPtr    environmentBRDFTexture;
        TextureRefPtr    ambientOcclusionTexture;
    } mEnvironmentTextures;
};
}

#endif // LIMITENGINEV2_RENDERSTATE_H_