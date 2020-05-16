/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_RenderState.h
 @brief Statement for rendering
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2016/11/01 Created by minseob
 ***********************************************************/
#pragma once

#include <LERenderer>

#include <LEFloatMatrix4x4.h>

#include "Core/Object.h"
#include "Core/ReferenceCountedPointer.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
    class RenderState : public Object<LimitEngineMemoryCategory_Graphics>
    {
    public:
        // ----------------------------------------------------
        // Flags
        // ----------------------------------------------------
        void SetSceneBegan(bool b) { mSceneBegan = b; }
        void SetModelDrawingBegan(bool b) { mModelDrawingBegan = b; }

        bool IsSceneBegan() const { return mSceneBegan; }
        bool IsModelDrawingBegan() const { return mModelDrawingBegan; }

        void SetTemporalContext(int FrameIndex, int NumTemporalFrames, int SamplesInFrame, int AllSamplesNum)
        {
            mTemporalContext.SetX(FrameIndex);
            mTemporalContext.SetY(NumTemporalFrames);
            mTemporalContext.SetZ(SamplesInFrame);
            mTemporalContext.SetW(AllSamplesNum);
        }
        const LEMath::IntVector4& GetTemporalContext() const { return mTemporalContext; }

        void SetBlueNoiseContext(float PerTextureSize)
        {
            mBlueNoiseContext.SetX(PerTextureSize);
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

    private:
        bool mSceneBegan;
        bool mModelDrawingBegan;

        LEMath::IntVector4 mTemporalContext; // x = FrameIndex, y = NumTemporalFrames, z = Samples in one frame, w = All Samples
        LEMath::FloatVector4 mBlueNoiseContext;

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
        } mEnvironmentTextures;
    };
}
