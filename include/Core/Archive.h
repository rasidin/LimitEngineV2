/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Archive.h
 @brief Archive for saving resource
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once
#include "Core/Object.h"

#include <LEIntVector2.h>
#include <LEIntVector3.h>
#include <LEIntVector4.h>
#include <LEFloatVector2.h>
#include <LEFloatVector3.h>
#include <LEFloatVector4.h>
#include <LEFloatMatrix4x4.h>

namespace LimitEngine {
    class Archive : public Object<LimitEngineMemoryCategory::Common>
    {
        enum class DataMode {
            Loading = 0,
            Saving,
        } mDataMode;
    public:
        Archive() : mDataMode(DataMode::Saving), mData(nullptr), mDataSize(0u), mDataOffset(0u){}
        Archive(void *InData, size_t InSize) : mDataMode(DataMode::Loading), mData(InData), mDataSize(InSize), mDataOffset(0u) {}
        virtual ~Archive() {
            if (mData && mDataMode == DataMode::Saving)
                MemoryAllocator::Free(mData);
            mData = nullptr;
            mDataSize = 0u;
        }

        template<typename T>
        Archive& operator << (T &Data);

        template<typename T>
        Archive& operator << (T *Data);

        //Archive& operator << (class SerializableResource &Resource);
        //Archive& operator << (class SerializableResource *Resource);
        //Archive& operator << (uint8  &Data) { SerializeData(Data); return *this; }
        //Archive& operator << ( int8  &Data) { SerializeData(Data); return *this; }
        //Archive& operator << (uint16 &Data) { SerializeData(Data); return *this; }
        //Archive& operator << ( int16 &Data) { SerializeData(Data); return *this; }
        //Archive& operator << (uint32 &Data) { SerializeData(Data); return *this; }
        //Archive& operator << ( int32 &Data) { SerializeData(Data); return *this; }
        //Archive& operator << (uint64 &Data) { SerializeData(Data); return *this; }
        //Archive& operator << ( int64 &Data) { SerializeData(Data); return *this; }
        //Archive& operator << (float  &Data) { SerializeData(Data); return *this; }
        //Archive& operator << (double &Data) { SerializeData(Data); return *this; }

        //Archive& operator << (LEMath::IntVector2     &Data) { SerializeData(Data); return *this; }
        //Archive& operator << (LEMath::IntVector3     &Data) { SerializeData(Data); return *this; }
        //Archive& operator << (LEMath::IntVector4     &Data) { SerializeData(Data); return *this; }
        //Archive& operator << (LEMath::FloatVector2   &Data) { SerializeData(Data); return *this; }
        //Archive& operator << (LEMath::FloatVector3   &Data) { SerializeData(Data); return *this; }
        //Archive& operator << (LEMath::FloatVector4   &Data) { SerializeData(Data); return *this; }
        //Archive& operator << (LEMath::FloatMatrix4x4 &Data) { SerializeData(Data); return *this; }

        template<typename T>
        void SerializeData(T &Data) {
            if (IsLoading()) {
                Data = *(T*)GetData(sizeof(T));
            }
            else {
                memcpy(AddSize(sizeof(T)), &Data, sizeof(T));
            }
        }

        bool IsLoading() const { return mDataMode == DataMode::Loading; }
        bool IsSaving()  const { return mDataMode == DataMode::Saving; }

        void* AddSize(size_t mSize) {
            size_t orgSize = mDataSize;
            mDataSize += mSize;
            void *newData = MemoryAllocator::Alloc(mDataSize);
            if (orgSize)
                memcpy(newData, mData, mDataSize - mSize);
            MemoryAllocator::Free(mData);
            mData = newData;
            return (uint8*)newData + orgSize;
        }
        void* GetData(size_t mSize) {
            size_t orgSize = mDataOffset;
            mDataOffset += mSize;
            return (uint8*)mData + orgSize;
        }

    private:
        void *mData;
        size_t mDataSize;
        size_t mDataOffset;

        friend class ResourceManager;
    };
}