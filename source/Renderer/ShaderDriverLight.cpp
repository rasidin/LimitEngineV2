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
        return paramMap.FindIndex("IBLDiffuseTexture") >= 0;
    }
    bool ShaderDriverLight::IsValid(const Shader *InShader) const
    {
        return InShader->GetUniformLocation("IBLDiffuseTexture") >= 0;
    }
	void ShaderDriverLight::Apply(const RenderState &rs, const Material *material)
    {
        if(IsValidShaderParameterPosition(mBRDFLUT_Position)) {
            DrawCommand::BindTexture(mBRDFLUT_Position, rs.GetBRDFLUT());
        }
        if (IsValidShaderParameterPosition(mIBLTexSize_Position)) {
            if (Texture *iblSpecTex = rs.GetIBLSpecularTexture()) {
                LEMath::IntSize texSize = iblSpecTex->GetSize();
                if (texSize.Width() == 0 || texSize.Height() == 0) { // Default設定
                    texSize.SetWidth(1024);
                    texSize.SetHeight(1024);
                }
                DrawCommand::SetShaderUniformFloat2(material->GetShader(), mIBLTexSize_Position, (LEMath::DataContainer)texSize);
            }
        }
        if(IsValidShaderParameterPosition(mIBLSpcTex_Position)) {
            DrawCommand::BindTexture(mIBLSpcTex_Position, rs.GetIBLSpecularTexture());
        }
        if(IsValidShaderParameterPosition(mIBLDifTex_Position)) {
            DrawCommand::BindTexture(mIBLDifTex_Position, rs.GetIBLDiffuseTexture());
        }
    }
	void ShaderDriverLight::setup(const ShaderParameterParser::ParameterMap &paramMap)
    {
        mBRDFLUT_Position =    getShaderTextureLocation(paramMap, "BRDFLUT");
        mIBLTexSize_Position = getShaderUniformLocation(paramMap, "IBLTextureSize");
		mIBLSpcTex_Position =  getShaderTextureLocation(paramMap, "IBLSpecularTexture");
        mIBLDifTex_Position =  getShaderTextureLocation(paramMap, "IBLDiffuseTexture");
    }
    void ShaderDriverLight::setup(const Shader *InShader)
    {
        mBRDFLUT_Position = InShader->GetUniformLocation("BRDFLUT");
        mIBLTexSize_Position = InShader->GetUniformLocation("IBLTextureSize");
        mIBLSpcTex_Position = InShader->GetUniformLocation("IBLSpecularTexture");
        mIBLDifTex_Position = InShader->GetUniformLocation("IBLDiffuseTexture");
    }
}