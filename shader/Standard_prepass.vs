/*********************************************************************
Copyright(c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this softwareand associated documentation
files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and /or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file Standard.prepass.vs
@brief Standard prepass vertex shader
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#include "CommonDefinitions.shh"

struct VS_INPUT
{
    float4 Position     : POSITION0;
    float4 Normal       : NORMAL;
    float4 Color        : COLOR0;
    float2 Texcoord0    : TEXCOORD;
//    float4 Tangent      : TANGENT;
//    float4 Binormal     : BINORMAL;
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
