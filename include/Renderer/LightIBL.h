/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_LightIBL.h
 @brief IBL Light Class
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2016/11/09 Created by minseob
 ***********************************************************/
#pragma once
#ifndef _LE_LIGHTIBL_H_
#define _LE_LIGHTIBL_H_

#include <LERenderer>

#include "Renderer/Light.h"
#include "Renderer/RenderState.h"

namespace LimitEngine {
    class RendererTask_SetupDrawIBLParameter;
    class LightIBL : public Light
    {
        friend RendererTask_SetupDrawIBLParameter;
    public:
        LightIBL();
        virtual ~LightIBL();
        void DrawBackground(const RenderState &rs) override;

        TYPE GetType()                            { return TYPE_IBL; }

        void Update();

        void SetIBLSpecularTexture(Texture *tex) { mIBLSpecularTexture = tex; }
        Texture* GetIBLSpecularTexture() const { return mIBLSpecularTexture; }
        void SetIBLDiffuseTexture(Texture *tex) { mIBLDiffuseTexture = tex; }
        Texture* GetIBLDiffuseTexture() const { return mIBLDiffuseTexture; }

        void MakeIBLSpecularMipmaps();

        Texture* GetBRDFLUTTexture() const { return mBRDFLUTTexture; }
    protected:
        void VariableChanged(const MetaDataVariable &mdv) override;
    private:
        bool mMadeIBLMipmaps;

        String mIBLSpecularTexturePath;
        Texture *mIBLSpecularTexture;
        String mIBLDiffuseTexturePath;
        Texture *mIBLDiffuseTexture;
        Texture *mBRDFLUTTexture;

        Shader *mDrawIBLShader;
        struct DrawIBLShaderParameter {
            int iblSpcTexLocation;
            int iblDifTexLocation;
            int iblUVConvLocation;
            int viewProjInvMatrixLocation;
            DrawIBLShaderParameter()
                : iblSpcTexLocation(-1)
                , iblDifTexLocation(-1)
                , iblUVConvLocation(0)
                , viewProjInvMatrixLocation(0)
            {}
            void Setup(Shader*);
        } mDrawIBLShaderParam;
#if PIXELSHADER_VERSION >= 4
        Shader *mMakeIBLMipmapShader;
        struct MakeIBLMipmapShaderParameter {
            int iblSpcTexLocation;
            int roughnessLocation;
            MakeIBLMipmapShaderParameter()
                : iblSpcTexLocation(-1)
                , roughnessLocation(0)
            {}
            void Setup(Shader*);
        } mMakeIBLMipmapShaderParameter;
#endif
    }; // LightIBL
} // LimitEngine

#endif // _LE_LIGHTIBL_H_