/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include "ComputeShaderSources.h"


namespace
{
    // HLSL 的 cbuffer 必须写成 SPIRV-Cross 跨编译产物的形式：块名 type_<Block>、
    // 成员名 <Block>_<member>。D3D11 反射的 getOriginalName 按这个约定硬剥前缀，
    // 名字不合规时 substr 越界抛异常（T3DD3D11ContextBase.cpp:150-177）。
    // GL / GLES 侧的反射会容错处理前缀，故 GLSL 保持自然命名。
    const unsigned char kDummyVK[] = { 0x03, 0x02, 0x23, 0x07 };

    ComputeShaderSource make(const char *hlsl, const char *gl, const char *gles)
    {
        ComputeShaderSource src;
        src.hlsl = hlsl;
        src.gl = gl;
        src.gles = gles;
        src.vk = kDummyVK;
        src.vkSize = sizeof(kDummyVK);
        return src;
    }
}


ComputeShaderSource linearWriteSource()
{
    static const char *hlsl = R"(
RWStructuredBuffer<uint> gOutput : register(u0);
[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    gOutput[id.x] = id.x * 2;
}
)";
    static const char *gl = R"(
#version 430
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer gOutput { uint data[]; };
void main()
{
    gOutput.data[gl_GlobalInvocationID.x] = gl_GlobalInvocationID.x * 2u;
}
)";
    static const char *gles = R"(
#version 310 es
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer gOutput { uint data[]; };
void main()
{
    gOutput.data[gl_GlobalInvocationID.x] = gl_GlobalInvocationID.x * 2u;
}
)";
    return make(hlsl, gl, gles);
}

ComputeShaderSource saxpySource()
{
    static const char *hlsl = R"(
cbuffer type_SaxpyParams : register(b0)
{
    float4 SaxpyParams_gScale;
    float4 SaxpyParams_gBias;
};
StructuredBuffer<float4> gInput : register(t0);
RWStructuredBuffer<float4> gOutput : register(u0);
[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    gOutput[id.x] = gInput[id.x] * SaxpyParams_gScale + SaxpyParams_gBias;
}
)";
    static const char *gl = R"(
#version 430
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout(std140, binding = 0) uniform SaxpyParams { vec4 gScale; vec4 gBias; };
layout(std430, binding = 0) readonly buffer gInput { vec4 data[]; } gInputBuf;
layout(std430, binding = 1) buffer gOutput { vec4 data[]; } gOutputBuf;
void main()
{
    uint i = gl_GlobalInvocationID.x;
    gOutputBuf.data[i] = gInputBuf.data[i] * gScale + gBias;
}
)";
    static const char *gles = R"(
#version 310 es
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout(std140, binding = 0) uniform SaxpyParams { vec4 gScale; vec4 gBias; };
layout(std430, binding = 0) readonly buffer gInput { vec4 data[]; } gInputBuf;
layout(std430, binding = 1) buffer gOutput { vec4 data[]; } gOutputBuf;
void main()
{
    uint i = gl_GlobalInvocationID.x;
    gOutputBuf.data[i] = gInputBuf.data[i] * gScale + gBias;
}
)";
    return make(hlsl, gl, gles);
}

ComputeShaderSource rawTypedSource()
{
    static const char *hlsl = R"(
RWByteAddressBuffer gRaw : register(u0);
RWBuffer<float4> gTyped : register(u1);
[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint i = id.x;
    gRaw.Store(i * 4, i ^ 0xA5A5A5A5u);
    gTyped[i] = float4((i & 255u) / 255.0f, 0.0f, 128.0f / 255.0f, 1.0f);
}
)";
    static const char *gl = R"(
#version 430
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer gRaw { uint data[]; } gRawBuf;
layout(std430, binding = 1) buffer gTyped { vec4 data[]; } gTypedBuf;
void main()
{
    uint i = gl_GlobalInvocationID.x;
    gRawBuf.data[i] = i ^ 0xA5A5A5A5u;
    gTypedBuf.data[i] = vec4(float(i & 255u) / 255.0, 0.0, 128.0 / 255.0, 1.0);
}
)";
    static const char *gles = R"(
