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
	bool ShaderDriverGeneral::IsValid(const ShaderParameterParser::ParameterMap &paramMap) 
	{
        return paramMap.FindIndex("WorldViewProjection") >= 0 || paramMap.FindIndex("EyePosition") >= 0;
	}
	void ShaderDriverGeneral::setup(const ShaderParameterParser::ParameterMap &paramMap)
	{
		mParameter_wvpMat = getShaderUniformLocation(paramMap, "WorldViewProjection");
		mParameter_invViewMat = getShaderUniformLocation(paramMap, "InverseViewMatrix");
        mParameter_worldMat = getShaderUniformLocation(paramMap, "WorldMatrix");
        mParameter_eyePos = getShaderUniformLocation(paramMap, "EyePosition");
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
        if(mParameter_worldMat>=0) {
            DrawCommand::SetShaderUniformMatrix4(mOwnerShader, mParameter_worldMat, 1, (float*)&rs.GetWorldMatrix());
        }
        if(mParameter_eyePos>=0) {
            const LEMath::FloatVector3 &camPos = LE_SceneManager.GetCamera()->GetPosition();
            DrawCommand::SetShaderUniformFloat4(mOwnerShader, mParameter_eyePos, (LEMath::DataContainer)camPos);
        }
	}
} // namespace LimitEngine