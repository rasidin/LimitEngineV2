/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  Sprite.vsh
 @brief Sprite Shader (Pixel)
 @author minseob (https://github.com/rasidin)
 -----------------------------------------------------------
 History:
 - 2012/6/25 Created by minseob
 ***********************************************************/
struct PS_INPUT
{
	float4 pos	: SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct PS_OUTPUT
{
	float4 color : SV_Target0;
};

SamplerState defaultSampler : register( s0 )
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D IBLSpecularTexture : register( t0 );
float4x4 viewProjInvMatrix;

#define PI 3.14159265358979323846

float3 getIBLfromWorldDirection(Texture2D ibltex, float3 dir)
{
	float2 iblUV = float2(1 + atan2(dir.x,-dir.z) / (PI*2), acos(dir.y+0.0000001) / PI);
	return ibltex.Sample(defaultSampler, iblUV);
}

PS_OUTPUT ps_main(PS_INPUT In)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.color.a = 1.0;
	float2 screenPosition = float2(In.uv.x*2.0-1.0, (1.0-In.uv.y)*2.0-1.0);
	float3 toScreenDirection = normalize(mul(viewProjInvMatrix, float4(screenPosition, 1, 1)).xyz);
	float3 iblColor = getIBLfromWorldDirection(IBLSpecularTexture, toScreenDirection);
	output.color.rgb = iblColor;
	return output;
}