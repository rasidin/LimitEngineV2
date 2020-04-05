/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  ArchiveFactory.h
@brief Creating data from archive
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once
#include "Containers/VectorArray.h"
#include "Factories/ResourceFactory.h"

namespace LimitEngine {
class ArchiveFactory : public ResourceFactory
{
public:
    ArchiveFactory();
    virtual ~ArchiveFactory();

    void* Create(const char *Format, const void *Data, size_t Size) override;
    void Release(void *Data) override {}
    uint32 GetResourceTypeCode() override { return 0u; }

private:
    VectorArray<class SerializableResource*> Generators;
};
}