/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TextParserFactory.h
@brief Creating data using text
@author minseob (https://github.com/rasidin)
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

        IReferenceCountedObject* Create(const ResourceSourceFactory *Format, const FileData &Data) override;
        void Release(IReferenceCountedObject *Data) override;
        uint32 GetResourceTypeCode() override { return 0u; }
    };
}