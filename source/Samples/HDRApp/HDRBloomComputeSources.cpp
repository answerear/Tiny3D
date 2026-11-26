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


#include "HDRBloomComputeSources.h"
#include "Kernel/T3DAgent.h"
#include "Material/T3DPass.h"
#include "Material/T3DShaderKeyword.h"
#include "Material/T3DShaderVariant.h"
#include "Material/T3DTechnique.h"
#include "Resource/T3DShader.h"
#include "Resource/T3DShaderManager.h"
#include "RHI/T3DRHIRenderer.h"

#define LOG_TAG_APP     "APP"


namespace Tiny3D
{
    namespace
    {
        // Sources of assets/samples/shaders/HDRBloomDownsample.cshader and HDRBloomUpsample.cshader.
        // The SRVs select one mip each, so mip 0 inside the shader is the selected level
        // and GetDimensions / textureSize report that level's size.

        const char *kDownsampleHLSL = R"(
Texture2D<float4> _MainTex : register(t0);
RWTexture2D<float4> _Output : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint w, h;
    _Output.GetDimensions(w, h);
    if (id.x >= w || id.y >= h)
        return;

    // Destination is floor(src / 2), so p + 1 never leaves the source
    int2 p = int2(id.xy) * 2;
    float3 color = (_MainTex.Load(int3(p, 0)).rgb + _MainTex.Load(int3(p + int2(1, 0), 0)).rgb
        + _MainTex.Load(int3(p + int2(0, 1), 0)).rgb + _MainTex.Load(int3(p + int2(1, 1), 0)).rgb) * 0.25f;
    _Output[id.xy] = float4(color, 1.0f);
}
)";

        const char *kUpsampleHLSL = R"(
Texture2D<float4> _MainTex : register(t0);
Texture2D<float4> _BloomTex : register(t1);
RWTexture2D<float4> _Output : register(u0);

// Bilinear tap at a texel-space coordinate, clamped to the edge
float3 bilinear(float2 coord, int2 size)
{
    float2 t = coord - 0.5f;
    int2 i0 = int2(floor(t));
    float2 f = t - float2(i0);
    int2 i1 = min(i0 + 1, size - 1);
    i0 = clamp(i0, int2(0, 0), size - 1);
    float3 a = _BloomTex.Load(int3(i0.x, i0.y, 0)).rgb;
    float3 b = _BloomTex.Load(int3(i1.x, i0.y, 0)).rgb;
    float3 c = _BloomTex.Load(int3(i0.x, i1.y, 0)).rgb;
    float3 d = _BloomTex.Load(int3(i1.x, i1.y, 0)).rgb;
    return lerp(lerp(a, b, f.x), lerp(c, d, f.x), f.y);
}

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint w, h;
    _Output.GetDimensions(w, h);
    if (id.x >= w || id.y >= h)
        return;

    uint bw, bh;
    _BloomTex.GetDimensions(bw, bh);
    int2 bsize = int2(bw, bh);
    float2 bscale = float2(bw, bh);

    float3 color = _MainTex.Load(int3(int2(id.xy), 0)).rgb;
    float2 d = 1.0f / float2(w, h);
    float2 uv = (float2(id.xy) + 0.5f) * d;
    float3 bloom = bilinear((uv + float2(-d.x, -d.y)) * bscale, bsize)
        + bilinear((uv + float2(d.x, -d.y)) * bscale, bsize)
        + bilinear((uv + float2(-d.x, d.y)) * bscale, bsize)
        + bilinear((uv + float2(d.x, d.y)) * bscale, bsize);
    _Output[id.xy] = float4(color + bloom * 0.25f, 1.0f);
}
)";

        // GL : sampler names follow the T3D_Combined_t<slot>sxl<len><name> convention so the
        // backend maps them to the same t# slots as HLSL; the image format is patched in.
        const char *kDownsampleGL = R"(#version 430
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
uniform sampler2D T3D_Combined_t0sxl8_MainTex;
layout(%FORMAT%, binding = 0) uniform writeonly image2D _Output;

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(_Output);
    if (id.x >= size.x || id.y >= size.y)
        return;

    ivec2 p = id * 2;
    vec3 color = (texelFetch(T3D_Combined_t0sxl8_MainTex, p, 0).rgb
        + texelFetch(T3D_Combined_t0sxl8_MainTex, p + ivec2(1, 0), 0).rgb
        + texelFetch(T3D_Combined_t0sxl8_MainTex, p + ivec2(0, 1), 0).rgb
        + texelFetch(T3D_Combined_t0sxl8_MainTex, p + ivec2(1, 1), 0).rgb) * 0.25;
    imageStore(_Output, id, vec4(color, 1.0));
}
)";

        const char *kUpsampleGL = R"(#version 430
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
uniform sampler2D T3D_Combined_t0sxl8_MainTex;
uniform sampler2D T3D_Combined_t1sxl9_BloomTex;
layout(%FORMAT%, binding = 0) uniform writeonly image2D _Output;

