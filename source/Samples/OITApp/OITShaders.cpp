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


#include "OITShaders.h"
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
    const char *const kOITSceneParamsName = "OITSceneParams";

    namespace
    {
        // HLSL matrices are row_major and multiplied as mul(M, v); the GL side uploads the
        // transpose and multiplies M * v, so both read the same Matrix4.

        const char *kCommonHLSL = R"(
cbuffer OITSceneParams : register(b0)
{
    row_major float4x4 gViewProj;
    float4 gLightPos[4];    // xyz - position, w - range
    float4 gLightColor[4];
    float4 gAmbient;
    float4 gEye;
    float4 gParams;         // x - scene specular, y - stained glass scatter, z - glass specular
};
struct VSOut
{
    float4 position : SV_POSITION;
    float3 world : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 color : TEXCOORD2;
};
void shade(float3 world, float3 n, out float3 diffuse, out float3 specular)
{
    float3 v = normalize(gEye.xyz - world);
    diffuse = gAmbient.rgb;
    specular = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; ++i)
    {
        float3 l = gLightPos[i].xyz - world;
        float dist = length(l);
        l /= dist;
        float atten = saturate(1.0f - dist / gLightPos[i].w);
        atten *= atten;
        float ndl = saturate(dot(n, l));
        diffuse += gLightColor[i].rgb * ndl * atten;
        float ndh = saturate(dot(n, normalize(l + v)));
        specular += gLightColor[i].rgb * pow(ndh, 48.0f) * ndl * atten;
    }
}
// Glass is seen from both sides
float3 facing(VSOut input)
{
    float3 n = normalize(input.normal);
    return dot(n, gEye.xyz - input.world) < 0.0f ? -n : n;
}
// Uneven thickness of hand made glass
float3 transmittance(VSOut input)
{
    float ripple = 0.95f + 0.05f * sin(input.world.x * 41.0f + 1.5f * sin(input.world.y * 7.0f));
    return saturate(input.color * ripple);
}
// The warm room reflected at grazing angles
float3 reflection(VSOut input, float3 n)
{
    float f = 1.0f - abs(dot(n, normalize(gEye.xyz - input.world)));
    return float3(0.5f, 0.4f, 0.32f) * (f * f * f);
}
)";

        const char *kSceneVSHLSL = R"(
struct VSIn { float3 position : POSITION; float3 normal : NORMAL; float3 color : COLOR; };
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
float4 main(VSOut input) : SV_TARGET
{
    // Lamp bulbs and the sunlit yard behind the window have no normal and are drawn unlit
    if (dot(input.normal, input.normal) < 0.01f)
        return float4(input.color, 1.0f);
    float3 diffuse, specular;
    shade(input.world, normalize(input.normal), diffuse, specular);
    return float4(input.color * diffuse + specular * gParams.x, 1.0f);
}
)";

        // Blend: dst = src0 * 1 + dst * src1, i.e. scattered light plus what the glass lets through
        const char *kWindowPSHLSL = R"(
struct PSOut
{
    float4 scatter : SV_Target0;
    float4 transmit : SV_Target1;
};
PSOut main(VSOut input)
{
    float3 diffuse, specular;
    shade(input.world, facing(input), diffuse, specular);
    float3 t = transmittance(input);
    PSOut o;
    o.scatter = float4(t * diffuse * gParams.y + specular * gParams.z, 1.0f);
    o.transmit = float4(t, 1.0f);
    return o;
}
)";

        const char *kWindowFallbackPSHLSL = R"(
float4 main(VSOut input) : SV_TARGET
{
    return float4(transmittance(input), 1.0f);
}
)";

        const char *kCaseGlassPSHLSL = R"(
float4 main(VSOut input) : SV_TARGET
{
    float3 n = facing(input);
    float3 diffuse, specular;
    shade(input.world, n, diffuse, specular);
    return float4(input.color * diffuse + specular * gParams.z + reflection(input, n), 1.0f);
}
)";

        const char *kCommonGL = R"(#version 430
layout(std140, binding = 0) uniform OITSceneParams
{
    mat4 gViewProj;
    vec4 gLightPos[4];
    vec4 gLightColor[4];
    vec4 gAmbient;
    vec4 gEye;
    vec4 gParams;
};
)";

        const char *kCommonPSGL = R"(
