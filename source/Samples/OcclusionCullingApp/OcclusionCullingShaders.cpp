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


#include "OcclusionCullingShaders.h"
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
    const char *const kCityParamsName = "CityParams";

    namespace
    {
        // HLSL matrices are row_major and multiplied as mul(M, v); the GL side uploads the
        // transpose and multiplies M * v, so both read the same Matrix4.

        const char *kVSHLSL = R"(
cbuffer CityParams : register(b0)
{
    row_major float4x4 gViewProj;
    float4 gCameraFog;  // xyz - camera position, w - fog density
    float4 gToSun;      // xyz - direction toward the sun
    float4 gSkyColor;
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

        const char *kPSHLSL = R"(
cbuffer CityParams : register(b0)
{
    row_major float4x4 gViewProj;
    float4 gCameraFog;
    float4 gToSun;
    float4 gSkyColor;
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
    float3 n = normalize(input.normal);
    float ndl = saturate(dot(n, gToSun.xyz));
    float hemi = 0.5f + 0.5f * n.y;
    float3 lit = input.color * (0.22f + 0.18f * hemi + 0.75f * ndl);
    float dist = length(input.world - gCameraFog.xyz);
    float fog = 1.0f - exp(-dist * gCameraFog.w);
    return float4(lerp(lit, gSkyColor.rgb, fog), 1.0f);
}
)";

        const char *kVSGL = R"(#version 430
layout(std140, binding = 0) uniform CityParams { mat4 gViewProj; vec4 gCameraFog; vec4 gToSun; vec4 gSkyColor; };
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

        const char *kPSGL = R"(#version 430
layout(std140, binding = 0) uniform CityParams { mat4 gViewProj; vec4 gCameraFog; vec4 gToSun; vec4 gSkyColor; };
in vec3 vWorld;
in vec3 vNormal;
in vec3 vColor;
out vec4 fragColor;
void main()
{
    vec3 n = normalize(vNormal);
    float ndl = clamp(dot(n, gToSun.xyz), 0.0, 1.0);
    float hemi = 0.5 + 0.5 * n.y;
    vec3 lit = vColor * (0.22 + 0.18 * hemi + 0.75 * ndl);
    float dist = length(vWorld - gCameraFog.xyz);
    float fog = 1.0 - exp(-dist * gCameraFog.w);
    fragColor = vec4(mix(lit, gSkyColor.rgb, fog), 1.0);
}
)";

        struct Source
        {
            const char *hlsl;
            const char *gl;
            SHADER_STAGE stage;
            const char *name;
        };

        const Source &lookup(CityShader id)
        {
            static const Source kSources[] =
            {
                { kVSHLSL, kVSGL, SHADER_STAGE::kVertex, "CityVS" },
                { kPSHLSL, kPSGL, SHADER_STAGE::kPixel, "CityPS" },
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

    ShaderVariantPtr createCityShader(CityShader id, ShaderPtr &dummyShader)
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
            T3D_LOG_ERROR(LOG_TAG_APP, "OcclusionCulling : failed to create dummy pass for %s", name.c_str());
            return nullptr;
        }
        variant->setPass(pass);

        TResult ret = variant->compile();
        if (T3D_SUCCEEDED(ret))
        {
            ret = variant->reflect();
        }
        if (T3D_FAILED(ret))
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "OcclusionCulling : failed to build %s ! ERROR [%d]", name.c_str(), ret);
            return nullptr;
        }

        dummyShader = shader;
        return variant;
    }
}
