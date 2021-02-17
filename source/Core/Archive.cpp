/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Archive.cpp
 @brief Archive for saving resource
 @author minseob (https://github.com/rasidin)
 ***********************************************************/
#include "Core/Archive.h"

#include <io.h>

#include "Core/SerializableResource.h"
#include "Renderer/AABB.h"

namespace LimitEngine {
   template<> Archive& Archive::operator << (SerializableResource &Resource) {
       Resource.Serialize(*this);
       return *this;
   }
   template<> Archive& Archive::operator << (SerializableResource *Resource) {
       Resource->Serialize(*this);
       return *this;
   }
   template<> Archive& Archive::operator << (uint8  &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (int8  &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (uint16 &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (int16 &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (uint32 &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (int32 &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (uint64 &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << ( int64 &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (float  &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (double &Data) { SerializeData(Data); return *this; }

   template<> Archive& Archive::operator << (LEMath::IntVector2     &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (LEMath::IntVector3     &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (LEMath::IntVector4     &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (LEMath::FloatVector2   &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (LEMath::FloatVector3   &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (LEMath::FloatVector4   &Data) { SerializeData(Data); return *this; }
   template<> Archive& Archive::operator << (LEMath::FloatMatrix4x4 &Data) { SerializeData(Data); return *this; }
}