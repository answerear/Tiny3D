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


#ifndef __T3D_RHI_VERTEX_ATTRIBUTE_H__
#define __T3D_RHI_VERTEX_ATTRIBUTE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @brief   顶点属性
     */
    class T3D_ENGINE_API RHIVertexAttribute
    {
    public:
        /**
         * @enum    Type
         * @brief   顶点属性类型
         */
        enum class Type : uint32_t
        {
            E_VAT_FLOAT1 = 0,   /**< 1 float */
            E_VAT_FLOAT2 = 1,   /**< 2 float tuple */
            E_VAT_FLOAT3 = 2,   /**< 3 float tuple */
            E_VAT_FLOAT4 = 3,   /**< 4 float tuple */
            E_VAT_COLOR = 4,    /**< color type */
            E_VAT_BYTE4 = 5,
            E_VAT_BYTE4_NORM = 6,
            E_VAT_UBYTE4 = 7,
            E_VAT_UBYTE4_NORM = 8,
            E_VAT_SHORT2 = 9,
            E_VAT_SHORT4 = 10,
            E_VAT_SHORT2_NORM = 11,
            E_VAT_SHORT4_NORM = 12,
            E_VAT_USHORT2 = 13,
            E_VAT_USHORT4 = 14,
            E_VAT_USHORT2_NORM = 15,
            E_VAT_USHORT4_NORM = 16,
            E_VAT_DOUBLE1 = 17,
            E_VAT_DOUBLE2 = 18,
            E_VAT_DOUBLE3 = 19,
            E_VAT_DOUBLE4 = 20,
            E_VAT_INT1 = 21,
            E_VAT_INT2 = 22,
            E_VAT_INT3 = 23,
            E_VAT_INT4 = 24,
            E_VAT_UINT1 = 25,
            E_VAT_UINT2 = 26,
            E_VAT_UINT3 = 27,
            E_VAT_UINT4 = 28,
            E_VAT_FLOAT16_2 = 29,
            E_VAT_FLOAT16_4 = 30,
        };

        /**
         * @enum    Semantic
         * @brief   顶点语义解释
         */
        enum class Semantic : uint32_t
        {
            E_VAS_POSITION = 0,     /**< 位置 */
            E_VAS_BLENDWEIGHT = 1,  /**< 骨骼混合权重 */
            E_VAS_BLENDINDICES = 2, /**< 骨骼索引 */
            E_VAS_NORMAL = 3,       /**< 法线 */
            E_VAS_DIFFUSE = 4,      /**< 漫反射值 */
            E_VAS_SPECULAR = 5,     /**< 镜面反射值 */
            E_VAS_TEXCOORD = 6,     /**< 纹理坐标 */
            E_VAS_TANGENT = 7,      /**< 切线 */
            E_VAS_BINORMAL = 8,     /**< 副法线 */
        };

        /**
         * @fn  VertexAttribute::VertexAttribute();
         * @brief   默认构造函数
         */
        RHIVertexAttribute();

        /**
         * @brief   构造函数
         * @param [in]  stream          数据流索引.
         * @param [in]  offset          相对当前顶点起始位置的偏移.
         * @param [in]  type            顶点数据类型.
         * @param [in]  semantic        顶点语义解释.
         * @param [in]  semanticIndex   语义索引.
         */
        RHIVertexAttribute(size_t stream, size_t offset, Type type, Semantic semantic, size_t semanticIndex);

        /**
         * @brief   析构函数
         */
        virtual ~RHIVertexAttribute();

        /**
         * @brief   获取数据流索引
         * @returns 返回数据流索引.
         */
        size_t getStream() const { return mStream; }

        /**
         * @brief   获取顶点相对一个顶点数据起点的偏移位置
         * @returns 返回偏移值.
         */
        size_t getOffset() const { return mOffset; }

        /**
         * @brief   获取顶点数据类型
         * @returns 返回顶点数据类型.
         */
        Type getType() const { return mType; }

        /**
         * @brief   获取顶点语义解释
         * @returns 返回顶点语义解释.
         */
        Semantic getSemantic() const { return mSemantic; }

        /**
         * @brief   获取顶点语义索引
         * @returns 返回顶点语义索引.
         */
        size_t getSemanticIndex() const { return mSemanticIndex; }

        /**
         * @brief   获取顶点数据字节大小
         * @returns 返回顶点数据字节大小.
         */
        size_t getSize() const;

    protected:
        Type        mType;          /**< 顶点属性数据类型 */
        Semantic    mSemantic;      /**< 顶点属性语义 */
        size_t      mOffset;        /**< 顶点属性定义的在顶点缓存中的偏移值 */
        size_t      mStream;        /**< 对应定义的顶点缓存索引 */
        size_t      mSemanticIndex; /**< 顶点语义索引 */
    };
}


#endif  /*__T3D_RHI_VERTEX_ATTRIBUTE_H__*/