#version 310 es
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer gRaw { uint data[]; } gRawBuf;
layout(std430, binding = 1) buffer gTyped { vec4 data[]; } gTypedBuf;
void main()
{
    uint i = gl_GlobalInvocationID.x;
    gRawBuf.data[i] = i ^ 0xA5A5A5A5u;
    gTypedBuf.data[i] = vec4(float(i & 255u) / 255.0, 0.0, 128.0 / 255.0, 1.0);
}
)";
    return make(hlsl, gl, gles);
}

ComputeShaderSource reducePass0Source()
{
    static const char *hlsl = R"(
StructuredBuffer<uint> gInput : register(t0);
RWStructuredBuffer<uint> gPartial : register(u0);
groupshared uint gs[256];
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID, uint3 gid : SV_GroupID, uint gi : SV_GroupIndex)
{
    gs[gi] = gInput[id.x];
    GroupMemoryBarrierWithGroupSync();
    [unroll]
    for (uint stride = 128; stride > 0; stride >>= 1)
    {
        if (gi < stride) gs[gi] += gs[gi + stride];
        GroupMemoryBarrierWithGroupSync();
    }
    if (gi == 0) gPartial[gid.x] = gs[0];
}
)";
    static const char *gl = R"(
#version 430
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) readonly buffer gInput { uint data[]; } gInputBuf;
layout(std430, binding = 1) buffer gPartial { uint data[]; } gPartialBuf;
shared uint gs[256];
void main()
{
    uint gi = gl_LocalInvocationIndex;
    gs[gi] = gInputBuf.data[gl_GlobalInvocationID.x];
    barrier();
    memoryBarrierShared();
    for (uint stride = 128u; stride > 0u; stride >>= 1u)
    {
        if (gi < stride) gs[gi] += gs[gi + stride];
        barrier();
        memoryBarrierShared();
    }
    if (gi == 0u) gPartialBuf.data[gl_WorkGroupID.x] = gs[0];
}
)";
    static const char *gles = R"(
#version 310 es
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) readonly buffer gInput { uint data[]; } gInputBuf;
layout(std430, binding = 1) buffer gPartial { uint data[]; } gPartialBuf;
shared uint gs[256];
void main()
{
    uint gi = gl_LocalInvocationIndex;
    gs[gi] = gInputBuf.data[gl_GlobalInvocationID.x];
    barrier();
    memoryBarrierShared();
    for (uint stride = 128u; stride > 0u; stride >>= 1u)
    {
        if (gi < stride) gs[gi] += gs[gi + stride];
        barrier();
        memoryBarrierShared();
    }
    if (gi == 0u) gPartialBuf.data[gl_WorkGroupID.x] = gs[0];
}
)";
    return make(hlsl, gl, gles);
}

ComputeShaderSource reducePass1Source()
{
    static const char *hlsl = R"(
RWStructuredBuffer<uint> gPartial : register(u0);
RWStructuredBuffer<uint> gResult : register(u1);
groupshared uint gs[256];
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID, uint gi : SV_GroupIndex)
{
    gs[gi] = gPartial[id.x];
    GroupMemoryBarrierWithGroupSync();
    [unroll]
    for (uint stride = 128; stride > 0; stride >>= 1)
    {
        if (gi < stride) gs[gi] += gs[gi + stride];
        GroupMemoryBarrierWithGroupSync();
    }
    if (gi == 0) gResult[0] = gs[0];
}
)";
    static const char *gl = R"(
#version 430
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer gPartial { uint data[]; } gPartialBuf;
layout(std430, binding = 1) buffer gResult { uint data[]; } gResultBuf;
shared uint gs[256];
void main()
{
    uint gi = gl_LocalInvocationIndex;
    gs[gi] = gPartialBuf.data[gl_GlobalInvocationID.x];
    barrier();
    memoryBarrierShared();
    for (uint stride = 128u; stride > 0u; stride >>= 1u)
    {
        if (gi < stride) gs[gi] += gs[gi + stride];
        barrier();
        memoryBarrierShared();
    }
    if (gi == 0u) gResultBuf.data[0] = gs[0];
}
)";
    static const char *gles = R"(
