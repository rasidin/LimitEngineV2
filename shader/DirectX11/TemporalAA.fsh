/***********************************************************
 LIMITEngine Shader File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  TemporalAA.fsh
 @brief TemporalAA post process
 @author minseob (https://github.com/rasidin)
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

SamplerState SceneColorSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState SceneColorHistorySampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D SceneColor;
Texture2D SceneColorHistory;

PS_OUTPUT ps_main(PS_INPUT In)
{
    PS_OUTPUT output = (PS_OUTPUT)0;
    float3 historyColor = SceneColorHistory.Sample(SceneColorHistorySampler, In.uv).rgb;
    output.color.rgb = SceneColor.Sample(SceneColorSampler, In.uv).rgb * 0.04 + historyColor * 0.96;
    output.color.a = 1.0;

    return output;
}
