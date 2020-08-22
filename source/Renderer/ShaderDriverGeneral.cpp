/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ShaderDriverGeneral.cpp
 @brief General driver for shader
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Renderer/ShaderDriverGeneral.h"

#include <LEFloatVector3.h>

#include "Renderer/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/DrawCommand.h"
#include "Managers/DrawManager.h"
#include "Managers/SceneManager.h"

namespace LimitEngine {
	bool ShaderDriverGeneral::IsValid(const ShaderParameterParser::ParameterMap &paramMap) const
	{
        return paramMap.FindIndex("WorldViewProjMatrix") >= 0 || paramMap.FindIndex("CameraPosition") >= 0;
	}
    bool ShaderDriverGeneral::IsValid(const Shader* InShader) const
    {
        return InShader->GetUniformLocation("WorldViewProjMatrix") >= 0 || InShader->GetUniformLocation("CameraPosition");
    }
	void ShaderDriverGeneral::setup(const ShaderParameterParser::ParameterMap &paramMap)
	{
		mParameter_wvpMat = getShaderUniformLocation(paramMap, "WorldViewProjMatrix");
		mParameter_invViewMat = getShaderUniformLocation(paramMap, "InverseViewMatrix");
        mParameter_invViewProjMat = getShaderUniformLocation(paramMap, "InvViewProjMatrix");
        mParameter_worldMat = getShaderUniformLocation(paramMap, "WorldMatrix");
        mParameter_camPos = getShaderUniformLocation(paramMap, "CameraPosition");
        mParameter_bluenoise = getShaderUniformLocation(paramMap, "BlueNoiseContext");
	}
    void ShaderDriverGeneral::setup(const Shader *InShader)
    {
        mParameter_wvpMat = InShader->GetUniformLocation("WorldViewProjMatrix");
        mParameter_invViewMat = InShader->GetUniformLocation("InverseViewMatrix");
        mParameter_invViewProjMat = InShader->GetUniformLocation("InvViewProjMatrix");
        mParameter_worldMat = InShader->GetUniformLocation("WorldMatrix");
        mParameter_camPos = InShader->GetUniformLocation("CameraPosition");
        mParameter_bluenoise = InShader->GetUniformLocation("BlueNoiseContext");
    }
	void ShaderDriverGeneral::Apply(const RenderState &rs, const Material *material)
	{
		if(mOwnerShader == NULL)
			return;
		if(IsValidShaderParameterPosition(mParameter_wvpMat)) {
			DrawCommand::SetShaderUniformMatrix4(mOwnerShader, material->GetConstantBuffer(rs.GetRenderPass()).Get(), mParameter_wvpMat, 1, (float*)&rs.GetWorldViewProjMatrix());
		}
		if(IsValidShaderParameterPosition(mParameter_invViewMat)) {
            DrawCommand::SetShaderUniformMatrix4(mOwnerShader, material->GetConstantBuffer(rs.GetRenderPass()).Get(), mParameter_invViewMat, 1, (float*)&rs.GetInvViewMatrix());
		}
        if(IsValidShaderParameterPosition(mParameter_invViewProjMat)) {
            DrawCommand::SetShaderUniformMatrix4(mOwnerShader, material->GetConstantBuffer(rs.GetRenderPass()).Get(), mParameter_invViewProjMat, 1, (float*)&rs.GetInvViewProjMatrix());
        }
        if(IsValidShaderParameterPosition(mParameter_worldMat)) {
            DrawCommand::SetShaderUniformMatrix4(mOwnerShader, material->GetConstantBuffer(rs.GetRenderPass()).Get(), mParameter_worldMat, 1, (float*)&rs.GetWorldMatrix());
        }
        if(IsValidShaderParameterPosition(mParameter_camPos)) {
            const LEMath::FloatVector3 &camPos = LE_SceneManager.GetCamera()->GetPosition();
            DrawCommand::SetShaderUniformFloat4(mOwnerShader, material->GetConstantBuffer(rs.GetRenderPass()).Get(), mParameter_camPos, (LEMath::DataContainer)camPos);
        }
        if(IsValidShaderParameterPosition(mParameter_bluenoise)) {
            DrawCommand::SetShaderUniformFloat4(mOwnerShader, material->GetConstantBuffer(rs.GetRenderPass()).Get(), mParameter_bluenoise, rs.GetBlueNoiseContext());
        }
	}
} // namespace LimitEngine