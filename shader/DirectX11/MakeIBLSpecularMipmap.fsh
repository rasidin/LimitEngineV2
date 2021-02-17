/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  MakeIBLSpecularImage.fsh
 @brief Make IBLSpecularImage
 @author minseob (https://github.com/rasidin)
 -----------------------------------------------------------
 History:
 - 2017/1/5 Created by minseob
 ***********************************************************/
struct PS_INPUT
{
	float4 color : COLOR;
	float2 uv : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 color : SV_Target;
};

SamplerState textureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float paramRoughness = 0;
Texture2D IBLSpecularOriginalTexture;

float radicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 Hammersley(uint i, uint N) {
    return float2(float(i)/float(N), radicalInverse_VdC(i));
}

#define PI 3.14159265

float3 ImportanceSampleGGX( float2 Xi, float Roughness, float3 N )
{
	float a = Roughness * Roughness;
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt( (1 - Xi.y) / ( 1 + (a*a - 1) * Xi.y ) );
	float SinTheta = sqrt( 1 - CosTheta * CosTheta );

	float3 H;
	H.x = SinTheta * cos( Phi );
	H.y = SinTheta * sin( Phi );
	H.z = CosTheta;

	float3 UpVector = abs(N.z) < 0.999 ? float3(0,0,1) : float3(1,0,0);
	float3 TangentX = normalize( cross( UpVector, N ) );
	float3 TangentY = cross( N, TangentX );
	
	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}

float3 getIBLColorfromWorldDirection(float3 dir)
{
	float2 iblUV = float2(1 + atan2(dir.x,-dir.z) / (PI*2), acos(dir.y+0.0000001) / PI);
	return IBLSpecularOriginalTexture.Sample(textureSampler, iblUV).rgb;
//	return tex2D(IBLSpecularOriginalTexture, iblUV);
}

float3 PrefilterEnvMap( float Roughness, float3 R )
{
	float3 N = R;
	float3 V = R;
	float3 PrefilteredColor = 0;
	const uint NumSamples = 1024;
	float TotalWeight = 0;
	for( uint i = 0; i < NumSamples; i++ )
	{
		float2 Xi = Hammersley( i, NumSamples );
		float3 H = ImportanceSampleGGX( Xi, Roughness, N );
		float3 L = 2 * dot( V, H ) * H - V;
		float NoL = saturate( dot( N, L ) );
		if( NoL > 0 )
		{
			PrefilteredColor += getIBLColorfromWorldDirection(L) * NoL;
			TotalWeight += NoL;
		}
	}
	return PrefilteredColor / TotalWeight;
//	return float3(1,1,1);
}

float3 GetIBLDirection(float2 uv)
{
	float theta = PI * (uv.x - 1);
	float phi = PI * (uv.y);
	return float3(sin(phi) * sin(theta), cos(phi), -sin(phi)*cos(theta));
}

PS_OUTPUT ps_main(PS_INPUT In)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	float3 iblDirection = GetIBLDirection(In.uv);

	output.color.a = 1.0;
	output.color.rgb = PrefilterEnvMap(0, iblDirection);
	return output;
}