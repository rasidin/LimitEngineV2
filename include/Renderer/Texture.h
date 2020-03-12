/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Texture.h
 @brief Texture
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/14 Created by minseob
 ***********************************************************/

#ifndef _LE_TEXTURE_H_
#define _LE_TEXTURE_H_

#include <LEIntVector2.h>
#include <LEIntVector3.h>
#include <LEIntVector4.h>

#include "Core/Object.h"
#include "Renderer/ByteColorRGBA.h"

namespace LimitEngine {
enum TEXTURE_USAGE
{
    TEXTURE_USAGE_RENDERTARGET = 1<<0,
    TEXTURE_USAGE_DEPTHSTENCIL = 1<<1,
    TEXTURE_USAGE_DYNAMIC = 1<<2,
};
enum TEXTURE_COLOR_FORMAT
{
    TEXTURE_COLOR_FORMAT_UNKNOWN = 0,

    TEXTURE_COLOR_FORMAT_R8G8B8,
    TEXTURE_COLOR_FORMAT_A8R8G8B8,

    TEXTURE_COLOR_FORMAT_R16F,
    TEXTURE_COLOR_FORMAT_G16R16F,
    TEXTURE_COLOR_FORMAT_A16B16G16R16F,

    TEXTURE_COLOR_FORMAT_R32F,
    TEXTURE_COLOR_FORMAT_G32R32F,
    TEXTURE_COLOR_FORMAT_A32B32G32R32F,

    TEXTURE_COLOR_FORMAT_MAX,
};
class TextureImpl : public Object<LimitEngineMemoryCategory_Graphics>
{public:
    TextureImpl() {}
    virtual ~TextureImpl() {}
    
    virtual LEMath::IntSize GetSize() = 0;
    virtual TEXTURE_COLOR_FORMAT GetFormat() = 0;
    virtual void* GetHandle() = 0;
    virtual void* GetDepthSurfaceHandle() = 0;
    
    virtual void LoadFromMemory(const void *data, size_t size) = 0;
    virtual bool Create(const LEMath::IntSize &size, TEXTURE_COLOR_FORMAT format, uint32 usage, uint32 mipLevels, void *initializeData = NULL, size_t initDataSize = 0u) = 0;
	virtual bool Create3D(const LEMath::IntVector3 &size, TEXTURE_COLOR_FORMAT format, uint32 usage, uint32 mipLevels, void *initializeData = NULL, size_t initDataSize = 0u) = 0;
	virtual void CreateScreenColor(const LEMath::IntSize &size) = 0;
    virtual void CreateColor(const LEMath::IntSize &size, const ByteColorRGBA &color) = 0;
    virtual void CreateDepth(const LEMath::IntSize &size) = 0;
    virtual void GenerateMipmap() = 0;

    virtual void* GetShaderResourceView() const = 0;
    virtual void* GetUnorderedAccessView() const = 0;

    virtual void* Lock(const LEMath::IntRect &rect, int mipLevel = 0) = 0;
    virtual void Unlock(int mipLevel = 0) = 0;
};
class RendererTask_LoadTextureFromMemory;
class RendererTask_LoadTextureFromMERLBRDFData;
class RendererTask_CreateTexture;
class RendererTask_CreateTexture3D;
class RendererTask_CreateScreenColor;
class RendererTask_CreateDepth;
class RendererTask_CreateColor;
class TextureFactory;
class Texture : public Object<LimitEngineMemoryCategory_Graphics>
{
    friend RendererTask_LoadTextureFromMemory;
	friend RendererTask_LoadTextureFromMERLBRDFData;
    friend RendererTask_CreateTexture;
	friend RendererTask_CreateTexture3D;
    friend RendererTask_CreateScreenColor;
    friend RendererTask_CreateDepth;
    friend RendererTask_CreateColor;
    friend TextureFactory;
public:
    Texture();
    virtual ~Texture();

    void LoadFromMemory(const void *data, size_t size);
	void LoadFromMERLBRDFData(const void *data, size_t size);

    void Create(const LEMath::IntSize &size, TEXTURE_COLOR_FORMAT format, uint32 usage = 0, uint32 mipLevels = 1, void *initailizeData = NULL, size_t initDataSize = 0u);
	void Create3D(const LEMath::IntVector3 &size, TEXTURE_COLOR_FORMAT format, uint32 usage = 0, uint32 mipLevels = 1, void *initializeData = NULL, size_t initDataSize = 0u);
    void CreateScreenColor(const LEMath::IntSize &size);
    void CreateColor(const LEMath::IntSize &size, const ByteColorRGBA &color);
    void CreateDepth(const LEMath::IntSize &size);

    void* Lock(const LEMath::IntRect &rect, int mipLevel = 0)
    { if (mImpl) { return mImpl->Lock(rect, mipLevel); } else return NULL; }
    void Unlock(int mipLevel = 0)
    { if (mImpl) { mImpl->Unlock(mipLevel); } }

    void GenerateMipmap()                               { if (mImpl) { mImpl->GenerateMipmap(); } }
    
    const LEMath::IntSize& GetSize() const              { return mSize; }
    void* GetHandle()                                   { return (mImpl)?mImpl->GetHandle():nullptr; }
    void* GetDepthSurfaceHandle()                       { return (mImpl)?mImpl->GetDepthSurfaceHandle():nullptr; }

    void* GetShaderResourceView() const                 { return (mImpl)?mImpl->GetShaderResourceView():nullptr; }
    void* GetUnorderedAccessView() const                { return (mImpl)?mImpl->GetUnorderedAccessView():nullptr; }

private:
    TextureImpl            *mImpl;
    LEMath::IntSize         mSize;
	uint32					mDepth;
    TEXTURE_COLOR_FORMAT    mFormat;
};
}

#endif // _LE_TEXTURE_H_