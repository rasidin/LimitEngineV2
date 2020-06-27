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
#include "Renderer/SamplerState.h"

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
            DrawCommand::BindSampler(mIBLRefSam_Position, 
                SamplerState::Get(SamplerStateDesc(
                    SamplerStateFilter::MIN_MAG_MIP_LINEAR,
                    SamplerStateAddressMode::Clamp,
                    SamplerStateAddressMode::Clamp,
                    SamplerStateAddressMode::Clamp
                )));
            DrawCommand::BindTexture(mIBLRefTex_Position, rs.GetIBLReflectionTexture().Get());
        }
        if(IsValidShaderParameterPosition(mIBLIrrTex_Position)) {
            DrawCommand::BindSampler(mIBLIrrSam_Position,
                SamplerState::Get(SamplerStateDesc(
                    SamplerStateFilter::MIN_MAG_MIP_LINEAR,
                    SamplerStateAddressMode::Clamp,
                    SamplerStateAddressMode::Clamp,
                    SamplerStateAddressMode::Clamp
                )));
            DrawCommand::BindTexture(mIBLIrrTex_Position, rs.GetIBLIrradianceTexture().Get());
        }
        if (IsValidShaderParameterPosition(mTemporalContext_Position)) {
            DrawCommand::SetShaderUniformInt4(material->GetShader(rs.GetRenderPass()).Get(), mTemporalContext_Position, rs.GetTemporalContext());
        }
        if (IsValidShaderParameterPosition(mEnvBRDFTex_Position)) {
            DrawCommand::BindSampler(mEnvBRDFSam_Position,
                SamplerState::Get(SamplerStateDesc(
                    SamplerStateFilter::MIN_MAG_MIP_LINEAR,
                    SamplerStateAddressMode::Clamp,
                    SamplerStateAddressMode::Clamp,
                    SamplerStateAddressMode::Clamp
                )));
            DrawCommand::BindTexture(mEnvBRDFTex_Position, rs.GetEnvironmentBRDFTexture().Get());
        }
        if (IsValidShaderParameterPosition(mAOTex_Position)) {
            DrawCommand::BindSampler(mAOSam_Position,
                SamplerState::Get(SamplerStateDesc(
                    SamplerStateFilter::MIN_MAG_MIP_LINEAR,
                    SamplerStateAddressMode::Clamp,
                    SamplerStateAddressMode::Clamp,
                    SamplerStateAddressMode::Clamp
                )));
            DrawCommand::BindTexture(mAOTex_Position, rs.GetAmbientOcclusionTexture().Get());
        }
    }
	void ShaderDriverLight::setup(const ShaderParameterParser::ParameterMap &paramMap)
    {
		mIBLRefTex_Position =  getShaderTextureLocation(paramMap, "IBLReflectionTexture");
        mIBLRefSam_Position =  getShaderTextureLocation(paramMap, "IBLReflectionSampler");
        mIBLIrrTex_Position =  getShaderTextureLocation(paramMap, "IBLIrradianceTexture");
        mIBLIrrSam_Position =  getShaderTextureLocation(paramMap, "IBLIrradianceSampler");
        mEnvBRDFTex_Position = getShaderTextureLocation(paramMap, "EnvironmentBRDFTexture");
        mEnvBRDFSam_Position = getShaderTextureLocation(paramMap, "EnvironmentBRDFSampler");
        mAOTex_Position      = getShaderTextureLocation(paramMap, "AmbientOcclusionTexture");
        mAOSam_Position      = getShaderTextureLocation(paramMap, "AmbientOcclusionSampler");
        mTemporalContext_Position = getShaderUniformLocation(paramMap, "TemporalContext");
    }
    void ShaderDriverLight::setup(const Shader *InShader)
    {
        mIBLRefTex_Position       = InShader->GetTextureLocation("IBLReflectionTexture");
        mIBLRefSam_Position       = InShader->GetTextureLocation("IBLReflectionSampler");
        mIBLIrrTex_Position       = InShader->GetTextureLocation("IBLIrradianceTexture");
        mIBLIrrSam_Position       = InShader->GetTextureLocation("IBLIrradianceSampler");
        mEnvBRDFTex_Position      = InShader->GetTextureLocation("EnvironmentBRDFTexture");
        mEnvBRDFSam_Position      = InShader->GetTextureLocation("EnvironmentBRDFSampler");
        mTemporalContext_Position = InShader->GetUniformLocation("TemporalContext");
        mAOTex_Position           = InShader->GetTextureLocation("AmbientOcclusionTexture");
        mAOSam_Position           = InShader->GetTextureLocation("AmbientOcclusionSampler");
    }
}