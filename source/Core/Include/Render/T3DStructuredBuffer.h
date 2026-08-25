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


#ifndef __T3D_STRUCTURED_BUFFER_H__
#define __T3D_STRUCTURED_BUFFER_H__


#include "Render/T3DRenderBuffer.h"
#include "Render/T3DRenderBufferDesc.h"


namespace Tiny3D
{
    /**
     * \brief 结构化缓冲渲染资源，compute shader 最常用的 I/O 载体
     * \remarks onLoad 时创建 RHI 结构化缓冲。是否创建 SRV / UAV 由 getGPUAccess()
     *          决定，是否可作为 indirect 参数缓冲由其 kGPUIndirectArgs 位决定。
     */
    class T3D_ENGINE_API StructuredBuffer : public RenderBuffer
    {
    public:
        /**
         * \brief 创建结构化缓冲对象（尚未创建底层 RHI 资源，需经 RenderResourceManager 加载）
         * \param [in] desc : 结构化缓冲描述
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途；带 kGPUUnorderedAccess 时必须为 Usage::kStatic
         * \param [in] accMode : CPU 访问模式（CPUAccessMode 组合）
         * \param [in] gpuAccess : GPU 侧附加访问权限（GPUAccessFlags 组合）
         * \return 描述合法时返回新建的 StructuredBuffer；否则打错误日志并返回 nullptr
         */
        static StructuredBufferPtr create(const StructuredBufferDesc &desc, MemoryType memType,
            Usage usage, uint32_t accMode, uint32_t gpuAccess);

        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kStructuredBuffer
         */
        Type getType() const override;

        /// 结构化缓冲描述
        const StructuredBufferDesc &getDescriptor() const { return mDesc; }

        /// 元素个数
        uint32_t getElementCount() const { return mDesc.elementCount; }

        /**
         * \brief 单个元素的字节数
         * \return kByteAddress 形态固定返回 4，其余形态返回 desc 中的 elementSize
         */
        uint32_t getElementSize() const { return mDesc.elementSize; }

        /// 是否带隐藏计数器或 Append/Consume 计数（copyStructureCount 的前置条件）
        bool hasUAVCounter() const { return mDesc.hasCounter || mDesc.isAppendConsume; }

        /**
         * \brief GPU 侧字节大小
         * \return elementSize * elementCount，与初始 CPU 数据是否存在无关
         */
        size_t getGPUSizeInBytes() const override
        {
            return (size_t)mDesc.elementSize * mDesc.elementCount;
        }

    protected:
        /**
         * \brief 构造结构化缓冲，保存已规范化的描述
         * \param [in] desc : 已通过 validateDescriptor 校验并规范化的描述
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] gpuAccess : GPU 侧附加访问权限
         */
        StructuredBuffer(const StructuredBufferDesc &desc, MemoryType memType, Usage usage,
            uint32_t accMode, uint32_t gpuAccess);

        ~StructuredBuffer() override = default;

        /**
         * \brief 校验描述并就地规范化（补齐 kByteAddress 的 elementSize 等）
         * \param [in,out] desc : 待校验的描述
         * \param [in] gpuAccess : GPU 侧附加访问权限
         * \return 描述合法返回 true；否则打错误日志并返回 false
         */
        static bool validateDescriptor(StructuredBufferDesc &desc, uint32_t gpuAccess);

        /**
         * \brief 通过活动 RHI 上下文创建 RHIStructuredBuffer
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 结构化缓冲引用
         * \return 始终返回 true
         */
        bool onUnload() override;

        /// 结构化缓冲描述
        StructuredBufferDesc    mDesc {};
    };
}


#endif  /*__T3D_STRUCTURED_BUFFER_H__*/
