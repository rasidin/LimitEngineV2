/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TextureFactory.h
@brief Factory for creating texture
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

#include "ResourceFactory.h"

namespace LimitEngine {
class TextureFactory : public ResourceFactory
{
public:
    TextureFactory() {}
    virtual ~TextureFactory() {}

    void* Create(const ResourceSourceFactory *SourceFactory, const void *data, size_t size) override;
    void Release(void *data) override;
    uint32 GetResourceTypeCode() override { return makeResourceTypeCode("LMDL"); }
};
}
