#ifndef TINY3D_STANDARD_INPUT_INCLUDED
#define TINY3D_STANDARD_INPUT_INCLUDED

#include "Tiny3D.cginc"

//------------------------------------------------------------------------------

half4 _Color;
half _Cutoff;

TEXTURE2D(_MainTex);

float4 _MainTex_ST;

//------------------------------------------------------------------------------
// Input structure for vertex shaders

struct VertexInput
{
   float3 position : POSITION;
   float3 normal : NORMAL;
   float2 uv : TEXCOORD0;
   float4 weight : BLENDWEIGHT;
   uint4 indices: BLENDINDICES;
};

#endif  /*TINY3D_STANDARD_INPUT_INCLUDED*/