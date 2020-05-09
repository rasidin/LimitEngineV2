/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TextureFactory.h
@brief Factory for creating texture
@author minseob (leeminseob@outlook.com)
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
#include <LEHalfVector4.h>

namespace LimitEngine {
IReferenceCountedObject* TextureFactory::Create(const ResourceSourceFactory *SourceFactory, const ResourceFactory::FileData &Data)
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
                            SourceImage->mFormat = static_cast<uint32>(TEXTURE_COLOR_FORMAT_A8R8G8B8);
                        }
                        else if (formatnode->values[0] == "R8G8B8") {
                            SourceImage->mFormat = static_cast<uint32>(TEXTURE_COLOR_FORMAT_R8G8B8);
                        }
                        else if (formatnode->values[0] == "R8") {
                            SourceImage->mFormat = static_cast<uint32>(TEXTURE_COLOR_FORMAT_R8);
                        }
                        else if (formatnode->values[0] == "R16F") {
                            SourceImage->mFormat = static_cast<uint32>(TEXTURE_COLOR_FORMAT_R16F);
                        }
                        else if (formatnode->values[0] == "R32F") {
                            SourceImage->mFormat = static_cast<uint32>(TEXTURE_COLOR_FORMAT_R32F);
                        }
                        else if (formatnode->values[0] == "A16B16G16R16F") {
                            SourceImage->mFormat = static_cast<uint32>(TEXTURE_COLOR_FORMAT_A16B16G16R16F);
                        }
                        else if (formatnode->values[0] == "A32B32G32R32F") {
                            SourceImage->mFormat = static_cast<uint32>(TEXTURE_COLOR_FORMAT_A32B32G32R32F);
                        }
                    }
                    switch (static_cast<TEXTURE_COLOR_FORMAT>(SourceImage->mFormat))
                    {
                    case TEXTURE_COLOR_FORMAT_R8G8B8:
                        SourceImage->mRowPitch = 3 * SourceImage->GetSize().X();
                        break;
                    case TEXTURE_COLOR_FORMAT_A8R8G8B8:
                        SourceImage->mRowPitch = 4 * SourceImage->GetSize().X();
                        break;
                    case TEXTURE_COLOR_FORMAT_R8:
                        SourceImage->mRowPitch = 1 * SourceImage->GetSize().X();
                        break;
                    case TEXTURE_COLOR_FORMAT_R16F:
                        SourceImage->mRowPitch = 2 * SourceImage->GetSize().X();
                        break;
                    case TEXTURE_COLOR_FORMAT_G16R16F:
                        SourceImage->mRowPitch = 4 * SourceImage->GetSize().X();
                        break;
                    case TEXTURE_COLOR_FORMAT_R32F:
                        SourceImage->mRowPitch = 4 * SourceImage->GetSize().X();
                        break;
                    case TEXTURE_COLOR_FORMAT_A16B16G16R16F:
                        SourceImage->mRowPitch = 8 * SourceImage->GetSize().X();
                        break;
                    case TEXTURE_COLOR_FORMAT_A32B32G32R32F:
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
                                    if (AutoPointer<ResourceManager::RESOURCE> Resource = LE_ResourceManager.GetResourceWithoutRegister(filepath.GetCharPtr(), TextureFactory::ID)) {
                                        TextureRefPtr LoadedTexture = static_cast<Texture*>(Resource->data);
                                        SerializedTextureSource *TextureSource = LoadedTexture->GetSourceImage();
                                        if (SourceImage->GetSize() == TextureSource->GetSize() && TextureSource->GetFormat() == SourceImage->mFormat) {
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
        }
    }
    else {
        if (TextureSourceImage* SourceImage = static_cast<TextureSourceImage*>(SourceFactory->ConvertRawData(Data.Data, Data.Size))) {
            if (mImportFilter != TextureImportFilter::None) {
                FilterSourceImage(SourceImage);
            }
            output = Texture::GenerateFromSourceImage(SourceImage);
            delete SourceImage;
        }
    } 

    return dynamic_cast<IReferenceCountedObject*>(output);
}
void TextureFactory::FilterSourceImage(TextureSourceImage *SourceImage)
{
    void *OrgData = malloc(SourceImage->GetColorDataSize());
    ::memcpy(OrgData, SourceImage->GetColorData(), SourceImage->GetColorDataSize());
    auto SampleImage = [SourceImage, OrgData](LEMath::FloatPoint uv) {
        while (uv.X() < 0.0f) {
            uv.SetX(uv.X() + 1.0f);
        }
        while (uv.X() > 1.0f) {
            uv.SetX(uv.X() - 1.0f);
        }
        while (uv.Y() < 0.0f) {
            uv.SetY(uv.Y() + 1.0f);
        }
        while (uv.Y() > 1.0f) {
            uv.SetY(uv.Y() - 1.0f);
        }
        LEMath::IntPoint pixelPos((int)(floorf(uv.X() * SourceImage->GetSize().X())), (int)(floorf(uv.Y() * SourceImage->GetSize().Y())));
        LEMath::FloatColorRGBA OutColor = LEMath::FloatColorRGBA::Zero;
        switch (SourceImage->GetFormat())
        {
        case TEXTURE_COLOR_FORMAT_A16B16G16R16F: {
            uint8* ColorDataPtr = (uint8*)OrgData + SourceImage->GetRowPitch() * pixelPos.Y() + pixelPos.X() * sizeof(LEMath::half) * 4;
            LEMath::half *ColorDataHalfPtr = (LEMath::half*)ColorDataPtr;
            OutColor = (LEMath::FloatColorRGBA)LEMath::HalfVector4(ColorDataHalfPtr[0], ColorDataHalfPtr[1], ColorDataHalfPtr[2], ColorDataHalfPtr[3]);
        } break;
        case TEXTURE_COLOR_FORMAT_A32B32G32R32F: {
            uint8* ColorDataPtr = (uint8*)OrgData + SourceImage->GetRowPitch() * pixelPos.Y() + pixelPos.X() * sizeof(float) * 4;
            float *ColorDataFloatPtr = (float*)ColorDataPtr;
            OutColor = LEMath::FloatColorRGBA(ColorDataFloatPtr[0], ColorDataFloatPtr[1], ColorDataFloatPtr[2], ColorDataFloatPtr[3]);
        } break;
        case TEXTURE_COLOR_FORMAT_A8R8G8B8: {
            uint8* ColorDataPtr = (uint8*)OrgData + SourceImage->GetRowPitch() * pixelPos.Y() + pixelPos.X() * sizeof(uint8) * 4;
            OutColor = LEMath::FloatColorRGBA(ColorDataPtr[0] / 255.0f, ColorDataPtr[1] / 255.0f, ColorDataPtr[2] / 255.0f, 1.0f);
        }
        default:
            break;
        }
        return OutColor;
    };
    auto WriteToImage = [SourceImage](const LEMath::IntPoint &pos, LEMath::FloatColorRGBA color) {
        switch (SourceImage->GetFormat())
        {
        case TEXTURE_COLOR_FORMAT_A16B16G16R16F: {
            uint8* ColorDataPtr = (uint8*)SourceImage->GetColorData() + SourceImage->GetRowPitch() * pos.Y() + pos.X() * sizeof(LEMath::half) * 4;
            *(LEMath::HalfVector4*)ColorDataPtr = (LEMath::HalfVector4)color;
        } break;
        case TEXTURE_COLOR_FORMAT_A32B32G32R32F: {
            uint8* ColorDataPtr = (uint8*)SourceImage->GetColorData() + SourceImage->GetRowPitch() * pos.Y() + pos.X() * sizeof(float) * 4;
            *(LEMath::FloatColorRGBA*)ColorDataPtr = color;
        } break;
        case TEXTURE_COLOR_FORMAT_A8R8G8B8: {
            uint8* ColorDataPtr = (uint8*)SourceImage->GetColorData() + SourceImage->GetRowPitch() * pos.Y() + pos.X() * sizeof(uint8) * 4;
            ColorDataPtr[0] = (uint8)(color.X() * 255);
            ColorDataPtr[1] = (uint8)(color.Y() * 255);
            ColorDataPtr[2] = (uint8)(color.Z() * 255);
            ColorDataPtr[3] = (uint8)(color.W() * 255);
        } break;
        default:
            break;
        }
    };

    LEMath::IntSize imageSize = (mFilteredImageSize != LEMath::IntVector2::Zero)?mFilteredImageSize:SourceImage->GetSize();
    static constexpr float SampleDelta = 0.025f;
    LE_TaskManager.ParallelFor(imageSize.Height(), [SampleImage, WriteToImage, imageSize](uint32 StepBegin, uint32 StepEnd) {
        for (uint32 y = StepBegin; y <= StepEnd; y++) {
            for (int x = 0; x < imageSize.Width(); x++) {
                LEMath::FloatColorRGBA irradiance(0.0f, 0.0f, 0.0f, 1.0f);
                uint32 numSamples = 0u;
                float centerTheta = LEMath::LEMath_PI * y / imageSize.Height();
                float centerPhi = LEMath::LEMath_PI * 2.0f * x / imageSize.Width();
                LEMath::FloatVector3 normal = LEMath::FloatVector3(-sinf(centerTheta) * cosf(centerPhi), cosf(centerTheta), sinf(centerTheta) * sinf(centerPhi));
                LEMath::FloatVector3 up = (y == 0 || y == imageSize.Height() - 1) ? (LEMath::FloatVector3(1.0f, 0.0f, 0.0f)) : LEMath::FloatVector3(0.0f, 1.0f, 0.0f);
                LEMath::FloatVector3 tangent = (up ^ normal).Normalize();
                LEMath::FloatVector3 binormal = (normal ^ tangent).Normalize();

                for (float phi = 0.0f; phi < 2.0f * LEMath::LEMath_PI; phi += SampleDelta) {
                    for (float theta = 0.0f; theta < 0.5f * LEMath::LEMath_PI; theta += SampleDelta) {
                        LEMath::FloatVector3 tangentNormal(sinf(theta) * cosf(phi), sinf(theta) * sinf(phi), cosf(theta));
                        LEMath::FloatVector3 sampleDirection = tangentNormal.X() * tangent + tangentNormal.Y() * binormal + tangentNormal.Z() * normal;
                        LEMath::FloatVector2 longlat = LEMath::FloatVector2(atan2f(sampleDirection.X(), sampleDirection.Z()), acos(sampleDirection.Y()));
                        LEMath::FloatPoint sampleUV = (longlat + LEMath::FloatVector2(0.5f * LEMath::LEMath_PI, 0.0f)) / LEMath::FloatVector2(2.0f * LEMath::LEMath_PI, LEMath::LEMath_PI);
                        irradiance += SampleImage(sampleUV) * cos(theta) * sin(theta);
                        numSamples++;
                    }
                }
                irradiance /= (float)numSamples;
                irradiance *= LEMath::LEMath_PI;
                irradiance.SetW(1.0f);
                WriteToImage(LEMath::IntPoint(x, y), irradiance);
            }
        }
    });
}
void TextureFactory::Release(void *data)
{
    if (data == NULL)
        return;
    delete static_cast<Texture*>(data);
}
}