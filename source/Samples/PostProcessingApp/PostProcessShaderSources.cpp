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


#include "PostProcessShaderSources.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIRenderer.h"

#include <cstring>


#if defined(T3D_OS_WINDOWS)
extern const char *POSTPROCESS_VERTEX_SHADER;
extern const char *GRAYSCALE_PIXEL_SHADER;
extern const char *INVERT_PIXEL_SHADER;
extern const char *TINT_PIXEL_SHADER;
extern const char *POSTPROCESS_VERTEX_SHADER_GL;
extern const char *GRAYSCALE_PIXEL_SHADER_GL;
extern const char *INVERT_PIXEL_SHADER_GL;
extern const char *TINT_PIXEL_SHADER_GL;
#endif
#if defined(T3D_OS_WINDOWS) || defined(T3D_OS_ANDROID)
extern const unsigned char POSTPROCESS_VERTEX_SHADER_VK[];
extern const size_t POSTPROCESS_VERTEX_SHADER_VK_SIZE;
extern const unsigned char GRAYSCALE_PIXEL_SHADER_VK[];
extern const size_t GRAYSCALE_PIXEL_SHADER_VK_SIZE;
extern const unsigned char INVERT_PIXEL_SHADER_VK[];
extern const size_t INVERT_PIXEL_SHADER_VK_SIZE;
extern const unsigned char TINT_PIXEL_SHADER_VK[];
extern const size_t TINT_PIXEL_SHADER_VK_SIZE;
#endif
#if defined(T3D_OS_ANDROID)
extern const char *POSTPROCESS_VERTEX_SHADER_GLES;
extern const char *GRAYSCALE_PIXEL_SHADER_GLES;
extern const char *INVERT_PIXEL_SHADER_GLES;
extern const char *TINT_PIXEL_SHADER_GLES;
#endif


namespace Tiny3D
{
    namespace
    {
        bool pickEmbeddedShader(const char *hlsl, const char *gl, const char *gles,
                                const unsigned char *vk, size_t vkSize,
                                const char *&code, size_t &size)
        {
            RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
            if (renderer == nullptr)
            {
                return false;
            }

            const String &rendererName = renderer->getName();

#if defined(T3D_OS_WINDOWS)
            if (rendererName == RHIRenderer::VULKAN)
            {
                code = reinterpret_cast<const char *>(vk);
                size = vkSize;
            }
            else if (rendererName == RHIRenderer::OPENGL4)
            {
                code = gl;
                size = (gl != nullptr) ? strlen(gl) : 0;
            }
            else
            {
                code = hlsl;
                size = (hlsl != nullptr) ? strlen(hlsl) : 0;
            }
#elif defined(T3D_OS_ANDROID)
            if (rendererName == RHIRenderer::VULKAN)
            {
                code = reinterpret_cast<const char *>(vk);
                size = vkSize;
            }
            else
            {
                code = gles;
                size = (gles != nullptr) ? strlen(gles) : 0;
            }
#else
            (void)hlsl;
            (void)gl;
            (void)gles;
            (void)vk;
            (void)vkSize;
            (void)rendererName;
            code = nullptr;
            size = 0;
#endif
            return (code != nullptr && size > 0);
        }

        bool pickPair(const char *vsHLSL, const char *vsGL, const char *vsGLES,
                      const unsigned char *vsVK, size_t vsVKSize,
                      const char *psHLSL, const char *psGL, const char *psGLES,
                      const unsigned char *psVK, size_t psVKSize,
                      const char *&vsCode, size_t &vsSize,
                      const char *&psCode, size_t &psSize)
        {
            return pickEmbeddedShader(vsHLSL, vsGL, vsGLES, vsVK, vsVKSize, vsCode, vsSize)
                && pickEmbeddedShader(psHLSL, psGL, psGLES, psVK, psVKSize, psCode, psSize);
        }
    }

    //--------------------------------------------------------------------------

