/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  IndexBuffer.h
 @brief Index Buffer Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LERenderer>

#include "Core/Object.h"
#include "Core/SerializableResource.h"

namespace LimitEngine {
    class IndexBufferImpl : public Object<LimitEngineMemoryCategory::Graphics>
    {public:
        IndexBufferImpl(class IndexBuffer* InBuffer) : mOwner(InBuffer) {}
        virtual ~IndexBufferImpl() {}
        
        virtual void Create(size_t size, void *buffer) = 0;
        virtual void Dispose() = 0;
        virtual void* GetHandle() = 0;
        virtual void* GetResource() const = 0;
        virtual void* GetBuffer() = 0;
    protected:
        class IndexBuffer* mOwner = nullptr;
    };
    class IndexBuffer : public Object<LimitEngineMemoryCategory::Graphics>, public SerializableResource
    {
    public:
        IndexBuffer();
        virtual ~IndexBuffer();
        
        void* GetResource() { return mImpl ? mImpl->GetResource() : nullptr; }

        ResourceState GetResourceState() const { return mResourceState; }
        void SetResourceState(const ResourceState& InState) { mResourceState = InState; }

        void* GetHandle() const { return mImpl ? mImpl->GetHandle() : nullptr; }
        size_t GetSize() const  { return mSize; }
        void Create(size_t size, void *buffer);
        void Bind();
        
    private:
        IndexBufferImpl     *mImpl;
        size_t               mSize;
        ResourceState        mResourceState;
    };
}
