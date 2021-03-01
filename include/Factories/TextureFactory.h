/*********************************************************************
Copyright(c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this softwareand associated documentation
files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and /or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file  TextureFactory.h
@brief texture factory
@author minseob(https://github.com/rasidin)
*********************************************************************/
#ifndef LIMITENGINEV2_FACTORIES_TEXTUREFACTORY_H_
#define LIMITENGINEV2_FACTORIES_TEXTUREFACTORY_H_

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

    SerializableRendererResource* Create(const ResourceSourceFactory *SourceFactory, const ResourceFactory::FileData &size) override;
    SerializableRendererResource* CreateEmpty(const LEMath::IntSize &Size, const RendererFlag::BufferFormat &Format);
    void Release(SerializableRendererResource *data) override;
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

#endif // LIMITENGINEV2_FACTORIES_TEXTUREFACTORY_H_