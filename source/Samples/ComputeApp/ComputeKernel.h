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

#ifndef __COMPUTE_KERNEL_H__
#define __COMPUTE_KERNEL_H__


#include <Tiny3D.h>

#include <memory>


/**
 * \brief 一个 compute shader 的四语言嵌入源码
 * \remarks 与 PostProcessingApp 的 pickEmbeddedShader 同构，按 active 后端选一份。
 *          Vulkan 是 SPIR-V 二进制，所以单独带长度；其余三份是以 0 结尾的文本。
 */
struct ComputeShaderSource
{
    const char          *hlsl {nullptr};
    const char          *gl {nullptr};
    const char          *gles {nullptr};
    const unsigned char *vk {nullptr};
    size_t               vkSize {0};
};


class ComputeKernel;
using ComputeKernelPtr = std::unique_ptr<ComputeKernel>;


/**
 * \brief 一个 compute 内核：持有 ShaderVariant 与反射出的线程组尺寸
 * \remarks 绕开 Pass / Material —— Pass 不接受 compute 变体，ForwardRenderPipeline
 *          也没有 compute pass 插入点，所以 Sample 直接用 ShaderVariant，
 *          它的 compile() 已能分发到 RHIContext::createComputeShader。
 *          设计见 doc/todo/ComputeApp-Sample-Design-todo.md §3.2。
 */
class ComputeKernel
{
public:
    /**
     * \brief 按当前 active 后端挑源码、创建变体、编译并反射线程组尺寸
     * \param [in] name : 内核名，只用于日志与 dummy Pass 的 shader 名
     * \param [in] source : 四语言嵌入源码
     * \param [in] fallbackGroupX : GL 侧 reflect 拿不到 numthreads 时的回退值
     * \param [in] fallbackGroupY : 回退 Y
     * \param [in] fallbackGroupZ : 回退 Z
     * \return 成功返回内核对象；挑不到源码或编译失败时返回 nullptr
     * \remarks D3D11 compileShader 会解引用 getPass()->getTechnique()->getShader()，
     *          所以这里挂一个不进 Material 的 dummy Pass，绕开 Pass 对 compute 的拒绝。
     *          GL 后端目前不回填 thread group size，回退值让 dispatchThreads 仍能换算。
     */
    static ComputeKernelPtr create(const char *name, const ComputeShaderSource &source,
        uint32_t fallbackGroupX = 64, uint32_t fallbackGroupY = 1, uint32_t fallbackGroupZ = 1);

    /**
     * \brief 给独立 ShaderVariant 挂 dummy Pass，供 D3D11 compileShader 取名字
     * \param [in] variant : 目标变体
     * \param [in] name : dummy Shader 资源名
     * \return 成功返回 dummy Shader（必须由调用方持有，D3D11 会通过 Pass 解引用它）
     */
    static Tiny3D::ShaderPtr attachDummyPass(Tiny3D::ShaderVariant *variant, const char *name);

    /**
     * \brief 把「线程总数」换算成「线程组数」并派发，不足一组向上取整
     * \param [in] threadsX : X 维线程总数
     * \param [in] threadsY : Y 维线程总数
     * \param [in] threadsZ : Z 维线程总数
     * \return 透传 RHIContext::dispatch 的返回值
     * \remarks 换算用的是反射出来的 numthreads 而非调用点硬编码的常数，
     *          shader 改了线程组尺寸不用同步改 C++，同时顺带验证了反射（C7）。
     */
    TResult dispatchThreads(uint32_t threadsX, uint32_t threadsY = 1, uint32_t threadsZ = 1);

    /// 供 setComputeShader 使用
    Tiny3D::ShaderVariant *getVariant() const { return mVariant; }

    /// 反射出的 numthreads
    void getGroupSize(uint32_t &x, uint32_t &y, uint32_t &z) const
    {
        x = mGroupSize[0];
        y = mGroupSize[1];
        z = mGroupSize[2];
    }

    const String &getName() const { return mName; }

private:
    ComputeKernel() = default;

    String                      mName;
    Tiny3D::ShaderVariantPtr    mVariant {nullptr};
    /// dummy Shader 必须活过 compile，D3D11 会通过 Pass 解引用它
    Tiny3D::ShaderPtr           mDummyShader {nullptr};
    /// 来自 reflect() 的 numthreads；GL 回退到 create() 传入的值
    uint32_t                    mGroupSize[3] {1, 1, 1};
};


#endif  /*__COMPUTE_KERNEL_H__*/
