/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  ArchiveFactory.cpp
@brief Factory for creating using archive
@author minseob (leeminseob@outlook.com)
***********************************************************/
#include "Factories/ArchiveFactory.h"

#include "Core/Archive.h"
#include "Renderer/Font.h"

namespace LimitEngine {
ArchiveFactory::ArchiveFactory()
{
    Generators.Add(new Font());
}
ArchiveFactory::~ArchiveFactory()
{
    for (uint32 genidx = 0; genidx < Generators.count(); genidx++) {
        delete Generators[genidx];
    }
    Generators.Clear();
}
void* ArchiveFactory::Create(const ResourceSourceFactory*, const void *Data, size_t Size)
{
    if (!Data || Size == 0u) return nullptr;

    struct ArchiveHeader
    {
        uint32 FileType;
        uint32 Version;
    };

    uint32 FileType = ((ArchiveHeader*)Data)->FileType;
    uint32 Version = ((ArchiveHeader*)Data)->Version;

    Archive LoadedArchive((uint8*)Data + sizeof(ArchiveHeader), Size - sizeof(ArchiveHeader));
    SerializableResource *Generator = nullptr;
    SerializableResource *newObject = nullptr;
    for (uint32 generatorIndex = 0; generatorIndex < Generators.count(); generatorIndex++) {
        if (Generators[generatorIndex]->GetFileType() == FileType) {
            Generator = Generators[generatorIndex];
            break;
        }
    }
    if (Generator) {
        newObject = Generator->GenerateNew();
        newObject->Serialize(LoadedArchive);
    }

    return dynamic_cast<void*>(newObject);
}
}