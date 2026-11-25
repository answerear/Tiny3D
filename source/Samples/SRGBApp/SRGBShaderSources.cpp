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


#include "SRGBShaderSources.h"
#include "Kernel/T3DAgent.h"
#include "Material/T3DShaderKeyword.h"
#include "Material/T3DShaderVariant.h"
#include "RHI/T3DRHIRenderer.h"

#include <cstring>


#if defined(T3D_OS_WINDOWS)
extern const char *POSTPROCESS_VERTEX_SHADER;
extern const char *SRGB_PATTERN_PIXEL_SHADER;
extern const char *SRGB_COMPOSITE_PIXEL_SHADER;

extern const char *POSTPROCESS_VERTEX_SHADER_GL;
extern const char *SRGB_PATTERN_PIXEL_SHADER_GL;
extern const char *SRGB_COMPOSITE_PIXEL_SHADER_GL;

extern const unsigned char POSTPROCESS_VERTEX_SHADER_VK[];
extern const size_t POSTPROCESS_VERTEX_SHADER_VK_SIZE;
extern const unsigned char SRGB_PATTERN_PIXEL_SHADER_VK[];
extern const size_t SRGB_PATTERN_PIXEL_SHADER_VK_SIZE;
extern const unsigned char SRGB_COMPOSITE_PIXEL_SHADER_VK[];
extern const size_t SRGB_COMPOSITE_PIXEL_SHADER_VK_SIZE;
#endif


namespace Tiny3D
{
    namespace
    {
        bool getSRGBShaderCode(SRGBShader id, const char *&code, size_t &size)
        {
            code = nullptr;
            size = 0;

            RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
            if (renderer == nullptr)
            {
                return false;
            }

#if defined(T3D_OS_WINDOWS)
            struct EmbeddedShader
            {
                const char *hlsl;
                const char *gl;
                const unsigned char *vk;
                const size_t *vkSize;
            };

            static const EmbeddedShader kShaders[] =
            {
                { POSTPROCESS_VERTEX_SHADER, POSTPROCESS_VERTEX_SHADER_GL, POSTPROCESS_VERTEX_SHADER_VK, &POSTPROCESS_VERTEX_SHADER_VK_SIZE },
                { SRGB_PATTERN_PIXEL_SHADER, SRGB_PATTERN_PIXEL_SHADER_GL, SRGB_PATTERN_PIXEL_SHADER_VK, &SRGB_PATTERN_PIXEL_SHADER_VK_SIZE },
                { SRGB_COMPOSITE_PIXEL_SHADER, SRGB_COMPOSITE_PIXEL_SHADER_GL, SRGB_COMPOSITE_PIXEL_SHADER_VK, &SRGB_COMPOSITE_PIXEL_SHADER_VK_SIZE },
            };

            const EmbeddedShader &shader = kShaders[static_cast<uint32_t>(id)];
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
    }

    //--------------------------------------------------------------------------

    ShaderVariantPtr createSRGBShaderVariant(SRGBShader id, SHADER_STAGE stage)
    {
        const char *code = nullptr;
        size_t size = 0;
        if (!getSRGBShaderCode(id, code, size))
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
