/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_RenderBuffer.cpp
 @brief Render Buffer Class
 @author minseob (rasidin1@hotmail.com)
 -----------------------------------------------------------
 History:
 - 2012/12/22 Created by minseob
 ***********************************************************/

#ifndef _LE_RENDERBUFFER_H_
#define _LE_RENDERBUFFER_H_

#include "LE_Object.h"

namespace LimitEngine {
    class RenderBufferImpl : public Object
    {
    public:
        RenderBufferImpl() {}
        virtual ~RenderBufferImpl() {}
    };
    
    class RenderBuffer : public Object
    {
    public:
        RenderBuffer();
        virtual ~RenderBuffer();
        
    private:
        RenderBufferImpl        *_impl;
    };
}

#endif
