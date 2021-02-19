/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TextureFactory.h
@brief Factory for creating texture
@author minseob (https://github.com/rasidin)
***********************************************************/
#include "Core/Debug.h"
#include "Core/Util.h"
#include "Core/TextParser.h"
#include "Factories/TextureFactory.h"
#include "Factories/ResourceSourceFactory.h"
#include "Renderer/Texture.h"
#include "Managers/DrawManager.h"
#include "Managers/TaskManager.h"
#include "Managers/ResourceManager.h"

#include <LEIntVector2.h>
#include <LEFloatVector2.h>
#include <LEFloatVector3.h>
#include <LEHalfVector2.h>
#include <LEHalfVector4.h>

namespace LimitEngine {
SerializableRendererResource* TextureFactory::Create(const ResourceSourceFactory *SourceFactory, const ResourceFactory::FileData &Data)
{
    if (!Data.IsValid() || !SourceFactory)
        return NULL;

    Texture *output = nullptr;
    if (SourceFactory->GetID() == GENERATE_RESOURCEFACTORY_ID("TEPA")) {
        if (TextParser *parser = static_cast<TextParser*>(SourceFactory->ConvertRawData(Data.Data, Data.Size))) {
            TextParser::NODE *typenode = parser->GetNode("FILETYPE");
            if (typenode && typenode->values[0] == "TEXTURE") {
                SerializedTextureSource *SourceImage = new SerializedTextureSource();
                if (TextParser::NODE *datanode = parser->GetNode("DATA")) {
                    if (TextParser::NODE *widthnode = datanode->FindChild("WIDTH")) {
                        SourceImage->mSize.SetX(widthnode->ToInt());
                    }
                    if (TextParser::NODE *heightnode = datanode->FindChild("HEIGHT")) {
                        SourceImage->mSize.SetY(heightnode->ToInt());
                    }
                    if (TextParser::NODE *depthnode = datanode->FindChild("DEPTH")) {
                        SourceImage->mSize.SetZ(depthnode->ToInt());
                    }
                    if (TextParser::NODE *formatnode = datanode->FindChild("FORMAT")) {
                        if (formatnode->values[0] == "A8R8G8B8") {
                            SourceImage->mFormat = static_cast<uint32>(RendererFlag::BufferFormat::R8G8B8A8_UInt);
                        }
                        else if (formatnode->values[0] == "R8G8B8") {
                            LEASSERT(false);
                        }
                        else if (formatnode->values[0] == "R8") {
                            SourceImage->mFormat = static_cast<uint32>(RendererFlag::BufferFormat::R8_UInt);
                        }
                        else if (formatnode->values[0] == "R16F") {
                            SourceImage->mFormat = static_cast<uint32>(RendererFlag::BufferFormat::R16_Float);
                        }
                        else if (formatnode->values[0] == "R32F") {
                            SourceImage->mFormat = static_cast<uint32>(RendererFlag::BufferFormat::R32_Float);
                        }
                        else if (formatnode->values[0] == "A16B16G16R16F") {
                            SourceImage->mFormat = static_cast<uint32>(RendererFlag::BufferFormat::R16G16B16A16_Float);
                        }
                        else if (formatnode->values[0] == "A32B32G32R32F") {
                            SourceImage->mFormat = static_cast<uint32>(RendererFlag::BufferFormat::R32G32B32A32_Float);
                        }
                    }
                    switch (static_cast<RendererFlag::BufferFormat>(SourceImage->mFormat))
                    {
                    case RendererFlag::BufferFormat::R8G8B8A8_UInt:
                        SourceImage->mRowPitch = 4 * SourceImage->GetSize().X();
                        break;
                    case RendererFlag::BufferFormat::R8_UInt:
                        SourceImage->mRowPitch = 1 * SourceImage->GetSize().X();
                        break;
                    case RendererFlag::BufferFormat::R16_Float:
                        SourceImage->mRowPitch = 2 * SourceImage->GetSize().X();
                        break;
                    case RendererFlag::BufferFormat::R16G16_Float:
                        SourceImage->mRowPitch = 4 * SourceImage->GetSize().X();
                        break;
                    case RendererFlag::BufferFormat::R32_Float:
                        SourceImage->mRowPitch = 4 * SourceImage->GetSize().X();
                        break;
                    case RendererFlag::BufferFormat::R16G16B16A16_UInt:
                        SourceImage->mRowPitch = 8 * SourceImage->GetSize().X();
                        break;
                    case RendererFlag::BufferFormat::R32G32B32A32_Float:
                        SourceImage->mRowPitch = 16 * SourceImage->GetSize().X();
                        break;
                    default:
                        LEASSERT(0);
                    }
                    SourceImage->mIsCubemap = false;
                    SourceImage->mColorData.Resize(SourceImage->GetRowPitch() * SourceImage->GetSize().Y() * SourceImage->GetDepth());
                    char baseFolderPath[0xff];
                    GetBaseFolderPath(Data.Filename, baseFolderPath);
                    if (TextParser::NODE *filesnode = datanode->FindChild("FILES")) {
                        uint32 ImageSliceSize = SourceImage->GetRowPitch() * SourceImage->GetSize().Y();
                        for (uint32 fileindex = 0; fileindex < filesnode->children.count(); fileindex++) {
                            TextParser::NODE *filenode = filesnode->children[fileindex];
                            if (TextParser::NODE *filenamenode = filenode->FindChild("NAME")) {
                                if (const char *filename = filenamenode->values[0]) {
                                    String filepath(baseFolderPath);
                                    filepath += "\\";
                                    filepath += filename;
                                    AutoPointer<ResourceManager::RESOURCE> Resource = LE_ResourceManager.GetResourceWithoutRegister(filepath.GetCharPtr(), TextureFactory::ID);
                                    if (Resource.Exists()) {
                                        TextureRefPtr LoadedTexture = static_cast<Texture*>(Resource->data);
                                        SerializedTextureSource *TextureSource = LoadedTexture->GetSourceImage();
                                        if (SourceImage->GetSize() == TextureSource->GetSize() && TextureSource->GetFormat() == static_cast<RendererFlag::BufferFormat>(SourceImage->mFormat)) {
                                            ::memcpy(SourceImage->mColorData.GetData() + ImageSliceSize * fileindex, TextureSource->GetColorData(), ImageSliceSize);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                output = Texture::GenerateFromSourceImage(SourceImage);
                delete SourceImage;
            }
            delete parser;
        }
    }
    else {
        if (TextureSourceImage* SourceImage = static_cast<TextureSourceImage*>(SourceFactory->ConvertRawData(Data.Data, Data.Size))) {
            if (mImageFilter) {
                SerializedTextureSource* filteredsourceimage = new SerializedTextureSource();
                LEMath::IntSize imageSize = (mFilteredImageSize != LEMath::IntVector2::Zero) ? mFilteredImageSize : SourceImage->GetSize();
                filteredsourceimage->mSize = LEMath::IntVector3(imageSize.X(), imageSize.Y(), 1);
                filteredsourceimage->mMipCount = mImageFilter->GetMipCount();
                filteredsourceimage->mIsCubemap = false;
                filteredsourceimage->mFormat = static_cast<uint32>(mImageFilter->GetFilteredImageFormat());

                uint32 colordatasize = 0u;
                for (uint32 mipidx = 0; mipidx < filteredsourceimage->mMipCount; mipidx++) {
                    colordatasize += (imageSize.X() >> mipidx) * (imageSize.Y() >> mipidx) * sizeof(LEMath::FloatVector4);
                }
                filteredsourceimage->mColorData.Resize(colordatasize);

                mImageFilter->FilterImage(SourceImage, filteredsourceimage);
                delete SourceImage;
                SourceImage = filteredsourceimage;
            }
            output = Texture::GenerateFromSourceImage(SourceImage);
            delete SourceImage;
        }
    } 

    return output;
}
SerializableRendererResource* TextureFactory::CreateEmpty(const LEMath::IntSize& Size, const RendererFlag::BufferFormat& Format)
{
    SerializedTextureSource* sourceimage = new SerializedTextureSource(
        LEMath::IntVector3(Size.Width(), Size.Height(), 1),
        mImageFilter?mImageFilter->GetMipCount():1,
        mImageFilter?mImageFilter->GetFilteredImageFormat():Format
    );
    size_t imagesize = Size.X() * Size.Y() * RendererFlag::BufferFormatByteSize[static_cast<uint32>(mImageFilter?mImageFilter->GetFilteredImageFormat():Format)];
    sourceimage->mColorData.Resize(static_cast<uint32>(imagesize));
    ::memset(sourceimage->GetColorData(), 0, imagesize);

    if (mImageFilter)
        mImageFilter->FilterImage(nullptr, sourceimage);

    return Texture::GenerateFromSourceImage(sourceimage);
}
void TextureFactory::Release(SerializableRendererResource*data)
{
    if (data == NULL)
        return;
    delete (Texture*)(data);
}
}