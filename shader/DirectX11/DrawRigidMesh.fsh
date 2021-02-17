/***********************************************************
 LIMITEngine Shader File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  DrawRigidMesh.fsh
 @brief Shader for drawing rigid mesh (FlagShader)
 @author minseob (https://github.com/rasidin)
 -----------------------------------------------------------
 History:
 - 2012/11/18 Created by minseob
 ***********************************************************/

struct PS_INPUT
{
    float4 position     : POSITION;
    float4 color        : COLOR;
    float2 texcoord     : TEXCOORD0;
};

sampler2D textureDiffuse;
sampler2D textureNormal;
sampler2D textureSpecular;

float4 ps_main(PS_INPUT In) : COLOR
{
    return float4(1, 0, 0, 1);
}
