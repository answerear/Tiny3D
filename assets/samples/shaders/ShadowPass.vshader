#pragma vertex vert
#pragma target 4.0


cbuffer Tiny3DPerFrame
{
   row_major float4x4 tiny3d_MatrixLightSpaceVP;
}

#define TINY3D_MATRIX_LIGHTSPACE tiny3d_MatrixLightSpaceVP

struct VertexInput
{
   float3 position : POSITION;
   float3 normal : NORMAL;
   float2 uv : TEXCOORD0;
};
struct VertexOutput
{
   float4 position : SV_POSITION;
};
VertexOutput vert(VertexInput input)
{
   VertexOutput output;
   output.position = mul(TINY3D_MATRIX_LIGHTSPACE, float4(input.position, 1.0f));
   return output;
};