/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_FrameBuffer.h
 @brief Frame Buffer
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/12/22 Created by minseob
 ***********************************************************/

#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "LE_Object.h"

namespace LimitEngine {
    class Texture;
    class FrameBufferImpl : public Object<LimitEngineMemoryCategory::Graphics>
    {
    public:
        enum BindTypes
        {
            BindTypeNone = 0,
            BindTypeColorTexture,
            BindTypeDepthTexture,
            BindTypeMax
        };
        
    public:
        FrameBufferImpl() {}
        virtual ~FrameBufferImpl() {}
        
        virtual bool CheckValid() = 0;
        
        virtual void Generate() = 0;
        virtual void Bind() = 0;
        virtual void BindEnd() = 0;
        virtual void BindTexture(BindTypes type, Texture *texture) = 0;
    };
    
    class FrameBuffer : public Object<LimitEngineMemoryCategory::Graphics>
    {
    public:
        FrameBuffer();
        virtual ~FrameBuffer();
        
        void Generate()             { if (mImpl) mImpl->Generate(); }
        
        void Bind()                 { if (mImpl) mImpl->Bind(); }
        void BindEnd()              { if (mImpl) mImpl->BindEnd(); }
        void BindTexture(FrameBufferImpl::BindTypes type, Texture *texture)
        { if (mImpl) mImpl->BindTexture(type, texture); }
        
        bool CheckValid()           { if (mImpl) return mImpl->CheckValid(); return false; }
        
    private:
        FrameBufferImpl        *mImpl;
    };
}

#endif
