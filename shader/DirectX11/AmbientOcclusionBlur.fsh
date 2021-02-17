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
@file  BilateralBlur.fsh
@brief Bilateral blur
@author minseob (https://github.com/rasidin)
**********************************************************************/

#define KERNEL_RADIUS 5

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

Texture2D AmbientOcclusionTexture;
SamplerState AmbientOcclusionSampler;

// XY : UVPerPixel Z : RadiusFalloff W : ZThreshold
float4 BlurParameters;

float GetBilateralWeight(float Radius, float Zdiff)
{
    return exp(-Radius * Radius * BlurParameters.z) * (Zdiff < BlurParameters.w ? 1.0 : 0.0);
}

float2 ps_bilateral_blur_main(PS_INPUT In) : SV_TARGET
{
    float2 AOValue = AmbientOcclusionTexture.Sample(AmbientOcclusionSampler, In.uv).xy;

    float totalAO = AOValue.x;
    float totalWeight = 1.0;
    for (int i = -KERNEL_RADIUS; i < KERNEL_RADIUS + 1; i++) {
        if (i == 0) continue;

        float2 targetUV = In.uv + ((float)i * BlurParameters.xy);
        if (targetUV.x < 0 || targetUV.x > 1 || targetUV.y < 0 || targetUV.y > 1) continue;
        float2 targetAOValue = AmbientOcclusionTexture.Sample(AmbientOcclusionSampler, targetUV).xy;
        float zdiff = abs(targetAOValue.y - AOValue.y);
        float weight = GetBilateralWeight((float)i, zdiff);
        totalAO += targetAOValue.x * weight;
        totalWeight += weight;
    }
    return float2(totalAO / totalWeight, AOValue.y);
}