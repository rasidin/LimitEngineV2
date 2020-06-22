/***********************************************************
 LIMITEngine Shader File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Standard.fsh
 @brief Shader for drawing rigid mesh (FlagShader)
 @author minseob (leeminseob@outlook.com)
***********************************************************/

#include "CommonDefinitions.shh"
#include "LightingUtilities.shh"
#include "BRDF.shh"

struct PS_INPUT
{
    float4 Position			: SV_POSITION;
    float4 Normal           : NORMAL;
    float4 Color			: COLOR0;
    float2 Texcoord0		: TEXCOORD0;
    float4 WorldPosition    : POSITION1;
    float4 WorldNormal      : POSITION2;
};

// Global parameters
float3 CameraPosition;

// Temporal context
int4 TemporalContext;

// Material parameters
//float3 BaseColor;
//float Metallic;
//float Roughness;

float4 ps_main_prepass(PS_INPUT In) : SV_TARGET
{
    return float4(In.WorldNormal.xyz, 1);
}

float4 ps_main_basepass(PS_INPUT In) : SV_TARGET
{
    // Test settings
    float3 BaseColor = float3(0.18, 0.18, 0.18);
    float Metallic = 0.02;
    float Roughness = 0.05;

    float F0 = Metallic;
    float3 V = normalize(CameraPosition.xyz - In.WorldPosition.xyz);
    float3 N = normalize(In.WorldNormal.xyz);

    float NoV = max(1.0e-4, dot(V, N));

    float F = Fresnel(F0, NoV);

    float3 diffuse = (1 - F) * Irradiance(N);
    float3 specular = Radiance(TemporalContext, F0, Roughness, N, V);

    float3 finalColor = diffuse + specular;

    return float4(finalColor, 1);
}

float4 ps_main(PS_INPUT In) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}
