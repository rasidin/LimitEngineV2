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
    enum class TextureImportFilter
    {
        None = 0,
        Irradiance,
    };

public:
    static constexpr ResourceFactory::ID ID = GENERATE_RESOURCEFACTORY_ID("TEXT");

    TextureFactory() {}
    virtual ~TextureFactory() {}

    void* Create(const ResourceSourceFactory *SourceFactory, const void *data, size_t size) override;
    void Release(void *data) override;
    uint32 GetResourceTypeCode() override { return makeResourceTypeCode("LMDL"); }

    void SetImportFilter(TextureImportFilter Filter) { mImportFilter = Filter; }

private:
    TextureImportFilter mImportFilter = TextureImportFilter::None;
};
}