#version 310 es
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer gPartial { uint data[]; } gPartialBuf;
layout(std430, binding = 1) buffer gResult { uint data[]; } gResultBuf;
shared uint gs[256];
void main()
{
    uint gi = gl_LocalInvocationIndex;
    gs[gi] = gPartialBuf.data[gl_GlobalInvocationID.x];
    barrier();
    memoryBarrierShared();
    for (uint stride = 128u; stride > 0u; stride >>= 1u)
    {
        if (gi < stride) gs[gi] += gs[gi + stride];
        barrier();
        memoryBarrierShared();
    }
    if (gi == 0u) gResultBuf.data[0] = gs[0];
}
)";
    return make(hlsl, gl, gles);
}

ComputeShaderSource textureWriteSource()
{
    static const char *hlsl = R"(
RWTexture2D<float4> gOutput : register(u0);
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    gOutput[id.xy] = float4(id.x / 255.0f, id.y / 255.0f, 128.0f / 255.0f, 1.0f);
}
)";
    return make(hlsl, nullptr, nullptr);
}

ComputeShaderSource reflectProbeSource()
{
    static const char *hlsl = R"(
StructuredBuffer<float4> gStructuredSRV : register(t0);
ByteAddressBuffer gByteSRV : register(t1);
RWStructuredBuffer<uint> gStructuredUAV : register(u0);
RWByteAddressBuffer gByteUAV : register(u1);
RWTexture2D<float4> gTextureUAV : register(u2);
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    // 本用例只查反射结果，不查计算结果，但 fxc 会把「多线程无条件写同一元素」
    // 判成 X3694 race condition 直接编译失败，所以把标量写收敛到 0 号线程。
    if (id.x == 0 && id.y == 0)
    {
        uint v = gStructuredUAV[0] + gByteSRV.Load(0) + (uint)gStructuredSRV[0].x;
        gStructuredUAV[0] = v;
        gByteUAV.Store(0, v);
    }
    gTextureUAV[id.xy] = float4(0, 0, 0, 1);
}
)";
    return make(hlsl, nullptr, nullptr);
}

ComputeShaderSource makeArgsSource()
{
    static const char *hlsl = R"(
RWByteAddressBuffer gArgs : register(u0);
[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    gArgs.Store(0, 4);
    gArgs.Store(4, 1);
    gArgs.Store(8, 1);
}
)";
    static const char *gl = R"(
#version 430
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer gArgs { uint data[]; };
void main()
{
    gArgs.data[0] = 4u;
    gArgs.data[1] = 1u;
    gArgs.data[2] = 1u;
}
)";
    return make(hlsl, gl, nullptr);
}

ComputeShaderSource filterCountSource()
{
    static const char *hlsl = R"(
RWStructuredBuffer<uint> gVisible : register(u0);
[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= 1024) return;
    if ((id.x & 1u) == 0u)
    {
        uint idx = gVisible.IncrementCounter();
        gVisible[idx] = id.x;
    }
}
)";
    static const char *gl = R"(
#version 430
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout(binding = 0, offset = 0) uniform atomic_uint gCounter;
layout(std430, binding = 0) buffer gVisible { uint data[]; };
void main()
{
    uint i = gl_GlobalInvocationID.x;
    if (i >= 1024u) return;
    if ((i & 1u) == 0u)
    {
        uint idx = atomicCounterIncrement(gCounter);
        gVisible.data[idx] = i;
    }
}
)";
    return make(hlsl, gl, nullptr);
}

