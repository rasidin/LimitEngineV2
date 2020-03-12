/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  RenderContext.cpp
 @brief RenderContext Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include <LEIntVector2.h>

#include "Renderer/ByteColorRGBA.h"
#include "Renderer/RenderContext.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
	RenderContext* SingletonRenderContext::mInstance = NULL;
	RenderContext::RenderContext()
	{
		mDefaultTextures.Resize((int)DefaultTextureTypeMax);
		// White texture
		{
			Texture *whiteTexture = new Texture();
			whiteTexture->CreateColor(LEMath::IntSize(2, 2), ByteColorRGBA(0xffffffff));
			mDefaultTextures[(int)DefaultTextureTypeWhite] = whiteTexture;
		}
	}
	RenderContext::~RenderContext()
	{
		for(uint32 dtidx=0;dtidx<mDefaultTextures.size();dtidx++) {
			delete mDefaultTextures[dtidx];
		}
		mDefaultTextures.Clear();
	}
} // LimitEngine