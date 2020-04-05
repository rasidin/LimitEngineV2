/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  SerializableResource.h
 @brief SerializableResource
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once
#include "Archive.h"

namespace LimitEngine {
    class SerializableResource 
    {
    public:
        SerializableResource() {}
        virtual ~SerializableResource() {}

        virtual void InitResource() {}

    protected:
        virtual bool Serialize(Archive &OutArchive) { return false; }

        virtual SerializableResource* GenerateNew() const { return nullptr; }

        virtual uint32 GetFileType() const { return 0u; }
        virtual uint32 GetVersion() const { return 0u; }

        friend class Archive;
        friend class ArchiveFactory;
        friend class ResourceManager;
    };
}