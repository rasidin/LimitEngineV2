/**********************************************
* LimitEngineTextureTool Header
* @file TextureConverter.h
* @brief Convert image to LETexture
************************************************/
#pragma once

#include <LEIntVector2.h>

class TextureConverter
{
public:
    struct ConvertOptions
    {
        LEMath::IntVector2 FilteredImageSize = LEMath::IntVector2::Zero;
        int SampleCount = 1024;

        bool GenerateIrradiance = false;
        bool GenerateReflection = false;
        bool GenerateEnvironmentBRDF = false;
    };

public:
    TextureConverter() {}
    ~TextureConverter() {}

    bool Convert(const char *InFilename, const char *OutFilename, const ConvertOptions &Options);
};