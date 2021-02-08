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

#include <LERenderer>

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
class TextureImpl : public Object<LimitEngineMemoryCategory::Graphics>
{
public:
    TextureImpl(class Texture *InOwner) : mOwner(InOwner) {}
    virtual ~TextureImpl() {}
    
    virtual const LEMath::IntSize& GetSize() const = 0;
    virtual const RendererFlag::BufferFormat& GetFormat() const = 0;
    virtual void* GetHandle() const = 0;
    virtual void* GetResource() const = 0;
    virtual void* GetDepthSurfaceHandle() const = 0;
    
    virtual void LoadFromMemory(const void *data, size_t size) = 0;
    virtual bool Create(const LEMath::IntSize &size, const RendererFlag::BufferFormat &format, uint32 usage, uint32 mipLevels, void *initializeData = NULL, size_t initDataSize = 0u) = 0;
	virtual bool Create3D(const LEMath::IntVector3 &size, const RendererFlag::BufferFormat &format, uint32 usage, uint32 mipLevels, void *initializeData = NULL, size_t initDataSize = 0u) = 0;
	virtual void CreateScreenColor(const LEMath::IntSize &size) = 0;
    virtual void CreateColor(const LEMath::IntSize &size, const ByteColorRGBA &color) = 0;
    virtual void CreateDepthStencil(const LEMath::IntSize &size, const RendererFlag::BufferFormat &format) = 0;
    virtual void CreateRenderTarget(const LEMath::IntSize &size, const RendererFlag::BufferFormat &format, uint32 usage) = 0;
    virtual void GenerateMipmap() = 0;

    virtual void* GetShaderResourceView() const = 0;
    virtual void* GetUnorderedAccessView() const = 0;
    virtual void* GetRenderTargetView() const = 0;
    virtual void* GetDepthStencilView() const = 0;

    virtual void* Lock(const LEMath::IntRect &rect, int mipLevel = 0) = 0;
    virtual void Unlock(int mipLevel = 0) = 0;

protected:
    class Texture* mOwner = nullptr;
};
class TextureSourceImage : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>
{
public:
    TextureSourceImage() {}
    virtual ~TextureSourceImage() {}

    virtual bool Load(const void *Data, size_t Size) = 0;
    virtual LEMath::IntSize GetSize() const = 0;
    virtual uint32 GetDepth() const = 0;
    virtual bool IsCubemap() const = 0;
    virtual RendererFlag::BufferFormat GetFormat() const = 0;
    virtual uint32 GetRowPitch() const = 0;
    virtual void* GetColorData() const = 0;
    virtual size_t GetColorDataSize() const = 0;
    virtual uint32 GetMipCount() const = 0;
};
class SerializedTextureSource : public TextureSourceImage
{
public:
    explicit SerializedTextureSource() : mSize(), mRowPitch(0u), mMipCount(1u), mFormat(static_cast<uint32>(RendererFlag::BufferFormat::Unknown)), mIsCubemap(false) {}
    explicit SerializedTextureSource(const LEMath::IntVector3& Size, uint32 MipCount, const RendererFlag::BufferFormat& Format)
        : mSize(Size), mMipCount(MipCount), mFormat(static_cast<uint32>(Format)), mIsCubemap(false)
    {}
    explicit SerializedTextureSource(const TextureSourceImage &SourceImage);
    virtual ~SerializedTextureSource();

    virtual bool Load(const void *Data, size_t Size) override { return false; }
    virtual LEMath::IntSize GetSize() const override { return mSize.XY(); }
    virtual uint32 GetDepth() const override { return mSize.Z(); }
    virtual bool IsCubemap() const override { return mIsCubemap; }
    virtual RendererFlag::BufferFormat GetFormat() const override { return static_cast<RendererFlag::BufferFormat>(mFormat); }
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
class TextureInterface : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>
{
protected:
    virtual const LEMath::IntSize& GetSize() const = 0;

    virtual void* GetShaderResourceView() const = 0;
    virtual void* GetUnorderedAccessView() const = 0;
    virtual void* GetRenderTargetView() const = 0;
    virtual void* GetDepthStencilView() const = 0;
    virtual void* GetResource() const = 0;

    virtual void SetResourceState(const ResourceState& state) = 0;
    virtual ResourceState GetResourceState() const = 0;

public:
    virtual RendererFlag::BufferFormat GetFormat() const = 0;

    friend class TextureRendererAccessor;
};
class TextureRendererAccessor
{
public:
    TextureRendererAccessor(TextureInterface* inter)
        : mInterface(inter)
    {}

    bool IsValid() const { return mInterface != nullptr; }

    TextureInterface* GetTexture() const { return mInterface.Get(); }

    void* GetShaderResourceView() const  { return mInterface.IsValid() ? mInterface->GetShaderResourceView() : nullptr; }
    void* GetUnorderedAccessView() const { return mInterface.IsValid() ? mInterface->GetUnorderedAccessView() : nullptr; }
    void* GetRenderTargetView() const    { return mInterface.IsValid() ? mInterface->GetRenderTargetView() : nullptr; }
    void* GetDepthStencilView() const    { return mInterface.IsValid() ? mInterface->GetDepthStencilView() : nullptr; }
    void* GetResource() const            { return mInterface.IsValid() ? mInterface->GetResource() : nullptr; }

