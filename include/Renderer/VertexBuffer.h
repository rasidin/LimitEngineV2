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

#include "Core/Memory.h"
#include "Core/ReferenceCountedObject.h"
#include "Core/AutoPointer.h"
#include "Managers/DrawManager.h"
#include "Renderer/PipelineStateDescriptor.h"
#include "Renderer/Vertex.h"

namespace LimitEngine {
    class Shader;
    class VertexBufferImpl : public Object<LimitEngineMemoryCategory::Graphics>
    {
    public:
        virtual ~VertexBufferImpl() {}
        
        virtual void Create(uint32 fvf, size_t stride, size_t size, uint32 flag, void *buffer) = 0;
        virtual void Dispose() = 0;
        virtual void* GetHandle() = 0;
        virtual void* GetResource() const = 0;
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

    class VertexBufferGeneric : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>, public SerializableResource
    {
    public:
        VertexBufferGeneric();
        virtual ~VertexBufferGeneric();

        virtual void Create(size_t size, void* initializeBuffer, uint32 flag) = 0;

        virtual void GenerateInputElementDescriptors(PipelineStateDescriptor& desc) = 0;

        virtual size_t GetSize() const = 0;

        virtual void* GetBuffer() const = 0;
        virtual uint32 GetFVF() const = 0;
        virtual uint32 GetBufferSize() const = 0;
        virtual uint32 GetStride() const = 0;

    protected: // RendererAccessorOnly
        virtual ResourceState GetResourceState() const = 0;
        virtual void SetResourceState(const ResourceState& state) = 0;

    protected:
        VertexBufferImpl* mImpl = nullptr;

        friend class VertexBufferRendererAccessor;
    };
    
    class VertexBufferRendererAccessor
    {
    public:
        VertexBufferRendererAccessor(VertexBufferGeneric* vbg)
            : mVertexBuffer(vbg)
            , mImpl(vbg->mImpl)
        {}

        inline void* GetHandle() const { return mImpl ? mImpl->GetHandle() : nullptr; }
        inline uint32 GetBufferSize() const { return mVertexBuffer->GetBufferSize(); }
        inline uint32 GetStride() const { return mVertexBuffer->GetStride();; }

        inline void* GetResource() const { return mImpl ? mImpl->GetResource() : nullptr; }
        inline  ResourceState GetResourceState() const { return mVertexBuffer->GetResourceState(); }
        inline void SetResourceState(const ResourceState& InState) { mVertexBuffer->SetResourceState(InState); }
    private:
        VertexBufferGeneric* mVertexBuffer = nullptr;
        VertexBufferImpl* mImpl = nullptr;
    };

