/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  FloatRay.h
 @brief Ray
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LEFloatVector3.h>

namespace LimitEngine {
    class fRay { public:
        LEMath::FloatVector3 org, tar;
        fRay()
			: org(0.0f)
			, tar(0.0f)
		{
            org = LEMath::FloatVector3::Zero;
            tar = LEMath::FloatVector3::Zero;
        }
        fRay(const LEMath::FloatVector3 &o, const LEMath::FloatVector3 &t) {
            org = o;
            tar = t;
        }
        LEMath::FloatVector3 GetDirection() const {
            return (tar - org).Normalize();
        }
        float GetLength() {
            return (tar - org).Length();
        }
        float GetLength() const {
            return (tar - org).Length();
        }
        LEMath::FloatVector3 GetMinimum() {
            return org.Minimum(tar);
        }
        LEMath::FloatVector3 GetMaximum() {
            return org.Maximum(tar);
        }
    };
}
