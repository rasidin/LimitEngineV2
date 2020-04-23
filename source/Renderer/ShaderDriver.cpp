/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ShaderDriver.cpp
 @brief Driver for shader
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include "Renderer/Shader.h"
#include "Renderer/ShaderDriver.h"
#include "Renderer/ShaderParameterParser.h"

namespace LimitEngine {
	ShaderDriver* ShaderDriver::Create(Shader *shader, const ShaderParameterParser::ParameterMap &mapParam)
	{
		ShaderDriver *output = create();
		output->mOwnerShader = shader;
		output->setup(mapParam);
		return output;
	}
    ShaderDriver* ShaderDriver::Create(Shader *InShader)
    {
        ShaderDriver *output = create();
        output->mOwnerShader = InShader;
        output->setup(InShader);
        return output;
    }
	int ShaderDriver::getShaderUniformLocation(const ShaderParameterParser::ParameterMap &mapParam, const String &paramWord)
	{
		int paramPos = mapParam.FindIndex(paramWord);
		if(paramPos >= 0) {
			const ShaderParameterParser::Parameter &param = mapParam.GetAt(paramPos).value;
			return mOwnerShader->GetUniformLocation(param.value);
		}
		return -1;
	}
	int ShaderDriver::getShaderTextureLocation(const ShaderParameterParser::ParameterMap &mapParam, const String &paramWord)
	{
		int paramPos = mapParam.FindIndex(paramWord);
		if(paramPos >= 0) {
			const ShaderParameterParser::Parameter &param = mapParam.GetAt(paramPos).value;
            int firstResult = mOwnerShader->GetTextureLocation(param.value);
            if (firstResult >= 0)
                return firstResult;

            if (param.attributes.FindIndex("sampler") >= 0) {
                String shaderTextureName = param.attributes["sampler"] + "+" + param.value;
                return mOwnerShader->GetTextureLocation(shaderTextureName);
            }
		}
		return -1;
	}
}