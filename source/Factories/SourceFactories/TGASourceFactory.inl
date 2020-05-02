/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TGASourceFactory.h
@brief Source factory for tga file
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

#include "Factories/ResourceSourceFactory.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
class TGAImage : public TextureSourceImage
{
    struct TGAHeader
    {
        enum class EColorType : uint8 {
            NoImage = 0,
            IndexColor,
            FullColor,
            Grayscale,
            IndexColorRLE,
            FullColorRLE,
            GrayscaleRLE
        };

        uint8 ID;
        uint8 ColorMap;
        uint8 ColorType;
        uint16 ColorMapEntry;
        uint8 ColorMapEntrySize;
        uint16 X;
        uint16 Y;
        uint16 Width;
        uint16 Height;
        uint8 ColorDepth;
        uint8 Attributes;
    };
public:
    TGAImage() : mColorData(nullptr) {}
    virtual ~TGAImage() {
        if (mColorData) 
            MemoryAllocator::Free(mColorData);
        mColorData = nullptr;
    }
    virtual bool Load(const void *Data, size_t Size) final override {
        memcpy(&mHeader, Data, sizeof(TGAHeader));
        uint8 *mSrcColorData = (uint8*)((TGAHeader*)Data + 1);

        if ((TGAHeader::EColorType)mHeader.ColorType == TGAHeader::EColorType::FullColor && mHeader.ColorDepth == 24) { // Convert colors
            mHeader.ColorDepth = 32;
            uint32 pitch = GetRowPitch();
            mColorData = MemoryAllocator::Alloc(pitch * mHeader.Height, LimitEngineMemoryCategory_Graphics);
            uint8* CurrentColorDataPtr = (uint8*)mColorData;
            for (int y = static_cast<int>(mHeader.Height) - 1; y >= 0; y--) {
                for (int x = 0, width = static_cast<int>(mHeader.Width); x < width; x++) {
                    CurrentColorDataPtr[0] = (mSrcColorData + (mHeader.Width * 3 * y) + 3 * x)[2];
                    CurrentColorDataPtr[1] = (mSrcColorData + (mHeader.Width * 3 * y) + 3 * x)[1];
                    CurrentColorDataPtr[2] = (mSrcColorData + (mHeader.Width * 3 * y) + 3 * x)[0];
                    CurrentColorDataPtr[3] = 0xff;
                    CurrentColorDataPtr += 4;
                }
            }
        }
        else { // Copy through
            uint32 pitch = GetRowPitch();
            mColorData = MemoryAllocator::Alloc(pitch * mHeader.Height, LimitEngineMemoryCategory_Graphics);
            uint8* CurrentColorData = static_cast<uint8*>(mColorData);
            for (int y = static_cast<int>(mHeader.Height) - 1; y >= 0; y--) {
                memcpy(CurrentColorData, mSrcColorData + pitch * y, pitch);
                CurrentColorData += pitch;
            }
        }
        return true;
    }
    virtual LEMath::IntSize GetSize() const final override {
        return LEMath::IntSize(mHeader.Width, mHeader.Height);
    }
    virtual uint32 GetDepth() const final override { return 1u; }
    virtual bool IsCubemap() const final override { return false; }
    virtual TEXTURE_COLOR_FORMAT GetFormat() const final override {
        uint8 tgaFormat = mHeader.ColorType;
        uint8 tgaDepth = mHeader.ColorDepth;
        switch ((TGAHeader::EColorType)tgaFormat)
        {
        case TGAHeader::EColorType::IndexColor:
            if (tgaDepth == 24)
                return TEXTURE_COLOR_FORMAT_R8G8B8;
            else
                return TEXTURE_COLOR_FORMAT_A8R8G8B8;
        case TGAHeader::EColorType::Grayscale:
            return TEXTURE_COLOR_FORMAT_R8;
        case TGAHeader::EColorType::FullColor:
            if (tgaDepth == 24)
                return TEXTURE_COLOR_FORMAT_R8G8B8;
            else
                return TEXTURE_COLOR_FORMAT_A8R8G8B8;
        default:
            LEASSERT(0);
        }
        return TEXTURE_COLOR_FORMAT_UNKNOWN;
    }
    virtual uint32 GetRowPitch() const {
        uint8 tgaFormat = mHeader.ColorType;
        uint8 tgaDepth = mHeader.ColorDepth;
        uint16 tgaWidth = mHeader.Width;
        switch ((TGAHeader::EColorType)tgaFormat)
        {
        case TGAHeader::EColorType::IndexColor:
            if (tgaDepth == 24)
                return 3 * tgaWidth;
            else
                return 4 * tgaWidth;
        case TGAHeader::EColorType::Grayscale:
            return tgaWidth;
        case TGAHeader::EColorType::FullColor:
            if (tgaDepth == 24)
                return 3 * tgaWidth;
            else
                return 4 * tgaWidth;
        default:
            LEASSERT(0);
        }
        return 0u;
    }
    virtual void* GetColorData() const final override {
        return mColorData;
    }
    virtual size_t GetColorDataSize() const final override {
        return GetRowPitch() * mHeader.Width;
    }
    virtual uint32 GetMipCount() const final override { return 1u; }
private:
    TGAHeader mHeader;
    void *mColorData;
};
class TGASourceFactory : public ResourceSourceFactory
{
public:
    static constexpr ID FactoryID = GENERATE_RESOURCEFACTORY_ID("TGAS");

    TGASourceFactory() {}
    virtual ~TGASourceFactory() {}

    virtual ID GetID() const override { return FactoryID; }

    virtual void* ConvertRawData(const void *Data, size_t Size) const {
        TGAImage *Output = new TGAImage();
        if (Output->Load(Data, Size)){
            return dynamic_cast<void*>(Output);
        }
        delete Output;
        return nullptr;
    }
};
}