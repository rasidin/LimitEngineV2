/***********************************************************
LIMITEngine Source File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TextParserFactory.cpp
@brief Creating data using text
@author minseob (leeminseob@outlook.com)
***********************************************************/
#include "Factories/TextParserFactory.h"

#include "Core/TextParser.h"
#include "Factories/ResourceSourceFactory.h"

namespace LimitEngine {
    TextParserFactory::TextParserFactory()
    {}
    TextParserFactory::~TextParserFactory()
    {}

    void* TextParserFactory::Create(const ResourceSourceFactory *Format, const void *Data, size_t Size)
    {
        if (!Data || !Size) return nullptr;
        return Format->ConvertRawData(Data, Size);
    }
    void TextParserFactory::Release(void *Data)
    {
        delete static_cast<TextParser*>(Data);
    }
}