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


#include "SoftParticleShaders.h"
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
    const char *const kSPSceneParamsName = "SPSceneParams";
    const char *const kSPParticleParamsName = "SPParticleParams";

    namespace
    {
        // HLSL matrices are row_major and multiplied as mul(M, v); the GL side uploads the
        // transpose and multiplies M * v, so both read the same Matrix4.

        const char *kSceneVSHLSL = R"(
cbuffer SPSceneParams : register(b0)
{
    row_major float4x4 gViewProj;
    float4 gFirePosRange;
    float4 gFireColor;
    float4 gMoonDir;
    float4 gMoonColor;
    float4 gAmbient;
    float4 gEyeFog;
    float4 gFogColor;
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
cbuffer SPSceneParams : register(b0)
{
    row_major float4x4 gViewProj;
    float4 gFirePosRange;   // xyz - fire light position, w - range
    float4 gFireColor;      // rgb - flickering fire color times intensity
    float4 gMoonDir;        // xyz - direction towards the moon
    float4 gMoonColor;
    float4 gAmbient;
    float4 gEyeFog;         // xyz - camera position, w - fog density
    float4 gFogColor;
};
struct VSOut
{
    float4 position : SV_POSITION;
    float3 world : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 color : TEXCOORD2;
};
float4 main(VSOut input) : SV_TARGET
{
    // Glowing embers have no normal and are drawn unlit
    if (dot(input.normal, input.normal) < 0.01f)
        return float4(input.color, 1.0f);

    float3 n = normalize(input.normal);
    float3 toFire = gFirePosRange.xyz - input.world;
    float dist = length(toFire);
    float ndl = saturate(dot(n, toFire / dist) * 0.85f + 0.15f);
    float atten = saturate(1.0f - dist / gFirePosRange.w);
    atten *= atten;
    float moon = saturate(dot(n, gMoonDir.xyz));

    float3 color = input.color * (gAmbient.rgb + gMoonColor.rgb * moon + gFireColor.rgb * ndl * atten);
    float fog = 1.0f - exp(-gEyeFog.w * length(input.world - gEyeFog.xyz));
    return float4(lerp(color, gFogColor.rgb, fog), 1.0f);
}
)";

        const char *kParticleVSHLSL = R"(
cbuffer SPParticleParams : register(b0)
{
    row_major float4x4 gViewProj;
    float4 gDepthParams;
};
struct VSIn { float3 position : POSITION; float4 color : COLOR; float3 params : TEXCOORD0; };
struct VSOut
{
    float4 position : SV_POSITION;
    float4 color : TEXCOORD0;
    float3 params : TEXCOORD1;
    float viewZ : TEXCOORD2;
};
VSOut main(VSIn input)
{
    VSOut o;
    o.position = mul(gViewProj, float4(input.position, 1.0f));
    o.color = input.color;
    o.params = input.params;
    o.viewZ = o.position.w;
    return o;
}
)";

        // The depth texture is read with Load at the pixel being shaded, which needs no
        // sampler and lands on the same texel on D3D11 and GL.
        const char *kParticlePSHLSL = R"(
cbuffer SPParticleParams : register(b0)
{
    row_major float4x4 gViewProj;
    float4 gDepthParams;    // x - near, y - far, z - soft on, w - near plane fade distance
};
Texture2D<float> gSceneDepth : register(t0);
struct VSOut
{
    float4 position : SV_POSITION;
    float4 color : TEXCOORD0;
    float3 params : TEXCOORD1;  // xy - corner uv, z - soft fade distance
    float viewZ : TEXCOORD2;
};
float4 main(VSOut input) : SV_TARGET
{
    float2 d = input.params.xy * 2.0f - 1.0f;
    float r2 = dot(d, d);
    if (r2 >= 1.0f)
        discard;
    float shape = 1.0f - r2;
    float alpha = input.color.a * shape * shape;

    if (gDepthParams.z > 0.5f)
    {
        float raw = gSceneDepth.Load(int3(int2(input.position.xy), 0));
        float n = gDepthParams.x;
        float f = gDepthParams.y;
        float sceneZ = n * f / (f - raw * (f - n));
        alpha *= saturate((sceneZ - input.viewZ) / input.params.z);
    }
    alpha *= saturate((input.viewZ - gDepthParams.x) / gDepthParams.w);
    return float4(input.color.rgb, alpha);
}
)";

        const char *kSceneVSGL = R"(#version 430
