/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LE_ShaderDriverLight.h
 @brief ShaderDriver for lights
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include "Renderer/ShaderDriver.h"

namespace LimitEngine {
    class ShaderDriverLight : public ShaderDriver
    {
        enum LightFlag
        {
            LightFlagNone = 0,
            LightFlagDirectional    = 1<<0,
            LightFlagPoint          = 1<<1,
            LightFlagSpot           = 1<<2,
            LightFlagIBL            = 1<<3,
        };
	public:
		bool IsValid(const ShaderParameterParser::ParameterMap &paramMap) override;
		void Apply(const RenderState &rs, const Material *material) override;
		const char* GetName() const override { return "ShaderDriverLight"; }
	private:
		ShaderDriver* create() override { return new ShaderDriverLight; }
		void setup(const ShaderParameterParser::ParameterMap &paramMap) override;
    private:
        uint32 mLightFlags;
        int32 mBRDFLUT_Position;
        int32 mIBLTexSize_Position;
        int32 mIBLSpcTex_Position;
        int32 mIBLDifTex_Position;
    };
} // LimitEngine