ComputeShaderSource particleUpdateSource()
{
    static const char *hlsl = R"(
struct Particle { float3 position; float life; float3 velocity; float size; };
cbuffer type_ParticleParams : register(b0)
{
    float4 ParticleParams_gTimeParams;
    float4 ParticleParams_gAttractor;
};
RWStructuredBuffer<Particle> gParticles : register(u0);
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    Particle p = gParticles[id.x];
    float dt = ParticleParams_gTimeParams.x;
    float3 dir = ParticleParams_gAttractor.xyz - p.position;
    float len2 = dot(dir, dir) + 1e-4f;
    p.velocity += dir * (rsqrt(len2) * ParticleParams_gAttractor.w * dt);
    p.velocity *= 0.98f;
    p.position += p.velocity * dt;
    p.life -= dt;
    if (p.life < 0.0f)
    {
        p.life = 4.0f + frac(ParticleParams_gTimeParams.y + id.x * 0.017f) * 4.0f;
        p.position = float3(frac(id.x * 0.0123f) * 8.0f - 4.0f,
                            frac(id.x * 0.0456f) * 8.0f - 4.0f,
                            frac(id.x * 0.0789f) * 8.0f - 4.0f);
        p.velocity = float3(0, 0, 0);
    }
    gParticles[id.x] = p;
}
)";
    static const char *gl = R"(
#version 430
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
struct Particle { vec3 position; float life; vec3 velocity; float size; };
layout(std140, binding = 0) uniform ParticleParams { vec4 gTimeParams; vec4 gAttractor; };
layout(std430, binding = 0) buffer gParticles { Particle data[]; };
void main()
{
    uint i = gl_GlobalInvocationID.x;
    Particle p = gParticles.data[i];
    float dt = gTimeParams.x;
    vec3 dir = gAttractor.xyz - p.position;
    float len2 = dot(dir, dir) + 1e-4;
    p.velocity += dir * (inversesqrt(len2) * gAttractor.w * dt);
    p.velocity *= 0.98;
    p.position += p.velocity * dt;
    p.life -= dt;
    if (p.life < 0.0)
    {
        p.life = 4.0 + fract(gTimeParams.y + float(i) * 0.017) * 4.0;
        p.position = vec3(fract(float(i) * 0.0123) * 8.0 - 4.0,
                          fract(float(i) * 0.0456) * 8.0 - 4.0,
                          fract(float(i) * 0.0789) * 8.0 - 4.0);
        p.velocity = vec3(0.0);
    }
    gParticles.data[i] = p;
}
)";
    static const char *gles = R"(
#version 310 es
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
struct Particle { vec3 position; float life; vec3 velocity; float size; };
layout(std140, binding = 0) uniform ParticleParams { vec4 gTimeParams; vec4 gAttractor; };
layout(std430, binding = 0) buffer gParticles { Particle data[]; };
void main()
{
    uint i = gl_GlobalInvocationID.x;
    Particle p = gParticles.data[i];
    float dt = gTimeParams.x;
    vec3 dir = gAttractor.xyz - p.position;
    float len2 = dot(dir, dir) + 1e-4;
    p.velocity += dir * (inversesqrt(len2) * gAttractor.w * dt);
    p.velocity *= 0.98;
    p.position += p.velocity * dt;
    p.life -= dt;
    if (p.life < 0.0)
    {
        p.life = 4.0 + fract(gTimeParams.y + float(i) * 0.017) * 4.0;
        p.position = vec3(fract(float(i) * 0.0123) * 8.0 - 4.0,
                          fract(float(i) * 0.0456) * 8.0 - 4.0,
                          fract(float(i) * 0.0789) * 8.0 - 4.0);
        p.velocity = vec3(0.0);
    }
    gParticles.data[i] = p;
}
)";
    return make(hlsl, gl, gles);
}

