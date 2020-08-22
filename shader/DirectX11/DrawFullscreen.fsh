/***********************************************************
 LIMITEngine Shader File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Drawfullscreen.fsh
 @brief Draw Fullscreen (used by scenemanager)
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
struct PS_INPUT
{
 	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 color : SV_Target0;
};

SamplerState defaultSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D tex0;

// xScale, yScale, LuminanceScale, Exposure
float4 ColorConvertParameters = float4(1.0, 1.0, 1.0, 0);

static const float3x3 sRGB2XYZ = {
    0.4124564, 0.3575761, 0.1804375,
    0.2126729, 0.7151522, 0.0721750,
    0.0193339, 0.1191920, 0.9503041,
};

static const float3x3 XYZ2sRGB = {
     3.2409699419, -1.5373831776, -0.4986107603,
    -0.9692436363,  1.8759675015,  0.0415550574,
     0.0556300797, -0.2039769589,  1.0569715142,
};

PS_OUTPUT ps_main(PS_INPUT In)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

    float3 color = tex0.Sample(defaultSampler, In.uv).rgb;
    float3 colorXYZ = mul(sRGB2XYZ, color);
    float divider = max(1e-04, colorXYZ.x + colorXYZ.y + colorXYZ.z);
    float2 colorxy = float2(colorXYZ.x / divider, colorXYZ.y / divider);

    divider *= ColorConvertParameters.z;
    colorxy *= ColorConvertParameters.xy;
    colorXYZ = float3(colorxy, 1.0 - colorxy.x - colorxy.y) * divider;

    color = mul(XYZ2sRGB, colorXYZ);
    output.color.rgb = color / pow(2.0, ColorConvertParameters.w);
    output.color.a = 1.0;

	return output;
}