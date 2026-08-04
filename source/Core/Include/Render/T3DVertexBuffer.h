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


#ifndef __T3D_VERTEX_BUFFER_H__
#define __T3D_VERTEX_BUFFER_H__


#include "Render/T3DRenderBuffer.h"


namespace Tiny3D
{
    /**
     * \brief 顶点缓冲渲染资源，onLoad 时创建 RHI 顶点缓冲
     */
    class T3D_ENGINE_API VertexBuffer : public RenderBuffer
    {
    public:
        /**
         * \brief 创建顶点缓冲对象（尚未创建底层 RHI 资源，需经 RenderResourceManager 加载）
         * \param [in] vertexSize : 单个顶点字节数
         * \param [in] vertexCount : 顶点数量
         * \param [in] buffer : 初始 CPU 数据
         * \param [in] memType : 内存类型
         * \param [in] usage : 缓冲用途
         * \param [in] accMode : CPU 访问模式
         * \return 新创建的 VertexBuffer 指针
         */
        static VertexBufferPtr create(uint32_t vertexSize, uint32_t vertexCount, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode);

        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kVertexBuffer
         */
        Type getType() const override;

        /// 返回顶点数量
        uint32_t getVertexCount() const { return mVertexCount; }

        /// 返回单个顶点字节数
        uint32_t getVertexSize() const { return mVertexSize; }
        
    protected:
        /**
         * \brief 构造顶点缓冲，保存顶点参数与 CPU 数据
         * \param [in] vertexSize : 单个顶点字节数
         * \param [in] vertexCount : 顶点数量
         * \param [in] buffer : 初始 CPU 数据
         * \param [in] memType : 内存类型
         * \param [in] usage : 缓冲用途
         * \param [in] accMode : CPU 访问模式
         */
        VertexBuffer(uint32_t vertexSize, uint32_t vertexCount, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode);

        ~VertexBuffer() override = default;
        
        /**
         * \brief 通过活动 RHI 上下文创建 RHIVertexBuffer
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 顶点缓冲引用
         * \return 始终返回 true
         */
        bool onUnload() override;

        /// 单个顶点字节数
        uint32_t  mVertexSize {0};
        /// 顶点数量
        uint32_t  mVertexCount {0};
    };
}


#endif  /*__T3D_VERTEX_BUFFER_H__*/
