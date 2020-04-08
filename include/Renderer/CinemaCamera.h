/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  CinemaCamera.h
 @brief Cinema Camera Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include "Camera.h"

namespace LimitEngine {
class CinemaCamera : public Camera 
{
public:
    CinemaCamera() {}
    virtual ~CinemaCamera() {}

    virtual void Update() override;


};
}