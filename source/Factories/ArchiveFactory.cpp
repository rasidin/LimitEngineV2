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
#include "Renderer/Texture.h"
#include "Renderer/Model.h"

namespace LimitEngine {
ArchiveFactory::ArchiveFactory()
{
    Generators.Add(new Font());
    Generators.Add(new Texture());
    Generators.Add(new Model());
}
ArchiveFactory::~ArchiveFactory()
{
    for (uint32 genidx = 0; genidx < Generators.count(); genidx++) {
        delete Generators[genidx];
    }
    Generators.Clear();
}
IReferenceCountedObject* ArchiveFactory::Create(const ResourceSourceFactory*, const FileData &Data)
{
    if (!Data.Data || Data.Size == 0u) return nullptr;

    struct ArchiveHeader
    {
        uint32 FileType;
        uint32 Version;
    };

    uint32 FileType = ((ArchiveHeader*)Data.Data)->FileType;
    uint32 Version = ((ArchiveHeader*)Data.Data)->Version;

    Archive LoadedArchive((uint8*)Data.Data + sizeof(ArchiveHeader), Data.Size - sizeof(ArchiveHeader));
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

    return dynamic_cast<IReferenceCountedObject*>(newObject);
}
void ArchiveFactory::Release(IReferenceCountedObject *data)
{
    if (data) {
        if (Font *fontData = dynamic_cast<Font*>(data)) {
            delete fontData;
        }
        else if (Texture *textureData = dynamic_cast<Texture*>(data)) {
            delete textureData;
        }
        else if (Model *modelData = dynamic_cast<Model*>(data)) {
            delete modelData;
        }
        data = nullptr;
    }
}
}