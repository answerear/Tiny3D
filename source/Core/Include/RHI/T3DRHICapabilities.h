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


namespace Tiny3D
{
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

        /// 每维最大线程组数
        uint32_t    maxDispatchGroupCount[3] {0, 0, 0};
        /// 每维最大线程组内线程数
        uint32_t    maxComputeGroupSize[3] {0, 0, 0};
        /// 单个线程组可用共享内存字节数
        uint32_t    maxComputeSharedMemory {0};
        /// 可同时绑定的 UAV 槽位数
        uint32_t    maxUnorderedAccessSlots {0};
    };
}


#endif  /*__T3D_RHI_CAPABILITIES_H__*/
