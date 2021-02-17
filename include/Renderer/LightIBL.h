/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LightIBL.h
 @brief IBL Light Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/
#pragma once

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

        TYPE GetType()                            { return TYPE_IBL; }

        void Update();

        void          SetIBLReflectionTexture(const TextureRefPtr &tex) { mIBLReflectionTexture = tex; }
        TextureRefPtr GetIBLReflectionTexture() const                   { return mIBLReflectionTexture; }
        void          SetIBLIrradianceTexture(const TextureRefPtr &tex) { mIBLIrradianceTexture = tex; }
        TextureRefPtr GetIBLIrradianceTexture() const                   { return mIBLIrradianceTexture; }
    protected:
        void VariableChanged(const MetaDataVariable &mdv) override;
    private:
        bool mMadeIBLMipmaps;

        TextureRefPtr mIBLReflectionTexture;
        TextureRefPtr mIBLIrradianceTexture;
    }; // LightIBL
} // LimitEngine
