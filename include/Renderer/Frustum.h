/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LE_Frustum.h
 @brief Frustum Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/
#pragma once

#include <LEFloatMatrix4x4.h>

#include "Core/Common.h"

namespace LimitEngine {
#define LE_DEFAULT_ASPECTRATIO      1.0f
#define LE_DEFAULT_FARCLIP_METERS   100.0f
#define LE_DEFAULT_NEARCLIP_METERS  0.1f
#define LE_DEFAULT_FOV_RADIANS      PI*0.333f

    /*@class
     *@breif Frustum class
     */
    class Frustum
    {
    public:
        Frustum()
        : mAspectRatio  (LE_DEFAULT_ASPECTRATIO)
        , mFarMeters    (LE_DEFAULT_FARCLIP_METERS)
        , mNearMeters   (LE_DEFAULT_NEARCLIP_METERS)
        , mFovRadians   (LE_DEFAULT_FOV_RADIANS)
        {}
        
        // Set aspect ratio of screen
        void  SetAspectRatio(float ratio)       { mAspectRatio = ratio; }
        // Get screen aspect ratio
        float GetAspectRatio() const            { return mAspectRatio; }
        // Set field of view (radians)
        void  SetFOVRadians(float fov)          { mFovRadians = fov; }
        // Get field of view (radians)
        float GetFOVRadians() const             { return mFovRadians; }
        // Set far clip of screen (meters)
        void  SetFarMeters(float f)             { mFarMeters = f; }
        // Get screen far clip (meters)
        float GetFarMeters() const              { return mFarMeters; }
        // Set near clip of screen (meters)
        void  SetNearMeters(float n)            { mNearMeters = n; }
        // Get screen near clip (meters)
        float GetNearMeters() const             { return mNearMeters; }
        
        // Get projection matrix of screen
        virtual LEMath::FloatMatrix4x4 GetProjectionMatrix()
        {
            const float topDistanceInScreen = tanf(mFovRadians * 0.5f) * mNearMeters;       // Vertical of screen
            const float rightDistanceInScreen = topDistanceInScreen / mAspectRatio; // Horizontal of screen
#if defined(USE_DX9) || defined(USE_DX11) // For directX
            return LEMath::FloatMatrix4x4(mNearMeters / rightDistanceInScreen, 0.0f,                              0.0f,                                                  0.0f,
                                          0.0f,                                mNearMeters / topDistanceInScreen, 0.0f,                                                  0.0f,
                                          0.0f,                                0.0f,                              mFarMeters               / (mFarMeters - mNearMeters), 1.0f,
                                          0.0f,                                0.0f,                             -mFarMeters * mNearMeters / (mFarMeters - mNearMeters), 0.0f);
#else
            return LEMath::FloatMatrix4x4(mNearMeters/ rightDistanceInScreen, 0.0f, 0.0f, 0.0f,
                              0.0f, mNearMeters/ topDistanceInScreen, 0.0f, 0.0f,
                              0.0f, 0.0f, (mFarMeters + mNearMeters)/(mFarMeters - mNearMeters), 1.0f,
                              0.0f, 0.0f, -2.0f*mFarMeters*mNearMeters /(mFarMeters - mNearMeters), 0.0f);
#endif
        }

        LEMath::FloatVector4 GetUVtoViewParameter() const
        {
            const float f = tanf(mFovRadians * 0.5f);
            const float rcpFocal1 = f / mAspectRatio;
            const float rcpFocal2 = f;
            return LEMath::FloatVector4(2.0f * rcpFocal1, -2.0f * rcpFocal2, -rcpFocal1, rcpFocal2);
        }

        LEMath::FloatVector4 GetPerspectiveProjectionParameters() const
        {
            return LEMath::FloatVector4(mNearMeters, mFarMeters, mFarMeters - mNearMeters, 0.0f);
        }

    protected:
        float       mFovRadians;        // Field of view (radians)
        float       mAspectRatio;       // Apect ratio of screen
        float       mFarMeters;         // Far clip of screen (meters)
        float       mNearMeters;        // Near clip of screen (meters)
    };
}