layout(std140, binding = 0) uniform SPSceneParams { mat4 gViewProj; vec4 gFirePosRange; vec4 gFireColor; vec4 gMoonDir; vec4 gMoonColor; vec4 gAmbient; vec4 gEyeFog; vec4 gFogColor; };
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

        const char *kScenePSGL = R"(#version 430
layout(std140, binding = 0) uniform SPSceneParams { mat4 gViewProj; vec4 gFirePosRange; vec4 gFireColor; vec4 gMoonDir; vec4 gMoonColor; vec4 gAmbient; vec4 gEyeFog; vec4 gFogColor; };
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
    vec3 toFire = gFirePosRange.xyz - vWorld;
    float dist = length(toFire);
    float ndl = clamp(dot(n, toFire / dist) * 0.85 + 0.15, 0.0, 1.0);
    float atten = clamp(1.0 - dist / gFirePosRange.w, 0.0, 1.0);
    atten *= atten;
    float moon = clamp(dot(n, gMoonDir.xyz), 0.0, 1.0);

    vec3 color = vColor * (gAmbient.rgb + gMoonColor.rgb * moon + gFireColor.rgb * ndl * atten);
    float fog = 1.0 - exp(-gEyeFog.w * length(vWorld - gEyeFog.xyz));
    fragColor = vec4(mix(color, gFogColor.rgb, fog), 1.0);
}
)";

        const char *kParticleVSGL = R"(#version 430
layout(std140, binding = 0) uniform SPParticleParams { mat4 gViewProj; vec4 gDepthParams; };
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inParams;
out vec4 vColor;
out vec3 vParams;
out float vViewZ;
void main()
{
    gl_Position = gViewProj * vec4(inPosition, 1.0);
    vColor = inColor;
    vParams = inParams;
    vViewZ = gl_Position.w;
}
)";

        // Sampler name follows T3D_Combined_t<slot>s<slot>l<len><texture>sampler<key> so the
        // backend binds texture slot 0 and sampler slot 0 like the HLSL registers.
        const char *kParticlePSGL = R"(#version 430
layout(std140, binding = 0) uniform SPParticleParams { mat4 gViewProj; vec4 gDepthParams; };
uniform sampler2D T3D_Combined_t0s0l11gSceneDepthsamplergSceneDepth;
in vec4 vColor;
in vec3 vParams;
in float vViewZ;
out vec4 fragColor;
void main()
{
    vec2 d = vParams.xy * 2.0 - 1.0;
    float r2 = dot(d, d);
    if (r2 >= 1.0)
        discard;
    float shape = 1.0 - r2;
    float alpha = vColor.a * shape * shape;

    if (gDepthParams.z > 0.5)
    {
        float raw = texelFetch(T3D_Combined_t0s0l11gSceneDepthsamplergSceneDepth, ivec2(gl_FragCoord.xy), 0).r;
        float n = gDepthParams.x;
        float f = gDepthParams.y;
        float sceneZ = n * f / (f - raw * (f - n));
        alpha *= clamp((sceneZ - vViewZ) / vParams.z, 0.0, 1.0);
    }
    alpha *= clamp((vViewZ - gDepthParams.x) / gDepthParams.w, 0.0, 1.0);
    fragColor = vec4(vColor.rgb, alpha);
}
)";

        struct Source
        {
            const char *hlsl;
            const char *gl;
            SHADER_STAGE stage;
            const char *name;
        };

        const Source &lookup(SPShader id)
        {
            static const Source kSources[] =
            {
                { kSceneVSHLSL, kSceneVSGL, SHADER_STAGE::kVertex, "SPSceneVS" },
                { kScenePSHLSL, kScenePSGL, SHADER_STAGE::kPixel, "SPScenePS" },
                { kParticleVSHLSL, kParticleVSGL, SHADER_STAGE::kVertex, "SPParticleVS" },
                { kParticlePSHLSL, kParticlePSGL, SHADER_STAGE::kPixel, "SPParticlePS" },
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

    ShaderVariantPtr createSPShader(SPShader id, ShaderPtr &dummyShader)
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
            T3D_LOG_ERROR(LOG_TAG_APP, "SoftParticle : failed to create dummy pass for %s", name.c_str());
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
            T3D_LOG_ERROR(LOG_TAG_APP, "SoftParticle : failed to build %s ! ERROR [%d]", name.c_str(), ret);
            return nullptr;
        }

        dummyShader = shader;
        return variant;
    }
}
