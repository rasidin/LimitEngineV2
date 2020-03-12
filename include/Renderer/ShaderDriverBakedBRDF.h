/***********************************************************
LIMITEngine Source File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  ShaderDriverBakedBRDF.h
@brief ShaderDriver for baked brdf lighting
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

#include "Renderer/Material.h"
#include "Renderer/ShaderDriver.h"
#include "Renderer/ShaderParameterParser.h"

namespace LimitEngine {
	class RendererTask_GenerateBakedBRDFTexture;
	class ShaderDriverBakedBRDF : public ShaderDriver
	{
		friend RendererTask_GenerateBakedBRDFTexture;
	public:
		ShaderDriverBakedBRDF() : mBakedBRDF_Position(InvalidShaderPosition), mBRDFLUT_Position(InvalidShaderPosition), mBakedBRDFTexture(NULL), mBRDFLUTTexture(NULL){}
		virtual ~ShaderDriverBakedBRDF();
	public:
		bool IsValid(const ShaderParameterParser::ParameterMap &paramMap) override;
		void Apply(const RenderState &rs, const Material *material) override;
		const char* GetName() const override { return "ShaderDriverBakedBRDF"; }
	private:
		ShaderDriver* create() override { return new ShaderDriverBakedBRDF; }
		void setup(const ShaderParameterParser::ParameterMap &paramMap) override;
	private:
		int32 mBakedBRDF_Position;
		int32 mBRDFLUT_Position;

		Texture *mBakedBRDFTexture;
		Texture *mBRDFLUTTexture;
	};
}
