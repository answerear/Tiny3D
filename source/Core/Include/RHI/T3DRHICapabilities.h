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
#include "RHI/T3DRHIQuery.h"


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
        /// SRV 绑定可以只选纹理的部分 mip / 层（TextureSubresource）。
        /// 不支持时 SRV 的子资源参数会被忽略并打 warning，仍绑定整张纹理；
        /// 渲染目标附件与 UAV 的子资源不受此位影响
        bool        supportsTextureSubresourceViews {false};
        /// 渲染目标可以一次绑定数组 / Cube 的多层，由 GS 输出 SV_RenderTargetArrayIndex 选层
        bool        supportsLayeredRendering {false};
        /// 渲染目标的深度/模板附件可以只读（RenderTarget::setDepthStencilReadOnly），
        /// 同一张深度纹理可以边做深度测试边作为 SRV 采样
        bool        supportsReadOnlyDepthStencil {false};
        /// 支持 QueryType::kOcclusion（样本计数）。GLES 只有布尔遮挡查询，这一位为 false
        bool        supportsOcclusionQuery {false};
        /// 支持 QueryType::kBinaryOcclusion（是否有样本通过）
        bool        supportsBinaryOcclusionQuery {false};
        /// 支持 QueryType::kTimestamp。GLES 需要 EXT_disjoint_timer_query
        bool        supportsTimestampQuery {false};
        /// 支持 QueryType::kPipelineStatistics。GL 需要 4.6 或 ARB_pipeline_statistics_query，
        /// Vulkan 需要设备特性 pipelineStatisticsQuery
        bool        supportsPipelineStatisticsQuery {false};
        /// 支持 RHIContext::setPredication（按布尔遮挡查询的结果跳过绘制）
        bool        supportsPredication {false};
        /// 调试分组、调试标记与资源名会转发到图形 API。为 false 时这些接口是空操作，不影响渲染结果
        bool        supportsDebugAnnotations {false};
        /// 混合因子可以用 BlendFactor::kSrc1Color 等双源因子。GLES 需要 EXT_blend_func_extended，
        /// Vulkan 需要设备特性 dualSrcBlend
        bool        supportsDualSourceBlend {false};

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

        /**
         * \brief 校验渲染纹理的维度、层数与 mip 数，不合法时打错误日志
         * \param [in] tag : 日志 tag
         * \param [in] func : 调用方函数名，用于日志
         * \param [in] desc : 渲染纹理描述
         * \param [in] gpuAccess : 渲染纹理的 GPUAccessFlags
         * \remarks 规则按各后端的交集定：Cube 要求宽高相等、arraySize 是 6 的倍数；
         *          MSAA 只能是单层单 mip 的 2D（D3D11 的 Cube 不能 MSAA，多 mip 的 MSAA 纹理也不存在）；
         *          mipmaps 不能超过完整 mip 链；带 kGPUGenerateMips 时格式须可作渲染目标并支持线性过滤，
         *          深度格式不能带
         */
        bool checkRenderTextureLayout(const char *tag, const char *func, const PixelBuffer2DDesc &desc,
            uint32_t gpuAccess) const;

        /**
         * \brief 校验 generateMips 的目标，不合法时打错误日志
         * \param [in] tag : 日志 tag
         * \param [in] func : 调用方函数名，用于日志
         * \param [in] buffer : 目标像素缓冲
         * \return 目标是带 kGPUGenerateMips、mipmaps > 1 且已创建 RHI 资源的渲染纹理时返回 true
         */
        bool checkGenerateMips(const char *tag, const char *func, PixelBuffer *buffer) const;

        /**
         * \brief 校验渲染目标的只读深度/模板设置，不合法时打错误日志
         * \param [in] tag : 日志 tag
         * \param [in] func : 调用方函数名，用于日志
         * \param [in] renderTarget : 要绑定的渲染目标
         * \return 没有只读位，或后端支持且有外挂的深度格式纹理时返回 true
         */
        bool checkReadOnlyDepthStencil(const char *tag, const char *func, const RenderTarget *renderTarget) const;

        /**
         * \brief 从要清除的部分里去掉渲染目标只读的部分，有去掉时打警告日志
         * \param [in] tag : 日志 tag
         * \param [in] func : 调用方函数名，用于日志
         * \param [in] renderTarget : 当前渲染目标，可为 nullptr
         * \param [in] parts : 要清除的部分，kReadOnlyDepth 表示深度、kReadOnlyStencil 表示模板
         * \return 仍然可以清除的部分，编码同 parts
         */
        static uint32_t filterDepthStencilClear(const char *tag, const char *func, const RenderTarget *renderTarget,
            uint32_t parts);

        /**
         * \brief 校验混合状态描述，不合法时打错误日志
         * \param [in] tag : 日志 tag
         * \param [in] func : 调用方函数名，用于日志
         * \param [in] desc : 混合状态描述
         * \return 没有用双源因子，或后端支持且只有 RT 0 用到时返回 true
         * \remarks 规则按各后端的交集定：双源因子只能出现在 RT 0 上（D3D11 与 GL 的双源混合都只有
         *          一个输出槽）。用到双源因子时，渲染目标也只能有一个颜色附件，这一条在绑定时才能确定，
         *          由调用方保证
         */
        bool checkBlendDesc(const char *tag, const char *func, const BlendDesc &desc) const;

        /// 是否支持某种查询类型
        bool supportsQuery(QueryType type) const;

        /**
         * \brief 校验 createQuery 的类型，不支持时打警告日志
         * \param [in] tag : 日志 tag
         * \param [in] type : 查询类型
         * \return 支持时返回 true
         */
        bool checkQueryType(const char *tag, QueryType type) const;

        /// 把 formatSupport 逐格式打到日志，后端填完能力位后调用
        void logFormatSupport(const char *tag) const;
    };
}


#endif  /*__T3D_RHI_CAPABILITIES_H__*/