ComputeShaderSource particleCullSource()
{
    static const char *hlsl = R"(
struct Particle { float3 position; float life; float3 velocity; float size; };
cbuffer type_CullParams : register(b0)
{
    row_major float4x4 CullParams_gViewProj;
    float4 CullParams_gClipScale;
};
StructuredBuffer<Particle> gParticles : register(t0);
RWStructuredBuffer<uint> gVisible : register(u0);
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    Particle p = gParticles[id.x];
    float4 clip = mul(CullParams_gViewProj, float4(p.position, 1.0f));
    float w = clip.w;
    float s = CullParams_gClipScale.x * w;
    bool visible = (abs(clip.x) <= s) && (abs(clip.y) <= s) && (clip.z >= 0.0f) && (clip.z <= w);
    if (visible)
    {
        uint idx = gVisible.IncrementCounter();
        gVisible[idx] = id.x;
    }
}
)";
    static const char *gl = R"(
#version 430
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
struct Particle { vec3 position; float life; vec3 velocity; float size; };
layout(std140, binding = 0) uniform CullParams { mat4 gViewProj; vec4 gClipScale; };
layout(std430, binding = 0) readonly buffer gParticles { Particle data[]; } gParticleBuf;
layout(binding = 1, offset = 0) uniform atomic_uint gCounter;
layout(std430, binding = 1) buffer gVisible { uint data[]; } gVisibleBuf;
void main()
{
    uint i = gl_GlobalInvocationID.x;
    Particle p = gParticleBuf.data[i];
    vec4 clip = gViewProj * vec4(p.position, 1.0);
    float w = clip.w;
    float s = gClipScale.x * w;
    bool visible = (abs(clip.x) <= s) && (abs(clip.y) <= s) && (clip.z >= 0.0) && (clip.z <= w);
    if (visible)
    {
        uint idx = atomicCounterIncrement(gCounter);
        gVisibleBuf.data[idx] = i;
    }
}
)";
    static const char *gles = R"(
#version 310 es
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
struct Particle { vec3 position; float life; vec3 velocity; float size; };
layout(std140, binding = 0) uniform CullParams { mat4 gViewProj; vec4 gClipScale; };
layout(std430, binding = 0) readonly buffer gParticles { Particle data[]; } gParticleBuf;
layout(binding = 1, offset = 0) uniform atomic_uint gCounter;
layout(std430, binding = 1) buffer gVisible { uint data[]; } gVisibleBuf;
void main()
{
    uint i = gl_GlobalInvocationID.x;
    Particle p = gParticleBuf.data[i];
    vec4 clip = gViewProj * vec4(p.position, 1.0);
    float w = clip.w;
    float s = gClipScale.x * w;
    bool visible = (abs(clip.x) <= s) && (abs(clip.y) <= s) && (clip.z >= 0.0) && (clip.z <= w);
    if (visible)
    {
        uint idx = atomicCounterIncrement(gCounter);
        gVisibleBuf.data[idx] = i;
    }
}
)";
    return make(hlsl, gl, gles);
}

ComputeShaderSource particleDrawVSSource()
{
    static const char *hlsl = R"(
struct Particle { float3 position; float life; float3 velocity; float size; };
cbuffer type_DrawParams : register(b0)
{
    row_major float4x4 DrawParams_gView;
    row_major float4x4 DrawParams_gProj;
    row_major float4x4 DrawParams_gViewProj;
    float4 DrawParams_gMode;
};
StructuredBuffer<Particle> gParticles : register(t0);
StructuredBuffer<uint> gVisible : register(t1);
struct VertexOutput { float4 position : SV_POSITION; float4 color : COLOR0; };
VertexOutput main(uint vid : SV_VertexID, uint iid : SV_InstanceID)
{
    uint particleIndex = (DrawParams_gMode.x > 0.5f) ? gVisible[iid] : iid;
    Particle p = gParticles[particleIndex];
    float2 corners[6] = { float2(-1,-1), float2(1,-1), float2(1,1), float2(-1,-1), float2(1,1), float2(-1,1) };
    float2 corner = corners[vid % 6];
    float3 camRight = float3(DrawParams_gView._11, DrawParams_gView._12, DrawParams_gView._13);
    float3 camUp    = float3(DrawParams_gView._21, DrawParams_gView._22, DrawParams_gView._23);
    float3 world = p.position + (camRight * corner.x + camUp * corner.y) * p.size;
    VertexOutput o;
    o.position = mul(DrawParams_gViewProj, float4(world, 1.0f));
    float t = saturate(p.life * 0.25f);
    o.color = float4(t, 0.6f, 1.0f - t, 1.0f);
    return o;
}
)";
    static const char *gl = R"(
