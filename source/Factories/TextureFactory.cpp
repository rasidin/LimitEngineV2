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
#include <LEHalfVector2.h>
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
                                    if (AutoPointer<ResourceManager::RESOURCE> Resource = LE_ResourceManager.GetResourceWithoutRegister(filepath.GetCharPtr(), TextureFactory::ID)) {
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
            if (mImportFilter != TextureImportFilter::None) {
                if (TextureSourceImage *FilteredSourceImage = FilterSourceImage(SourceImage)) {
                    delete SourceImage;
                    SourceImage = FilteredSourceImage;
                }
            }
            if (mImageFilter) {
                SerializedTextureSource* filteredsourceimage = new SerializedTextureSource();
                LEMath::IntSize imageSize = (mFilteredImageSize != LEMath::IntVector2::Zero) ? mFilteredImageSize : SourceImage->GetSize();
                filteredsourceimage->mSize = LEMath::IntVector3(imageSize.X(), imageSize.Y(), (mImportFilter == TextureImportFilter::Reflection) ? 5 : 1);
                filteredsourceimage->mMipCount = 1;
                filteredsourceimage->mIsCubemap = false;
                filteredsourceimage->mFormat = static_cast<uint32>(mImageFilter->GetFilteredImageFormat());

                mImageFilter->FilterImage(SourceImage, filteredsourceimage);
                delete SourceImage;
                SourceImage = filteredsourceimage;
            }
            output = Texture::GenerateFromSourceImage(SourceImage);
            delete SourceImage;
        }
    } 

    return (IReferenceCountedObject*)(output);
}
TextureSourceImage* TextureFactory::FilterSourceImage(TextureSourceImage *SourceImage)
{
    void *OrgData = malloc(SourceImage->GetColorDataSize());
    ::memcpy(OrgData, SourceImage->GetColorData(), SourceImage->GetColorDataSize());
    auto SampleImage = [SourceImage, OrgData](LEMath::FloatPoint uv) {
        while (uv.X() < 0.0f) {
            uv.SetX(uv.X() + 1.0f);
        }
        while (uv.X() >= 1.0f) {
            uv.SetX(uv.X() - 1.0f);
        }
        while (uv.Y() < 0.0f) {
            uv.SetY(uv.Y() + 1.0f);
        }
        while (uv.Y() >= 1.0f) {
            uv.SetY(uv.Y() - 1.0f);
        }
        LEMath::IntPoint pixelPos((int)(floorf(uv.X() * SourceImage->GetSize().X())), (int)(floorf(uv.Y() * SourceImage->GetSize().Y())));
        LEMath::FloatColorRGBA OutColor = LEMath::FloatColorRGBA::Zero;
        switch (SourceImage->GetFormat())
        {
        case RendererFlag::BufferFormat::R16G16_Float: {
            uint8* ColorDataPtr = (uint8*)OrgData + SourceImage->GetRowPitch() * pixelPos.Y() + pixelPos.X() * sizeof(LEMath::half) * 2;
            LEMath::half *ColorDataHalfPtr = (LEMath::half*)ColorDataPtr;
            OutColor = (LEMath::FloatColorRGBA)LEMath::HalfVector4(ColorDataHalfPtr[0], ColorDataHalfPtr[1], 0, 0);
        } break;
        case RendererFlag::BufferFormat::R32G32_Float: {
            uint8* ColorDataPtr = (uint8*)OrgData + SourceImage->GetRowPitch() * pixelPos.Y() + pixelPos.X() * sizeof(float) * 2;
            float *ColorDataFloatPtr = (float*)ColorDataPtr;
            OutColor = LEMath::FloatColorRGBA(ColorDataFloatPtr[0], ColorDataFloatPtr[1], 0.0f, 0.0f);
        }
        case RendererFlag::BufferFormat::R16G16B16A16_Float: {
            uint8* ColorDataPtr = (uint8*)OrgData + SourceImage->GetRowPitch() * pixelPos.Y() + pixelPos.X() * sizeof(LEMath::half) * 4;
            LEMath::half *ColorDataHalfPtr = (LEMath::half*)ColorDataPtr;
            OutColor = (LEMath::FloatColorRGBA)LEMath::HalfVector4(ColorDataHalfPtr[0], ColorDataHalfPtr[1], ColorDataHalfPtr[2], ColorDataHalfPtr[3]);
        } break;
        case RendererFlag::BufferFormat::R32G32B32A32_Float: {
            uint8* ColorDataPtr = (uint8*)OrgData + SourceImage->GetRowPitch() * pixelPos.Y() + pixelPos.X() * sizeof(float) * 4;
            float *ColorDataFloatPtr = (float*)ColorDataPtr;
            OutColor = LEMath::FloatColorRGBA(ColorDataFloatPtr[0], ColorDataFloatPtr[1], ColorDataFloatPtr[2], ColorDataFloatPtr[3]);
        } break;
        case RendererFlag::BufferFormat::R8G8B8A8_UInt: {
            uint8* ColorDataPtr = (uint8*)OrgData + SourceImage->GetRowPitch() * pixelPos.Y() + pixelPos.X() * sizeof(uint8) * 4;
            OutColor = LEMath::FloatColorRGBA(ColorDataPtr[0] / 255.0f, ColorDataPtr[1] / 255.0f, ColorDataPtr[2] / 255.0f, 1.0f);
        }
        default:
            break;
        }
        return OutColor;
    };
    SerializedTextureSource *filteredSourceImage = new SerializedTextureSource();
    auto WriteToImage = [filteredSourceImage](const LEMath::IntVector3 &pos, LEMath::FloatColorRGBA color) {
        uint32 sliceStride = filteredSourceImage->GetRowPitch() * filteredSourceImage->GetSize().Y();
        switch (filteredSourceImage->GetFormat())
        {
        case RendererFlag::BufferFormat::R16G16_Float: {
            uint32 colorDataOffset = sliceStride * pos.Z() + filteredSourceImage->GetRowPitch() * pos.Y() + pos.X() * sizeof(LEMath::half) * 2;
            LEASSERT(colorDataOffset < filteredSourceImage->GetColorDataSize());
            uint8* ColorDataPtr = (uint8*)filteredSourceImage->GetColorData() + colorDataOffset;
            *(LEMath::HalfVector2*)ColorDataPtr = (LEMath::HalfVector2)color;
        } break;
        case RendererFlag::BufferFormat::R32G32_Float: {
            uint32 colorDataOffset = sliceStride * pos.Z() + filteredSourceImage->GetRowPitch() * pos.Y() + pos.X() * sizeof(float) * 2;
            LEASSERT(colorDataOffset < filteredSourceImage->GetColorDataSize());
            uint8* ColorDataPtr = (uint8*)filteredSourceImage->GetColorData() + colorDataOffset;
            *(LEMath::FloatVector2*)ColorDataPtr = LEMath::FloatVector2(color.X(), color.Y());
        } break;
        case RendererFlag::BufferFormat::R16G16B16A16_Float: {
            uint32 colorDataOffset = sliceStride * pos.Z() + filteredSourceImage->GetRowPitch() * pos.Y() + pos.X() * sizeof(LEMath::half) * 4;
            LEASSERT(colorDataOffset < filteredSourceImage->GetColorDataSize());
            uint8* ColorDataPtr = (uint8*)filteredSourceImage->GetColorData() + colorDataOffset;
            *(LEMath::HalfVector4*)ColorDataPtr = (LEMath::HalfVector4)color;
        } break;
        case RendererFlag::BufferFormat::R32G32B32A32_Float: {
            uint32 colorDataOffset = sliceStride * pos.Z() + filteredSourceImage->GetRowPitch() * pos.Y() + pos.X() * sizeof(float) * 4;
            LEASSERT(colorDataOffset < filteredSourceImage->GetColorDataSize());
            uint8* ColorDataPtr = (uint8*)filteredSourceImage->GetColorData() + colorDataOffset;
            *(LEMath::FloatColorRGBA*)ColorDataPtr = color;
        } break;
        default:
            break;
        }
    };

    LEMath::IntSize imageSize = (mFilteredImageSize != LEMath::IntVector2::Zero)?mFilteredImageSize:SourceImage->GetSize();
    filteredSourceImage->mSize = LEMath::IntVector3(imageSize.X(), imageSize.Y(), (mImportFilter==TextureImportFilter::Reflection)?5:1);
    filteredSourceImage->mMipCount = 1;
    filteredSourceImage->mIsCubemap = false;
    filteredSourceImage->mFormat = static_cast<uint32>((mImportFilter == TextureImportFilter::EnvironmentBRDF) ? RendererFlag::BufferFormat::R32G32_Float : SourceImage->GetFormat());
    switch (filteredSourceImage->GetFormat())
    {
    case RendererFlag::BufferFormat::R8_UInt:
        filteredSourceImage->mRowPitch = filteredSourceImage->mSize.X() * 1;
        break;
    case RendererFlag::BufferFormat::R16_Float:
        filteredSourceImage->mRowPitch = filteredSourceImage->mSize.X() * 2;
        break;
    case RendererFlag::BufferFormat::R32_Float:
    case RendererFlag::BufferFormat::R16G16_Float:
    case RendererFlag::BufferFormat::R8G8B8A8_UNorm:
        filteredSourceImage->mRowPitch = filteredSourceImage->mSize.X() * 4;
        break;
    case RendererFlag::BufferFormat::R32G32_Float:
    case RendererFlag::BufferFormat::R16G16B16A16_Float:
        filteredSourceImage->mRowPitch = filteredSourceImage->mSize.X() * 8;
        break;
    case RendererFlag::BufferFormat::R32G32B32A32_Float:
        filteredSourceImage->mRowPitch = filteredSourceImage->mSize.X() * 16;
        break;
    default:
        delete filteredSourceImage;
        return nullptr;
    }
    uint32 sliceStride = filteredSourceImage->mRowPitch * filteredSourceImage->GetSize().Y();
    filteredSourceImage->mColorData.Resize(sliceStride * filteredSourceImage->mSize.Z());

    auto saturate = [](float v) {
        return min(1.0f, max(0.0f, v));
    };

    auto radicalInverse_VdC = [](uint32 bits) {
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
        return float(bits) * 2.3283064365386963e-10f; // / 0x100000000
    };
    auto hammersley2d = [radicalInverse_VdC](uint32 i, uint32 N) {
        return LEMath::FloatVector2(float(i) / float(N), radicalInverse_VdC(i));
    };

    auto GSmith = [](float d, float r) {
        float k = (r + 1) * (r + 1) / 8;
        return d / ((1 - k) * d + k);
    };

    uint32 numSamples = mSampleCount;

    auto IntergrateBRDF = [numSamples, hammersley2d, GSmith, saturate](float Roughness, float NoV) {
        float r2 = Roughness * Roughness;
        LEMath::FloatVector3 V((NoV>=1.0f)?0.0f:sqrtf(1.0f - NoV * NoV), NoV, 0.0f);

        LEMath::FloatVector3 N(0.0f, 1.0f, 0.0f);
        LEMath::FloatVector3 T(1.0f, 0.0f, 0.0f);
        LEMath::FloatVector3 B(0.0f, 0.0f, 1.0f);

        float X = 0.0f, Y = 0.0f;
        for (uint32 sampleIndex = 0; sampleIndex < numSamples; sampleIndex++) {
            LEMath::FloatVector2 xi = hammersley2d(sampleIndex, numSamples);
            float phi = xi.Y() * 2.0f * LEMath::LEMath_PI;
            float cosTheta = sqrtf((1.0f - xi.X()) / (1.0f + (r2 * r2 - 1.0f) * xi.X()));
            float sinTheta = (cosTheta >= 1.0f)?0.0f:sqrtf(1.0f - cosTheta * cosTheta);
            LEMath::FloatVector3 sampleDirectionInTangent(cosf(phi) * sinTheta, sinf(phi) * sinTheta, cosTheta);
            LEMath::FloatVector3 H = sampleDirectionInTangent.X() * T + sampleDirectionInTangent.Y() * B + sampleDirectionInTangent.Z() * N;
            LEMath::FloatVector3 L = 2 * (N | H) * H - N;

            float NoL = saturate(N | L);
            float NoV = saturate(N | V);
            float VoH = saturate(V | H);
            float NoH = saturate(N | H);
            if (NoL > 0.0f) {
                float G = GSmith(NoL, Roughness) * GSmith(NoV, Roughness);
                float Visualization = (NoH == 0 || NoV == 0)?0.0f:(G * VoH / (NoH * NoV));
                float FresnelFactor = powf(1.0f - VoH, 5.0f);
                X += (1.0f - FresnelFactor) * Visualization;
                Y += FresnelFactor * Visualization;
            }
        }

        return LEMath::FloatVector2(X, Y) / static_cast<float>(numSamples);
    };

    switch (mImportFilter) {
    case TextureImportFilter::Irradiance: {
        LE_TaskManager.ParallelFor(imageSize.Height(), [SampleImage, WriteToImage, hammersley2d, imageSize, numSamples](uint32 StepBegin, uint32 StepEnd) {
            for (int y = static_cast<int>(StepBegin); y <= static_cast<int>(StepEnd); y++) {
                for (int x = 0; x < imageSize.Width(); x++) {
                    LEMath::FloatColorRGBA irradiance(0.0f, 0.0f, 0.0f, 1.0f);
                    float centerTheta = LEMath::LEMath_PI * y / imageSize.Height();
                    float centerPhi = LEMath::LEMath_PI * 2.0f * x / imageSize.Width();
                    LEMath::FloatVector3 normal = LEMath::FloatVector3(-sinf(centerTheta) * cosf(centerPhi), cosf(centerTheta), sinf(centerTheta) * sinf(centerPhi));
                    LEMath::FloatVector3 up = (y == 0 || y == imageSize.Height() - 1) ? (LEMath::FloatVector3(1.0f, 0.0f, 0.0f)) : LEMath::FloatVector3(0.0f, 1.0f, 0.0f);
                    LEMath::FloatVector3 tangent = (up ^ normal).Normalize();
                    LEMath::FloatVector3 binormal = (normal ^ tangent).Normalize();

                    for (uint32 sample = 0; sample < numSamples; sample++) {
                        LEMath::FloatVector2 xi = hammersley2d(sample, numSamples);
                        float phi = xi.Y() * 2.0f * LEMath::LEMath_PI;
                        float cosTheta = 1.0f - xi.X();
                        float sinTheta = (sample > 0) ? sqrtf(1.0f - cosTheta * cosTheta) : 0.0f;
                        LEMath::FloatVector3 sampleDirectionInTangent(cosf(phi) * sinTheta, sinf(phi) * sinTheta, cosTheta);
                        LEMath::FloatVector3 sampleDirection = sampleDirectionInTangent.X() * tangent + sampleDirectionInTangent.Y() * binormal + sampleDirectionInTangent.Z() * normal;
                        LEMath::FloatVector2 longlat = LEMath::FloatVector2(atan2f(sampleDirection.X(), sampleDirection.Z()), acos(sampleDirection.Y()));
                        LEMath::FloatPoint sampleUV = (longlat + LEMath::FloatVector2(0.5f * LEMath::LEMath_PI, 0.0f)) / LEMath::FloatVector2(2.0f * LEMath::LEMath_PI, LEMath::LEMath_PI);
                        irradiance += SampleImage(sampleUV) * cosTheta * sinTheta;
                    }
                    irradiance /= (float)numSamples;
                    irradiance *= LEMath::LEMath_PI;
                    irradiance.SetW(1.0f);
                    WriteToImage(LEMath::IntVector3(x, y, 0), irradiance);
                }
            }
        });
    } break;
    case TextureImportFilter::Reflection: {
        LE_TaskManager.ParallelFor(5, [SampleImage, WriteToImage, hammersley2d, imageSize, numSamples](uint32 StepBegin, uint32 StepEnd) {
            float roughness = StepBegin * 0.25f;
            for (int y = 0; y < static_cast<int>(imageSize.Height()); y++) {
                for (int x = 0; x < static_cast<int>(imageSize.Width()); x++) {
                    LEMath::FloatColorRGBA radiance(0.0f, 0.0f, 0.0f, 1.0f);
                    float centerTheta = LEMath::LEMath_PI * y / imageSize.Height();
                    float centerPhi = LEMath::LEMath_PI * 2.0f * x / imageSize.Width();
                    LEMath::FloatVector3 normal = LEMath::FloatVector3(-sinf(centerTheta) * cosf(centerPhi), cosf(centerTheta), sinf(centerTheta) * sinf(centerPhi));
                    LEMath::FloatVector3 up = (y == 0 || y == imageSize.Height() - 1) ? (LEMath::FloatVector3(1.0f, 0.0f, 0.0f)) : LEMath::FloatVector3(0.0f, 1.0f, 0.0f);
                    LEMath::FloatVector3 tangent = (up ^ normal).Normalize();
                    LEMath::FloatVector3 binormal = (normal ^ tangent).Normalize();

                    float weight = 0.0f;
                    uint32 sampleCount = numSamples * StepBegin + 1;
                    for (uint32 sample = 0; sample < sampleCount; sample++) {
                        LEMath::FloatVector2 xi = hammersley2d(sample, sampleCount);
                        
                        float r2 = roughness * roughness;

                        float phi = xi.Y() * 2.0f * LEMath::LEMath_PI;
                        float cosTheta = sqrtf((1.0f - xi.X()) / (1.0f + (r2 * r2 - 1.0f) * xi.X()));
                        float sinTheta = sqrtf(max(1.0e-4f, 1.0f - cosTheta * cosTheta));
                        LEMath::FloatVector3 sampleDirectionInTangent(cosf(phi) * sinTheta, sinf(phi) * sinTheta, cosTheta);
                        LEMath::FloatVector3 H = sampleDirectionInTangent.X() * tangent + sampleDirectionInTangent.Y() * binormal + sampleDirectionInTangent.Z() * normal;
                        LEMath::FloatVector3 L = 2 * (normal | H) * H - normal;

                        float NoL = normal | L;
                        if (NoL > 0) {
                            LEMath::FloatVector2 longlat = LEMath::FloatVector2(atan2f(L.X(), L.Z()), acos(L.Y()));
                            LEMath::FloatPoint sampleUV = (longlat + LEMath::FloatVector2(0.5f * LEMath::LEMath_PI, 0.0f)) / LEMath::FloatVector2(2.0f * LEMath::LEMath_PI, LEMath::LEMath_PI);
                            radiance += SampleImage(sampleUV) * NoL;
                            weight += NoL;
                        }
                    }
                    radiance /= weight;
                    radiance.SetW(1.0f);
                    WriteToImage(LEMath::IntVector3(x, y, StepBegin), radiance);
                }
            }
        });
    } break;
    case TextureImportFilter::EnvironmentBRDF: {
        LE_TaskManager.ParallelFor(imageSize.Height(), [WriteToImage, IntergrateBRDF, imageSize](uint32 StepBegin, uint32 StepEnd) {
            for (int y = static_cast<int>(StepBegin); y <= static_cast<int>(StepEnd); y++) {
                for (int x = 0; x < imageSize.Width(); x++) {
                    float roughness = (float)y / imageSize.Height();
                    float NoV = 1.0f - (float)x / imageSize.Width();
                    LEMath::FloatVector2 envBRDF = IntergrateBRDF(roughness, NoV);
                    WriteToImage(LEMath::IntVector3(x, y, 0), LEMath::FloatVector4(envBRDF.X(), envBRDF.Y(), 0.0f, 0.0f));
                }
            }
        });
    } break;
    }

    return static_cast<TextureSourceImage*>(filteredSourceImage);
}
void TextureFactory::Release(IReferenceCountedObject *data)
{
    if (data == NULL)
        return;
    delete dynamic_cast<Texture*>(data);
}
}