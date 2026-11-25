#pragma vertex vert
#pragma target 4.0

cbuffer Tiny3DPerDraw
{
   row_major float4x4 tiny3d_ObjectToWorld;
   row_major float4x4 tiny3d_WorldToObject;
}

cbuffer Tiny3DPerFrame
{
   row_major float4x4 tiny3d_MatrixV;
   row_major float4x4 tiny3d_MatrixP;
   row_major float4x4 tiny3d_MatrixVP;
   row_major float4x4 tiny3d_MatrixLightSpaceVP;
}

struct VertexInput
{
   float3 position : POSITION;
   float3 normal : NORMAL;
   float2 uv : TEXCOORD0;
};

struct VertexOutput
{
   float4 position : SV_POSITION;
   float2 uv : TEXCOORD0;
};

VertexOutput vert(VertexInput input)
{
   VertexOutput output;
   output.position = mul(mul(tiny3d_MatrixVP, tiny3d_ObjectToWorld), float4(input.position, 1.0f));
   output.uv = input.uv;
   return output;
};
