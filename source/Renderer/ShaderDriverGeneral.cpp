﻿/***********************************************************
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
	}
    void ShaderDriverGeneral::setup(const Shader *InShader)
    {
        mParameter_wvpMat = InShader->GetUniformLocation("WorldViewProjMatrix");
        mParameter_invViewMat = InShader->GetUniformLocation("InverseViewMatrix");
        mParameter_invViewProjMat = InShader->GetUniformLocation("InvViewProjMatrix");
        mParameter_worldMat = InShader->GetUniformLocation("WorldMatrix");
        mParameter_camPos = InShader->GetUniformLocation("CameraPosition");
    }
	void ShaderDriverGeneral::Apply(const RenderState &rs, const Material *material)
	{
		if(mOwnerShader == NULL)
			return;
		if(mParameter_wvpMat>=0) {
			DrawCommand::SetShaderUniformMatrix4(mOwnerShader, mParameter_wvpMat, 1, (float*)&rs.GetWorldViewProjMatrix());
		}
		if(mParameter_invViewMat>=0) {
            DrawCommand::SetShaderUniformMatrix4(mOwnerShader, mParameter_invViewMat, 1, (float*)&rs.GetInvViewMatrix());
		}
        if (mParameter_invViewProjMat >= 0) {
            DrawCommand::SetShaderUniformMatrix4(mOwnerShader, mParameter_invViewProjMat, 1, (float*)&rs.GetInvViewProjMatrix());
        }
        if(mParameter_worldMat>=0) {
            DrawCommand::SetShaderUniformMatrix4(mOwnerShader, mParameter_worldMat, 1, (float*)&rs.GetWorldMatrix());
        }
        if(mParameter_camPos >=0) {
            const LEMath::FloatVector3 &camPos = LE_SceneManager.GetCamera()->GetPosition();
            DrawCommand::SetShaderUniformFloat4(mOwnerShader, mParameter_camPos, (LEMath::DataContainer)camPos);
        }
	}
} // namespace LimitEngine