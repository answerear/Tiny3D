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


#ifndef __T3D_RHI_CAPABILITIES_H__
#define __T3D_RHI_CAPABILITIES_H__


#include "T3DPrerequisites.h"
#include "Kernel/T3DPixelFormat.h"


namespace Tiny3D
{
    /**
     * \brief 单个像素格式在当前后端上可用的用途位标志
     * \remarks 组合进 RHICapabilities::formatSupport。各位相互独立，例如
     *          整数格式通常有 kFormatSupportRenderTarget 而没有 kFormatSupportBlend。
     */
    enum PixelFormatSupportFlags : uint32_t
    {
        kFormatSupportNone              = 0,
        /// 可创建 2D 纹理并在着色器中读取（Load / Sample）
        kFormatSupportTexture           = (1 << 0),
        /// 采样时支持线性过滤；没有该位时只能用点采样或 Load
        kFormatSupportLinearFilter      = (1 << 1),
        /// 可作为颜色渲染目标
        kFormatSupportRenderTarget      = (1 << 2),
        /// 作为颜色渲染目标时支持混合
        kFormatSupportBlend             = (1 << 3),
        /// 可作为多重采样渲染目标；颜色格式还要求能 Resolve 到单采样纹理
        kFormatSupportMultisample       = (1 << 4),
        /// 可作为深度模板目标
        kFormatSupportDepthStencil      = (1 << 5),
        /// 纹理可建 UAV 并在 compute 中写入
        kFormatSupportUnorderedAccess   = (1 << 6),
        /// 可作为 StructuredBufferKind::kTyped 缓冲的元素格式
        kFormatSupportTypedBuffer       = (1 << 7),
    };

    /**
     * \brief 后端能力查询结果
     * \remarks 由各后端在 init() 中填充；上层据此决定走 GPU 路径还是 CPU 降级路径。
     *          所有能力位默认为 false / 0，后端漏填时上层只会走降级路径，
     *          损失性能但不会产生错误结果。
     *
     *          **降级契约**：上层在使用任何 GPU-driven 路径前必须先查能力位。
     *          能力位为 false 时，对应的 RHIContext 接口是 T3D_RHI_UNSUPPORTED
     *          规约 stub，会返回 T3D_ERR_NOT_IMPLEMENT 并打警告日志。
     *          二者由该宏内的断言强制保持一致，不会出现「能力位说支持但接口是空壳」
     *          的矛盾状态。
     */
    struct T3D_ENGINE_API RHICapabilities
    {
        /// 支持 compute shader 与 dispatch
        bool        supportsCompute {false};
        /// 支持 UAV / storage image / SSBO
        bool        supportsUnorderedAccess {false};
        /// 支持结构化缓冲（StructuredBuffer / SSBO）
        bool        supportsStructuredBuffer {false};
        /// 支持实例化绘制
        bool        supportsInstancing {false};
        /// 支持实例化绘制的非零 startInstance（GL 需 4.2+，GLES 不支持）
        bool        supportsBaseInstance {false};
        /// 支持 indirect draw
        bool        supportsIndirectDraw {false};
        /// 支持 indirect dispatch
        bool        supportsIndirectDispatch {false};
        /// 支持 Append/Consume 缓冲与隐藏计数器
        bool        supportsAppendConsumeBuffer {false};
        /// 支持 GPU→CPU 读回（map / unmap）
        bool        supportsReadback {false};

        /// 每维最大线程组数
        uint32_t    maxDispatchGroupCount[3] {0, 0, 0};
        /// 每维最大线程组内线程数
        uint32_t    maxComputeGroupSize[3] {0, 0, 0};
        /// 单个线程组可用共享内存字节数
        uint32_t    maxComputeSharedMemory {0};
        /// 可同时绑定的 UAV 槽位数
        uint32_t    maxUnorderedAccessSlots {0};
        /// 纹理 UAV 与 Typed 结构化缓冲 UAV 可用的槽位数，槽号须小于该值；
        /// 不超过 maxUnorderedAccessSlots（GL 受 image unit 数限制，可能更小）
        uint32_t    maxImageUnorderedAccessSlots {0};
        /// 带计数器或 Append/Consume 的结构化缓冲 UAV 可用的槽位数，槽号须小于该值；
        /// 不超过 maxUnorderedAccessSlots（GL 受 atomic counter 绑定点数限制，可能更小）
        uint32_t    maxCounterUnorderedAccessSlots {0};

        /// 各像素格式可用的用途，按 PixelFormat 下标，取值为 PixelFormatSupportFlags 的组合
        uint32_t    formatSupport[kPixelFormatCount] {};

        uint32_t getFormatSupport(PixelFormat format) const
        {
            const uint32_t index = static_cast<uint32_t>(format);
            return (index < kPixelFormatCount) ? formatSupport[index] : kFormatSupportNone;
        }

        /// flags 中的位必须全部支持才返回 true
        bool supportsFormat(PixelFormat format, uint32_t flags) const
        {
            return (getFormatSupport(format) & flags) == flags;
        }

        /**
         * \brief 按 formatSupport 校验渲染纹理的格式，不支持时打错误日志
         * \param [in] tag : 日志 tag
         * \param [in] func : 调用方函数名，用于日志
         * \param [in] format : 渲染纹理格式
         * \param [in] sampleCount : 采样数，0 和 1 都表示不开 MSAA
         * \param [in] wantsUAV : 是否请求了 kGPUUnorderedAccess；MSAA 时不校验，由后端忽略该请求
         */
        bool checkRenderTextureFormat(const char *tag, const char *func, PixelFormat format,
            uint32_t sampleCount, bool wantsUAV) const;

        /**
         * \brief 校验块压缩纹理的创建参数，非压缩格式直接返回 true
         * \param [in] tag : 日志 tag
         * \param [in] func : 调用方函数名，用于日志
         * \param [in] format : 纹理格式
         * \param [in] dimension : 1 / 2 / 3，Cubemap 与 2D 数组按 2 传
         * \param [in] width : mip 0 宽度
         * \param [in] height : mip 0 高度
         * \param [in] sampleCount : 采样数
         * \param [in] wantsUAV : 是否请求了 kGPUUnorderedAccess
         * \remarks 规则按各后端的交集定：只能是 2D / 2D 数组 / Cubemap（D3D11 没有 1D 压缩，
         *          GL 不保证 3D 的 S3TC），mip 0 宽高必须是 4 的倍数（D3D11 的硬性要求），
         *          不能 MSAA，不能 UAV，并且设备要报告 kFormatSupportTexture
         */
        bool checkCompressedTextureFormat(const char *tag, const char *func, PixelFormat format,
            uint32_t dimension, uint32_t width, uint32_t height, uint32_t sampleCount, bool wantsUAV) const;

        /// 把 formatSupport 逐格式打到日志，后端填完能力位后调用
        void logFormatSupport(const char *tag) const;
    };
}


#endif  /*__T3D_RHI_CAPABILITIES_H__*/
