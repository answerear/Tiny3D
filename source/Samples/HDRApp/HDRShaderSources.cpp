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


#include "HDRShaderSources.h"
#include "Kernel/T3DAgent.h"
#include "Material/T3DShaderKeyword.h"
#include "Material/T3DShaderVariant.h"
#include "RHI/T3DRHIRenderer.h"

#include <cstring>


#if defined(T3D_OS_WINDOWS)
extern const char *FORWARD_VERTEX_SHADER;
extern const char *FORWARD_PIXEL_SHADER;
extern const char *SHADOW_VERTEX_SHADER;
extern const char *POSTPROCESS_VERTEX_SHADER;
extern const char *HDR_EMISSIVE_VERTEX_SHADER;
extern const char *HDR_EMISSIVE_PIXEL_SHADER;
extern const char *HDR_BLOOM_PREFILTER_PIXEL_SHADER;
extern const char *HDR_BLOOM_DOWNSAMPLE_PIXEL_SHADER;
extern const char *HDR_BLOOM_UPSAMPLE_PIXEL_SHADER;
extern const char *HDR_TONEMAP_PIXEL_SHADER;

extern const char *FORWARD_VERTEX_SHADER_GL;
extern const char *FORWARD_PIXEL_SHADER_GL;
extern const char *SHADOW_VERTEX_SHADER_GL;
extern const char *POSTPROCESS_VERTEX_SHADER_GL;
extern const char *HDR_EMISSIVE_VERTEX_SHADER_GL;
extern const char *HDR_EMISSIVE_PIXEL_SHADER_GL;
extern const char *HDR_BLOOM_PREFILTER_PIXEL_SHADER_GL;
extern const char *HDR_BLOOM_DOWNSAMPLE_PIXEL_SHADER_GL;
extern const char *HDR_BLOOM_UPSAMPLE_PIXEL_SHADER_GL;
extern const char *HDR_TONEMAP_PIXEL_SHADER_GL;

extern const unsigned char FORWARD_VERTEX_SHADER_VK[];
extern const size_t FORWARD_VERTEX_SHADER_VK_SIZE;
extern const unsigned char FORWARD_PIXEL_SHADER_VK[];
extern const size_t FORWARD_PIXEL_SHADER_VK_SIZE;
extern const unsigned char SHADOW_VERTEX_SHADER_VK[];
extern const size_t SHADOW_VERTEX_SHADER_VK_SIZE;
extern const unsigned char POSTPROCESS_VERTEX_SHADER_VK[];
extern const size_t POSTPROCESS_VERTEX_SHADER_VK_SIZE;
extern const unsigned char HDR_EMISSIVE_VERTEX_SHADER_VK[];
extern const size_t HDR_EMISSIVE_VERTEX_SHADER_VK_SIZE;
extern const unsigned char HDR_EMISSIVE_PIXEL_SHADER_VK[];
extern const size_t HDR_EMISSIVE_PIXEL_SHADER_VK_SIZE;
extern const unsigned char HDR_BLOOM_PREFILTER_PIXEL_SHADER_VK[];
extern const size_t HDR_BLOOM_PREFILTER_PIXEL_SHADER_VK_SIZE;
extern const unsigned char HDR_BLOOM_DOWNSAMPLE_PIXEL_SHADER_VK[];
extern const size_t HDR_BLOOM_DOWNSAMPLE_PIXEL_SHADER_VK_SIZE;
extern const unsigned char HDR_BLOOM_UPSAMPLE_PIXEL_SHADER_VK[];
extern const size_t HDR_BLOOM_UPSAMPLE_PIXEL_SHADER_VK_SIZE;
extern const unsigned char HDR_TONEMAP_PIXEL_SHADER_VK[];
extern const size_t HDR_TONEMAP_PIXEL_SHADER_VK_SIZE;
#endif


namespace Tiny3D
{
#if defined(T3D_OS_WINDOWS)
    namespace
    {
        struct EmbeddedShader
        {
            const char *hlsl;
            const char *gl;
            const unsigned char *vk;
            const size_t *vkSize;
        };

