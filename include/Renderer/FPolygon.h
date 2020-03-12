/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  FPolygon.h
 @brief Polygon
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#ifndef _LE_FPOLYGON_H_
#define _LE_FPOLYGON_H_

#include <LEFloatVector3.h>

#include "Containers/Pair.h"
#include "Renderer/fRay.h"

namespace LimitEngine {
    class fPolygon { public:
        typedef Pair<bool, float> INTERSECT_RESULT;
        
        LEMath::FloatVector3 p[3];
        LEMath::FloatVector3 n;
        
        fPolygon() {
            p[0] = LEMath::FloatVector3::Zero;
            p[1] = LEMath::FloatVector3::Zero;
            p[2] = LEMath::FloatVector3::Zero;
        }
        fPolygon(const LEMath::FloatVector3 &p0, const LEMath::FloatVector3 &p1, const LEMath::FloatVector3 &p2) {
            p[0] = p0;
            p[1] = p1;
            p[2] = p2;
            
            MakeNormal();
        }
        fPolygon(const LEMath::FloatVector3 &p0, const LEMath::FloatVector3 &p1, const LEMath::FloatVector3 &p2, const LEMath::FloatVector3 normal)
        {
            p[0] = p0;
            p[1] = p1;
            p[2] = p2;
            n = normal;
        }
        void MakeNormal() {
            LEMath::FloatVector3 p0p1((p[1] - p[0]).Normalize());
            LEMath::FloatVector3 p0p2((p[2] - p[0]).Normalize());
            
            n = (p0p1 ^ p0p2).Normalize();
        }
        INTERSECT_RESULT Intersect(const fRay &r, bool positive, bool negative);
        
        static INTERSECT_RESULT INTERSECT_FAIL;
    };
}

#endif
