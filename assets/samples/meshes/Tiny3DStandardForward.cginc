#ifndef TINY3D_STANDARD_CORE_FORWARD_INCLUDED
#define TINY3D_STANDARD_CORE_FORWARD_INCLUDED

#include "Tiny3D.cginc"
#include "Tiny3DStandardInput.cginc"
#include "Tiny3DStandardForwardOutput.cginc"

#if defined (T3D_SKINNED_MESH) && defined (T3D_GPU_SKINNING)
    #include "Tiny3DSkinnedMeshForward.cginc"
    VertexOutputForwardBase vertForward(VertexInput v) { return vertForwardBaseSkinned(v); }
#else
    #include "Tiny3DStaticMeshForward.cginc"
    VertexOutputForwardBase vertForward(VertexInput v) { return vertForwardBaseStatic(v); }
#endif

#include "Tiny3DStandardForwardFragment.cginc"
float4 fragForward(VertexOutputForwardBase i) : SV_Target { return fragForwardBase(i); }

#endif  /*TINY3D_STANDARD_CORE_FORWARD_INCLUDED*/