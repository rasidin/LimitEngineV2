/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  IndexBuffer.h
 @brief Index Buffer Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include "Core/Object.h"

namespace LimitEngine {
    class IndexBufferImpl : public Object<LimitEngineMemoryCategory_Graphics>
    {public:
        virtual ~IndexBufferImpl() {}
        
        virtual void Create(size_t size, void *buffer) = 0;
        virtual void Dispose() = 0;
        virtual void* GetHandle() = 0;
        virtual void* GetBuffer() = 0;
    };
    class IndexBuffer : public Object<LimitEngineMemoryCategory_Graphics>
    {
    public:
        IndexBuffer();
        virtual ~IndexBuffer();
        
        size_t GetSize()                            { return mSize; }
        void Create(size_t size, void *buffer);
        void Bind();
        
    private:
        IndexBufferImpl     *mImpl;
        size_t               mSize;
    };
}
