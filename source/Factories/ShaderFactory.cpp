﻿/***********************************************************
LIMITEngine Source File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  LE_ShaderFactory.cpp
@brief Factory for creating shader
@author minseob (leeminseob@outlook.com)
***********************************************************/
#include "Factories/ShaderFactory.h"

#include "Renderer/Shader.h"
#include "Managers/ShaderManager.h"

namespace LimitEngine {
IReferenceCountedObject* ShaderFactory::Create(const ResourceSourceFactory*, const FileData &Data)
{
	if (Data.Data == NULL)
		return NULL;
	Shader* output = new Shader();
	return dynamic_cast<IReferenceCountedObject*>(output);
}
void* ShaderFactory::CreateFromShaderText(const char *shaderName, const char *vstext, const char *pstext)
{
	if (vstext == NULL || pstext == NULL)
		return NULL;
	Shader *output = new Shader(shaderName);
	if (output->compileCode(vstext, Shader::TYPE_VERTEX)) {
		if (output->compileCode(pstext, Shader::TYPE_PIXEL)) {
			LE_ShaderManager.AddShader(output);
			return output;
		}
	}
	delete output;
	return NULL;
}
void ShaderFactory::Release(void *data)
{
	if (data == NULL)
		return;
	delete static_cast<Shader*>(data);
}
}