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

// Material parameters
float3 BaseColor = float3(0.18, 0.18, 0.18);
float F0 = 0.02;
float Metallic = 0.0;
float Roughness = 0.1;

float4 ps_main(PS_INPUT In) : SV_TARGET
{
    float3 posToCam = CameraPosition.xyz - In.WorldPosition.xyz;
    float3 N = In.WorldNormal.xyz;

    float3 V = normalize(posToCam);
    float NoV = max(0.0, dot(V,N));

    float F = Fresnel(F0, NoV);
    float3 diffuse = (1 - F) * Irradiance(N) / PI;
    float3 specular = Radiance(In.WorldNormal, camToPosDirection);

    float3 finalColor = diffuse + specular;

    return float4(finalColor, 1);
}