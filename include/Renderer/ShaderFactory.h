/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2012
-----------------------------------------------------------
@file  LE_ShaderFactory.h
@brief Factory for creating shader
@author minseob (leeminseob@outlook.com)
-----------------------------------------------------------
History:
- 2017/5/19 Created by minseob
***********************************************************/
#pragma once
#ifndef _LE_SHADERFACTORY_H_
#define _LE_SHADERFACTORY_H_

#include "LE_ResourceFactory.h"

namespace LimitEngine {
class ShaderFactory : public ResourceFactory
{
public:
	ShaderFactory() {}
	virtual ~ShaderFactory() {}

	void* Create(const char *format, const void *data, size_t size) override;
	void* CreateFromShaderText(const char *shaderName, const char *vstext, const char *pstext);
	void Release(void *data) override;
	uint32 GetResourceTypeCode() override { return makeResourceTypeCode("SHAD"); }
};
}

#endif // _LE_SHADERFACTORY_H_