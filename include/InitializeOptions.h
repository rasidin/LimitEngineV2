/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  InitializeOptions.h
 @brief Option for initialization of LimitEngine
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LEIntVector2.h>

namespace LimitEngine {
struct InitializeOptions
{
    enum class ColorSpace {
        sRGB,
        PQ,
        Linear
    };
    LEMath::IntSize Resolution;
    ColorSpace SceneColorSpace;
    ColorSpace OutputColorSpace;

    explicit InitializeOptions(const LEMath::IntSize &InResolution, ColorSpace InSceneColorSpace, ColorSpace InOutputColorSpace)
        : Resolution(InResolution)
        , SceneColorSpace(InSceneColorSpace)
        , OutputColorSpace(InOutputColorSpace)
    {}
};
}