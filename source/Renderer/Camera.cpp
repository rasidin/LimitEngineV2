/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Camera.cpp
 @brief Camera Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Renderer/Camera.h"

#include <LEFloatVector3.h>
#include <LEFloatMatrix4x4.h>

namespace LimitEngine {
const float Camera::DefaultFOVRadians = PI*0.5f;

    Camera::Camera()
    : mPosition(0.0f, 0.0f, 0.0f)
    , mDirection(0.0f, 0.0f, 1.0f)
    , mUp (0.0f, 1.0f, 0.0f)
    , mViewMatrix()
    , mFrustum(NULL)
    {
        mFrustum = new Frustum();
        mViewMatrix = LEMath::FloatMatrix4x4::Identity;

		setupMetaData();
    }

	void Camera::setupMetaData()
	{
		AddMetaDataVariable("Position",  "fVector3", METADATA_POINTER(mPosition));
		AddMetaDataVariable("Direction", "fVector3", METADATA_POINTER(mDirection));
		AddMetaDataVariable("Up",        "fVector3", METADATA_POINTER(mUp));
	}
    
    Camera::~Camera()
    {
        if (mFrustum) delete mFrustum; 
        mFrustum = NULL;
    }
    
    void Camera::Update()
    {
        LEMath::FloatVector3 right = (mUp ^ mDirection).Normalize();
        LEMath::FloatVector3 up = (mDirection ^ right).Normalize();
        mViewMatrix = LEMath::FloatMatrix4x4(
            right.X(), up.X(), mDirection.X(), 0.0f,
            right.Y(), up.Y(), mDirection.Y(), 0.0f,
            right.Z(), up.Z(), mDirection.Z(), 0.0f,
            (-mPosition | right), (-mPosition | up), (-mPosition | mDirection), 1.0f
        );
    }
    
    void Camera::SetAim(const LEMath::FloatVector3 &t/*AimTarget*/)
    {
        mDirection = (t - mPosition).Normalize();
    }
}