#version 430
struct Particle { vec3 position; float life; vec3 velocity; float size; };
layout(std140, binding = 0) uniform DrawParams { mat4 gView; mat4 gProj; mat4 gViewProj; vec4 gMode; };
layout(std430, binding = 0) readonly buffer gParticles { Particle data[]; } gParticleBuf;
layout(std430, binding = 1) readonly buffer gVisible { uint data[]; } gVisibleBuf;
out vec4 vColor;
void main()
{
    uint iid = uint(gl_InstanceID);
    uint vid = uint(gl_VertexID);
    uint particleIndex = (gMode.x > 0.5) ? gVisibleBuf.data[iid] : iid;
    Particle p = gParticleBuf.data[particleIndex];
    vec2 corners[6] = vec2[](vec2(-1.0,-1.0), vec2(1.0,-1.0), vec2(1.0,1.0), vec2(-1.0,-1.0), vec2(1.0,1.0), vec2(-1.0,1.0));
    vec2 corner = corners[vid % 6u];
    vec3 camRight = vec3(gView[0][0], gView[0][1], gView[0][2]);
    vec3 camUp    = vec3(gView[1][0], gView[1][1], gView[1][2]);
    vec3 world = p.position + (camRight * corner.x + camUp * corner.y) * p.size;
    gl_Position = gViewProj * vec4(world, 1.0);
    float t = clamp(p.life * 0.25, 0.0, 1.0);
    vColor = vec4(t, 0.6, 1.0 - t, 1.0);
}
)";
    static const char *gles = R"(
#version 310 es
struct Particle { vec3 position; float life; vec3 velocity; float size; };
layout(std140, binding = 0) uniform DrawParams { mat4 gView; mat4 gProj; mat4 gViewProj; vec4 gMode; };
layout(std430, binding = 0) readonly buffer gParticles { Particle data[]; } gParticleBuf;
layout(std430, binding = 1) readonly buffer gVisible { uint data[]; } gVisibleBuf;
out vec4 vColor;
void main()
{
    uint iid = uint(gl_InstanceID);
    uint vid = uint(gl_VertexID);
    uint particleIndex = (gMode.x > 0.5) ? gVisibleBuf.data[iid] : iid;
    Particle p = gParticleBuf.data[particleIndex];
    vec2 corners[6] = vec2[](vec2(-1.0,-1.0), vec2(1.0,-1.0), vec2(1.0,1.0), vec2(-1.0,-1.0), vec2(1.0,1.0), vec2(-1.0,1.0));
    vec2 corner = corners[vid % 6u];
    vec3 camRight = vec3(gView[0][0], gView[0][1], gView[0][2]);
    vec3 camUp    = vec3(gView[1][0], gView[1][1], gView[1][2]);
    vec3 world = p.position + (camRight * corner.x + camUp * corner.y) * p.size;
    gl_Position = gViewProj * vec4(world, 1.0);
    float t = clamp(p.life * 0.25, 0.0, 1.0);
    vColor = vec4(t, 0.6, 1.0 - t, 1.0);
}
)";
    return make(hlsl, gl, gles);
}

