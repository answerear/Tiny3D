#pragma vertex vert
#pragma target 4.0

//cbuffer ConstantBuffer : register(b0)
//{
//   float4x4 modelMatrix;
//   float4x4 viewMatrix;
//   float4x4 projectionMatrix;
//}
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
}
//
//float4x4 tiny3d_ObjectToWorld;
//float4x4 tiny3d_WorldToObject;
//float4x4 tiny3d_MatrixV;
//float4x4 tiny3d_MatrixP;
//float4x4 tiny3d_MatrixVP;

static float4x4 tiny3d_MatrixMVP = mul(tiny3d_MatrixVP, tiny3d_ObjectToWorld);
static float4x4 tiny3d_MatrixMV = mul(tiny3d_MatrixV, tiny3d_ObjectToWorld);

#define TINY3D_MATRIX_M    tiny3d_ObjectToWorld
#define TINY3D_MATRIX_V    tiny3d_MatrixV
#define TINY3D_MATRIX_P    tiny3d_MatrixP
#define TINY3D_MATRIX_VP   tiny3d_MatrixVP
#define TINY3D_MATRIX_MV   tiny3d_MatrixMV
#define TINY3D_MATRIX_MVP  tiny3d_MatrixMVP

struct VertexInput
{
   float3 position : POSITION;
   float3 normal : NORMAL;
   float2 uv : TEXCOORD0;
};
struct VertexOutput
{
   float4 position : SV_POSITION;
   float3 worldNormal : COLOR0;
   float2 uv : TEXCOORD0;
};
VertexOutput vert(VertexInput input)
{
   VertexOutput output;
   output.position = mul(TINY3D_MATRIX_MVP, float4(input.position, 1.0f));
   output.worldNormal = normalize(mul((float3x3)TINY3D_MATRIX_M, input.normal));
   output.uv = input.uv;
   return output;
};