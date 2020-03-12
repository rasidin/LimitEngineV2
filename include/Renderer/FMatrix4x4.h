/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_FMatrix4x4.h
 @brief Rect Class
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/10/20 Created by minseob
 ***********************************************************/

#ifndef _LE_FMATRIX4x4_H_
#define _LE_FMATRIX4x4_H_

#include <math.h>

#include "LE_MemoryAllocator.h"
#include "LE_FVector2.h"
#include "LE_FVector3.h"
#include "LE_FVector4.h"

namespace LimitEngine {
    class fMatrix4x4
    {public:
		fMatrix4x4()
		{
			Identity();
		}
        fMatrix4x4(float m00, float m01, float m02, float m03, 
                   float m04, float m05, float m06, float m07, 
                   float m08, float m09, float m10, float m11,
                   float m12, float m13, float m14, float m15) {
            m[ 0] = m00; m[ 1] = m01; m[ 2] = m02; m[ 3] = m03;
            m[ 4] = m04; m[ 5] = m05; m[ 6] = m06; m[ 7] = m07;
            m[ 8] = m08; m[ 9] = m09; m[10] = m10; m[11] = m11;
            m[12] = m12; m[13] = m13; m[14] = m14; m[15] = m15;
        }
        void operator=(const fMatrix4x4 &mat)
        {
            ::memcpy(&m[0], &mat.m[0], 16 * sizeof(float));
        }
        
        fVector2 operator*(const fVector2 &v) { return fVector2(v.x*m[0]+v.y*m[1], v.x*m[4]+v.y*m[5]); }
        fVector3 operator*(const fVector3 &v) const
        { return fVector3(v.x*m[0]+v.y*m[1]+v.z*m[2], v.x*m[4]+v.y*m[5]+v.z*m[6], v.x*m[8]+v.y*m[9]+v.z*m[10]); }
        fVector4 operator*(const fVector4 &v) const
        { return fVector4(v.x*m[ 0]+v.y*m[ 1]+v.z*m[ 2]+v.w*m[ 3],
                          v.x*m[ 4]+v.y*m[ 5]+v.z*m[ 6]+v.w*m[ 7],
                          v.x*m[ 8]+v.y*m[ 9]+v.z*m[10]+v.w*m[11],
                          v.x*m[12]+v.y*m[13]+v.z*m[14]+v.w*m[15]); }
        fMatrix4x4 operator*(const fMatrix4x4 &mat)
        {
			fMatrix4x4 out;
            out.m[ 0] = m[ 0]*mat.m[0] + m[ 1]*mat.m[4] + m[ 2]*mat.m[ 8] + m[ 3]*mat.m[12];
            out.m[ 1] = m[ 0]*mat.m[1] + m[ 1]*mat.m[5] + m[ 2]*mat.m[ 9] + m[ 3]*mat.m[13];
            out.m[ 2] = m[ 0]*mat.m[2] + m[ 1]*mat.m[6] + m[ 2]*mat.m[10] + m[ 3]*mat.m[14];
            out.m[ 3] = m[ 0]*mat.m[3] + m[ 1]*mat.m[7] + m[ 2]*mat.m[11] + m[ 3]*mat.m[15];
            out.m[ 4] = m[ 4]*mat.m[0] + m[ 5]*mat.m[4] + m[ 6]*mat.m[ 8] + m[ 7]*mat.m[12];
            out.m[ 5] = m[ 4]*mat.m[1] + m[ 5]*mat.m[5] + m[ 6]*mat.m[ 9] + m[ 7]*mat.m[13];
            out.m[ 6] = m[ 4]*mat.m[2] + m[ 5]*mat.m[6] + m[ 6]*mat.m[10] + m[ 7]*mat.m[14];
            out.m[ 7] = m[ 4]*mat.m[3] + m[ 5]*mat.m[7] + m[ 6]*mat.m[11] + m[ 7]*mat.m[15];
            out.m[ 8] = m[ 8]*mat.m[0] + m[ 9]*mat.m[4] + m[10]*mat.m[ 8] + m[11]*mat.m[12];
            out.m[ 9] = m[ 8]*mat.m[1] + m[ 9]*mat.m[5] + m[10]*mat.m[ 9] + m[11]*mat.m[13];
            out.m[10] = m[ 8]*mat.m[2] + m[ 9]*mat.m[6] + m[10]*mat.m[10] + m[11]*mat.m[14];
            out.m[11] = m[ 8]*mat.m[3] + m[ 9]*mat.m[7] + m[10]*mat.m[11] + m[11]*mat.m[15];
            out.m[12] = m[12]*mat.m[0] + m[13]*mat.m[4] + m[14]*mat.m[ 8] + m[15]*mat.m[12];
            out.m[13] = m[12]*mat.m[1] + m[13]*mat.m[5] + m[14]*mat.m[ 9] + m[15]*mat.m[13];
            out.m[14] = m[12]*mat.m[2] + m[13]*mat.m[6] + m[14]*mat.m[10] + m[15]*mat.m[14];
            out.m[15] = m[12]*mat.m[3] + m[13]*mat.m[7] + m[14]*mat.m[11] + m[15]*mat.m[15];
            return out;
        }

