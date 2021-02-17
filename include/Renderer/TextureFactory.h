/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2012
-----------------------------------------------------------
@file  LE_TextureFactory.h
@brief Factory for creating texture
@author minseob (https://github.com/rasidin)
-----------------------------------------------------------
History:
- 2017/5/19 Created by minseob
***********************************************************/
#pragma once
#ifndef _LE_TEXTUREFACTORY_H_
#define _LE_TEXTUREFACTORY_H_

#include "LE_ResourceFactory.h"

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

#endif // _LE_TEXTUREFACTORY_H_