    bool getGrayscaleEffectShader(const char *&vsCode, size_t &vsSize,
                                  const char *&psCode, size_t &psSize)
    {
#if defined(T3D_OS_WINDOWS)
        return pickPair(POSTPROCESS_VERTEX_SHADER, POSTPROCESS_VERTEX_SHADER_GL, nullptr,
                        POSTPROCESS_VERTEX_SHADER_VK, POSTPROCESS_VERTEX_SHADER_VK_SIZE,
                        GRAYSCALE_PIXEL_SHADER, GRAYSCALE_PIXEL_SHADER_GL, nullptr,
                        GRAYSCALE_PIXEL_SHADER_VK, GRAYSCALE_PIXEL_SHADER_VK_SIZE,
                        vsCode, vsSize, psCode, psSize);
#elif defined(T3D_OS_ANDROID)
        return pickPair(nullptr, nullptr, POSTPROCESS_VERTEX_SHADER_GLES,
                        POSTPROCESS_VERTEX_SHADER_VK, POSTPROCESS_VERTEX_SHADER_VK_SIZE,
                        nullptr, nullptr, GRAYSCALE_PIXEL_SHADER_GLES,
                        GRAYSCALE_PIXEL_SHADER_VK, GRAYSCALE_PIXEL_SHADER_VK_SIZE,
                        vsCode, vsSize, psCode, psSize);
#else
        vsCode = nullptr;
        vsSize = 0;
        psCode = nullptr;
        psSize = 0;
        return false;
#endif
    }

    //--------------------------------------------------------------------------

    bool getInvertEffectShader(const char *&vsCode, size_t &vsSize,
                               const char *&psCode, size_t &psSize)
    {
#if defined(T3D_OS_WINDOWS)
        return pickPair(POSTPROCESS_VERTEX_SHADER, POSTPROCESS_VERTEX_SHADER_GL, nullptr,
                        POSTPROCESS_VERTEX_SHADER_VK, POSTPROCESS_VERTEX_SHADER_VK_SIZE,
                        INVERT_PIXEL_SHADER, INVERT_PIXEL_SHADER_GL, nullptr,
                        INVERT_PIXEL_SHADER_VK, INVERT_PIXEL_SHADER_VK_SIZE,
                        vsCode, vsSize, psCode, psSize);
#elif defined(T3D_OS_ANDROID)
        return pickPair(nullptr, nullptr, POSTPROCESS_VERTEX_SHADER_GLES,
                        POSTPROCESS_VERTEX_SHADER_VK, POSTPROCESS_VERTEX_SHADER_VK_SIZE,
                        nullptr, nullptr, INVERT_PIXEL_SHADER_GLES,
                        INVERT_PIXEL_SHADER_VK, INVERT_PIXEL_SHADER_VK_SIZE,
                        vsCode, vsSize, psCode, psSize);
#else
        vsCode = nullptr;
        vsSize = 0;
        psCode = nullptr;
        psSize = 0;
        return false;
#endif
    }

    //--------------------------------------------------------------------------

    bool getTintEffectShader(const char *&vsCode, size_t &vsSize,
                             const char *&psCode, size_t &psSize)
    {
#if defined(T3D_OS_WINDOWS)
        return pickPair(POSTPROCESS_VERTEX_SHADER, POSTPROCESS_VERTEX_SHADER_GL, nullptr,
                        POSTPROCESS_VERTEX_SHADER_VK, POSTPROCESS_VERTEX_SHADER_VK_SIZE,
                        TINT_PIXEL_SHADER, TINT_PIXEL_SHADER_GL, nullptr,
                        TINT_PIXEL_SHADER_VK, TINT_PIXEL_SHADER_VK_SIZE,
                        vsCode, vsSize, psCode, psSize);
#elif defined(T3D_OS_ANDROID)
        return pickPair(nullptr, nullptr, POSTPROCESS_VERTEX_SHADER_GLES,
                        POSTPROCESS_VERTEX_SHADER_VK, POSTPROCESS_VERTEX_SHADER_VK_SIZE,
                        nullptr, nullptr, TINT_PIXEL_SHADER_GLES,
                        TINT_PIXEL_SHADER_VK, TINT_PIXEL_SHADER_VK_SIZE,
                        vsCode, vsSize, psCode, psSize);
#else
        vsCode = nullptr;
        vsSize = 0;
        psCode = nullptr;
        psSize = 0;
        return false;
#endif
    }
}
