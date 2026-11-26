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


#include "PointLightShadowShaders.h"
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
    const char *const kPLShadowParamsName = "PLShadowParams";
    const char *const kPLSceneParamsName = "PLSceneParams";

    namespace
    {
        // HLSL matrices are row_major and multiplied as mul(M, v); the GL side uploads the
        // transpose and multiplies M * v, so both read the same Matrix4.

        const char *kShadowVSHLSL = R"(
cbuffer PLShadowParams : register(b0)
{
    row_major float4x4 gFaceViewProj;
    float4 gLightPosRange;
};
struct VSIn { float3 position : POSITION; float3 normal : NORMAL; float3 color : COLOR; };
struct VSOut { float4 position : SV_POSITION; float3 world : TEXCOORD0; };
VSOut main(VSIn input)
{
    VSOut o;
    o.position = mul(gFaceViewProj, float4(input.position, 1.0f));
    o.world = input.position;
    return o;
}
)";

        const char *kShadowPSHLSL = R"(
cbuffer PLShadowParams : register(b0)
{
    row_major float4x4 gFaceViewProj;
    float4 gLightPosRange;
};
struct VSOut { float4 position : SV_POSITION; float3 world : TEXCOORD0; };
float4 main(VSOut input) : SV_TARGET
{
    float d = length(input.world - gLightPosRange.xyz) / gLightPosRange.w;
    return float4(d, d * d, 0.0f, 1.0f);
}
)";

        const char *kSceneVSHLSL = R"(
cbuffer PLSceneParams : register(b0)
{
    row_major float4x4 gViewProj;
    float4 gLightPosRange;
    float4 gLightColor;
    float4 gShadowParams;
};
struct VSIn { float3 position : POSITION; float3 normal : NORMAL; float3 color : COLOR; };
struct VSOut
{
    float4 position : SV_POSITION;
    float3 world : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 color : TEXCOORD2;
};
VSOut main(VSIn input)
{
    VSOut o;
    o.position = mul(gViewProj, float4(input.position, 1.0f));
    o.world = input.position;
    o.normal = input.normal;
    o.color = input.color;
    return o;
}
)";

        const char *kScenePSHLSL = R"(
cbuffer PLSceneParams : register(b0)
{
    row_major float4x4 gViewProj;
    float4 gLightPosRange;  // xyz - light position, w - range
    float4 gLightColor;
    float4 gShadowParams;   // x - mip to sample, y - min variance, z - light bleeding cut, w - shadows on
};
TextureCube<float4> gShadowCube : register(t0);
SamplerState samplergShadowCube : register(s0);
struct VSOut
{
    float4 position : SV_POSITION;
    float3 world : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 color : TEXCOORD2;
};
float4 main(VSOut input) : SV_TARGET
{
    // The light marker has no normal and is drawn unlit
    if (dot(input.normal, input.normal) < 0.01f)
        return float4(input.color, 1.0f);

    float3 n = normalize(input.normal);
    float3 toLight = gLightPosRange.xyz - input.world;
    float dist = length(toLight);
    float3 l = toLight / dist;
    float ndl = saturate(dot(n, l));
    float atten = saturate(1.0f - dist / gLightPosRange.w);
    atten *= atten;

    float lit = 1.0f;
    if (gShadowParams.w > 0.5f)
    {
        float2 m = gShadowCube.SampleLevel(samplergShadowCube, -l, gShadowParams.x).xy;
        float d = dist / gLightPosRange.w;
        if (d > m.x)
        {
            float variance = max(m.y - m.x * m.x, gShadowParams.y);
            float diff = d - m.x;
            float p = variance / (variance + diff * diff);
            lit = saturate((p - gShadowParams.z) / (1.0f - gShadowParams.z));
        }
    }

    float3 color = input.color * (0.06f + gLightColor.rgb * ndl * atten * lit);
    return float4(color, 1.0f);
}
)";

        const char *kShadowVSGL = R"(#version 430
layout(std140, binding = 0) uniform PLShadowParams { mat4 gFaceViewProj; vec4 gLightPosRange; };
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
out vec3 vWorld;
void main()
{
    gl_Position = gFaceViewProj * vec4(inPosition, 1.0);
    vWorld = inPosition;
}
)";

        const char *kShadowPSGL = R"(#version 430
layout(std140, binding = 0) uniform PLShadowParams { mat4 gFaceViewProj; vec4 gLightPosRange; };
in vec3 vWorld;
out vec4 fragColor;
void main()
{
    float d = length(vWorld - gLightPosRange.xyz) / gLightPosRange.w;
    fragColor = vec4(d, d * d, 0.0, 1.0);
}
)";

        const char *kSceneVSGL = R"(#version 430
