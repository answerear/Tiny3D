#ifndef TINY3D_SKINNED_MESH_SHADOW_INCLUDED
#define TINY3D_SKINNED_MESH_SHADOW_INCLUDED

VertexOutputShadowCaster vertShadowSkinned(VertexInput input)
{
    VertexOutputShadowCaster output;

    float4 pos = float4(0,0,0,0);

    [unroll]
    for (int i = 0; i < kMaxBlendBones; i++)
    {
        uint idx = input.indices[i];
        float w = input.weight[i];
        float4x4 m = tiny3d_BoneMatrices[idx];

        pos += mul(m, float4(input.position, 1.0f)) * w;
    }
    
    output.position = mul(TINY3D_MATRIX_LIGHTSPACE_VP, pos);
    
    return output;
};

#endif  /*TINY3D_SKINNED_MESH_SHADOW_INCLUDED*/