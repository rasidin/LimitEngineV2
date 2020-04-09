/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TextureFactory.h
@brief Factory for creating texture
@author minseob (leeminseob@outlook.com)
***********************************************************/
#include "Factories/TextureFactory.h"
#include "Factories/ResourceSourceFactory.h"
#include "Renderer/Texture.h"
#include "Managers/DrawManager.h"

namespace LimitEngine {
void* TextureFactory::Create(const ResourceSourceFactory *SourceFactory, const void *data, size_t size)
{
    if (!data || !SourceFactory)
        return NULL;

    Texture *output = nullptr;
    if (TextureSourceImage* SourceImage = static_cast<TextureSourceImage*>(SourceFactory->ConvertRawData(data, size))) {
        output = Texture::GenerateFromSourceImage(SourceImage);
        delete SourceImage;
    }

    return output;
}
void TextureFactory::Release(void *data)
{
    if (data == NULL)
        return;
    delete static_cast<Texture*>(data);
}
}