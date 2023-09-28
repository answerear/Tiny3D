/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#ifndef __T3D_RHI_VERTEX_BUFFER_H__
#define __T3D_RHI_VERTEX_BUFFER_H__


#include "T3DRHIBuffer.h"


namespace Tiny3D
{
    /**
     * @class   RHIVertexBuffer
     * @brief   顶点缓冲区
     */
    class T3D_ENGINE_API RHIVertexBuffer : public RHIBuffer
    {
    public:
        /**
         * @fn  virtual HardwareVertexBuffer::~HardwareVertexBuffer();
         * @brief   析构函数
         */
        ~RHIVertexBuffer() override;

        /**
         * @fn  size_t HardwareVertexBuffer::getVertexSize() const
         * @brief   获取当个顶点的字节数大小
         * @returns 返回顶点大小.
         */
        size_t getVertexSize() const { return mVertexSize; }

        /**
         * @fn  size_t HardwareVertexBuffer::getVertexCount() const
         * @brief   获取顶点数
         * @returns 返回定点数.
         */
        size_t getVertexCount() const { return mVertexCount; }

        ResourceType getResourceType() const override { return ResourceType::kVertexBuffer; }
        
    protected:
        /**
         * @fn  HardwareVertexBuffer::HardwareVertexBuffer(size_t vertexSize, 
         *      size_t vertexCount, Usage usage, uint32_t mode);
         * @brief   构造函数
         * @param [in]  vertexSize  顶点字节数大小.
         * @param [in]  vertexCount 顶点数量.
         * @param [in]  usage       缓冲区用法.
         * @param [in]  mode        缓冲区访问方式.
         */
        RHIVertexBuffer(size_t vertexSize, size_t vertexCount, Usage usage, uint32_t mode);

    protected:
        size_t  mVertexCount;       /**< 顶点数量 */
        size_t  mVertexSize;        /**< 顶点字节数大小 */
    };
}


#endif  /*__T3D_RHI_VERTEX_BUFFER_H__*/