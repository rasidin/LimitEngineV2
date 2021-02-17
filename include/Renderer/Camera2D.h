/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Camera2D.h
 @brief Camera Class in 2D
 @author minseob (https://github.com/rasidin)
 -----------------------------------------------------------
 History:
 - 2013/05/19 Created by minseob
 ***********************************************************/

#ifndef _LE_CAMERA2D_H_
#define _LE_CAMERA2D_H_

#include "LE_Camera.h"

namespace LimitEngine {
    class Camera2D : public Camera
    {
    public:
        Camera2D();
        virtual ~Camera2D();

        void Update();
        void SetScreenSize(int width, int height);
        void SetScale(float scale)      { mScale = scale; }
    private:
        float   mScale;
    };
}

#endif
