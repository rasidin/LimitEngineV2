/***********************************************************
 LIMITEngine Shader File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Standard.vsh
 @brief Standard shader for models
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "CommonDefinitions.shh"

struct VS_INPUT
{
    float4 Position     : POSITION0;
    float4 Normal       : NORMAL;
    float4 Color        : COLOR0;
    float2 Texcoord0    : texcoord0;
    float4 Tangent      : texcoord1;
    float4 Binormal     : texcoord2;
};

struct VS_OUTPUT
{
    float4 Position      : SV_POSITION;
    float4 Normal        : NORMAL;
    float4 Color         : COLOR0;
    float2 Texcoord0     : TEXCOORD0;
    float4 WorldPosition : POSITION1;
    float4 WorldNormal   : POSITION2;
};

VS_OUTPUT vs_main(VS_INPUT In)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    Out.Position = mul(WorldViewProjMatrix, In.Position);
    Out.Normal = In.Normal;
    Out.Color = In.Color;
    Out.Texcoord0 = In.Texcoord0;
    Out.WorldPosition = mul(WorldMatrix, In.Position);
    Out.WorldNormal.xyz = normalize(mul((float3x3)WorldMatrix, In.Normal.xyz));
    return Out;
}
