/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Light.cpp
 @brief Light Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/

#include "Renderer/Light.h"

namespace LimitEngine {
// ==========================================================
// Light (Common)
// ==========================================================
Light::Light()
    : mType(TYPE_NONE)
    , mName()
    , mIntensity(1.0f)
{
    AddMetaDataVariable("Name",     "String",   METADATA_POINTER(mName));
    AddMetaDataVariable("Intesity", "float",    METADATA_POINTER(mIntensity));
    AddMetaDataVariable("Color",    "fVector3", METADATA_POINTER(mColor));
}

Light::~Light()
{
}

// ==========================================================
// DirectionalLight
// ==========================================================
DirectionalLight::DirectionalLight()
    : Light()
    , mDirection(0.0f,-1.0f, 0.0f)
{
    mType = TYPE_DIRECTIONAL;

    AddMetaDataVariable("Direction", "fVector3", METADATA_POINTER(mDirection));
}

DirectionalLight::~DirectionalLight()
{
}

// ==========================================================
// AmbientLight
// ==========================================================
AmbientLight::AmbientLight()
    : Light()
{
    mType = TYPE_AMBIENT;
}

AmbientLight::~AmbientLight()
{
}

// ==========================================================
// SpotLight
// ==========================================================
SpotLight::SpotLight()
    : Light()
{
    AddMetaDataVariable("Position",  "fVector3", METADATA_POINTER(mPosition));
    AddMetaDataVariable("Direction", "fVector3", METADATA_POINTER(mDirection));
}

SpotLight::~SpotLight()
{}

// ==========================================================
// PointLight
// ==========================================================
PointLight::PointLight()
    : Light()
	, mPosition(0.0f, 0.0f, 0.0f)
	, mScale(1.0f)
    , mRange(10.0f)
    , mExponent(2.0f)
{
    mType = TYPE_POINT;

    AddMetaDataVariable("Position", "fVector3", METADATA_POINTER(mPosition));
    AddMetaDataVariable("Scale",    "fVector3", METADATA_POINTER(mScale));
    AddMetaDataVariable("Range",    "float",    METADATA_POINTER(mRange));
    AddMetaDataVariable("Exponent", "float",    METADATA_POINTER(mExponent));
}

PointLight::~PointLight()
{
}
}