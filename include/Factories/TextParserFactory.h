/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TextParserFactory.h
@brief Creating data using text
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once
#include "Containers/VectorArray.h"
#include "Factories/ResourceFactory.h"

namespace LimitEngine {
    class TextParserFactory : public ResourceFactory
    {
    public:
        static constexpr ResourceFactory::ID ID = GENERATE_RESOURCEFACTORY_ID("TEPA");

        TextParserFactory();
        virtual ~TextParserFactory();

        void* Create(const ResourceSourceFactory *Format, const void *Data, size_t Size) override;
        void Release(void *Data) override;
        uint32 GetResourceTypeCode() override { return 0u; }
    };
}