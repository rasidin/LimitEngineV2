/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_RenderContext.h
 @brief RenderContext Class
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2016/11/07 Created by minseob
 ***********************************************************/
#pragma once
#ifndef _LE_RENDERCONTEXT_H_
#define _LE_RENDERCONTEXT_H_

#include <LERenderer>

#include "Core/Singleton.h"
#include "Core/ReferenceCountedPointer.h"
#include "Containers/MapArray.h"

namespace LimitEngine {
	typedef Singleton<RenderContext, LimitEngineMemoryCategory_Graphics> SingletonRenderContext;
	class RenderContext : public SingletonRenderContext
	{
	public:
		enum DefaultTextureType {
			DefaultTextureTypeWhite = 0,

			DefaultTextureTypeMax
		};
	public:
		RenderContext();
		virtual ~RenderContext();

		Texture* GetDefaultTexture(DefaultTextureType type) const { return mDefaultTextures[(int)type].Get(); }
	private:
		VectorArray<TextureRefPtr> mDefaultTextures;
	};
} // LimitEngine

#endif // _LE_RENDERCONTEXT_H_