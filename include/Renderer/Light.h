/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Light.h
 @brief Light Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LEFloatVector3.h>

#include "Core/ReferenceCountedObject.h"
#include "Core/String.h"
#include "Core/MetaData.h"
#include "Renderer/RenderState.h"

namespace LimitEngine {
/*
 *!@class Light (Common)
 */
class Light : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>, public MetaData
{
public:
    enum TYPE 
    {
        TYPE_NONE = 0,
        TYPE_DIRECTIONAL,
        TYPE_POINT,
        TYPE_SPOT,
        TYPE_AMBIENT,
        TYPE_IBL,
        TYPE_MAX,
    };

public:
    Light();
    virtual ~Light();

    virtual TYPE GetType() = 0;

    void SetName(const String &name)        { mName = name; }
    const String& GetName() const           { return mName; }

    void SetColor(const LEMath::FloatVector3 &c)        { mColor = c; }
    void SetIntensity(float i)              { mIntensity = i; }

    const LEMath::FloatVector3& GetColor() const        { return mColor; }
    const float& GetIntensity() const       { return mIntensity; }

    virtual void Update() {}
    virtual void DrawBackground(const RenderState &rs) {}
    virtual void Draw(const RenderState &rs) {}

protected:
    virtual void VariableChanged(const MetaDataVariable &mdv) override {}

protected:
    uint8           mType;
    String          mName;
    LEMath::FloatVector3        mColor;

    float           mIntensity;
};

/*
 *!@class Directional Light
 */
class DirectionalLight : public Light
{
public:
    DirectionalLight();
    virtual ~DirectionalLight();

    TYPE GetType()                            { return TYPE_DIRECTIONAL; }

    void SetDirection(const LEMath::FloatVector3 &r)     { mDirection = r; }
    const LEMath::FloatVector3& GetDirection() const     { return mDirection; }

protected:
    LEMath::FloatVector3        mDirection;
};

/*
 *!@class Ambient Light
 */
class AmbientLight : public Light
{
public:
    AmbientLight();
    virtual ~AmbientLight();

    TYPE GetType()                            { return TYPE_AMBIENT; }
};

/*
 *!@class Point Light
 */
class SpotLight : public Light
{
public:
    SpotLight();
    virtual ~SpotLight();

    TYPE GetType()                            { return TYPE_SPOT; }

    void SetPosition(const LEMath::FloatVector3 &p)    { mPosition = p; }
    const LEMath::FloatVector3& GetPosition() const        { return mPosition; }
    void SetDirection(const LEMath::FloatVector3 &d) {mDirection = d; }
    const LEMath::FloatVector3& GetDirection() const { return mDirection; }
private:
    LEMath::FloatVector3    mPosition;
    LEMath::FloatVector3    mDirection;
};

/*
 *!@class Point Light
 */
class PointLight : public Light
{
public:
    PointLight();
    virtual ~PointLight();

    TYPE GetType()                            { return TYPE_POINT; }

    void SetPosition(const LEMath::FloatVector3 &p)     { mPosition = p; }
    void SetScale(const LEMath::FloatVector3 &s)        { mScale = s; }
    const LEMath::FloatVector3& GetPosition() const     { return mPosition; }
    const LEMath::FloatVector3& GetScale() const        { return mScale; }

    void SetRange(const float &r)       { mRange = r; }
    void SetExponent(const float &e)    { mExponent = e; }
    float GetRange() const              { return mRange;; }
    float GetExponent() const           { return mExponent; }

protected:
    LEMath::FloatVector3        mPosition;
    LEMath::FloatVector3        mScale;

    float           mRange;
    float           mExponent;
};
}
