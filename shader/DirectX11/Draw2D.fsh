/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  Sprite.vsh
 @brief Sprite Shader (Pixel)
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/25 Created by minseob
 ***********************************************************/
struct PS_INPUT
{
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

PS_OUTPUT ps_main(PS_INPUT In)
{
	PS_OUTPUT output = (PS_OUTPUT)0;
	output.color = tex0.Sample(defaultSampler, In.uv) * In.color;
	return output;
}