/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Archive.cpp
 @brief Archive for saving resource
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include "Core/Archive.h"

#include <io.h>

#include "Core/SerializableResource.h"

namespace LimitEngine {
    Archive& Archive::operator << (SerializableResource &Resource) {
        Resource.Serialize(*this);
        return *this;
    }
    Archive& Archive::operator << (SerializableResource *Resource) {
        Resource->Serialize(*this);
        return *this;
    }
}