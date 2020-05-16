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
	public:
		bool IsValid(const ShaderParameterParser::ParameterMap &paramMap) const override;
        bool IsValid(const Shader *InShader) const override;
		void Apply(const RenderState &rs, const Material *material) override;
		const char* GetName() const override { return "ShaderDriverLight"; }
	private:
		ShaderDriver* create() override { return new ShaderDriverLight; }
		void setup(const ShaderParameterParser::ParameterMap &paramMap) override;
        void setup(const Shader *InShader) override;
    private:
        int32 mIBLRefTex_Position;
        int32 mIBLIrrTex_Position;
        int32 mEnvBRDFTex_Position;
        int32 mTemporalContext_Position;
    };
} // LimitEngine
