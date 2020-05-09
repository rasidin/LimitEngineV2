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

    IReferenceCountedObject* TextParserFactory::Create(const ResourceSourceFactory *Format, const FileData &Data)
    {
        if (!Data.IsValid()) return nullptr;
        return dynamic_cast<ReferenceCountedObject<>*>((TextParser*)Format->ConvertRawData(Data.Data, Data.Size));
    }
    void TextParserFactory::Release(void *Data)
    {
        delete static_cast<TextParser*>(Data);
    }
}