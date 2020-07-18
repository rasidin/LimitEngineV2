/**********************************************
* LimitEngineTextureTool Source Code
* @file TextureConverter.cpp
* @brief Convert image to LETexture
************************************************/
#include "TextureConverter.h"

#include <Factories/TextureFactory.h>
#include <Renderer/Texture.h>
#include <Managers/ResourceManager.h>

#include "Logger.h"
#include "Definitions.h"
#include "EXRSourceFactory.h"

bool TextureConverter::Convert(const char *InFilename, const char *OutFilename, const TextureConverter::ConvertOptions &Options)
{
    LOG_SUBLOG << "Convert " | InFilename | " -> " | OutFilename;

    LimitEngine::EXRSourceFactory *EXRFactory = new LimitEngine::EXRSourceFactory();
    LimitEngine::ResourceManager::GetSingleton().AddSourceFactory("exr", EXRFactory);

    if (Options.GenerateIrradiance || Options.GenerateReflection || Options.GenerateEnvironmentBRDF)
    if (LimitEngine::TextureFactory *textureFactory = (LimitEngine::TextureFactory*)LimitEngine::ResourceManager::GetSingleton().GetFactory(LimitEngine::TextureFactory::ID)) {
        if (Options.GenerateIrradiance)
            textureFactory->SetImportFilter(LimitEngine::TextureFactory::TextureImportFilter::Irradiance);
        if (Options.GenerateReflection)
            textureFactory->SetImportFilter(LimitEngine::TextureFactory::TextureImportFilter::Reflection);
        if (Options.GenerateEnvironmentBRDF)
            textureFactory->SetImportFilter(LimitEngine::TextureFactory::TextureImportFilter::EnvironmentBRDF);
        if (Options.FilteredImageSize != LEMath::IntVector2::Zero)
            textureFactory->SetSizeFilteredImage(Options.FilteredImageSize);
        textureFactory->SetSampleCount(Options.SampleCount);
    }

    LimitEngine::AutoPointer<LimitEngine::ResourceManager::RESOURCE> loadedResource = LimitEngine::ResourceManager::GetSingleton().GetResourceWithoutRegister(InFilename, LimitEngine::TextureFactory::ID);
    if (LimitEngine::Texture *texture = static_cast<LimitEngine::Texture*>(loadedResource->data)) {
        LimitEngine::ResourceManager::GetSingleton().SaveResource(OutFilename, texture);
    }
    
    return true;
}