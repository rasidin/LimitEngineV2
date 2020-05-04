/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TextureFactory.h
@brief Factory for creating texture
@author minseob (leeminseob@outlook.com)
***********************************************************/
#include "Core/Debug.h"
#include "Factories/TextureFactory.h"
#include "Factories/ResourceSourceFactory.h"
#include "Renderer/Texture.h"
#include "Managers/DrawManager.h"
#include "Managers/TaskManager.h"

#include <LEIntVector2.h>
#include <LEFloatVector2.h>
#include <LEFloatVector3.h>
#include <LEHalfVector4.h>

namespace LimitEngine {
void* TextureFactory::Create(const ResourceSourceFactory *SourceFactory, const void *data, size_t size)
{
    if (!data || !SourceFactory)
        return NULL;

    Texture *output = nullptr;
    if (TextureSourceImage* SourceImage = static_cast<TextureSourceImage*>(SourceFactory->ConvertRawData(data, size))) {
        if (mImportFilter != TextureImportFilter::None) {
            FilterSourceImage(SourceImage);
        }
        output = Texture::GenerateFromSourceImage(SourceImage);
        delete SourceImage;
    }

    return output;
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

    LEMath::IntSize imageSize = SourceImage->GetSize();
    static constexpr float SampleDelta = 0.025f;
    LE_TaskManager.ParallelFor(imageSize.Height(), [SampleImage, WriteToImage, imageSize](uint32 StepBegin, uint32 StepEnd) {
        for (uint32 y = StepBegin; y <= StepEnd; y++) {
            for (int x = 0; x < imageSize.Width(); x++) {
                LEMath::FloatColorRGBA irradiance(0.0f, 0.0f, 0.0f, 1.0f);
                uint32 numSamples = 0u;
                float centerTheta = LEMath::LEMath_PI * y / imageSize.Height();
                float centerPhi = LEMath::LEMath_PI * 2.0f * x / imageSize.Width();
                LEMath::FloatVector3 normal = LEMath::FloatVector3(-sinf(centerTheta) * cosf(centerPhi), cosf(centerTheta), sinf(centerTheta) * sinf(centerPhi));
                LEMath::FloatVector3 tangent = (y == 0 || y == imageSize.Height() - 1) ? (LEMath::FloatVector3(1.0f, 0.0f, 0.0f)) : LEMath::FloatVector3(0.0f, 1.0f, 0.0f);
                LEMath::FloatVector3 binormal = normal ^ tangent;
                tangent = binormal ^ normal;

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