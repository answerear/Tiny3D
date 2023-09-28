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


#ifndef __T3D_RHI_INDEX_BUFFER_H__
#define __T3D_RHI_INDEX_BUFFER_H__


#include "RHI/T3DRHIBuffer.h"


namespace Tiny3D
{
    /**
     * @brief   索引缓冲区
     */
    class T3D_ENGINE_API RHIIndexBuffer : public RHIBuffer
    {
    public:
        /**
         * @enum    Type
         * @brief   索引大小类型
         */
        enum class Type : uint32_t
        {
            E_IT_16BITS = 0,    /**< 16位索引 */
            E_IT_32BITS,        /**< 32位索引 */
        };

        /**
         * @brief   析构函数
         */
        ~RHIIndexBuffer() override;

        /**
         * @brief   获取索引类型
         * @returns 返回索引类型.
         */
        Type getIndexType() const { return mType; }

        /**
         * @brief   获取索引大小
         * @returns 返回索引大小.
         */
        size_t getIndexSize() const { return mIndexSize; }

        /**
         * @brief   获取索引数量
         * @returns 返回索引数量.
         */
        size_t getIndexCount() const { return mIndexCount; }

        ResourceType getResourceType() const override { return ResourceType::kIndexBuffer; }

    protected:
        /**
         * @brief   构造函数
         * @param [in]  indexType   索引类型.
         * @param [in]  indexCount  索引数量.
         * @param [in]  usage       缓冲区使用方式.
         * @param [in]  mode        缓冲区访问方式.
         */
        RHIIndexBuffer(Type indexType, size_t indexCount, Usage usage, uint32_t mode);

    protected:
        Type    mType;          /**< 索引类型，是16位还是32位 */
        size_t  mIndexSize;     /**< 索引大小 */
        size_t  mIndexCount;    /**< 索引数量 */
    };
}


#endif  /*__T3D_RHI_INDEX_BUFFER_H__*/
