#pragma vertex vert
#pragma target 4.0

struct VertexInput
{
   float3 position : POSITION;
};

struct VertexOutput
{
   float4 position : SV_POSITION;
   float2 uv : TEXCOORD0;
};

VertexOutput vert(VertexInput input)
{
   VertexOutput output;
   output.position = float4(input.position.xy, 0.0f, 1.0f);
   // D3D 纹理原点在左上；OpenGL / GLES RTT 原点在左下，编译期用平台宏区分
#if defined(TINY3D_OPENGL) || defined(TINY3D_OPENGLES)
   output.uv = float2(input.position.x * 0.5f + 0.5f, input.position.y * 0.5f + 0.5f);
#else
   output.uv = float2(input.position.x * 0.5f + 0.5f, 0.5f - input.position.y * 0.5f);
#endif
   return output;
};
