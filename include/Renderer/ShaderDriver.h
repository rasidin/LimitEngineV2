/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ShaderDriver.h
 @brief Driver for shader
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LERenderer>

#include "Core/Object.h"
#include "Renderer/ShaderParameterParser.h"
#include "Renderer/RenderState.h"

namespace LimitEngine {
	class ShaderDriver : public Object<LimitEngineMemoryCategory_Graphics>
	{
	protected:
		enum
		{
			InvalidShaderPosition = -1,
		};
	public:
		virtual ~ShaderDriver() {}

		// Is driver valid?
		virtual bool IsValid(const ShaderParameterParser::ParameterMap &mapParam) = 0;
		// Create own class (duplicate)
		ShaderDriver* Create(Shader *shader, const ShaderParameterParser::ParameterMap &mapParam);
		// Apply shader driver
		virtual void Apply(const RenderState &rs, const Material *material) = 0;
		// ShaderDriverName
		virtual const char* GetName() const = 0;

	protected:
		virtual ShaderDriver* create() = 0;
		virtual void setup(const ShaderParameterParser::ParameterMap &paramMap) = 0;
		int getShaderUniformLocation(const ShaderParameterParser::ParameterMap &mapParam, const String &paramWord);
		int getShaderTextureLocation(const ShaderParameterParser::ParameterMap &mapParam, const String &paramWord);

		static bool IsValidShaderParameterPosition(int32 v) { return v >= 0; }

	protected:
		Shader *mOwnerShader;
	};
} // namespace LimitEngine
