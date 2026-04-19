#ifndef TINY3D_SKINNED_MESH_FORWARD_INCLUDED
#define TINY3D_SKINNED_MESH_FORWARD_INCLUDED

#include "Tiny3DShaderVariables.cginc"

VertexOutputForwardBase vertForwardBaseSkinned(VertexInput input)
{
    VertexOutputForwardBase output;

    float4 pos = float4(0, 0, 0, 0);
    float3 normal = float3(0, 0, 0);

    [unroll]
    for (int i = 0; i < kMaxBlendBones; i++)
    {
        uint idx = input.indices[i];
        float w = input.weight[i];
        float4x4 m = tiny3d_BoneMatrices[idx];

        pos += mul(m, float4(input.position, 1.0f)) * w;
        normal += mul((float3x3)m, input.normal) * w;
    }

    output.position = mul(TINY3D_MATRIX_MVP, pos);
    output.worldNormal = normalize(mul((float3x3)TINY3D_MATRIX_M, normal));
    output.worldPos = mul(TINY3D_MATRIX_M, pos).xyz;
    output.lightSpacePos = mul(TINY3D_MATRIX_LIGHTSPACE, pos);
    output.uv = input.uv;
    return output;
}

#endif  /*TINY3D_SKINNED_MESH_FORWARD_INCLUDED*/