/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  Sprite.vsh
 @brief Sprite Shader (Vertex)
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
struct VS_INPUT
{
	float4 pos		: POSITION;
	float4 color	: COLOR0;
	float2 uv		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR0;
	float2 uv		: TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT In)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos.x = In.pos.x * 2 - 1;
	output.pos.y = 1 - In.pos.y * 2;
    output.pos.z = In.pos.z;
	output.pos.w = 1;
	output.color = In.color;
    output.uv = In.uv;
	return output;
}