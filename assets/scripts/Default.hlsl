// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

cbuffer type_Globals_VP : register(b0, space0)
{
	column_major float4x4 _Globals_ViewProj;
}

cbuffer type_Globals_M : register(b1, space0)
{
	column_major float4x4 _Globals_Model;
}


struct VS_IN
{
	float4 pos : POSITION;
	float4 col : COLOR;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

PS_IN VS( VS_IN input )
{
	PS_IN output = (PS_IN)0;
	
	float4x4 temp = mul(_Globals_ViewProj, _Globals_Model);
	output.pos = mul(input.pos, temp);
	output.col = input.col;
	
	return output;
}

float4 PS( PS_IN input ) : SV_Target
{
	return input.col;
}
