//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

#include "BuiltInConstant.inc"

Texture2D gTex : register(t0);
SamplerState gSampler : register(s0);

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
	float2 UV : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float2 UV : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	//vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.PosH = mul(TINY3D_MATRIX_MVP, float4(vin.PosL, 1.0f));
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    vout.UV = vin.UV;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
//    return pin.Color;
	return gTex.Sample(gSampler, pin.UV);
}

