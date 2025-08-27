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

static float4x4 tiny3d_MatrixMVP = mul(tiny3d_MatrixVP, tiny3d_ObjectToWorld);
static float4x4 tiny3d_MatrixMV = mul(tiny3d_MatrixV, tiny3d_ObjectToWorld);
static float4x4 tiny3d_MatrixLightSpaceMVP = mul(tiny3d_MatrixLightSpaceVP, tiny3d_ObjectToWorld);

#define TINY3D_MATRIX_M          tiny3d_ObjectToWorld
#define TINY3D_MATRIX_V          tiny3d_MatrixV
#define TINY3D_MATRIX_P          tiny3d_MatrixP
#define TINY3D_MATRIX_VP         tiny3d_MatrixVP
#define TINY3D_MATRIX_MV         tiny3d_MatrixMV
#define TINY3D_MATRIX_MVP        tiny3d_MatrixMVP
#define TINY3D_MATRIX_LIGHTSPACE tiny3d_MatrixLightSpaceMVP

struct VertexInput
{
   float3 position : POSITION;
   float3 normal : NORMAL;
   float2 uv : TEXCOORD0;
   float4 weight : BLENDWEIGHT;
	uint4 indices : BLENDINDICES;
};
struct VertexOutput
{
   float4 position : SV_POSITION;
   float3 worldNormal : NORMAL;
   float3 worldPos : COLOR0;
   float4 lightSpacePos : COLOR1;
   float2 uv : TEXCOORD0;
};
VertexOutput vert(VertexInput input)
{
   VertexOutput output;
   output.position = mul(TINY3D_MATRIX_MVP, float4(input.position, 1.0f));
   output.worldNormal = normalize(mul((float3x3)TINY3D_MATRIX_M, input.normal));
   output.worldPos = mul(TINY3D_MATRIX_M, float4(input.position, 1.0f)).xyz;
   output.lightSpacePos = mul(TINY3D_MATRIX_LIGHTSPACE, float4(input.position, 1.0f));
   output.uv = input.uv;
   return output;
};