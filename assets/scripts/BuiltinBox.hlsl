//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

cbuffer cbPerObject
{
	float4x4 TINY3D_MATRIX_M;
	float4x4 TINY3D_MATRIX_I_M;
	float4x4 TINY3D_MATRIX_T_M;
	float4x4 TINY3D_MATRIX_IT_M;
	
	float4x4 TINY3D_MATRIX_MV;
	float4x4 TINY3D_MATRIX_I_MV;
	float4x4 TINY3D_MATRIX_T_MV;
	float4x4 TINY3D_MATRIX_IT_MV;
	
	float4x4 TINY3D_MATRIX_MVP;
	float4x4 TINY3D_MATRIX_I_MVP;
	float4x4 TINY3D_MATRIX_T_MVP;
	float4x4 TINY3D_MATRIX_IT_MVP;
};

cbuffer cbPerFrame
{
	float4x4 TINY3D_MATRIX_V;
	float4x4 TINY3D_MATRIX_I_V;
	float4x4 TINY3D_MATRIX_T_V;
	float4x4 TINY3D_MATRIX_IT_V;
	
	float4x4 TINY3D_MATRIX_VP;
	float4x4 TINY3D_MATRIX_I_VP;
	float4x4 TINY3D_MATRIX_T_VP;
	float4x4 TINY3D_MATRIX_IT_VP;
};

cbuffer cbRarely
{
	float4x4 TINY3D_MATRIX_P;
	float4x4 TINY3D_MATRIX_I_P;
	float4x4 TINY3D_MATRIX_T_P;
	float4x4 TINY3D_MATRIX_IT_P;
}

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	//vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.PosH = mul(TINY3D_MATRIX_MVP, float4(vin.PosL, 1.0f));
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color;
}

