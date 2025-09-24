#ifndef TINY3D_INCLUDED
#define TINY3D_INCLUDED

//------------------------------------------------------------------------------
//  float 精度的纹理采样器定义
//------------------------------------------------------------------------------

struct sampler1D_f      { Texture1D<float4> t; SamplerState s; };
struct sampler2D_f      { Texture2D<float4> t; SamplerState s; };
struct sampler3D_f      { Texture3D<float4> t; SamplerState s; };
struct samplerCUBE_f    { TextureCube<float4> t; SamplerState s; };

//------------------------------------------------------------------------------
//  float 精度的纹理采样函数
//------------------------------------------------------------------------------

float4 tex1D(sampler1D_f x, float v)        { return x.t.Sample(x.s, v); }
float4 tex2D(sampler2D_f x, float2 v)       { return x.t.Sample(x.s, v); }
float4 tex3D(sampler3D_f x, float3 v)       { return x.t.Sample(x.s, v); }
float4 texCUBE(samplerCUBE_f x, float3 v)   { return x.t.Sample(x.s, v); }

float4 tex1Dbias(sampler1D_f x, in float4 t)        { return x.t.SampleBias(x.s, t.x, t.w); }
float4 tex2Dbias(sampler2D_f x, in float4 t)        { return x.t.SampleBias(x.s, t.xy, t.w); }
float4 tex3Dbias(sampler3D_f x, in float4 t)        { return x.t.SampleBias(x.s, t.xyz, t.w); }
float4 texCUBEbias(samplerCUBE_f x, in float4 t)    { return x.t.SampleBias(x.s, t.xyz, t.w); }

float4 tex1Dlod(sampler1D_f x, in float4 t)     { return x.t.SampleLevel(x.s, t.x, t.w); }
float4 tex2Dlod(sampler2D_f x, in float4 t)     { return x.t.SampleLevel(x.s, t.xy, t.w); }
float4 tex3Dlod(sampler3D_f x, in float4 t)     { return x.t.SampleLevel(x.s, t.xyz, t.w); }
float4 texCUBElod(samplerCUBE_f x, in float4 t) { return x.t.SampleLevel(x.s, t.xyz, t.w); }

float4 tex1Dgrad(sampler1D_f x, float t, float dx, float dy)        { return x.t.SampleGrad(x.s, t, dx, dy); }
float4 tex2Dgrad(sampler2D_f x, float2 t, float2 dx, float2 dy)     { return x.t.SampleGrad(x.s, t, dx, dy); }
float4 tex3Dgrad(sampler3D_f x, float3 t, float3 dx, float3 dy)     { return x.t.SampleGrad(x.s, t, dx, dy); }
float4 texCUBEgrad(samplerCUBE_f x, float3 t, float3 dx, float3 dy) { return x.t.SampleGrad(x.s, t, dx, dy); }

float4 tex1D(sampler1D_f x, float t, float dx, float dy)        { return x.t.SampleGrad(x.s, t, dx, dy); }
float4 tex2D(sampler2D_f x, float2 t, float2 dx, float2 dy)     { return x.t.SampleGrad(x.s, t, dx, dy); }
float4 tex3D(sampler3D_f x, float3 t, float3 dx, float3 dy)     { return x.t.SampleGrad(x.s, t, dx, dy); }
float4 texCUBE(samplerCUBE_f x, float3 t, float3 dx, float3 dy) { return x.t.SampleGrad(x.s, t, dx, dy); }

float4 tex1Dproj(sampler1D_f s, in float2 t)        { return tex1D(s, t.x / t.y); }
float4 tex1Dproj(sampler1D_f s, in float4 t)        { return tex1D(s, t.x / t.w); }
float4 tex2Dproj(sampler2D_f s, in float3 t)        { return tex2D(s, t.xy / t.z); }
float4 tex2Dproj(sampler2D_f s, in float4 t)        { return tex2D(s, t.xy / t.w); }
float4 tex3Dproj(sampler3D_f s, in float4 t)        { return tex3D(s, t.xyz / t.w); }
float4 texCUBEproj(samplerCUBE_f s, in float4 t)    { return texCUBE(s, t.xyz / t.w); }

//------------------------------------------------------------------------------
//  half 精度的纹理采样器
//------------------------------------------------------------------------------
struct sampler1D_h      { Texture1D<min16float4> t; SamplerState s; };
struct sampler2D_h      { Texture2D<min16float4> t; SamplerState s; };
struct sampler3D_h      { Texture3D<min16float4> t; SamplerState s; };
struct samplerCUBE_h    { TextureCube<min16float4> t; SamplerState s; };

//------------------------------------------------------------------------------
//  half 精度的纹理采样函数
//------------------------------------------------------------------------------