in vec3 vWorld;
in vec3 vNormal;
in vec3 vColor;
void shade(vec3 world, vec3 n, out vec3 diffuse, out vec3 specular)
{
    vec3 v = normalize(gEye.xyz - world);
    diffuse = gAmbient.rgb;
    specular = vec3(0.0);
    for (int i = 0; i < 4; ++i)
    {
        vec3 l = gLightPos[i].xyz - world;
        float dist = length(l);
        l /= dist;
        float atten = clamp(1.0 - dist / gLightPos[i].w, 0.0, 1.0);
        atten *= atten;
        float ndl = clamp(dot(n, l), 0.0, 1.0);
        diffuse += gLightColor[i].rgb * ndl * atten;
        float ndh = clamp(dot(n, normalize(l + v)), 0.0, 1.0);
        specular += gLightColor[i].rgb * pow(ndh, 48.0) * ndl * atten;
    }
}
vec3 facing()
{
    vec3 n = normalize(vNormal);
    return dot(n, gEye.xyz - vWorld) < 0.0 ? -n : n;
}
vec3 transmittance()
{
    float ripple = 0.95 + 0.05 * sin(vWorld.x * 41.0 + 1.5 * sin(vWorld.y * 7.0));
    return clamp(vColor * ripple, 0.0, 1.0);
}
vec3 reflection(vec3 n)
{
    float f = 1.0 - abs(dot(n, normalize(gEye.xyz - vWorld)));
    return vec3(0.5, 0.4, 0.32) * (f * f * f);
}
)";

        const char *kSceneVSGL = R"(
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

        const char *kScenePSGL = R"(
out vec4 fragColor;
void main()
{
    if (dot(vNormal, vNormal) < 0.01)
    {
        fragColor = vec4(vColor, 1.0);
        return;
    }
    vec3 diffuse, specular;
    shade(vWorld, normalize(vNormal), diffuse, specular);
    fragColor = vec4(vColor * diffuse + specular * gParams.x, 1.0);
}
)";

        const char *kWindowPSGL = R"(
layout(location = 0, index = 0) out vec4 fragScatter;
layout(location = 0, index = 1) out vec4 fragTransmit;
void main()
{
    vec3 diffuse, specular;
    shade(vWorld, facing(), diffuse, specular);
    vec3 t = transmittance();
    fragScatter = vec4(t * diffuse * gParams.y + specular * gParams.z, 1.0);
    fragTransmit = vec4(t, 1.0);
}
)";

        const char *kWindowFallbackPSGL = R"(
out vec4 fragColor;
void main()
{
    fragColor = vec4(transmittance(), 1.0);
}
)";

        const char *kCaseGlassPSGL = R"(
out vec4 fragColor;
void main()
{
    vec3 n = facing();
    vec3 diffuse, specular;
    shade(vWorld, n, diffuse, specular);
    fragColor = vec4(vColor * diffuse + specular * gParams.z + reflection(n), 1.0);
}
)";

        struct Source
        {
            const char *hlsl;
            const char *gl;
            SHADER_STAGE stage;
            const char *name;
        };

        const Source &lookup(OITShader id)
        {
            static const Source kSources[] =
            {
                { kSceneVSHLSL, kSceneVSGL, SHADER_STAGE::kVertex, "OITSceneVS" },
                { kScenePSHLSL, kScenePSGL, SHADER_STAGE::kPixel, "OITScenePS" },
                { kWindowPSHLSL, kWindowPSGL, SHADER_STAGE::kPixel, "OITWindowPS" },
                { kWindowFallbackPSHLSL, kWindowFallbackPSGL, SHADER_STAGE::kPixel, "OITWindowFallbackPS" },
                { kCaseGlassPSHLSL, kCaseGlassPSGL, SHADER_STAGE::kPixel, "OITCaseGlassPS" },
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

    ShaderVariantPtr createOITShader(OITShader id, ShaderPtr &dummyShader)
    {
        dummyShader = nullptr;

        RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
        if (renderer == nullptr)
        {
            return nullptr;
        }

        const Source &source = lookup(id);
        String code;
        const String &rendererName = renderer->getName();
        if (rendererName == RHIRenderer::DIRECT3D11)
        {
            code = String(kCommonHLSL) + source.hlsl;
        }
        else if (rendererName == RHIRenderer::OPENGL4)
        {
            code = String(kCommonGL) + (source.stage == SHADER_STAGE::kPixel ? kCommonPSGL : "") + source.gl;
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
            T3D_LOG_ERROR(LOG_TAG_APP, "OIT : failed to create dummy pass for %s", name.c_str());
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
            T3D_LOG_ERROR(LOG_TAG_APP, "OIT : failed to build %s ! ERROR [%d]", name.c_str(), ret);
            return nullptr;
        }

        dummyShader = shader;
        return variant;
    }
}
