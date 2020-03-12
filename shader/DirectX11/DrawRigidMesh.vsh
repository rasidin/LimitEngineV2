/***********************************************************
 LIMITEngine Shader File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  DrawRigidMesh.vsh
 @brief Shader for drawing rigid mesh (VertexShader)
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/11/18 Created by minseob
 ***********************************************************/

struct VS_INPUT
{
    float4 position     : POSITION;
    float4 normal       : NORMAL;
    float4 color        : COLOR;
    float2 texcoord     : TEXCOORD0;
    float4 tangent      : TEXCOORD1;
    float4 binormal     : TEXCOORD2;
};

struct VS_OUTPUT
{
    float4 position     : POSITION;
    float4 color        : COLOR;
    float2 texcoord     : TEXCOORD0;
};

float4 diffuseColor;
float normalScale;
float specularScale;
float4x4 wvpmatrix;
float4x4 viewinvmatrix;

VS_OUTPUT vs_main(VS_INPUT In)
{
    VS_OUTPUT Out;
    Out.position = mul(wvpmatrix, In.position);
    Out.color = In.color;
    Out.texcoord = In.texcoord;
    return Out;
}
