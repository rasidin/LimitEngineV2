/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  CinemaCamera.cpp
 @brief Cinema Camera Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include "Renderer/CinemaCamera.h"

#include "Managers/DrawManager.h"

#include <math.h>

namespace LimitEngine {
float CinemaCamera::GetFovRadians() const
{
    return 2.0f * atanf(mSensorSize.Height() * mCloppedScale.X() * 0.5f / mFocalLength);
}
float CinemaCamera::GetExposure() const
{
    float av = log2f(mFStop);
    float tv = log2f(mShutterSpeed);
    float step = log2f(mISO / 100.0f);
    return (av + tv) * step + mExposureOffset;
}
void CinemaCamera::Update()
{
    Camera::Update();
}
}