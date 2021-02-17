/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_PostFilter_Filmic.h
 @brief PostFilter Filmic
 @author minseob (https://github.com/rasidin)
 -----------------------------------------------------------
 History:
 - 2012/12/22 Created by minseob
 ***********************************************************/

#ifndef _LE_POSTFILTER_FILMIC_H_
#define _LE_POSTFILTER_FILMIC_H_

#include "LE_PostFilter.h"
#include "LE_VertexBuffer.h"

namespace LimitEngine {
    class Texture;
    class Shader;
    class PostFilter_Filmic : public PostFilter
    {
    public:
        PostFilter_Filmic();
        virtual ~PostFilter_Filmic();

        void Init() {}
        void Filtering(Texture *color, Texture *depth);
        
    private:
        Shader                          *_shader;
    };
}

#endif