min16float4 tex1D(sampler1D_h x, float v)       { return x.t.Sample(x.s, v); }
min16float4 tex2D(sampler2D_h x, float2 v)      { return x.t.Sample(x.s, v); }
min16float4 tex3D(sampler3D_h x, float3 v)      { return x.t.Sample(x.s, v); }
min16float4 texCUBE(samplerCUBE_h x, float3 v)  { return x.t.Sample(x.s, v); }

min16float4 tex1Dbias(sampler1D_h x, in float4 t)       { return x.t.SampleBias(x.s, t.x, t.w); }
min16float4 tex2Dbias(sampler2D_h x, in float4 t)       { return x.t.SampleBias(x.s, t.xy, t.w); }
min16float4 tex3Dbias(sampler3D_h x, in float4 t)       { return x.t.SampleBias(x.s, t.xyz, t.w); }
min16float4 texCUBEbias(samplerCUBE_h x, in float4 t)   { return x.t.SampleBias(x.s, t.xyz, t.w); }

min16float4 tex1Dlod(sampler1D_h x, in float4 t)        { return x.t.SampleLevel(x.s, t.x, t.w); }
min16float4 tex2Dlod(sampler2D_h x, in float4 t)        { return x.t.SampleLevel(x.s, t.xy, t.w); }
min16float4 tex3Dlod(sampler3D_h x, in float4 t)        { return x.t.SampleLevel(x.s, t.xyz, t.w); }
min16float4 texCUBElod(samplerCUBE_h x, in float4 t)    { return x.t.SampleLevel(x.s, t.xyz, t.w); }

min16float4 tex1Dgrad(sampler1D_h x, float t, float dx, float dy)           { return x.t.SampleGrad(x.s, t, dx, dy); }
min16float4 tex2Dgrad(sampler2D_h x, float2 t, float2 dx, float2 dy)        { return x.t.SampleGrad(x.s, t, dx, dy); }
min16float4 tex3Dgrad(sampler3D_h x, float3 t, float3 dx, float3 dy)        { return x.t.SampleGrad(x.s, t, dx, dy); }
min16float4 texCUBEgrad(samplerCUBE_h x, float3 t, float3 dx, float3 dy)    { return x.t.SampleGrad(x.s, t, dx, dy); }

min16float4 tex1D(sampler1D_h x, float t, float dx, float dy)           { return x.t.SampleGrad(x.s, t, dx, dy); }
min16float4 tex2D(sampler2D_h x, float2 t, float2 dx, float2 dy)        { return x.t.SampleGrad(x.s, t, dx, dy); }
min16float4 tex3D(sampler3D_h x, float3 t, float3 dx, float3 dy)        { return x.t.SampleGrad(x.s, t, dx, dy); }
min16float4 texCUBE(samplerCUBE_h x, float3 t, float3 dx, float3 dy)    { return x.t.SampleGrad(x.s, t, dx, dy); }

min16float4 tex1Dproj(sampler1D_h s, in float2 t)       { return tex1D(s, t.x / t.y); }
min16float4 tex1Dproj(sampler1D_h s, in float4 t)       { return tex1D(s, t.x / t.w); }
min16float4 tex2Dproj(sampler2D_h s, in float3 t)       { return tex2D(s, t.xy / t.z); }
min16float4 tex2Dproj(sampler2D_h s, in float4 t)       { return tex2D(s, t.xy / t.w); }
min16float4 tex3Dproj(sampler3D_h s, in float4 t)       { return tex3D(s, t.xyz / t.w); }
min16float4 texCUBEproj(samplerCUBE_h s, in float4 t)   { return texCUBE(s, t.xyz / t.w); }

#define half min16float
#define half2 min16float2
#define half3 min16float3
#define half4 min16float4
#define half2x2 min16float2x2
#define half3x3 min16float3x3
#define half4x4 min16float4x4

#define sampler1D_half sampler1D
#define sampler1D_float sampler1D
#define sampler2D_half sampler2D
#define sampler2D_float sampler2D
#define samplerCUBE_half samplerCUBE
#define samplerCUBE_float samplerCUBE
#define sampler3D_float sampler3D
#define sampler3D_half sampler3D
#define Texture2D_half Texture2D
#define Texture2D_float Texture2D
#define Texture2DArray_half Texture2DArray
#define Texture2DArray_float Texture2DArray
#define Texture2DMS_half Texture2DMS
#define Texture2DMS_float Texture2DMS
#define TextureCube_half TextureCube
#define TextureCube_float TextureCube
#define TextureCubeArray_half TextureCubeArray
#define TextureCubeArray_float TextureCubeArray
#define Texture3D_float Texture3D
#define Texture3D_half Texture3D

#endif  /*TINY3D_INCLUDED*/