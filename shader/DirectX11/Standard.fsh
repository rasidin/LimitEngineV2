/***********************************************************
 LIMITEngine Shader File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Standard.fsh
 @brief Shader for drawing rigid mesh (FlagShader)
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

 struct PS_INPUT
 {
	float4 Position			: SV_POSITION;
	float4 Color			: COLOR0;
	float2 Texcoord0		: TEXCOORD0;
 };

 float4 ps_main(PS_INPUT In) : SV_TARGET
 {
	return float4(1, 1, 1, 1);
 }