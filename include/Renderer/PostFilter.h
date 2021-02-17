/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_PostFilter.h
 @brief PostFilter (Common)
 @author minseob (https://github.com/rasidin)
 -----------------------------------------------------------
 History:
 - 2012/12/22 Created by minseob
 ***********************************************************/

#ifndef _LE_POSTFILTER_H_
#define _LE_POSTFILTER_H_

#include "LE_Common.h"
#include "LE_Object.h"

namespace LimitEngine {
    class Texture;
    class RenderBuffer;
    class PostFilter : public Object<LimitEngineMemoryCategory::Graphics>
    {
    public:
                 PostFilter()               {}
        virtual ~PostFilter()               {}
        
        virtual void Init() = 0;
        virtual void Filtering(Texture *color, Texture *depth) = 0;
    };
}

#endif
