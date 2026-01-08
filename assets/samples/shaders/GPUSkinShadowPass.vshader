#pragma vertex vert
#pragma target 4.0

#define kMaxBones 256
#define kMaxBlendBones 4

cbuffer Tiny3DPerDraw
{
   row_major float4x4 tiny3d_ObjectToWorld;
   row_major float4x4 tiny3d_WorldToObject;
}

cbuffer Tiny3DPerFrame
{
   row_major float4x4 tiny3d_MatrixLightSpaceVP;
   row_major float4x4 tiny3d_BoneMatrices[kMaxBones];
}

static float4x4 tiny3d_MatrixLightSpaceMVP = mul(tiny3d_MatrixLightSpaceVP, tiny3d_ObjectToWorld);

#define TINY3D_MATRIX_LIGHTSPACE tiny3d_MatrixLightSpaceMVP
#define TINY3D_MATRIX_LIGHTSPACE_VP tiny3d_MatrixLightSpaceVP

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
};

VertexOutput vert(VertexInput input)
{
   VertexOutput output;

   float4 pos = float4(0,0,0,0);

   [unroll]
   for (int i = 0; i < kMaxBlendBones; i++)
   {
      uint idx = input.indices[i];
      float w = input.weight[i];
      float4x4 m = tiny3d_BoneMatrices[idx];

      pos += mul(m, float4(input.position, 1.0f)) * w;
   }
   
   output.position = mul(TINY3D_MATRIX_LIGHTSPACE, pos);
   
   return output;
};