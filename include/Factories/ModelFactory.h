/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2012
-----------------------------------------------------------
@file  LE_ModelFactory.h
@brief Factory for creating models
@author minseob (https://github.com/rasidin)
-----------------------------------------------------------
History:
- 2017/5/19 Created by minseob
***********************************************************/
#pragma once

#include "ResourceFactory.h"

namespace LimitEngine {
    class ModelFactory : public ResourceFactory 
    {
    public:
        static constexpr ResourceFactory::ID ID = GENERATE_RESOURCEFACTORY_ID("MODE");

        ModelFactory() {}
        virtual ~ModelFactory() {}

        SerializableRendererResource* Create(const ResourceSourceFactory *format, const FileData &Data) override;
        void Release(SerializableRendererResource*data) override;
        uint32 GetResourceTypeCode() override { return makeResourceTypeCode("LMDL"); }
    private:
        void* createFromText(const char *text);
        void* createFromBinary(const void *data);
    };
}
