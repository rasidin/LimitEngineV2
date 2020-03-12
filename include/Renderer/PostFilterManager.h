/*****************************************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 * @file	LE_PostFilterManager.h
 * @brief	Post Filter Manager
 * @author	minseob
 * @date	2012/06/11	minseob	êVãKçÏê¨
 -----------------------------------------------------------
 History:
 - 2012/12/22 Created by minseob
 *****************************************************************************/

#ifndef _LE_POSTFILTERMANAGER_H_
#define _LE_POSTFILTERMANAGER_H_

#include "LE_Singleton.h"
#include "LE_VectorArray.h"

namespace LimitEngine {
    class PostFilter;
    class FrameBuffer;
    class Texture;
	class PostFilterManager;
	typedef Singleton<PostFilterManager, LimitEngineMemoryCategory_Graphics> SingletonPostFilterManager;
    class PostFilterManager : public SingletonPostFilterManager
    {
    public:
        PostFilterManager();
        virtual ~PostFilterManager();

        void Init();
        
        void BeginScene();
        void AddPostFilter(PostFilter *filter)      { _postFilters.Add(filter); }
        void Filtering();
    private:
        VectorArray<PostFilter*>             _postFilters;
        FrameBuffer                         *_frameBuffer[2];
        Texture                             *_colorTexture[2];
        Texture                             *_depthTexture[2];
    };
}

#endif