    void SetResourceState(const ResourceState& state) { if (mInterface.IsValid()) mInterface->SetResourceState(state); }
    ResourceState GetResourceState() const { return mInterface.IsValid() ? mInterface->GetResourceState() : ResourceState::Common; }

    RendererFlag::BufferFormat GetFormat() const { return mInterface.IsValid() ? mInterface->GetFormat() : RendererFlag::BufferFormat::Unknown; }

private:
    ReferenceCountedPointer<TextureInterface> mInterface;
};
class Texture : public TextureInterface, public SerializableResource
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

    static constexpr uint32 DebugNameLength = 0x40;

public:
    Texture();
    virtual ~Texture();

    void Release();

    virtual void InitResource() override;

    void LoadFromMemory(const void *data, size_t size);
	void LoadFromMERLBRDFData(const void *data, size_t size);

    void Create(const LEMath::IntSize &size, const RendererFlag::BufferFormat &format, uint32 usage = 0, uint32 mipLevels = 1, void *initailizeData = NULL, size_t initDataSize = 0u);
	void Create3D(const LEMath::IntVector3 &size, const RendererFlag::BufferFormat &format, uint32 usage = 0, uint32 mipLevels = 1, void *initializeData = NULL, size_t initDataSize = 0u);
    void CreateScreenColor(const LEMath::IntSize &size);
    void CreateColor(const LEMath::IntSize &size, const ByteColorRGBA &color);
    void CreateDepthStencil(const LEMath::IntSize &size, const RendererFlag::BufferFormat &format);
    void CreateRenderTarget(const LEMath::IntSize &size, const RendererFlag::BufferFormat &format, uint32 usage = 0);

    void CreateUsingSourceData();

    void* Lock(const LEMath::IntRect &rect, int mipLevel = 0)
    { if (mImpl) { return mImpl->Lock(rect, mipLevel); } else return NULL; }
    void Unlock(int mipLevel = 0)
    { if (mImpl) { mImpl->Unlock(mipLevel); } }

    void GenerateMipmap()                                           { if (mImpl) { mImpl->GenerateMipmap(); } }
    
    void* GetHandle()                                               { return (mImpl)?mImpl->GetHandle():nullptr; }
    void* GetDepthSurfaceHandle()                                   { return (mImpl)?mImpl->GetDepthSurfaceHandle():nullptr; }

    // Implementation of TextureInterface
protected:
    virtual void* GetShaderResourceView() const override            { return (mImpl)?mImpl->GetShaderResourceView():nullptr; }
    virtual void* GetUnorderedAccessView() const override           { return (mImpl)?mImpl->GetUnorderedAccessView():nullptr; }
    virtual void* GetRenderTargetView() const override              { return (mImpl)?mImpl->GetRenderTargetView():nullptr; }
    virtual void* GetDepthStencilView() const override              { return (mImpl)?mImpl->GetDepthStencilView():nullptr; }
    virtual void* GetResource() const override { return mImpl->GetResource(); }

    virtual void SetResourceState(const ResourceState& InState) override { mResourceState = InState; }
    virtual ResourceState GetResourceState() const override { return mResourceState; }

public:
    virtual const LEMath::IntSize& GetSize() const override         { return mSize; }
    virtual RendererFlag::BufferFormat GetFormat() const override   { return mFormat; }

    void SetSourceImage(SerializedTextureSource *InSource)          { if (mSource) delete mSource; mSource = InSource; if (InSource) { mSize = InSource->GetSize(); mDepth = InSource->GetDepth(); mFormat = InSource->GetFormat(); } }
    SerializedTextureSource* GetSourceImage() const                 { return mSource; }


    virtual bool Serialize(Archive &OutArchive) override;

    void SetDebugName(const char *InDebugName);

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
    RendererFlag::BufferFormat mFormat;

    ResourceState               mResourceState;

    SerializedTextureSource    *mSource;

    char                        mDebugName[DebugNameLength];

    friend class TextureFactory;
    friend class CommandBuffer;
};
class FrameBufferTexture : public TextureInterface
{
public:
    FrameBufferTexture(const LEMath::IntSize &size) : mSize(size) {}
    virtual ~FrameBufferTexture() {}

    // Implementation of TextureInterface
    virtual const LEMath::IntSize& GetSize() const override { return mSize; }
    virtual RendererFlag::BufferFormat GetFormat() const override;

protected:
    virtual void* GetShaderResourceView() const override { return nullptr; }
    virtual void* GetUnorderedAccessView() const override { return nullptr; }
    virtual void* GetRenderTargetView() const override;
    virtual void* GetDepthStencilView() const override { return nullptr; }
    virtual void* GetResource() const override;

    virtual void SetResourceState(const ResourceState& state) override;
    virtual ResourceState GetResourceState() const override;

private:
    LEMath::IntSize mSize;
};
}
