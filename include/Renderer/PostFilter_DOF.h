/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_PostFilter_DOF.h
 @brief PostFilter Depth of Field
 @author minseob (https://github.com/rasidin)
 -----------------------------------------------------------
 History:
 - 2012/12/22 Created by minseob
 ***********************************************************/

#ifndef _LE_POSTFILTER_DOF_H_
#define _LE_POSTFILTER_DOF_H_

#include "LE_PostFilter.h"

namespace LimitEngine {
    class FrameBuffer;
    class Texture;
    class Shader;
    class PostFilter_DOF : public PostFilter
    {
    private:
        enum ShaderTextureLocation
        {
            ShaderTextureLocationColor = 0,
            ShaderTextureLocationMini,
            ShaderTextureLocationMax
        };
        
    public:
        PostFilter_DOF();
        virtual ~PostFilter_DOF();
        
        void Init();
        void Filtering(Texture *color, Texture *depth);
        
    private:
        Shader              *_shader_scr;
        Shader              *_shader;
        FrameBuffer         *_quarterFrameBuffer;
        Texture             *_quarterColorTexture;
        Texture             *_quartedDepthTexture;
        
        uint32               _shader_texloc[ShaderTextureLocationMax];
        uint32               _attr_colorTexRec;
    };
}
#endif
