/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  CinemaCamera.h
 @brief Cinema Camera Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/
#pragma once

#include "Camera.h"

#include <LEFloatVector2.h>

namespace LimitEngine {
class CinemaCamera : public Camera 
{
public:
    CinemaCamera() {}
    virtual ~CinemaCamera() {}

    virtual void Update() override;

    virtual float GetFovRadians() const override;
    virtual float GetExposure() const override;

    void SetFocalLength(float InFocalLength) { mFocalLength = InFocalLength; }
    float GetFocalLength() const { return mFocalLength; }
    void SetShutterSpeed(float InShutterSpeed) { mShutterSpeed = InShutterSpeed; }
    void SetFStop(float InFStop) { mFStop = InFStop; }
    void SetISO(float InISO) { mISO = InISO; }
    void SetExposureOffset(float ExposureOffset) { mExposureOffset = ExposureOffset; }
    void SetCloppedScale(const LEMath::FloatSize &InCloppedScale) { mCloppedScale = InCloppedScale; }

private:
    LEMath::FloatSize mSensorSize = LEMath::FloatSize(36.0f, 24.0f); // 36mm fullsize sensor
    LEMath::FloatSize mCloppedScale = LEMath::FloatSize(1.0f, 1.0f);
    float mFocalLength = 35.0f;              // 35mm Lens
    float mShutterSpeed = 1.0f/60.0f;
    float mFStop = 1.0f;
    float mISO = 100.0f;
    float mExposureOffset = 0.0f;
};
}