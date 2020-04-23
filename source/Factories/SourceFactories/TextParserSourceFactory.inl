/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  ResourceSourceFactory.h
@brief Resource Source Factory for ResourceManager
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

#include "Core/TextParser.h"
#include "Factories/ResourceSourceFactory.h"

namespace LimitEngine {
class TextParserSourceFactory : public ResourceSourceFactory
{
public:
    static constexpr ResourceSourceFactory::ID FactoryID = GENERATE_RESOURCEFACTORY_ID("TEPA");

    TextParserSourceFactory() {}
    virtual ~TextParserSourceFactory() {}

    virtual ID GetID() const override { return FactoryID; }

    virtual void* ConvertRawData(const void *Data, size_t Size) const {
        TextParser *Output = new TextParser();
        if (Output->Parse((const char*)Data)) {
            return (void*)(Output);
        }
        delete Output;
        return nullptr;
    }
};
}