    VertexBufferImpl* CreateImplementation();
    template<uint32 tFVF> void GenerateInputElementDescriptorsUsingFVF(PipelineStateDescriptor& desc) {
        desc.InputElementCount = 0u;
        uint32 descriptorindex = 0u;
        uint32 byteoffset = 0u;
        if (tFVF & FVF_TYPE_POSITION) {
            desc.InputElementDescriptors[descriptorindex].SemanticName = FVF_NAMES[FVF_INDEX_POSITION];
            desc.InputElementDescriptors[descriptorindex].SemanticIndex = 0;
            desc.InputElementDescriptors[descriptorindex].Format = FVF_FORMATS[FVF_INDEX_POSITION];
            desc.InputElementDescriptors[descriptorindex].InputSlot = 0;
            desc.InputElementDescriptors[descriptorindex].AlignedByteOffset = byteoffset;
            desc.InputElementDescriptors[descriptorindex].InputSlotClass = RendererFlag::InputClassification::PerVertexData;
            desc.InputElementDescriptors[descriptorindex].InstanceDataStepRate = 0;
            byteoffset += FVF_SIZE_POSITION;
            descriptorindex++;
            desc.InputElementCount++;
        }
        if (tFVF & FVF_TYPE_NORMAL) {
            desc.InputElementDescriptors[descriptorindex].SemanticName = FVF_NAMES[FVF_INDEX_NORMAL];
            desc.InputElementDescriptors[descriptorindex].SemanticIndex = 0;
            desc.InputElementDescriptors[descriptorindex].Format = FVF_FORMATS[FVF_INDEX_NORMAL];
            desc.InputElementDescriptors[descriptorindex].InputSlot = 0;
            desc.InputElementDescriptors[descriptorindex].AlignedByteOffset = byteoffset;
            desc.InputElementDescriptors[descriptorindex].InputSlotClass = RendererFlag::InputClassification::PerVertexData;
            desc.InputElementDescriptors[descriptorindex].InstanceDataStepRate = 0;
            byteoffset += FVF_SIZE_NORMAL;
            descriptorindex++;
            desc.InputElementCount++;
        }
        if (tFVF & FVF_TYPE_COLOR) {
            desc.InputElementDescriptors[descriptorindex].SemanticName = FVF_NAMES[FVF_INDEX_COLOR];
            desc.InputElementDescriptors[descriptorindex].SemanticIndex = 0;
            desc.InputElementDescriptors[descriptorindex].Format = FVF_FORMATS[FVF_INDEX_COLOR];
            desc.InputElementDescriptors[descriptorindex].InputSlot = 0;
            desc.InputElementDescriptors[descriptorindex].AlignedByteOffset = byteoffset;
            desc.InputElementDescriptors[descriptorindex].InputSlotClass = RendererFlag::InputClassification::PerVertexData;
            desc.InputElementDescriptors[descriptorindex].InstanceDataStepRate = 0;
            byteoffset += FVF_SIZE_COLOR;
            descriptorindex++;
            desc.InputElementCount++;
        }
        if (tFVF & FVF_TYPE_TEXCOORD) {
            desc.InputElementDescriptors[descriptorindex].SemanticName = FVF_NAMES[FVF_INDEX_TEXCOORD];
            desc.InputElementDescriptors[descriptorindex].SemanticIndex = 0;
            desc.InputElementDescriptors[descriptorindex].Format = FVF_FORMATS[FVF_INDEX_TEXCOORD];
            desc.InputElementDescriptors[descriptorindex].InputSlot = 0;
            desc.InputElementDescriptors[descriptorindex].AlignedByteOffset = byteoffset;
            desc.InputElementDescriptors[descriptorindex].InputSlotClass = RendererFlag::InputClassification::PerVertexData;
            desc.InputElementDescriptors[descriptorindex].InstanceDataStepRate = 0;
            byteoffset += FVF_SIZE_TEXCOORD;
            descriptorindex++;
            desc.InputElementCount++;
        }
        if (tFVF & FVF_TYPE_WEIGHT) {
            desc.InputElementDescriptors[descriptorindex].SemanticName = FVF_NAMES[FVF_INDEX_WEIGHT];
            desc.InputElementDescriptors[descriptorindex].SemanticIndex = 0;
            desc.InputElementDescriptors[descriptorindex].Format = FVF_FORMATS[FVF_INDEX_WEIGHT];
            desc.InputElementDescriptors[descriptorindex].InputSlot = 0;
            desc.InputElementDescriptors[descriptorindex].AlignedByteOffset = byteoffset;
            desc.InputElementDescriptors[descriptorindex].InputSlotClass = RendererFlag::InputClassification::PerVertexData;
            desc.InputElementDescriptors[descriptorindex].InstanceDataStepRate = 0;
            byteoffset += FVF_SIZE_WEIGHT;
            descriptorindex++;
            desc.InputElementCount++;
        }
        if (tFVF & FVF_TYPE_TANGENT) {
            desc.InputElementDescriptors[descriptorindex].SemanticName = FVF_NAMES[FVF_INDEX_TANGENT];
            desc.InputElementDescriptors[descriptorindex].SemanticIndex = 0;
            desc.InputElementDescriptors[descriptorindex].Format = FVF_FORMATS[FVF_INDEX_TANGENT];
            desc.InputElementDescriptors[descriptorindex].InputSlot = 0;
            desc.InputElementDescriptors[descriptorindex].AlignedByteOffset = byteoffset;
            desc.InputElementDescriptors[descriptorindex].InputSlotClass = RendererFlag::InputClassification::PerVertexData;
            desc.InputElementDescriptors[descriptorindex].InstanceDataStepRate = 0;
            byteoffset += FVF_SIZE_TANGENT;
            descriptorindex++;
            desc.InputElementCount++;
        }
        if (tFVF & FVF_TYPE_BINORMAL) {
            desc.InputElementDescriptors[descriptorindex].SemanticName = FVF_NAMES[FVF_INDEX_BINORMAL];
            desc.InputElementDescriptors[descriptorindex].SemanticIndex = descriptorindex;
            desc.InputElementDescriptors[descriptorindex].Format = FVF_FORMATS[FVF_INDEX_BINORMAL];
            desc.InputElementDescriptors[descriptorindex].InputSlot = 0;
            desc.InputElementDescriptors[descriptorindex].AlignedByteOffset = byteoffset;
            desc.InputElementDescriptors[descriptorindex].InputSlotClass = RendererFlag::InputClassification::PerVertexData;
            desc.InputElementDescriptors[descriptorindex].InstanceDataStepRate = 0;
            byteoffset += FVF_SIZE_BINORMAL;
            descriptorindex++;
            desc.InputElementCount++;
        }
    }

    template <uint32 tFVF, size_t tSize>
    class VertexBuffer : public VertexBufferGeneric
    {
    public:
        VertexBuffer()
            : mVertex(0)
            , mCreationFlag(0)
            , mSize(0)
        {
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

        virtual void Create(size_t size, void *initializeBuffer, uint32 flag) override
        {
            if (mVertex) delete[] mVertex;
            mSize = size;
            mCreationFlag = flag;
            mVertex = new Vertex<tFVF, tSize>[size]();
            if (initializeBuffer) {
                ::memcpy(mVertex, initializeBuffer, size * tSize);
            }
        }
        
        Vertex<tFVF, tSize>* GetVertices()              { return mVertex; }
        virtual size_t GetSize() const override         { return mSize; }

        virtual void GenerateInputElementDescriptors(PipelineStateDescriptor& desc) { GenerateInputElementDescriptorsUsingFVF<tFVF>(desc); }

        virtual void* GetBuffer() const override { return &mVertex[0]; }
        virtual uint32 GetFVF() const override { return tFVF; }
        virtual uint32 GetBufferSize() const override { return static_cast<uint32>(mSize * tSize); }
        virtual uint32 GetStride() const override { return static_cast<uint32>(tSize); }

    protected: // Renderer accessor only
        virtual ResourceState GetResourceState() const override { return mResourceState; }
        virtual void SetResourceState(const ResourceState& InState) override { mResourceState = InState; }

    private:        // Private Members
        Vertex<tFVF, tSize>         *mVertex;
        size_t                       mSize;
        uint32                       mCreationFlag;
        ResourceState                mResourceState;
    };
}
