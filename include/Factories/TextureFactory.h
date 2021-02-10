/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TextureFactory.h
@brief Factory for creating texture
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

#include <LERenderer>

#include "ResourceFactory.h"

namespace LimitEngine {
class TextureImageFilter
{
public:
    virtual bool FilterImage(class TextureSourceImage *srcimg, class SerializedTextureSource *tarimg) = 0;
    virtual RendererFlag::BufferFormat GetFilteredImageFormat() const = 0;
    virtual uint32 GetMipCount() const = 0;
};
class TextureFactory : public ResourceFactory
{
public:
    static constexpr ResourceFactory::ID ID = GENERATE_RESOURCEFACTORY_ID("TEXT");

    TextureFactory() {}
    virtual ~TextureFactory() {}

    IReferenceCountedObject* Create(const ResourceSourceFactory *SourceFactory, const ResourceFactory::FileData &size) override;
    IReferenceCountedObject* CreateEmpty(const LEMath::IntSize &Size, const RendererFlag::BufferFormat &Format);
    void Release(IReferenceCountedObject *data) override;
    uint32 GetResourceTypeCode() override { return makeResourceTypeCode("LMDL"); }

    void SetImageFilter(TextureImageFilter* filter) { mImageFilter = filter; }

    class TextureSourceImage* FilterSourceImage(class TextureSourceImage *SourceImage);
    void SetSizeFilteredImage(const LEMath::IntVector2 &InSize) { mFilteredImageSize = InSize; }
    void SetSampleCount(uint32 count) { mSampleCount = count; }

private:
    TextureImageFilter* mImageFilter = nullptr;
    LEMath::IntVector2  mFilteredImageSize = LEMath::IntVector2::Zero;
    uint32 mSampleCount = 1024u;
};
}