ComputeShaderSource particleDrawCpuVSSource()
{
    static const char *hlsl = R"(
cbuffer type_DrawParams : register(b0)
{
    row_major float4x4 DrawParams_gView;
    row_major float4x4 DrawParams_gProj;
    row_major float4x4 DrawParams_gViewProj;
    float4 DrawParams_gMode;
};
struct VSIn
{
    float3 position : POSITION;
    float  life : TEXCOORD0;
    float3 velocity : TEXCOORD1;
    float  size : TEXCOORD2;
};
struct VertexOutput { float4 position : SV_POSITION; float4 color : COLOR0; };
VertexOutput main(VSIn input, uint vid : SV_VertexID)
{
    float2 corners[6] = { float2(-1,-1), float2(1,-1), float2(1,1), float2(-1,-1), float2(1,1), float2(-1,1) };
    float2 corner = corners[vid % 6];
    float3 camRight = float3(DrawParams_gView._11, DrawParams_gView._12, DrawParams_gView._13);
    float3 camUp    = float3(DrawParams_gView._21, DrawParams_gView._22, DrawParams_gView._23);
    float3 world = input.position + (camRight * corner.x + camUp * corner.y) * input.size;
    VertexOutput o;
    o.position = mul(DrawParams_gViewProj, float4(world, 1.0f));
    float t = saturate(input.life * 0.25f);
    o.color = float4(t, 0.6f, 1.0f - t, 1.0f);
    return o;
}
)";
    static const char *gl = R"(
#version 430
layout(std140, binding = 0) uniform DrawParams { mat4 gView; mat4 gProj; mat4 gViewProj; vec4 gMode; };
layout(location = 0) in vec3 inPosition;
layout(location = 1) in float inLife;
layout(location = 2) in vec3 inVelocity;
layout(location = 3) in float inSize;
out vec4 vColor;
void main()
{
    uint vid = uint(gl_VertexID);
    vec2 corners[6] = vec2[](vec2(-1.0,-1.0), vec2(1.0,-1.0), vec2(1.0,1.0), vec2(-1.0,-1.0), vec2(1.0,1.0), vec2(-1.0,1.0));
    vec2 corner = corners[vid % 6u];
    vec3 camRight = vec3(gView[0][0], gView[0][1], gView[0][2]);
    vec3 camUp    = vec3(gView[1][0], gView[1][1], gView[1][2]);
    vec3 world = inPosition + (camRight * corner.x + camUp * corner.y) * inSize;
    gl_Position = gViewProj * vec4(world, 1.0);
    float t = clamp(inLife * 0.25, 0.0, 1.0);
    vColor = vec4(t, 0.6, 1.0 - t, 1.0);
}
)";
    static const char *gles = R"(
#version 310 es
layout(std140, binding = 0) uniform DrawParams { mat4 gView; mat4 gProj; mat4 gViewProj; vec4 gMode; };
layout(location = 0) in vec3 inPosition;
layout(location = 1) in float inLife;
layout(location = 2) in vec3 inVelocity;
layout(location = 3) in float inSize;
out vec4 vColor;
void main()
{
    uint vid = uint(gl_VertexID);
    vec2 corners[6] = vec2[](vec2(-1.0,-1.0), vec2(1.0,-1.0), vec2(1.0,1.0), vec2(-1.0,-1.0), vec2(1.0,1.0), vec2(-1.0,1.0));
    vec2 corner = corners[vid % 6u];
    vec3 camRight = vec3(gView[0][0], gView[0][1], gView[0][2]);
    vec3 camUp    = vec3(gView[1][0], gView[1][1], gView[1][2]);
    vec3 world = inPosition + (camRight * corner.x + camUp * corner.y) * inSize;
    gl_Position = gViewProj * vec4(world, 1.0);
    float t = clamp(inLife * 0.25, 0.0, 1.0);
    vColor = vec4(t, 0.6, 1.0 - t, 1.0);
}
)";
    return make(hlsl, gl, gles);
}

ComputeShaderSource particleDrawPSSource()
{
    static const char *hlsl = R"(
struct VertexOutput { float4 position : SV_POSITION; float4 color : COLOR0; };
float4 main(VertexOutput input) : SV_TARGET
{
    return input.color;
}
)";
    static const char *gl = R"(
#version 430
in vec4 vColor;
out vec4 fragColor;
void main()
{
    fragColor = vColor;
}
)";
    static const char *gles = R"(
#version 310 es
precision mediump float;
in vec4 vColor;
out vec4 fragColor;
void main()
{
    fragColor = vColor;
}
)";
    return make(hlsl, gl, gles);
}
