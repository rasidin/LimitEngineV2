/**********************************************
* LimitEngineTextureTool Header
* @file FontConverter.h
* @brief Convert image to Font
************************************************/
#pragma once

class FontConverter
{
public:
    FontConverter() {}
    ~FontConverter() {}

    bool Convert(const char *InImageFileName, const char *InTextFileName, const char *OutFileName);
};