layout(std140, binding = 0) uniform PLSceneParams { mat4 gViewProj; vec4 gLightPosRange; vec4 gLightColor; vec4 gShadowParams; };
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
out vec3 vWorld;
out vec3 vNormal;
out vec3 vColor;
void main()
{
    gl_Position = gViewProj * vec4(inPosition, 1.0);
    vWorld = inPosition;
    vNormal = inNormal;
    vColor = inColor;
}
)";

        // Sampler name follows T3D_Combined_t<slot>s<slot>l<len><texture>sampler<key> so the
        // backend binds texture slot 0 and sampler slot 0 like the HLSL registers.
        const char *kScenePSGL = R"(#version 430
layout(std140, binding = 0) uniform PLSceneParams { mat4 gViewProj; vec4 gLightPosRange; vec4 gLightColor; vec4 gShadowParams; };
uniform samplerCube T3D_Combined_t0s0l11gShadowCubesamplergShadowCube;
in vec3 vWorld;
in vec3 vNormal;
in vec3 vColor;
out vec4 fragColor;
void main()
{
    if (dot(vNormal, vNormal) < 0.01)
    {
        fragColor = vec4(vColor, 1.0);
        return;
    }

    vec3 n = normalize(vNormal);
    vec3 toLight = gLightPosRange.xyz - vWorld;
    float dist = length(toLight);
    vec3 l = toLight / dist;
    float ndl = clamp(dot(n, l), 0.0, 1.0);
    float atten = clamp(1.0 - dist / gLightPosRange.w, 0.0, 1.0);
    atten *= atten;

    float lit = 1.0;
    if (gShadowParams.w > 0.5)
    {
        vec2 m = textureLod(T3D_Combined_t0s0l11gShadowCubesamplergShadowCube, -l, gShadowParams.x).xy;
        float d = dist / gLightPosRange.w;
        if (d > m.x)
        {
            float variance = max(m.y - m.x * m.x, gShadowParams.y);
            float diff = d - m.x;
            float p = variance / (variance + diff * diff);
            lit = clamp((p - gShadowParams.z) / (1.0 - gShadowParams.z), 0.0, 1.0);
        }
    }

    vec3 color = vColor * (0.06 + gLightColor.rgb * ndl * atten * lit);
    fragColor = vec4(color, 1.0);
}
)";

        struct Source
        {
            const char *hlsl;
            const char *gl;
            SHADER_STAGE stage;
            const char *name;
        };

        const Source &lookup(PLShader id)
        {
            static const Source kSources[] =
            {
                { kShadowVSHLSL, kShadowVSGL, SHADER_STAGE::kVertex, "PLShadowVS" },
                { kShadowPSHLSL, kShadowPSGL, SHADER_STAGE::kPixel, "PLShadowPS" },
                { kSceneVSHLSL, kSceneVSGL, SHADER_STAGE::kVertex, "PLSceneVS" },
                { kScenePSHLSL, kScenePSGL, SHADER_STAGE::kPixel, "PLScenePS" },
            };
            return kSources[static_cast<uint32_t>(id)];
        }

        uint32_t nextShaderId()
        {
            static uint32_t counter = 0;
            return ++counter;
        }
    }

    //--------------------------------------------------------------------------

    ShaderVariantPtr createPLShader(PLShader id, ShaderPtr &dummyShader)
    {
        dummyShader = nullptr;

        RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
        if (renderer == nullptr)
        {
            return nullptr;
        }

        const Source &source = lookup(id);
        const char *code = nullptr;
        const String &rendererName = renderer->getName();
        if (rendererName == RHIRenderer::DIRECT3D11)
        {
            code = source.hlsl;
        }
        else if (rendererName == RHIRenderer::OPENGL4)
        {
            code = source.gl;
        }
        if (code == nullptr)
        {
            return nullptr;
        }

        ShaderKeyword keyword;
        keyword.addKeyword("");
        keyword.generate();

        ShaderVariantPtr variant = ShaderVariant::create(std::move(keyword), code, strlen(code));
        if (variant == nullptr)
        {
            return nullptr;
        }
        variant->setShaderStage(source.stage);
        variant->setLanguage(renderer->getShadingLanguage());

        // D3D11 compileShader walks Pass -> Shader for the name, so every variant hangs on a
        // dummy pass that never enters a material; the names must be unique per variant.
        const String name = String(source.name) + "-" + std::to_string(nextShaderId());
        ShaderPtr shader = T3D_SHADER_MGR.createShader("Hidden-" + name);
        TechniquePtr tech = Technique::create(name);
        PassPtr pass = Pass::create(name);
        if (shader == nullptr || tech == nullptr || pass == nullptr
            || !tech->addPass(pass) || !shader->addTechnique(tech))
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "PointLightShadow : failed to create dummy pass for %s", name.c_str());
            return nullptr;
        }
        variant->setPass(pass);

        // GL assigns texture units from the reflected sampler names, so reflect after compiling
        TResult ret = variant->compile();
        if (T3D_SUCCEEDED(ret))
        {
            ret = variant->reflect();
        }
        if (T3D_FAILED(ret))
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "PointLightShadow : failed to build %s ! ERROR [%d]", name.c_str(), ret);
            return nullptr;
        }

        dummyShader = shader;
        return variant;
    }
}
