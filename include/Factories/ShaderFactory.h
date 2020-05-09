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

#include "ResourceFactory.h"

namespace LimitEngine {
class ShaderFactory : public ResourceFactory
{
public:
    static constexpr ResourceFactory::ID ID = GENERATE_RESOURCEFACTORY_ID("SHAD");

	ShaderFactory() {}
	virtual ~ShaderFactory() {}

    IReferenceCountedObject* Create(const ResourceSourceFactory *format, const FileData &Data) override;
	void* CreateFromShaderText(const char *shaderName, const char *vstext, const char *pstext);
	void Release(void *data) override;
	uint32 GetResourceTypeCode() override { return makeResourceTypeCode("SHAD"); }
};
}
