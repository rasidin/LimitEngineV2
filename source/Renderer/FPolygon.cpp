/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LE_Frustum2D.h
 @brief Frustum Class for 2D
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Renderer/FPolygon.h"

#include <float.h>
#include <LEFloatVector3.h>

#include "Renderer/FRay.h"

namespace LimitEngine {
    fPolygon::INTERSECT_RESULT fPolygon::INTERSECT_FAIL = INTERSECT_RESULT(false, 0.0f);
    
    fPolygon::INTERSECT_RESULT
    fPolygon::Intersect(const fRay &r, bool positive, bool negative) {
        float t;
        LEMath::FloatVector3 raydir = r.GetDirection();
        { // Check with normal
            float denom = n | raydir;
            if (denom > +FLT_EPSILON) {
                if (!negative)
                    return INTERSECT_FAIL;
            }
            else if (denom < -FLT_EPSILON) {
                if (!positive)
                    return INTERSECT_FAIL;
            }
            else return INTERSECT_FAIL;
            
            t = (n | (p[0] - r.org)) / denom;
            
            if (t < 0)
                return INTERSECT_FAIL;
        }
        
        int i0, i1;
        {
            float nx = fabsf(n.X());
            float ny = fabsf(n.Y());
            float nz = fabsf(n.Z());
            
            i0 = 1; i1 = 2;
            if (ny > nz && ny > nx) i0 = 0;
            if (nz > nx && nz > nx) i1 = 0;
        }
        
        {
            float u1 = (p[1])[i0] - (p[0])[i0];
            float v1 = (p[1])[i1] - (p[0])[i1];
            float u2 = (p[2])[i0] - (p[0])[i0];
            float v2 = (p[2])[i1] - (p[0])[i1];
            float u0 = t * raydir[i0] + r.org[i0] - (p[0])[i0];
            float v0 = t * raydir[i1] + r.org[i1] - (p[0])[i1];
            
            float alpha = u0 * v2 - u2 * v0;
            float beta = u1 * v0 - u0 * v1;
            float area = u1 * v2 - u2 * v1;
            
            float tolerance = -FLT_EPSILON * area;
            if (area > 0) {
                if (alpha < tolerance || beta < tolerance || alpha + beta > area-tolerance)
                    return INTERSECT_FAIL;
            }
            else {
                if (alpha > tolerance || beta > tolerance || alpha + beta < area - tolerance)
                    return INTERSECT_FAIL;
            }
        }
        return INTERSECT_RESULT(true, t);
    }
}