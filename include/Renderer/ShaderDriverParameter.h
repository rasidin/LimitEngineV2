/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ShaderDriverParameter.h
 @brief Parameter driver for shader
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LEFloatVector2.h>
#include <LEFloatVector3.h>
#include <LEFloatVector4.h>
#include <LEFloatMatrix4x4.h>

#include "Containers/MapArray.h"
#include "Renderer/ShaderDriver.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
	class ShaderDriverParameter : public ShaderDriver
	{
	public:
		struct ShaderParameter
		{
			enum ParameterType {
				ParameterType_None,
				ParameterType_Texture,
				ParameterType_Float,
				ParameterType_Float2,
				ParameterType_Float3,
				ParameterType_Float4,
				ParameterType_Float4x4,

				ParameterType_Max,
			};
			ParameterType type;
			String name;
			String sampler;
			int paramLocation;
			union Data {
				LimitEngine::Texture *texture;
				float floatData[16];
				int intData[16];
			} data;
			void SetData(const LEMath::FloatVector2 &v);
			void SetData(const LEMath::FloatVector3 &v);
			void SetData(const LEMath::FloatVector4 &v);
			void SetData(const LEMath::FloatMatrix4x4 &m);
		};
	public:
		virtual ~ShaderDriverParameter();

		bool IsValid(const ShaderParameterParser::ParameterMap &paramMap) override;
		void Apply(const RenderState &rs, const Material *material) override;
		const char* GetName() const override { return "ShaderDriverParameter"; }

		uint32 GetParameterCount() const { return uint32(mParameters.size()); }
		ShaderParameter& GetParameter(uint32 n) { return mParameters.GetAt(n).value; }
		ShaderParameter& GetParameter(const String &name) { return mParameters[name]; }
	private:
		ShaderDriver* create() override { return new ShaderDriverParameter; }
		void setup(const ShaderParameterParser::ParameterMap &paramMap) override;
	private:
		MapArray<String, ShaderParameter> mParameters;
	};
}
