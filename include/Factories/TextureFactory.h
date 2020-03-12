/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2012
-----------------------------------------------------------
@file  LE_TextureFactory.h
@brief Factory for creating texture
@author minseob (leeminseob@outlook.com)
-----------------------------------------------------------
History:
- 2017/5/19 Created by minseob
***********************************************************/
#pragma once

#include "ResourceFactory.h"

namespace LimitEngine {
class TextureFactory : public ResourceFactory
{
public:
    TextureFactory() {}
    virtual ~TextureFactory() {}

    void* Create(const char *format, const void *data, size_t size) override;
    void Release(void *data) override;
    uint32 GetResourceTypeCode() override { return makeResourceTypeCode("LMDL"); }
};
}
