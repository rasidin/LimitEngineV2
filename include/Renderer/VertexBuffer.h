/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  VertexBuffer.cpp
 @brief Vertex Buffer
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LERenderer>

#include "Core/Object.h"
#include "Core/AutoPointer.h"
#include "Managers/DrawManager.h"
#include "Renderer/Vertex.h"

namespace LimitEngine {
    class Shader;
    class VertexBufferImpl : public Object<LimitEngineMemoryCategory::Graphics>
    {
    public:
        virtual ~VertexBufferImpl() {}
        
        virtual void Create(uint32 fvf, size_t stride, size_t size, uint32 flag, void *buffer) = 0;
        virtual void Dispose() = 0;
        virtual void Bind(Shader *shader) = 0;
        virtual void* GetHandle() = 0;
        virtual void* GetResource() const = 0;
        virtual void DrawPrimitive(uint32 type, size_t count) = 0;
    };

    class RendererTask_CreateVertexBuffer : public RendererTask
    {
    public:
        RendererTask_CreateVertexBuffer(VertexBufferImpl *owner, uint32 fvf, uint32 fvfSize, size_t dataSize, uint32 flag, void *initializeBuffer)
            : mOwner(owner)
            , mFvf(fvf)
            , mFvfSize(fvfSize)
            , mDataSize(dataSize)
            , mFlag(flag)
            , mInitializeBuffer(NULL)
        {
            if (initializeBuffer) {
                size_t dataSize = mFvfSize * mDataSize;
                mInitializeBuffer = malloc(dataSize);
                ::memcpy(mInitializeBuffer, initializeBuffer, dataSize);
            }
        }
        virtual ~RendererTask_CreateVertexBuffer()
        {
            if (mInitializeBuffer)
                free(mInitializeBuffer);
        }
        void Run() override
        {
            if (mOwner) {
                mOwner->Create(mFvf, mFvfSize, mDataSize, mFlag, mInitializeBuffer);
            }
        }
    private:
        VertexBufferImpl *mOwner;
        uint32 mFvf;
        uint32 mFvfSize;
        size_t mDataSize;
        uint32 mFlag;
        void *mInitializeBuffer;
    };

    class VertexBufferGeneric : public Object<LimitEngineMemoryCategory::Graphics>, public SerializableResource
    {
    public:
		VertexBufferGeneric(){};
		virtual ~VertexBufferGeneric(){};

        virtual uint32 GetFVF() const = 0;
        virtual void*  GetHandle() const = 0;
        virtual void*  GetBuffer() const = 0;
        virtual uint32 GetBufferSize() const = 0;
        virtual uint32 GetStride() const = 0;

        virtual void* GetResource() const = 0;
        virtual ResourceState GetResourceState() const = 0;
        virtual void SetResourceState(const ResourceState &InState) = 0;
    };
    
    VertexBufferImpl* CreateImplementation();

    template <uint32 tFVF, size_t tSize>
    class VertexBuffer : public VertexBufferGeneric
    {
    public:
        VertexBuffer()
            : mVertex(0)
            , mCreationFlag(0)
            , mImpl(0)
            , mSize(0)
        {
            mImpl = CreateImplementation();
        }
        virtual ~VertexBuffer()
        {
            if (mImpl)
            {
                mImpl->Dispose();
                delete mImpl;
                mImpl = nullptr;
            }
            if (mVertex) delete[] mVertex; mVertex = nullptr;
            mSize = 0;
        }

        virtual void InitResource() override
        {
            AutoPointer<RendererTask> rt_createVertexBuffer = new RendererTask_CreateVertexBuffer(mImpl, tFVF, tSize, mSize, mCreationFlag, mVertex);
            LE_DrawManager.AddRendererTask(rt_createVertexBuffer);
        }

        virtual bool Serialize(Archive &Ar) override 
        {
            Ar << mSize;
            Ar << mCreationFlag;
            size_t dataSize = tSize * mSize;
            if (Ar.IsLoading()) {
                mVertex = new Vertex<tFVF, tSize>[mSize]();
                ::memcpy(mVertex, Ar.GetData(dataSize), dataSize);
            }
            else {
                ::memcpy(Ar.AddSize(dataSize), mVertex, dataSize);
            }
            return true;
        }

        void Create(size_t size, void *initializeBuffer, uint32 flag)
        {
            if (mVertex) delete[] mVertex;
            mSize = size;
            mCreationFlag = flag;
            mVertex = new Vertex<tFVF, tSize>[size]();
            if (initializeBuffer) {
                ::memcpy(mVertex, initializeBuffer, size * tSize);
            }
        }
        void Bind(Shader *sh) { if (mImpl) mImpl->Bind(sh); }
        void DrawPrimitive(uint32 type)         { if (mImpl) mImpl->DrawPrimitive(type, mSize); }
        
        Vertex<tFVF, tSize>* GetVertices()      { return mVertex; }
        size_t GetSize()                        { return mSize; }
        size_t GetVertexSize()                  { return __size; }

        virtual void* GetHandle() const override { if (mImpl) return mImpl->GetHandle(); return nullptr; }
        virtual void* GetBuffer() const override { return &mVertex[0]; }
        virtual uint32 GetFVF() const override { return tFVF; }
        virtual uint32 GetBufferSize() const override { return static_cast<uint32>(mSize * tSize); }
        virtual uint32 GetStride() const override { return static_cast<uint32>(tSize); }
        virtual void* GetResource() const override { if (mImpl) return mImpl->GetResource(); return nullptr; }
        virtual ResourceState GetResourceState() const override { return mResourceState; }
        virtual void SetResourceState(const ResourceState& InState) override { mResourceState = InState; }

    private:        // Private Members
        VertexBufferImpl            *mImpl;
        Vertex<tFVF, tSize>         *mVertex;
        size_t                       mSize;
        uint32                       mCreationFlag;
        ResourceState                mResourceState;
    };
}
