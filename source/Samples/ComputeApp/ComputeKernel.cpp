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

#include "ComputeKernel.h"
#include "../Common/SampleApp.h"

#include <cstring>
#include <string>


using namespace Tiny3D;


namespace
{
    /**
     * \brief 每个 dummy Shader 都要一个全局唯一的资源名
     * \remarks ShaderManager::createShader 命中同名缓存时返回的是**已存在**的 Shader，
     *          此时 Shader::addTechnique 遇到重名 Technique 会直接返回 false 且不调
     *          setShader()，Technique 的 shader 指针留空。D3D11 compileShader 随后
     *          解引用 getPass()->getTechnique()->getShader() 就是访问违例。
     *          同一内核被两个用例各建一次（K1 与 K7 都用 ComputeLinearWrite）就会踩到。
     */
    uint32_t nextDummyId()
    {
        static uint32_t counter = 0;
        return ++counter;
    }

    bool pickSource(const ComputeShaderSource &source, const char *&code, size_t &size)
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
            code = reinterpret_cast<const char *>(source.vk);
            size = source.vkSize;
        }
        else if (rendererName == RHIRenderer::OPENGL4)
        {
            code = source.gl;
            size = (source.gl != nullptr) ? strlen(source.gl) : 0;
        }
        else
        {
            code = source.hlsl;
            size = (source.hlsl != nullptr) ? strlen(source.hlsl) : 0;
        }
#elif defined(T3D_OS_ANDROID)
        if (rendererName == RHIRenderer::VULKAN)
        {
            code = reinterpret_cast<const char *>(source.vk);
            size = source.vkSize;
        }
        else
        {
            code = source.gles;
            size = (source.gles != nullptr) ? strlen(source.gles) : 0;
        }
#else
        (void)rendererName;
        code = nullptr;
        size = 0;
#endif

        return (code != nullptr && size > 0);
    }
}


ShaderPtr ComputeKernel::attachDummyPass(ShaderVariant *variant, const char *name)
{
    if (variant == nullptr)
    {
        return nullptr;
    }

    const String suffix = std::to_string(nextDummyId());
    ShaderPtr shader = T3D_SHADER_MGR.createShader(String("ComputeDummy-") + name + "-" + suffix);
    if (shader == nullptr)
    {
        return nullptr;
    }

    TechniquePtr tech = Technique::create("ComputeDummy-" + suffix);
    PassPtr pass = Pass::create("ComputeDummy-" + suffix);
    if (tech == nullptr || pass == nullptr)
    {
        return nullptr;
    }

    if (!tech->addPass(pass) || !shader->addTechnique(tech))
    {
        return nullptr;
    }

    variant->setPass(pass);
    return shader;
}

ComputeKernelPtr ComputeKernel::create(const char *name, const ComputeShaderSource &source,
    uint32_t fallbackGroupX, uint32_t fallbackGroupY, uint32_t fallbackGroupZ)
{
    const char *code = nullptr;
    size_t codeSize = 0;

    if (!pickSource(source, code, codeSize))
    {
        APP_LOG_DEBUG("[kernel] no embedded source for kernel [%s] on this backend !", name);
        return nullptr;
    }

    RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();

    ShaderKeyword keyword;
    keyword.addKeyword("");
    keyword.generate();

    ShaderVariantPtr variant = ShaderVariant::create(std::move(keyword), code, codeSize);
    if (variant == nullptr)
    {
        APP_LOG_DEBUG("[kernel] failed to create shader variant for kernel [%s] !", name);
        return nullptr;
    }

    variant->setShaderStage(SHADER_STAGE::kCompute);
    variant->setLanguage(renderer->getShadingLanguage());

    // Pass 不接受 kCompute，但 D3D11 compileShader 又要沿 Pass 取 Shader 名，
    // 所以挂一个不进 Material 的 dummy Pass。挂不上就别往下编，否则后端空指针解引用。
    ShaderPtr dummyShader = attachDummyPass(variant.get(), name);
    if (dummyShader == nullptr)
    {
        APP_LOG_DEBUG("[kernel] failed to attach dummy pass for kernel [%s] !", name);
        return nullptr;
    }

    TResult ret = variant->compile();
    if (T3D_FAILED(ret))
    {
        APP_LOG_DEBUG("[kernel] failed to compile kernel [%s] ! ERROR [%d]", name, ret);
        return nullptr;
    }

    // 必须先 compile 再 reflect：reflect() 见到未编译状态时会在反射完成后调
    // invalidateRHI() 把刚建好的 RHIShader 丢掉（T3DShaderVariant.cpp:192-196）。
    ret = variant->reflect();
    if (T3D_FAILED(ret))
    {
        APP_LOG_DEBUG("[kernel] failed to reflect kernel [%s] ! ERROR [%d]", name, ret);
        return nullptr;
    }

    uint32_t gx = 0, gy = 0, gz = 0;
    variant->getThreadGroupSize(gx, gy, gz);
    if (gx == 0 || gy == 0 || gz == 0)
    {
        APP_LOG_DEBUG("[kernel] kernel [%s] reflection has no numthreads, fallback to (%u, %u, %u)",
            name, fallbackGroupX, fallbackGroupY, fallbackGroupZ);
        gx = fallbackGroupX;
        gy = fallbackGroupY;
        gz = fallbackGroupZ;
    }

    if (gx == 0 || gy == 0 || gz == 0)
    {
        APP_LOG_DEBUG("[kernel] kernel [%s] still has zero numthreads !", name);
        return nullptr;
    }

    ComputeKernelPtr kernel(new ComputeKernel());
    kernel->mName = name;
    kernel->mVariant = variant;
    kernel->mDummyShader = dummyShader;
    kernel->mGroupSize[0] = gx;
    kernel->mGroupSize[1] = gy;
    kernel->mGroupSize[2] = gz;

    APP_LOG_DEBUG("[kernel] kernel [%s] ready, numthreads=(%u, %u, %u)", name, gx, gy, gz);

    return kernel;
}

TResult ComputeKernel::dispatchThreads(uint32_t threadsX, uint32_t threadsY, uint32_t threadsZ)
{
    RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
    if (ctx == nullptr)
    {
        return T3D_ERR_SYS_NOT_INIT;
    }

    const uint32_t groupsX = (threadsX + mGroupSize[0] - 1) / mGroupSize[0];
    const uint32_t groupsY = (threadsY + mGroupSize[1] - 1) / mGroupSize[1];
    const uint32_t groupsZ = (threadsZ + mGroupSize[2] - 1) / mGroupSize[2];

    return ctx->dispatch(groupsX, groupsY, groupsZ);
}
