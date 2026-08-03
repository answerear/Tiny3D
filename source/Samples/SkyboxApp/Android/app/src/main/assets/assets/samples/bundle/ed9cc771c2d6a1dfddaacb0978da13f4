#ifndef TINY3D_STANDARD_SHADOW_INCLUDED
#define TINY3D_STANDARD_SHADOW_INCLUDED

#include "Tiny3D.cginc"
#include "Tiny3DStandardInput.cginc"
#include "Tiny3DShaderVariables.cginc"

struct VertexOutputShadowCaster
{
   float4 position : SV_POSITION;
};

#if defined (T3D_SKINNED_MESH) && defined (T3D_GPU_SKINNING)
    #include "Tiny3DSkinnedMeshShadow.cginc"
    VertexOutputShadowCaster vertShadowCaster(VertexInput v) { return vertShadowSkinned(v); }
#else
    #include "Tiny3DStaticMeshShadow.cginc"
    VertexOutputShadowCaster vertShadowCaster(VertexInput v) { return vertShadowStatic(v); }
#endif

#endif  /*TINY3D_STANDARD_SHADOW_INCLUDED*/