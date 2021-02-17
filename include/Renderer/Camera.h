/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Camera.h
 @brief Camera Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/
#pragma once

#include <LEIntVector2.h>
#include <LEFloatVector3.h>
#include <LEFloatMatrix4x4.h>

#include "Core/ReferenceCountedObject.h"
#include "Core/MetaData.h"
#include "Renderer/Frustum.h"

namespace LimitEngine {
    class Camera : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>, public MetaData
    {
        static const float DefaultFOVRadians;
    public:
        Camera();
        virtual ~Camera();

        // Update by frame
        virtual void Update();
        
        // Set position of camera
        virtual void SetPosition(const LEMath::FloatVector3 &p)         { mPosition = p; }
        // Get camera position
        const LEMath::FloatVector3& GetPosition() const                 { return mPosition; }
        // Set direction of camera
        void SetDirection(const LEMath::FloatVector3 &d)                { mDirection = d; }
        // Get camera direction
        const LEMath::FloatVector3& GetDirection() const                { return mDirection; }
        // Set up vector of camera
        void SetUp(const LEMath::FloatVector3 &u)                       { mUp = u; }
        // Get camera up vector
        const LEMath::FloatVector3& GetUp() const                       { return mUp; }
        // Set aim target position of camera
        void SetAim(const LEMath::FloatVector3 &t);

        // Set screen size for screen frustum
		virtual void SetScreenSize(int width, int height)               { mScreenSize = LEMath::IntSize(width, height); if (mFrustum) mFrustum->SetAspectRatio(float(height) / width); }
		virtual LEMath::IntSize GetScreenSize() const                   { return mScreenSize; }
        // Get aspect ratio of frustum
        float GetAspectRatio()                                          { if(mFrustum) return mFrustum->GetAspectRatio(); return 1.0f; }
        // Set field of view (radians)
        virtual void SetFovRadians(float fov)                           { if(mFrustum) mFrustum->SetFOVRadians(fov); }
        // Get field of view (radians)
        virtual float GetFovRadians() const                             { if(mFrustum) return mFrustum->GetFOVRadians(); return DefaultFOVRadians; }
        // Get view matrix
        const LEMath::FloatMatrix4x4& GetViewMatrix() const             { return mViewMatrix; }
        // Get projection matrix
        LEMath::FloatMatrix4x4  GetProjectionMatrix()                   { if(mFrustum) return mFrustum->GetProjectionMatrix(); return LEMath::FloatMatrix4x4::Identity; }

        LEMath::FloatVector4 GetUVtoViewParameter() const               { if (mFrustum) return mFrustum->GetUVtoViewParameter(); return LEMath::FloatVector4::Zero; }
        LEMath::FloatVector4 GetPerspectiveProjectionParameters() const { if (mFrustum) return mFrustum->GetPerspectiveProjectionParameters(); return LEMath::FloatVector4::Zero; }

        // Get exposure (EV)
        virtual float GetExposure() const                               { return 1.0f; }
	private:
		// Setup metadata for editor
		void setupMetaData();
    private:
        LEMath::FloatVector3        mDirection;     // Direction of camera
        LEMath::FloatVector3        mUp;            // Up vector of camera
        
		LEMath::IntSize			    mScreenSize;	// Screen size

    protected:
        LEMath::FloatVector3        mPosition;      // Position of camera
        Frustum                    *mFrustum;       // Frustum for camera view
        LEMath::FloatMatrix4x4      mViewMatrix;    // View matrix made by camera
    };
}
