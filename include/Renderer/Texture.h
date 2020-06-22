/***********************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------
@file  Texture.h
@brief Texture
@author minseob (leeminseob@outlook.com)
***********************************************************/

#pragma once

#include <LEIntVector2.h>
#include <LEIntVector3.h>
#include <LEIntVector4.h>

#include "Core/ReferenceCountedObject.h"
#include "Core/ReferenceCountedPointer.h"
#include "Core/SerializableResource.h"
#include "Containers/VectorArray.h"
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

    TEXTURE_COLOR_FORMAT_R8,

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
enum TEXTURE_DEPTH_FORMAT
{
    TEXTURE_DEPTH_FORMAT_UNKNOWN = 0,

    TEXTURE_DEPTH_FORMAT_D32F,
    TEXTURE_DEPTH_FORMAT_D24S8,
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
    virtual void CreateDepthStencil(const LEMath::IntSize &size, TEXTURE_DEPTH_FORMAT format) = 0;
    virtual void CreateRenderTarget(const LEMath::IntSize &size, TEXTURE_COLOR_FORMAT format, uint32 usage) = 0;
    virtual void GenerateMipmap() = 0;

    virtual void* GetShaderResourceView() const = 0;
    virtual void* GetUnorderedAccessView() const = 0;
    virtual void* GetRenderTargetView() const = 0;
    virtual void* GetDepthStencilView() const = 0;

    virtual void* Lock(const LEMath::IntRect &rect, int mipLevel = 0) = 0;
    virtual void Unlock(int mipLevel = 0) = 0;
};
class TextureSourceImage : public ReferenceCountedObject<LimitEngineMemoryCategory_Graphics>
{
public:
    TextureSourceImage() {}
    virtual ~TextureSourceImage() {}

    virtual bool Load(const void *Data, size_t Size) = 0;
    virtual LEMath::IntSize GetSize() const = 0;
    virtual uint32 GetDepth() const = 0;
    virtual bool IsCubemap() const = 0;
    virtual TEXTURE_COLOR_FORMAT GetFormat() const = 0;
    virtual uint32 GetRowPitch() const = 0;
    virtual void* GetColorData() const = 0;
    virtual size_t GetColorDataSize() const = 0;
    virtual uint32 GetMipCount() const = 0;
};
class SerializedTextureSource : public TextureSourceImage
{
public:
    explicit SerializedTextureSource() : mSize(), mRowPitch(0u), mMipCount(1u), mFormat(TEXTURE_COLOR_FORMAT_MAX), mIsCubemap(false) {}
    explicit SerializedTextureSource(const TextureSourceImage &SourceImage);
    virtual ~SerializedTextureSource();

    virtual bool Load(const void *Data, size_t Size) override { return false; }
    virtual LEMath::IntSize GetSize() const override { return mSize.XY(); }
    virtual uint32 GetDepth() const override { return mSize.Z(); }
    virtual bool IsCubemap() const override { return mIsCubemap; }
    virtual TEXTURE_COLOR_FORMAT GetFormat() const override { return (TEXTURE_COLOR_FORMAT)mFormat; }
    virtual uint32 GetRowPitch() const override { return mRowPitch; }
    virtual void* GetColorData() const override { return mColorData.GetData(); }
    virtual size_t GetColorDataSize() const override { return mColorData.count();  }
    virtual uint32 GetMipCount() const override { return mMipCount; }

private:
    LEMath::IntVector3 mSize;
    uint32 mRowPitch;
    uint32 mMipCount;

    uint32 mFormat;
    VectorArray<uint8> mColorData;

    bool mIsCubemap;

    friend class TextureFactory;
    friend TextureSourceImage;
    friend Archive;
};
class Texture : public ReferenceCountedObject<LimitEngineMemoryCategory_Graphics>, public SerializableResource
{
    friend class RendererTask_LoadTextureFromMemory;
	friend class RendererTask_LoadTextureFromMERLBRDFData;
    friend class RendererTask_CreateTexture;
	friend class RendererTask_CreateTexture3D;
    friend class RendererTask_CreateScreenColor;
    friend class RendererTask_CreateDepth;
    friend class RendererTask_CreateColor;
    friend class RendererTask_CreateRenderTarget;
    friend class RendererTask_CreateDepthStencil;
    friend class TextureFactory;
    
    enum class FileVersion : uint32 {
        FirstVersion = 1,

        CurrentVersion = FirstVersion
    };

public:
    Texture();
    virtual ~Texture();

    virtual void InitResource() override;

    void LoadFromMemory(const void *data, size_t size);
	void LoadFromMERLBRDFData(const void *data, size_t size);

    void Create(const LEMath::IntSize &size, TEXTURE_COLOR_FORMAT format, uint32 usage = 0, uint32 mipLevels = 1, void *initailizeData = NULL, size_t initDataSize = 0u);
	void Create3D(const LEMath::IntVector3 &size, TEXTURE_COLOR_FORMAT format, uint32 usage = 0, uint32 mipLevels = 1, void *initializeData = NULL, size_t initDataSize = 0u);
    void CreateScreenColor(const LEMath::IntSize &size);
    void CreateColor(const LEMath::IntSize &size, const ByteColorRGBA &color);
    void CreateDepthStencil(const LEMath::IntSize &size, TEXTURE_DEPTH_FORMAT format);
    void CreateRenderTarget(const LEMath::IntSize &size, TEXTURE_COLOR_FORMAT format, uint32 usage = 0);

    void CreateUsingSourceData();

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
    void* GetRenderTargetView() const                   { return (mImpl)?mImpl->GetRenderTargetView():nullptr; }
    void* GetDepthStencilView() const                   { return (mImpl)?mImpl->GetDepthStencilView():nullptr; }

    void SetSourceImage(SerializedTextureSource *InSource) { if (mSource) delete mSource; mSource = InSource; if (InSource) { mSize = InSource->GetSize(); mDepth = InSource->GetDepth(); mFormat = InSource->GetFormat(); } }
    SerializedTextureSource* GetSourceImage() const     { return mSource; }

    virtual bool Serialize(Archive &OutArchive) override;

public: // Generators
    static Texture* GenerateFromSourceImage(const TextureSourceImage *SourceImage);

    SerializableResource* GenerateNew() const override { return dynamic_cast<SerializableResource*>(new Texture()); }

protected: // Interface for serializing
    virtual uint32 GetFileType() const { return ('T' | ('E' << 8) | ('X' << 16) | ('T' << 24)); }
    virtual uint32 GetVersion() const { return static_cast<uint32>(FileVersion::CurrentVersion); }

private:
    TextureImpl                *mImpl;
    LEMath::IntSize             mSize;
	uint32					    mDepth;
    TEXTURE_COLOR_FORMAT        mFormat;

    SerializedTextureSource    *mSource;

    friend class TextureFactory;
};
}
