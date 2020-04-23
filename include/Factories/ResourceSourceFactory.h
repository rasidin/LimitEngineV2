/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  ResourceSourceFactory.h
@brief Resource Source Factory for ResourceManager
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

namespace LimitEngine {
class ResourceSourceFactory 
{
public:
    typedef uint32 ID;

    virtual ID GetID() const = 0;
    virtual void* ConvertRawData(const void *Data, size_t Size) const = 0;
};
}