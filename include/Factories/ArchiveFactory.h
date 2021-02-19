/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  ArchiveFactory.h
@brief Creating data from archive
@author minseob (https://github.com/rasidin)
***********************************************************/
#pragma once
#include "Containers/VectorArray.h"
#include "Factories/ResourceFactory.h"
#include "Renderer/SerializableRendererResource.h"

namespace LimitEngine {
class ArchiveFactory : public ResourceFactory
{
public:
    static constexpr ResourceFactory::ID ID = GENERATE_RESOURCEFACTORY_ID("ARCH");

    ArchiveFactory();
    virtual ~ArchiveFactory();

    SerializableRendererResource* Create(const ResourceSourceFactory *Format, const FileData &Data) override;
    void Release(SerializableRendererResource *Data) override;
    uint32 GetResourceTypeCode() override { return 0u; }

private:
    VectorArray<class SerializableRendererResource*> Generators;
};
}