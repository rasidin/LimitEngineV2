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
-----------------------------------------------------------------------
@file  AmbientOcclusion.fsh
@brief Post Process Ambient Occlusion (GTAO [Activision 2016])
@author minseob (leeminseob@outlook.com)
**********************************************************************/

#include "CommonDefinitions.shh"

#define DIRECTION_COUNT 2
#define MAX_STEP_COUNT 5

// (radius, radius * radius, 1 / radius, tanBias)
float4 AOParameters;
// (width, height, 1 / width, 1 / height)
float4 AOResolutionParameters;

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

float2 GetRotation(float r)
{
    float2 output;
    sincos(r * PI * 0.25, output.x, output.y);
    return output;
}

float FindHorizonHeight(float2 Delta, float2 UV, float3 Position, float RandomValue)
{
    float output = PI;
    [unroll]
    for (int i = 0; i < MAX_STEP_COUNT; i++) 
    {
        float3 targetPosition = GetViewPosition(UV + float2(1,-1) * Delta * ((float)i + RandomValue));
        if (targetPosition.z < PerspectiveProjectionParameters.y) {
            float3 PtoH = targetPosition - Position;

            float PtoHLength = max(1e-4, length(PtoH));
            float CosinePtoH = abs(FastAcos(clamp(-PtoH.z / PtoHLength, -1.0, 1.0)));
            output = min(output, CosinePtoH);
        }
    }
    return output;
}

float CalculateVisibilityIntergral(float3 ViewNormal, float2 Rotation, float4 H) 
{
    float tangentCosine = dot(ViewNormal.xy, Rotation.xy);
    float binormalCosine = dot(ViewNormal.yx * float2(1,-1), Rotation.xy);
    float output = 0;
    // Slice0
    {
        float normalLengthInSlice = max(1e-4, length(float3(ViewNormal.xy - float2(-Rotation.y, Rotation.x) * binormalCosine, ViewNormal.z)));
        float cosViewNormal       = clamp(-ViewNormal.z / normalLengthInSlice, -1.0, 1.0);
        float sinViewNormal       = sign(tangentCosine) * sqrt(1.0 - cosViewNormal * cosViewNormal);
        float cosViewNormalAngle  = sign(tangentCosine) * FastAcos(cosViewNormal);

        float2 ntoh = float2(min(H.x - cosViewNormalAngle, PI * 0.5), max(-H.y - cosViewNormalAngle,-PI * 0.5));

        float2 arc = -cos(2.0 * ntoh + cosViewNormalAngle.xx) + cosViewNormal.xx + 2.0 * (ntoh + cosViewNormalAngle.xx) * sinViewNormal;
        arc *= normalLengthInSlice;
        output += arc.x + arc.y;
    }
    // Slice1
    {
        float normalLengthInSlice = max(1e-4, length(float3(ViewNormal.xy - float2( Rotation.x, Rotation.y) * tangentCosine, ViewNormal.z)));
        float cosViewNormal       = clamp(-ViewNormal.z / normalLengthInSlice, -1.0, 1.0);
        float sinViewNormal       = sign(binormalCosine) * sqrt(1.0 - cosViewNormal * cosViewNormal);
        float cosViewNormalAngle  = sign(binormalCosine) * FastAcos(cosViewNormal);

        float2 ntoh = float2(min(H.z - cosViewNormalAngle, PI * 0.5), max(-H.w - cosViewNormalAngle, -PI * 0.5));
        
        float2 arc = -cos(2.0 * ntoh + cosViewNormalAngle.xx) + cosViewNormal.xx + 2.0 * (ntoh + cosViewNormalAngle.xx) * sinViewNormal;
        arc *= normalLengthInSlice;
        output += arc.x + arc.y;
    }
    return output * 0.5 * 0.25;
}

float CalculateGroundTruthAmbientOcclusion(float2 Rotation, float2 UV, float2 StepUV, float3 Position, float3 ViewNormal, float RandomValue)
{
    float2 rotations[4] = { float2( Rotation.x, Rotation.y), float2(-Rotation.x,-Rotation.y), float2(-Rotation.y, Rotation.x), float2( Rotation.y,-Rotation.x) };
    float4 horizonHeight = 0;
    [unroll]
    for (int i = 0; i < 4; i++) 
    {
        float2 uvDelta = rotations[i] * StepUV;
        RandomValue = max(RandomValue, length(AOResolutionParameters.zw) / length(uvDelta));
        horizonHeight[i] = FindHorizonHeight(uvDelta, UV, Position, RandomValue);
    }
    return CalculateVisibilityIntergral(ViewNormal, Rotation, horizonHeight);
}

float2 ps_gtao_main(PS_INPUT In) : SV_TARGET
{
    float3 position = GetViewPosition(In.uv);
    float3 viewNormal = GetViewNormal(In.uv);

    if (position.z >= PerspectiveProjectionParameters.y)
        return 0.0;

    const float radiusInUV = 0.5 * AOParameters.x / (-UVtoViewParameter.z) / position.z;
    const float radiusInPixels = radiusInUV * AOResolutionParameters.x;
    
    // Calculate steps
    float2 stepInUV = AOResolutionParameters.zw * (float)radiusInPixels / MAX_STEP_COUNT;

    // Calculate occlusion
    const float4 blueNoiseValue = GetBlueNoiseValue4(float3(In.pos.xy, (float)FrameIndexContext.x));

    const float DirectionOneStep = PI * 0.5 / DIRECTION_COUNT;
    float ao = 0.0;
    [unroll]
    for (int direction = 0; direction < DIRECTION_COUNT; direction++) 
    {
        float2 rotation = GetRotation((blueNoiseValue.w + direction) * DirectionOneStep);
        ao += CalculateGroundTruthAmbientOcclusion(rotation, In.uv, stepInUV, position, viewNormal, blueNoiseValue.z);
    }

    return float2(ao / float(DIRECTION_COUNT), position.z);
}