        void operator*=(const fMatrix4x4 &mat)
        {
            m[ 0] = m[ 0]*mat.m[0] + m[ 1]*mat.m[4] + m[ 2]*mat.m[ 8] + m[ 3]*mat.m[12];
            m[ 1] = m[ 0]*mat.m[1] + m[ 1]*mat.m[5] + m[ 2]*mat.m[ 9] + m[ 3]*mat.m[13];
            m[ 2] = m[ 0]*mat.m[2] + m[ 1]*mat.m[6] + m[ 2]*mat.m[10] + m[ 3]*mat.m[14];
            m[ 3] = m[ 0]*mat.m[3] + m[ 1]*mat.m[7] + m[ 2]*mat.m[11] + m[ 3]*mat.m[15];
            m[ 4] = m[ 4]*mat.m[0] + m[ 5]*mat.m[4] + m[ 6]*mat.m[ 8] + m[ 7]*mat.m[12];
            m[ 5] = m[ 4]*mat.m[1] + m[ 5]*mat.m[5] + m[ 6]*mat.m[ 9] + m[ 7]*mat.m[13];
            m[ 6] = m[ 4]*mat.m[2] + m[ 5]*mat.m[6] + m[ 6]*mat.m[10] + m[ 7]*mat.m[14];
            m[ 7] = m[ 4]*mat.m[3] + m[ 5]*mat.m[7] + m[ 6]*mat.m[11] + m[ 7]*mat.m[15];
            m[ 8] = m[ 8]*mat.m[0] + m[ 9]*mat.m[4] + m[10]*mat.m[ 8] + m[11]*mat.m[12];
            m[ 9] = m[ 8]*mat.m[1] + m[ 9]*mat.m[5] + m[10]*mat.m[ 9] + m[11]*mat.m[13];
            m[10] = m[ 8]*mat.m[2] + m[ 9]*mat.m[6] + m[10]*mat.m[10] + m[11]*mat.m[14];
            m[11] = m[ 8]*mat.m[3] + m[ 9]*mat.m[7] + m[10]*mat.m[11] + m[11]*mat.m[15];
            m[12] = m[12]*mat.m[0] + m[13]*mat.m[4] + m[14]*mat.m[ 8] + m[15]*mat.m[12];
            m[13] = m[12]*mat.m[1] + m[13]*mat.m[5] + m[14]*mat.m[ 9] + m[15]*mat.m[13];
            m[14] = m[12]*mat.m[2] + m[13]*mat.m[6] + m[14]*mat.m[10] + m[15]*mat.m[14];
            m[15] = m[12]*mat.m[3] + m[13]*mat.m[7] + m[14]*mat.m[11] + m[15]*mat.m[15];
        }
        
		float& operator[](const uint32 &num)
		{
			return m[num];
		}

        void Identity() {
            ::memset(m, 0, sizeof(float) * 16);
            m[0] = m[5] = m[10] = m[15] = 1.0f;
        }
        
        fMatrix4x4 Transpose() const {
            return fMatrix4x4(m[ 0], m[ 4], m[ 8], m[12],
                              m[ 1], m[ 5], m[ 9], m[13],
                              m[ 2], m[ 6], m[10], m[14],
                              m[ 3], m[ 7], m[11], m[15]);
        }
        
