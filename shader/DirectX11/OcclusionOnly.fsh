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
@file  OcclusionOnly.fsh
@brief Shader for drawing occlusion (FlagShader)
@author minseob (https://github.com/rasidin)
**********************************************************************/

#include "CommonDefinitions.shh"
#include "LightingUtilities.shh"
#include "BRDF.shh"

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR0;
    float2 Texcoord0 : TEXCOORD0;
    float4 WorldPosition : POSITION1;
    float4 WorldNormal : POSITION2;
};

float4 ps_main_prepass(PS_INPUT In) : SV_TARGET
{
    return float4(In.WorldNormal.xyz, 1);
}

float4 ps_main_basepass(PS_INPUT In) : SV_TARGET
{
    uint3 aoResolution = 0;
    AmbientOcclusionTexture.GetDimensions(0, aoResolution.x, aoResolution.y, aoResolution.z);
    float2 positionInAOUV = float2((In.Position.x + 0.5) / (float)aoResolution.x, (In.Position.y + 0.5) / (float)aoResolution.y);
    float AmbientOcclusion = GetAmbientOcclusionValue(positionInAOUV);
    return float4(0.0, 0.0, 0.0, clamp(1.0 - pow(AmbientOcclusion, 0.8), 0, 1));
}