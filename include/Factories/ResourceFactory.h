/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  ResourceFactory.h
@brief Resource Factory for ResourceManager
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once
#include "Core/Object.h"
#include "Core/Memory.h"

#define GENERATE_RESOURCEFACTORY_ID(a) a[0] | (a[1] << 8) | (a[2] << 16) | (a[3] << 24)

namespace LimitEngine {
class ResourceFactory : public Object<LimitEngineMemoryCategory_Common>
{
    static constexpr uint32 FixedSize = 16;
public:
    typedef uint32 ID;

    virtual void* Create(const class ResourceSourceFactory *SourceFactory, const void *data, size_t size) = 0;
    virtual void Release(void *data) = 0;
    virtual uint32 GetResourceTypeCode() = 0;
protected:
    uint32 makeResourceTypeCode(const char *typeCode) { return typeCode[0] | (typeCode[1] << 8) | (typeCode[2] << 16) | (typeCode[3] << 24); }
};
}
