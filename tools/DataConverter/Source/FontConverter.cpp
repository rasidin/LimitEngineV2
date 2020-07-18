/**********************************************
* LimitEngineTextureTool Source Code
* @file FontConverter.cpp
* @brief Convert image to LEFont
************************************************/
#include "FontConverter.h"

#include <Renderer/Font.h>
#include <Managers/ResourceManager.h>

#include "Logger.h"
#include "Definitions.h"

bool FontConverter::Convert(const char *InImageFileName, const char *InTextFileName, const char *OutFileName)
{
    LOG_SUBLOG << "Convert" | InImageFileName | " + " | InTextFileName | " -> " | OutFileName;

    if (LimitEngine::Font *OutFont = LimitEngine::Font::GenerateFromFile(InImageFileName, InTextFileName)) {
        LimitEngine::ResourceManager::GetSingleton().SaveResource(OutFileName, OutFont);
    }

    return true;
}