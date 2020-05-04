/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LE_ShaderDriverLight.cpp
 @brief ShaderDriver for lights
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include <LEIntVector2.h>

#include "Managers/DrawManager.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderDriverLight.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/Material.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
    class RendererTask_SetupShaderDriverLight : public RendererTask
    {
    public:
        RendererTask_SetupShaderDriverLight()
        {}
        void Run() override
        {
        }
    };
	bool ShaderDriverLight::IsValid(const ShaderParameterParser::ParameterMap &paramMap) const
    {
        return paramMap.FindIndex("IBLIrradianceTexture") >= 0 || paramMap.FindIndex("IBLReflectionTexture") >= 0;
    }
    bool ShaderDriverLight::IsValid(const Shader *InShader) const
    {
        return InShader->GetTextureLocation("IBLIrradianceTexture") >= 0 || InShader->GetTextureLocation("IBLReflectionTexture") >= 0;
    }
	void ShaderDriverLight::Apply(const RenderState &rs, const Material *material)
    {
        if(IsValidShaderParameterPosition(mIBLRefTex_Position)) {
            DrawCommand::BindTexture(mIBLRefTex_Position, rs.GetIBLReflectionTexture().Get());
        }
        if(IsValidShaderParameterPosition(mIBLIrrTex_Position)) {
            DrawCommand::BindTexture(mIBLIrrTex_Position, rs.GetIBLIrradianceTexture().Get());
        }
    }
	void ShaderDriverLight::setup(const ShaderParameterParser::ParameterMap &paramMap)
    {
		mIBLRefTex_Position =  getShaderTextureLocation(paramMap, "IBLReflectionTexture");
        mIBLIrrTex_Position =  getShaderTextureLocation(paramMap, "IBLIrradinaceTexture");
    }
    void ShaderDriverLight::setup(const Shader *InShader)
    {
        mIBLRefTex_Position = InShader->GetTextureLocation("IBLReflectionTexture");
        mIBLIrrTex_Position = InShader->GetTextureLocation("IBLIrradianceTexture");
    }
}