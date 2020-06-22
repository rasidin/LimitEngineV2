/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Transform.h
 @brief Transform Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LEFloatVector4.h>
#include <LEFloatMatrix4x4.h>

namespace LimitEngine {
    struct Transform
    {
        LEMath::FloatVector4 Position = LEMath::FloatVector4::Zero;
        LEMath::FloatVector4 Rotation = LEMath::FloatVector4::Zero;
        LEMath::FloatVector4 Scaling  = LEMath::FloatVector4::One;
        
        Transform() {}
        Transform(const LEMath::FloatVector4 &InPosition, const LEMath::FloatVector4 &InRotation, const LEMath::FloatVector4 &InScaling)
            : Position(InPosition), Rotation(InRotation), Scaling(InScaling) {}

        LEMath::FloatMatrix4x4 ToMatrix4x4() const {
            LEMath::FloatMatrix4x4 transMatrix = LEMath::FloatMatrix4x4::GenerateTransform(Position);
            LEMath::FloatMatrix4x4 rotXMatrix = LEMath::FloatMatrix4x4::GenerateRotationX(Rotation.X());
            LEMath::FloatMatrix4x4 rotYMatrix = LEMath::FloatMatrix4x4::GenerateRotationY(Rotation.Y());
            LEMath::FloatMatrix4x4 rotZMatrix = LEMath::FloatMatrix4x4::GenerateRotationZ(Rotation.Z());
            LEMath::FloatMatrix4x4 sclMatrix = LEMath::FloatMatrix4x4::GenerateScaling(Scaling);
            return sclMatrix * rotZMatrix * rotXMatrix * rotYMatrix * transMatrix;
        }
    };
}