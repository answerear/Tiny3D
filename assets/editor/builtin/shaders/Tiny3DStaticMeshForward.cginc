#ifndef TINY3D_STATIC_MESH_FORWARD_INCLUDED
#define TINY3D_STATIC_MESH_FORWARD_INCLUDED

#include "Tiny3DShaderVariables.cginc"

VertexOutputForwardBase vertForwardBaseStatic(VertexInput input)
{
    VertexOutputForwardBase output;
    output.position = mul(TINY3D_MATRIX_MVP, float4(input.position, 1.0f));
    output.worldNormal = normalize(mul((float3x3)TINY3D_MATRIX_M, input.normal));
    output.worldPos = mul(TINY3D_MATRIX_M, float4(input.position, 1.0f)).xyz;
    output.lightSpacePos = mul(TINY3D_MATRIX_LIGHTSPACE, float4(input.position, 1.0f));
    output.uv = input.uv;
    return output;
}

#endif  /*TINY3D_STATIC_MESH_FORWARD_INCLUDED*/