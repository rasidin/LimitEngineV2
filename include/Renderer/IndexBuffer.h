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

#include "Core/ReferenceCountedObject.h"
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
    class IndexBuffer : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>, public SerializableResource
    {
    public:
        IndexBuffer();
        virtual ~IndexBuffer();
        
        size_t GetSize() const  { return mSize; }
        void Create(size_t size, void *buffer);
        void Bind();
        
    private:
        IndexBufferImpl     *mImpl;
        size_t               mSize;
        ResourceState        mResourceState;

        friend class IndexBufferRendererAccessor;
    };
    class IndexBufferRendererAccessor
    {
    public:
        IndexBufferRendererAccessor(IndexBuffer* ib)
            : mIndexBuffer(ib)
            , mImpl(ib->mImpl)
            , mSize(ib->GetSize())
        {}

        void* GetResource() { return mImpl ? mImpl->GetResource() : nullptr; }
        ResourceState GetResourceState() const { return mIndexBuffer->mResourceState; }
        void SetResourceState(const ResourceState& InState) { mIndexBuffer->mResourceState = InState; }
        inline void* GetHandle() const { return mImpl ? mImpl->GetHandle() : nullptr; }
        inline size_t GetSize() const { return mSize; }

    private:
        IndexBuffer* mIndexBuffer = nullptr;
        IndexBufferImpl* mImpl = nullptr;
        size_t mSize = 0u;
    };
}
