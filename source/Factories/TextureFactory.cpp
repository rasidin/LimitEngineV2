/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  TextureFactory.h
@brief Factory for creating texture
@author minseob (leeminseob@outlook.com)
***********************************************************/
#include "Factories/TextureFactory.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
void* TextureFactory::Create(const char *format, const void *data, size_t size)
{
    if (data == NULL)
        return NULL;

	Texture *output = new Texture();
	if (strcmp("bin", format) == 0) { // BRDF binary (MERL)
		output->LoadFromMERLBRDFData(data, size);
	}
	else { // Default picture files
		output->LoadFromMemory(data, size);
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