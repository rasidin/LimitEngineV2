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
#include "ResourceSourceFactory.h"

namespace LimitEngine {
class ResourceFactory : public Object<LimitEngineMemoryCategory_Common>
{
    static constexpr uint32 FixedSize = 16;
public:
    struct ID
    {
        ID() { type[0] = 0; format[0] = 0; }
        ID(const char* t, const char *f)
        {
            type[0] = 0; 
            if (t[0]) ::strcpy_s(type, FixedSize, t);
            format[0] = 0;
            if (f[0]) ::strcpy_s(format, FixedSize, f);
        }
        char type[FixedSize];
        char format[FixedSize];
        bool operator == (const ID &id) const {
            return ((type[0] == 0 || strcmp(id.type, type) == 0) && (format[0] == 0 || strcmp(id.format, format) == 0));
        }
    };
    virtual void* Create(const ResourceSourceFactory *SourceFactory, const void *data, size_t size) = 0;
    virtual void Release(void *data) = 0;
    virtual uint32 GetResourceTypeCode() = 0;
protected:
    uint32 makeResourceTypeCode(const char *typeCode) { return typeCode[0] | (typeCode[1] << 8) | (typeCode[2] << 16) | (typeCode[3] << 24); }
};
}
