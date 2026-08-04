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


#ifndef __T3D_INDEX_BUFFER_H__
#define __T3D_INDEX_BUFFER_H__


#include "Render/T3DRenderBuffer.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief GPU 索引缓冲区，保存图元索引数据
     */
    class T3D_ENGINE_API IndexBuffer : public RenderBuffer
    {
    public:
        /**
         * \brief 创建索引缓冲区对象（尚未创建底层 RHI 资源，需经 RenderResourceManager 加载）
         * \param [in] idxType : 索引位宽（16 位或 32 位）
         * \param [in] idxCount : 索引个数
         * \param [in] buffer : 初始 CPU 索引数据；kBoth 时会拷贝一份，kVRAM 时直接接管 Data 指针
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式（CPUAccessMode 组合）
         * \return 新建的 IndexBuffer 智能指针
         */
        static IndexBufferPtr create(IndexType idxType, uint32_t idxCount, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode);

        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kIndexBuffer
         */
        Type getType() const override;

        /// 索引位宽类型
        IndexType getIndexType() const { return mIndexType; }

        /// 索引个数
        uint32_t getIndexCount() const { return mIndexCount; }

        /// 单个索引的字节数（16 位为 2，32 位为 4）
        uint32_t getIndexSize() const { return mIndexSize; }
        
    protected:
        /**
         * \brief 构造索引缓冲区，按 idxType 推导 mIndexSize
         * \param [in] idxType : 索引位宽
         * \param [in] idxCount : 索引个数
         * \param [in] buffer : 初始 CPU 索引数据
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         */
        IndexBuffer(IndexType idxType, uint32_t idxCount, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode);

        ~IndexBuffer() override = default;
        
        /**
         * \brief 通过活动 RHI 上下文创建 RHIIndexBuffer
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 索引缓冲区引用
         * \return 始终返回 true
         */
        bool onUnload() override;

        IndexType   mIndexType {IndexType::E_IT_16BITS};
        uint32_t  mIndexSize {0};
        uint32_t  mIndexCount {0};
    };
}


#endif  /*__T3D_INDEX_BUFFER_H__*/
