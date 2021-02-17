/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LE_AABB.cpp
 @brief AABB Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/

#include "Renderer/AABB.h"

#include <LEFloatVector3.h>

#include "Core/Archive.h"
#include "Core/Util.h"
#include "Renderer/FRay.h"

namespace LimitEngine {
    template<> Archive& Archive::operator << (AABB &InAABB)
    {
        *this << InAABB.minimum;
        *this << InAABB.maximum;
        return *this;
    }

	AABB::INTERSECT_RESULT AABB::INTERSECT_FAIL = AABB::INTERSECT_RESULT(false, LEMath::FloatVector3::Zero);
	AABB::INTERSECT_RESULT
	AABB::Intersect(const fRay &r) {
		INTERSECT_RESULT result(false, LEMath::FloatVector3::Zero);

		LEMath::FloatVector3 dir = r.GetDirection();
		float tmin = (minimum.x - r.org.x) / dir.x;
		float tmax = (maximum.x - r.org.x) / dir.x;
		if (tmin > tmax) Swap(tmin, tmax);
		float tymin = (minimum.y - r.org.y) / dir.y;
		float tymax = (maximum.y - r.org.y) / dir.y;
		if (tymin > tymax) Swap(tymin, tymax);
		if ((tmin > tymax) || (tymin > tmax)) return INTERSECT_FAIL;
		if (tymin > tmin) tmin = tymin;
		if (tymax < tmax) tmax = tymax;
		float tzmin = (minimum.z - r.org.z) / dir.z;
		float tzmax = (maximum.z - r.org.z) / dir.z;
		if (tzmin > tzmax) Swap(tzmin, tzmax);
		if ((tmin > tzmax) || (tzmin > tmax)) return INTERSECT_FAIL;
		if (tzmin > tmin) tmin = tzmin;
		if (tzmax < tmax) tmax = tzmax;

		return INTERSECT_RESULT(true, LEMath::FloatVector3(tmin, tmax, 0.0f));
	}
	LEMath::FloatVector3
	AABB::GetNormal(const LEMath::FloatVector3 &v)
	{
			const static float CheckEpsilon = 1.0e-5f;
			if (abs(v.x - minimum.x) < CheckEpsilon) {
				return LEMath::FloatVector3(-1.0f, 0.0f, 0.0f);
			}
			if (abs(v.y - minimum.y) < CheckEpsilon) {
				return LEMath::FloatVector3( 0.0f,-1.0f, 0.0f);
			}
			if (abs(v.z - minimum.z) < CheckEpsilon) {
				return LEMath::FloatVector3( 0.0f, 0.0f,-1.0f);
			}
			if (abs(v.x - maximum.x) < CheckEpsilon) {
				return LEMath::FloatVector3( 1.0f, 0.0f, 0.0f);
			}
			if (abs(v.y - maximum.y) < CheckEpsilon) {
				return LEMath::FloatVector3( 0.0f, 1.0f, 0.0f);
			}
			if (abs(v.z - maximum.z) < CheckEpsilon) {
				return LEMath::FloatVector3( 0.0f, 0.0f, 1.0f);
			}
			return LEMath::FloatVector3();
	}
}