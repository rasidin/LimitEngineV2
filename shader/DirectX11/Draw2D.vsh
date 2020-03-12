/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  Sprite.vsh
 @brief Sprite Shader (Vertex)
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/25 Created by minseob
 ***********************************************************/
struct VS_INPUT
{
	float4 pos		: POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
};

VS_OUTPUT vs_main(VS_INPUT In)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos.x = In.pos.x * 2 - 1;
	output.pos.y = 1 - In.pos.y * 2;
    output.pos.z = 0;
	output.pos.w = 1;
	output.color = In.color;
	output.uv = In.uv;
	return output;
}