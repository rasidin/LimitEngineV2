/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_AABB.h
 @brief AABB Class
 @author minseob (https://github.com/rasidin)
 -----------------------------------------------------------
 History:
 - 2012/12/02 Created by minseob
 ***********************************************************/

#ifndef _LE_AABB_H_
#define _LE_AABB_H_

#include <float.h>
#include <LEFloatVector3.h>
#include <LEFloatVector4.h>
#include <LEFloatMatrix4x4.h>

#include "Containers/Pair.h"
#include "Renderer/FRay.h"

namespace LimitEngine {
    class AABB
    {public:
        typedef Pair<bool, LEMath::FloatVector3> INTERSECT_RESULT;
        
        AABB() : maximum(0.0f), minimum(FLT_MAX) {}
        AABB(const LEMath::FloatVector3 &mini, const LEMath::FloatVector3 &maxi) : minimum(mini), maximum(maxi) {}
        
        void operator|=(const LEMath::FloatVector3 &v)
        {
            Merge(v);
        }
        
        bool IsIn(const LEMath::FloatVector3 &v) { if (minimum < v && maximum > v) return true; return false; }
        INTERSECT_RESULT Intersect(const fRay &r);
        
		LEMath::FloatVector3 GetNormal(const LEMath::FloatVector3 &v);
        LEMath::FloatVector3 GetCenter() { return (maximum + minimum) / 2.0f; }
        float GetLength() { return (maximum - minimum).Length(); }
        
        inline void Merge(const LEMath::FloatVector3 &v)
        {
            maximum.SetX((maximum.X()>v.X())?maximum.X():v.X());
            maximum.SetY((maximum.Y()>v.Y())?maximum.Y():v.Y());
            maximum.SetZ((maximum.Z()>v.Z())?maximum.Z():v.Z());
            minimum.SetX((minimum.X()<v.X())?minimum.X():v.X());
            minimum.SetY((minimum.Y()<v.Y())?minimum.Y():v.Y());
            minimum.SetZ((minimum.Z()<v.Z())?minimum.Z():v.Z());            
        }
        
        AABB Transform(const LEMath::FloatMatrix4x4 &m)
        {
            AABB output;
            output.maximum = (LEMath::FloatVector3)m.Translate(LEMath::FloatVector4(maximum).SetW(1.0f));
            output.minimum = (LEMath::FloatVector3)m.Translate(LEMath::FloatVector4(minimum).SetW(1.0f));
            return output;
        }
        
        LEMath::FloatVector3 maximum;
        LEMath::FloatVector3 minimum;
        
        static INTERSECT_RESULT INTERSECT_FAIL;
    };
}

#endif
