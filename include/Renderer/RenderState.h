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
        void SetBRDFLUT(Texture *brdflut)                       { mEnvironmentTextures.brdfLut = brdflut; }
        Texture* GetBRDFLUT() const                             { return mEnvironmentTextures.brdfLut; }
        void SetIBLSpecularTexture(Texture *ibltex)             { mEnvironmentTextures.iblSpecularTexture = ibltex; }
        Texture* GetIBLSpecularTexture() const                  { return mEnvironmentTextures.iblSpecularTexture; }
        void SetIBLDiffuseTexture(Texture *ibltex)              { mEnvironmentTextures.iblDiffuseTexture = ibltex; }
        Texture* GetIBLDiffuseTexture() const                   { return mEnvironmentTextures.iblDiffuseTexture; }

    private:
        bool mSceneBegan;
        bool mModelDrawingBegan;

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
            Texture    *brdfLut;
            Texture    *iblDiffuseTexture;
            Texture    *iblSpecularTexture;
            EnvironmentTextures()
                : brdfLut(NULL)
                , iblDiffuseTexture(NULL)
                , iblSpecularTexture(NULL)
            {}
        } mEnvironmentTextures;
    };
}