vec3 bilinear(vec2 coord, ivec2 size)
{
    vec2 t = coord - 0.5;
    ivec2 i0 = ivec2(floor(t));
    vec2 f = t - vec2(i0);
    ivec2 i1 = min(i0 + 1, size - 1);
    i0 = clamp(i0, ivec2(0), size - 1);
    vec3 a = texelFetch(T3D_Combined_t1sxl9_BloomTex, ivec2(i0.x, i0.y), 0).rgb;
    vec3 b = texelFetch(T3D_Combined_t1sxl9_BloomTex, ivec2(i1.x, i0.y), 0).rgb;
    vec3 c = texelFetch(T3D_Combined_t1sxl9_BloomTex, ivec2(i0.x, i1.y), 0).rgb;
    vec3 d = texelFetch(T3D_Combined_t1sxl9_BloomTex, ivec2(i1.x, i1.y), 0).rgb;
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(_Output);
    if (id.x >= size.x || id.y >= size.y)
        return;

    ivec2 bsize = textureSize(T3D_Combined_t1sxl9_BloomTex, 0);
    vec2 bscale = vec2(bsize);

    vec3 color = texelFetch(T3D_Combined_t0sxl8_MainTex, id, 0).rgb;
    vec2 d = 1.0 / vec2(size);
    vec2 uv = (vec2(id) + 0.5) * d;
    vec3 bloom = bilinear((uv + vec2(-d.x, -d.y)) * bscale, bsize)
        + bilinear((uv + vec2(d.x, -d.y)) * bscale, bsize)
        + bilinear((uv + vec2(-d.x, d.y)) * bscale, bsize)
        + bilinear((uv + vec2(d.x, d.y)) * bscale, bsize);
    imageStore(_Output, id, vec4(color + bloom * 0.25, 1.0));
}
)";

        const char *glImageFormat(PixelFormat format)
        {
            switch (format)
            {
            case PixelFormat::E_PF_R16G16B16A16_FLOAT:
                return "rgba16f";
            case PixelFormat::E_PF_R32G32B32A32_FLOAT:
                return "rgba32f";
            case PixelFormat::E_PF_R11G11B10_FLOAT:
                return "r11f_g11f_b10f";
            case PixelFormat::E_PF_R8G8B8A8:
            case PixelFormat::E_PF_B8G8R8A8:
                return "rgba8";
            default:
                return nullptr;
            }
        }

        uint32_t nextKernelId()
        {
            static uint32_t counter = 0;
            return ++counter;
        }
    }

    //--------------------------------------------------------------------------

    ShaderVariantPtr createHDRBloomKernel(HDRBloomKernel kernel, PixelFormat format, ShaderPtr &dummyShader)
    {
        dummyShader = nullptr;

        RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
        if (renderer == nullptr)
        {
            return nullptr;
        }

        const bool isUp = (kernel == HDRBloomKernel::kUpsample);
        String code;
        const String &rendererName = renderer->getName();
        if (rendererName == RHIRenderer::DIRECT3D11)
        {
            code = isUp ? kUpsampleHLSL : kDownsampleHLSL;
        }
        else if (rendererName == RHIRenderer::OPENGL4)
        {
            const char *qualifier = glImageFormat(format);
            if (qualifier == nullptr)
            {
                return nullptr;
            }
            code = isUp ? kUpsampleGL : kDownsampleGL;
            const String placeholder = "%FORMAT%";
            code.replace(code.find(placeholder), placeholder.size(), qualifier);
        }
        else
        {
            return nullptr;
        }

        ShaderKeyword keyword;
        keyword.addKeyword("");
        keyword.generate();

        ShaderVariantPtr variant = ShaderVariant::create(std::move(keyword), code.c_str(), code.size());
        if (variant == nullptr)
        {
            return nullptr;
        }
        variant->setShaderStage(SHADER_STAGE::kCompute);
        variant->setLanguage(renderer->getShadingLanguage());

        // Pass does not take compute variants, but D3D11 compileShader walks Pass -> Shader
        // for the name, so the kernel hangs on a dummy pass that never enters a material.
        const String suffix = std::to_string(nextKernelId());
        const String name = String(isUp ? "HDRBloomUpsampleCS-" : "HDRBloomDownsampleCS-") + suffix;
        ShaderPtr shader = T3D_SHADER_MGR.createShader("Hidden-" + name);
        TechniquePtr tech = Technique::create(name);
        PassPtr pass = Pass::create(name);
        if (shader == nullptr || tech == nullptr || pass == nullptr
            || !tech->addPass(pass) || !shader->addTechnique(tech))
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "HDRBloom : failed to create dummy pass for %s", name.c_str());
            return nullptr;
        }
        variant->setPass(pass);

        // reflect() must follow compile(), otherwise it drops the freshly created RHI shader
        TResult ret = variant->compile();
        if (T3D_SUCCEEDED(ret))
        {
            ret = variant->reflect();
        }
        if (T3D_FAILED(ret))
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "HDRBloom : failed to build compute kernel %s ! ERROR [%d]", name.c_str(), ret);
            return nullptr;
        }

        dummyShader = shader;
        return variant;
    }
}
