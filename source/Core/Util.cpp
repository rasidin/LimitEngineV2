/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Util.cpp
 @brief Utilities
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include <LEIntVector2.h>
#include <LEFloatVector2.h>

#include "Core/Util.h"
#include "Managers/DrawManager.h"

namespace LimitEngine {
	size_t GetSizeAlign(size_t size, size_t align)	{ return (size + align - 1) & (~(align-1)); }
    void Swap(float &a, float &b)                   { float c = b; b = a; a = c; }
    bool IsNan(float f) { return f != f; }
    void LowerCase(char *text)
    {
        size_t length = strlen(text);
        for(size_t i=0;i<length;i++)
        {
            if (text[i]>0x40 && text[i]<0x5b) text[i] += 0x20;
        }
    }
    void UpperCase(char *text)
    {
        size_t length = strlen(text);
        for(size_t i=0;i<length;i++)
        {
            if (text[i]>0x60 && text[i]<0x7b) text[i] -= 0x20;
        }        
    }
    LEMath::IntSize ConvertReal2Virtual(const LEMath::IntSize &p)
	{
		static const uint32 FixedHeight = 1000;
		return LEMath::IntSize(FixedHeight * p.X() / p.Y(), FixedHeight);
	}
    LEMath::FloatPoint ConvertReal2Virtual(const LEMath::FloatPoint &p)
	{
		static const uint32 FixedHeight = 1000;
		return LEMath::FloatPoint(FixedHeight * p.X() / p.Y(), float(FixedHeight));
	}
    LEMath::IntSize ConvertVirtual2Real(const LEMath::IntSize &p)
    {
        LEMath::IntSize output(p);
        LEMath::IntSize virtualScreenSize = LE_DrawManager.GetVirtualScreenSize();
        LEMath::IntSize realScreenSize = LE_DrawManager.GetRealScreenSize();
        output.SetX(p.X() * realScreenSize.Width() / virtualScreenSize.Width());
        output.SetY(p.Y() * realScreenSize.Height() / virtualScreenSize.Height());
        return output;
    }
    LEMath::FloatPoint ConvertVirtual2Real(const LEMath::FloatPoint &p)
    {
        LEMath::FloatPoint output(p);
        LEMath::IntPoint virtualScreenSize = LE_DrawManager.GetVirtualScreenSize();
        LEMath::IntPoint realScreenSize = LE_DrawManager.GetRealScreenSize();
        output.SetX(p.X() * realScreenSize.Width() / virtualScreenSize.Width());
        output.SetY(p.Y() * realScreenSize.Height() / virtualScreenSize.Height());
        return output;
    }
    void GetBaseFolderPath(const char *path, char *basefolderpath)
    {
        char *copiedPath = _strdup(path);

        size_t idx = 0;
        for (idx = strlen(copiedPath); idx > 0; idx--) {
            if (copiedPath[idx] == '\\' || copiedPath[idx] == '/') {
                break;
            }
        }
        if (idx == 0) return;

        copiedPath[idx] = 0;
        ::memcpy(basefolderpath, copiedPath, strlen(copiedPath) + 1);

        ::free(copiedPath);
    }
    void GetFileName(const char *path, bool containExtension, char *filename)
    {
        char *copiedPath = _strdup(path);
        int lastDirectoryWord = 0;
        int lastDotWord = 0;
        for (int idx = 0, length = (int)strlen(copiedPath); idx < length; idx++) {
            if (copiedPath[idx] == '\\' || copiedPath[idx] == '/') {
                lastDirectoryWord = idx;
            }
            else if (copiedPath[idx] == '.') {
                lastDotWord = idx;
            }
        }
        if (!containExtension) {
            copiedPath[lastDotWord] = 0;
        }
        char *srcFileName = &copiedPath[lastDirectoryWord?(lastDirectoryWord+1):0];
        ::memcpy(filename, srcFileName, strlen(srcFileName));

        ::free(copiedPath);
    }
    void GetExtension(const char *path, char *ext)
    {
        size_t length = strlen(path);
        size_t extpos = length;
        for(;extpos>0 && path[extpos]!='.';extpos--);
        if (extpos>0)
        {
            size_t l=0;
            for (size_t i=extpos;i<length;i++, l++)
            {
                ext[l] = path[i];
            }
            ext[l] = 0;
            LowerCase(ext);
        }
    }
    void GetTypeFromFileName(const char *path, char *type)
    {
        size_t length = strlen(path);
        size_t extpos = length;
        int firstPoint = 0;
        int secondPoint = 0;
        for (; extpos > 0; extpos--) {
            if (path[extpos] == '.') {
                if (firstPoint == 0)
                    firstPoint = static_cast<int>(extpos);
                else if (secondPoint == 0) {
                    secondPoint = static_cast<int>(extpos);
                    break;
                }
            }
        }
        if (secondPoint>0)
        {
            size_t l = 0;
            for (size_t i = secondPoint + 1; i<firstPoint; i++, l++)
            {
                type[l] = path[i];
            }
            type[l] = 0;
            LowerCase(type);
        }
    }
    void GetFormatFromFileName(const char *path, char *format)
    {
        size_t length = strlen(path);
        size_t extpos = length;
        for (; extpos>0 && path[extpos] != '.'; extpos--);
        if (extpos>0)
        {
            size_t l = 0;
            for (size_t i = extpos + 1; i<length; i++, l++)
            {
                format[l] = path[i];
            }
            format[l] = 0;
            LowerCase(format);
        }
    }
    uint8 FloatToUInt8(float f)
    {
        return MAX(0, MIN(255, uint32(f*255.0f)));
    }
}