        const EmbeddedShader &lookup(HDRShader id)
        {
            static const EmbeddedShader kShaders[] =
            {
                { FORWARD_VERTEX_SHADER, FORWARD_VERTEX_SHADER_GL, FORWARD_VERTEX_SHADER_VK, &FORWARD_VERTEX_SHADER_VK_SIZE },
                { FORWARD_PIXEL_SHADER, FORWARD_PIXEL_SHADER_GL, FORWARD_PIXEL_SHADER_VK, &FORWARD_PIXEL_SHADER_VK_SIZE },
                { SHADOW_VERTEX_SHADER, SHADOW_VERTEX_SHADER_GL, SHADOW_VERTEX_SHADER_VK, &SHADOW_VERTEX_SHADER_VK_SIZE },
                { HDR_EMISSIVE_VERTEX_SHADER, HDR_EMISSIVE_VERTEX_SHADER_GL, HDR_EMISSIVE_VERTEX_SHADER_VK, &HDR_EMISSIVE_VERTEX_SHADER_VK_SIZE },
                { HDR_EMISSIVE_PIXEL_SHADER, HDR_EMISSIVE_PIXEL_SHADER_GL, HDR_EMISSIVE_PIXEL_SHADER_VK, &HDR_EMISSIVE_PIXEL_SHADER_VK_SIZE },
                { POSTPROCESS_VERTEX_SHADER, POSTPROCESS_VERTEX_SHADER_GL, POSTPROCESS_VERTEX_SHADER_VK, &POSTPROCESS_VERTEX_SHADER_VK_SIZE },
                { HDR_BLOOM_PREFILTER_PIXEL_SHADER, HDR_BLOOM_PREFILTER_PIXEL_SHADER_GL, HDR_BLOOM_PREFILTER_PIXEL_SHADER_VK, &HDR_BLOOM_PREFILTER_PIXEL_SHADER_VK_SIZE },
                { HDR_BLOOM_DOWNSAMPLE_PIXEL_SHADER, HDR_BLOOM_DOWNSAMPLE_PIXEL_SHADER_GL, HDR_BLOOM_DOWNSAMPLE_PIXEL_SHADER_VK, &HDR_BLOOM_DOWNSAMPLE_PIXEL_SHADER_VK_SIZE },
                { HDR_BLOOM_UPSAMPLE_PIXEL_SHADER, HDR_BLOOM_UPSAMPLE_PIXEL_SHADER_GL, HDR_BLOOM_UPSAMPLE_PIXEL_SHADER_VK, &HDR_BLOOM_UPSAMPLE_PIXEL_SHADER_VK_SIZE },
                { HDR_TONEMAP_PIXEL_SHADER, HDR_TONEMAP_PIXEL_SHADER_GL, HDR_TONEMAP_PIXEL_SHADER_VK, &HDR_TONEMAP_PIXEL_SHADER_VK_SIZE },
            };
            return kShaders[static_cast<uint32_t>(id)];
        }
    }
#endif

    //--------------------------------------------------------------------------

    bool getHDRShaderCode(HDRShader id, const char *&code, size_t &size)
    {
        code = nullptr;
        size = 0;

        RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
        if (renderer == nullptr)
        {
            return false;
        }

#if defined(T3D_OS_WINDOWS)
        const EmbeddedShader &shader = lookup(id);
        const String &rendererName = renderer->getName();
        if (rendererName == RHIRenderer::VULKAN)
        {
            code = reinterpret_cast<const char *>(shader.vk);
            size = *shader.vkSize;
        }
        else if (rendererName == RHIRenderer::OPENGL4)
        {
            code = shader.gl;
            size = (code != nullptr) ? strlen(code) : 0;
        }
        else
        {
            code = shader.hlsl;
            size = (code != nullptr) ? strlen(code) : 0;
        }
#else
        (void)id;
#endif
        return code != nullptr && size > 0;
    }

    //--------------------------------------------------------------------------

    ShaderVariantPtr createHDRShaderVariant(HDRShader id, SHADER_STAGE stage)
    {
        const char *code = nullptr;
        size_t size = 0;
        if (!getHDRShaderCode(id, code, size))
        {
            return nullptr;
        }

        ShaderKeyword keyword;
        keyword.addKeyword("");
        keyword.generate();

        ShaderVariantPtr variant = ShaderVariant::create(std::move(keyword), code, size);
        variant->setShaderStage(stage);
        variant->setLanguage(T3D_AGENT.getActiveRHIRenderer()->getShadingLanguage());
        return variant;
    }
}
