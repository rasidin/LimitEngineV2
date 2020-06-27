/*********************************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file  Standard.fsh
@brief Shader for drawing rigid mesh (FlagShader)
@author minseob (leeminseob@outlook.com)
**********************************************************************/

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
    uint3 aoResolution = 0;
    AmbientOcclusionTexture.GetDimensions(0, aoResolution.x, aoResolution.y, aoResolution.z);
    float2 positionInAOUV = float2((In.Position.x + 0.5) / (float)aoResolution.x, (In.Position.y + 0.5) / (float)aoResolution.y);
    finalColor *= GetAmbientOcclusionValue(positionInAOUV);

    return float4(finalColor, 1);
}

float4 ps_main(PS_INPUT In) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}
