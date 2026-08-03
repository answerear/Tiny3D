#ifndef TINY3D_STATIC_MESH_SHADOW_INCLUDED
#define TINY3D_STATIC_MESH_SHADOW_INCLUDED

VertexOutputShadowCaster vertShadowStatic(VertexInput input)
{
    VertexOutputShadowCaster output;
    output.position = mul(TINY3D_MATRIX_LIGHTSPACE, float4(input.position, 1.0f));
    return output;
};

#endif  /*TINY3D_STATIC_MESH_SHADOW_INCLUDED*/