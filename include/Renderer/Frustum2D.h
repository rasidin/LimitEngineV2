/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Frustum2D.h
 @brief Frustum Class for 2D
 @author minseob (https://github.com/rasidin)
 -----------------------------------------------------------
 History:
 - 2013/05/19 Created by minseob
 ***********************************************************/

#ifndef _LE_FRUSTUM2D_H_
#define _LE_FRUSTUM2D_H_

#include "LE_Frustum.h"
#include "LE_FVector2.h"

namespace LimitEngine {
    class Frustum2D : public Frustum
    {
    public:
        Frustum2D()
        : mWidth(480)
        , mHeight(320)
        {}
        
        void SetScreenSize(float width, float height)
        {
            mWidth = width;
            mHeight = height;
        }
        
        fSize GetScreenSize()
        {
            return fSize(mWidth, mHeight);
        }
        
        fMatrix4x4 GetProjectionMatrix()
        {
#if defined(USE_DX9) || defined(USE_DX11)
            return fMatrix4x4(
                               2.0f/ mWidth, 0.0f, 0.0f, 0.0f,
                               0.0f,-2.0f/ mHeight, 0.0f, 0.0f,
                               0.0f, 0.0f, 1.0f/(mFarMeters - mNearMeters), mNearMeters /(mFarMeters - mNearMeters),
                               0.0f, 0.0f, 0.0f, 1.0f
            );
#else
            return fMatrix4x4(
                               2.0f/ mWidth, 0.0f, 0.0f, 0.0f,
                               0.0f,-2.0f/ mHeight, 0.0f, 0.0f,
                               0.0f, 0.0f,-1.0f/(mFarMeters-mNearMeters), 0.0f,
                               0.0f, 0.0f, _near/(mFarMeters - mNearMeters), 1.0f
            );
#endif
        }
    private:
        float       mWidth;
        float       mHeight;
    };
}

#endif
