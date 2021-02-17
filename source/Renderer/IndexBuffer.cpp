/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  IndexBuffer.cpp
 @brief Index Buffer Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/

#include "Renderer/IndexBuffer.h"

#include "Renderer/DrawCommand.h"
#include "Managers/DrawManager.h"
#ifdef USE_OPENGLES
#include "OpenGLES/LE_IndexBufferImpl_OpenGLES.h"
#elif defined(USE_DX9)
#include "DirectX9/LE_IndexBufferImpl_DirectX9.h"
#elif defined(USE_DX11)
#include "../Platform/DirectX11/IndexBufferImpl_DirectX11.inl"
#elif defined(USE_DX12)
#include "../Platform/DirectX12/IndexBufferImpl_DirectX12.inl"
#else
#error No implementation for this platform
#endif

namespace LimitEngine {
class RendererTask_CreateIndexBuffer : public RendererTask
{
public:
    RendererTask_CreateIndexBuffer(IndexBufferImpl *owner, uint32 size, void *buffer) 
        : mOwner(owner)
        , mSize(size)
        , mBuffer(buffer)
    {}
    void Run() override
    {
        if (mOwner)
            mOwner->Create(mSize, mBuffer);
    }
private:
    IndexBufferImpl *mOwner;
    uint32 mSize;
    void *mBuffer;
};

IndexBuffer::IndexBuffer()
: mImpl(NULL)
, mSize(0)
{
#ifdef USE_OPENGLES
    mImpl = new IndexBufferImpl_OpenGLES();
#elif defined(USE_DX9)
	mImpl = new IndexBufferImpl_DirectX9();
#elif defined(USE_DX11)
    mImpl = new IndexBufferImpl_DirectX11();
#elif defined(USE_DX12)
    mImpl = new IndexBufferImpl_DirectX12(this);
#else
#error No implementation for this platform
#endif        
}
IndexBuffer::~IndexBuffer()
{
    if (mImpl) {
        mImpl->Dispose();
        delete mImpl; mImpl = NULL;
    }
}
void IndexBuffer::Create(size_t size, void *buffer)
{
    mSize = size;
    AutoPointer<RendererTask> rt_createIndexBuffer = new RendererTask_CreateIndexBuffer( mImpl, static_cast<uint32>(size), buffer );
    LE_DrawManager.AddRendererTask(rt_createIndexBuffer);
}
}