        fMatrix4x4 Inverse() {
            float m00 = m[ 0], m01 = m[ 1], m02 = m[ 2], m03 = m[ 3];
            float m10 = m[ 4], m11 = m[ 5], m12 = m[ 6], m13 = m[ 7];
            float m20 = m[ 8], m21 = m[ 9], m22 = m[10], m23 = m[11];
            float m30 = m[12], m31 = m[13], m32 = m[14], m33 = m[15];
            
            float v0 = m20 * m31 - m21 * m30;
            float v1 = m20 * m32 - m22 * m30;
            float v2 = m20 * m33 - m23 * m30;
            float v3 = m21 * m32 - m22 * m31;
            float v4 = m21 * m33 - m23 * m31;
            float v5 = m22 * m33 - m23 * m32;
            
            float t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
            float t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
            float t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
            float t30 = - (v3 * m10 - v1 * m11 + v0 * m12);
            
            float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
            
            float d00 = t00 * invDet;
            float d10 = t10 * invDet;
            float d20 = t20 * invDet;
            float d30 = t30 * invDet;
            
            float d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
            float d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
            float d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
            float d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;
            
            v0 = m10 * m31 - m11 * m30;
            v1 = m10 * m32 - m12 * m30;
            v2 = m10 * m33 - m13 * m30;
            v3 = m11 * m32 - m12 * m31;
            v4 = m11 * m33 - m13 * m31;
            v5 = m12 * m33 - m13 * m32;
            
            float d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
            float d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
            float d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
            float d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;
            
            v0 = m21 * m10 - m20 * m11;
            v1 = m22 * m10 - m20 * m12;
            v2 = m23 * m10 - m20 * m13;
            v3 = m22 * m11 - m21 * m12;
            v4 = m23 * m11 - m21 * m13;
            v5 = m23 * m12 - m22 * m13;
            
            float d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
            float d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
            float d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
            float d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;
            
            return fMatrix4x4(d00, d01, d02, d03,
                              d10, d11, d12, d13,
                              d20, d21, d22, d23,
                              d30, d31, d32, d33);
        }
        
        fMatrix4x4 InverseAffine() {
            float m10 = m[4], m11 = m[5], m12 = m[6];
            float m20 = m[8], m21 = m[9], m22 = m[10];
            
            float t00 = m22 * m11 - m21 * m12;
            float t10 = m20 * m12 - m22 * m10;
            float t20 = m21 * m10 - m20 * m11;
            
            float m00 = m[0], m01 = m[1], m02 = m[2];
            
            float invDet = 1 / (m00 * t00 + m01 * t10 + m02 * t20);
            
            t00 *= invDet; t10 *= invDet; t20 *= invDet;
            
            m00 *= invDet; m01 *= invDet; m02 *= invDet;
            
            float r00 = t00;
            float r01 = m02 * m21 - m01 * m22;
            float r02 = m01 * m12 - m02 * m11;
            
            float r10 = t10;
            float r11 = m00 * m22 - m02 * m20;
            float r12 = m02 * m10 - m00 * m12;
            
            float r20 = t20;
            float r21 = m01 * m20 - m00 * m21;
            float r22 = m00 * m11 - m01 * m10;
            
            float m03 = m[3], m13 = m[7], m23 = m[11];
            
            float r03 = - (r00 * m03 + r01 * m13 + r02 * m23);
            float r13 = - (r10 * m03 + r11 * m13 + r12 * m23);
            float r23 = - (r20 * m03 + r21 * m13 + r22 * m23);
            
            return fMatrix4x4(r00, r01, r02, r03,
                              r10, r11, r12, r13,
                              r20, r21, r22, r23,
                              0,   0,   0,   1);
        }
        static fMatrix4x4 GetIdentity() {
			fMatrix4x4 out;
            out.Identity();
            return out;
        }
        static fMatrix4x4 Translate(const fVector3 &p) {
            fMatrix4x4 out;
            out.m[ 3] = p.x;
            out.m[ 7] = p.y;
            out.m[11] = p.z;
            return out;
        }
        static fMatrix4x4 RotationX(float x) {
            fMatrix4x4 out;
            out.m[ 5] = cosf(x);
            out.m[ 6] =-sinf(x);
            out.m[ 9] = sinf(x);
            out.m[10] = cosf(x);
            return out;
        }
        static fMatrix4x4 RotationY(float y) {
            fMatrix4x4 out;
            out.m[ 0] = cosf(y);
            out.m[ 2] = sinf(y);
            out.m[ 8] =-sinf(y);
            out.m[10] = cosf(y);
            return out;
        }
        static fMatrix4x4 RotationZ(float z) {
            fMatrix4x4 out;
            out.m[ 0] = cosf(z);
            out.m[ 1] =-sinf(z);
            out.m[ 4] = sinf(z);
            out.m[ 5] = cosf(z);
            return out;
        }
        static fMatrix4x4 RotationZXY(const fVector3 &rot) {
            return RotationZ(rot.z) * RotationX(rot.x) * RotationY(rot.y);
        }
        static fMatrix4x4 RotationZYX(const fVector3 &rot) {
            return RotationZ(rot.z) * RotationY(rot.y) * RotationX(rot.x);
        }
        static fMatrix4x4 Scale(const fVector3 &scl) {
            fMatrix4x4 out;
            out.m[ 0] = scl.x;
            out.m[ 5] = scl.y;
            out.m[10] = scl.z;
            return out;
        }
        static fMatrix4x4 MakeTransform(const fVector3 &pos, const fVector3 &rot, const fVector3 &scl) {
            return Scale(scl) * RotationZXY(rot) * Translate(pos).Transpose();
        }
        
        float m[16];
    };
}

#endif
