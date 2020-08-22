/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ShaderDriverParameter.h
 @brief Parameter driver for shader
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include "Renderer/ShaderDriverParameter.h"

#include "Managers/DrawManager.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderParameterparser.h"
#include "Renderer/Material.h"
#include "Renderer/RenderContext.h"

namespace LimitEngine {
    ShaderDriverParameter::~ShaderDriverParameter()
    {
    }
    bool ShaderDriverParameter::IsValid(const ShaderParameterParser::ParameterMap &paramMap) const
    {
        for(uint32 pmidx=0;pmidx<paramMap.size();pmidx++) {
            if(paramMap.GetAt(pmidx).value.attributes.FindIndex("type") >= 0 && paramMap.GetAt(pmidx).value.attributes["type"] == "Parameter") {
                return true;
            }
        }
        return false;
    }
    void ShaderDriverParameter::Apply(const RenderState &rs, const Material *material)
    {
        for(uint32 pmidx=0;pmidx<mParameters.size();pmidx++) {
            ShaderParameter &param = mParameters.GetAt(pmidx).value;
            if (param.paramLocation < 0)
                continue;
            switch(param.type) {
            case ShaderParameter::ParameterType_Texture:
                if(param.data.texture) {
                    DrawCommand::BindTexture(param.paramLocation, param.data.texture);
                } else {
                    DrawCommand::BindTexture(param.paramLocation, RenderContext::GetSingleton().GetDefaultTexture(RenderContext::DefaultTextureTypeWhite));
                }
                break;
            case ShaderParameter::ParameterType_Float4x4:
                DrawCommand::SetShaderUniformMatrix4(mOwnerShader, material->GetConstantBuffer(rs.GetRenderPass()).Get(), param.paramLocation, sizeof(float) * 16, param.data.floatData);
                break;
            case ShaderParameter::ParameterType_Float:
                DrawCommand::SetShaderUniformFloat1(mOwnerShader, material->GetConstantBuffer(rs.GetRenderPass()).Get(), param.paramLocation, param.data.floatData[0]);
                break;
            case ShaderParameter::ParameterType_Float2:
                break;
            case ShaderParameter::ParameterType_Float3:
                break;
            case ShaderParameter::ParameterType_Float4:
                break;
            }
        }
    }
    void ShaderDriverParameter::setup(const ShaderParameterParser::ParameterMap &paramMap) 
    {
        if(mOwnerShader == NULL) 
            return;

        for(uint32 pmidx=0;pmidx<paramMap.size();pmidx++) {
            ShaderParameterParser::ParameterMap::MapArrayItem &item = paramMap.GetAt(pmidx);
            if(item.value.attributes.FindIndex("type") >= 0 && item.value.attributes["type"] == "Parameter") {
                ShaderParameter &shaderParam = mParameters[item.key];
                shaderParam.name = item.key;
                // Set type
                if(item.value.type == "Texture2D" || item.value.type == "SamplerState") {
                    shaderParam.type = ShaderParameter::ParameterType_Texture;
                    shaderParam.paramLocation = getShaderTextureLocation(paramMap, item.key);
                    shaderParam.sampler = item.value.attributes["sampler"];
                    shaderParam.data.texture = NULL;
                } else if(item.value.type == "float4x4") {
                    shaderParam.type = ShaderParameter::ParameterType_Float4x4;
                    shaderParam.paramLocation = getShaderUniformLocation(paramMap, item.key);
                } else if(item.value.type == "float") {
                    shaderParam.type = ShaderParameter::ParameterType_Float;
                    shaderParam.paramLocation = getShaderUniformLocation(paramMap, item.key);
                    if(item.value.attributes.FindIndex("default") >= 0) {
                        shaderParam.data.floatData[0] = item.value.attributes["default"].ToFloat();
                    }
                } else if(item.value.type == "float2") {
                    shaderParam.type = ShaderParameter::ParameterType_Float2;
                    shaderParam.paramLocation = getShaderUniformLocation(paramMap, item.key);
                } else if(item.value.type == "float3") {
                    shaderParam.type = ShaderParameter::ParameterType_Float3;
                    shaderParam.paramLocation = getShaderUniformLocation(paramMap, item.key);
                } else if(item.value.type == "float4") {
                    shaderParam.type = ShaderParameter::ParameterType_Float4;
                    shaderParam.paramLocation = getShaderUniformLocation(paramMap, item.key);
                }
            }
        }
    }
}