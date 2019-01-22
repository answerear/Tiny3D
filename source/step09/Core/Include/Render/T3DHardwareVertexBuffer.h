/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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


#ifndef __T3D_HARDWARE_VERTEX_BUFFER_H__
#define __T3D_HARDWARE_VERTEX_BUFFER_H__


#include "Render/T3DHardwareBuffer.h"


namespace Tiny3D
{
    /**
     * @brief 顶点缓冲区
     */
    class T3D_ENGINE_API HardwareVertexBuffer : public HardwareBuffer
    {
    public:
        /**
         * @brief 析构函数
         */
        virtual ~HardwareVertexBuffer();

        /**
         * @brief 获取当个顶点的字节数大小
         */
        size_t getVertexSize() const { return mVertexSize; }

        /**
         * @brief 获取顶点数
         */
        size_t getVertexCount() const { return mVertexCount; }

    protected:
        /**
         * @brief 构造函数
         * @param [in] vertexSize : 顶点字节数大小
         * @param [in] vertexCount : 顶点数量
         * @param [in] usage : 缓冲区用法
         * @param [in] useSystemMemory : 是否使用系统内存
         * @param [in] useShadowBuffer : 是否使用影子缓存
         */
        HardwareVertexBuffer(size_t vertexSize, size_t vertexCount, Usage usage, 
            bool useSystemMemory, bool useShadowBuffer);

    protected:
        size_t  mVertexCount;       /**< 顶点数量 */
        size_t  mVertexSize;        /**< 顶点字节数大小 */
    };


    /**
     * @brief 顶点属性
     */
    class T3D_ENGINE_API VertexAttribute : public Object
    {
    public:
        /**< 顶点属性类型 */
        enum Type
        {
            E_VAT_FLOAT1 = 0,
            E_VAT_FLOAT2 = 1,
            E_VAT_FLOAT3 = 2,
            E_VAT_FLOAT4 = 3,
            E_VAT_COLOR = 4,
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

        /**< 顶点语法解释 */
        enum Semantic
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
         * @brief 默认构造函数
         */
        VertexAttribute();

        /**
         * @brief 构造函数
         * @param [in] stream : 数据流索引
         * @param [in] offset : 相对当前顶点起始位置的偏移
         * @param [in] type : 顶点数据类型
         * @param [in] semantic : 顶点语法解释
         */
        VertexAttribute(size_t stream, size_t offset, Type type, 
            Semantic semantic);

        /**
         * @brief 析构函数
         */
        virtual ~VertexAttribute();

        /**
         * @brief 获取数据流索引
         */
        size_t getStream() const { return mStream; }

        /**
         * @brief 获取顶点相对一个顶点数据起点的偏移位置
         */
        size_t getOffset() const { return mOffset; }

        /**
         * @brief 获取顶点数据类型
         */
        Type getType() const { return mType; }

        /**
         * @brief 获取顶点语法解释
         */
        Semantic getSemantic() const { return mSemantic; }

        /**
         * @brief 获取顶点数据字节大小
         */
        size_t getSize() const;

    protected:
        Type        mType;      /**< 顶点属性数据类型 */
        Semantic    mSemantic;  /**< 顶点属性语义 */
        size_t      mOffset;    /**< 这个顶点属性定义的在顶点缓存中的起始点偏移值 */
        size_t      mStream;    /**< 对应定义的顶点缓存索引 */
    };


    /**
     * @brief 顶点声明
     */
    class T3D_ENGINE_API VertexDeclaration : public Object
    {
    public:
        typedef std::list<VertexAttribute>          VertexAttriList;
        typedef VertexAttriList::iterator           VertexAttriListItr;
        typedef VertexAttriList::const_iterator     VertexAttriListConstItr;

        /**
         * @brief 构造函数
         */
        VertexDeclaration();

        /**
         * @brief 析构函数
         */
        virtual ~VertexDeclaration();

        /**
         * @brief 获取顶点属性数量
         */
        size_t getAttributeCount() const 
        { 
            return mVertexAttributes.size(); 
        }

        /**
         * @brief 获取顶点属性列表
         */
        const VertexAttriList &getAttributes() const
        { 
            return mVertexAttributes; 
        }

        /**
         * @brief 获取顶点属性
         * @param [in] index : 顶点属性数组中的索引
         * @return 返回顶点属性
         */
        const VertexAttribute &getAttribute(size_t index) const;

        /**
         * @brief 新增一个顶点属性
         * @param [in] stream : 数据流索引
         * @param [in] offset : 相对一个顶点数据开始位置的偏移
         * @param [in] type : 顶点数据类型
         * @param [in] semantic : 顶点语法解释
         * @return 返回一个新的顶点属性
         */
        virtual const VertexAttribute &addAttribute(size_t stream, 
            size_t offset, VertexAttribute::Type type, 
            VertexAttribute::Semantic semantic);

        /**
         * @brief 新增插入一个顶点属性
         * @param [in] pos : 插入位置
         * @param [in] stream : 数据流索引
         * @param [in] offset : 相对一个顶点数据开始位置的偏移
         * @param [in] type : 顶点数据类型
         * @param [in] semantic : 顶点语法解释
         * @return 返回一个新的顶点属性
         */
        virtual const VertexAttribute &insertAttribute(size_t pos, 
            size_t stream, size_t offset, VertexAttribute::Type type, 
            VertexAttribute::Semantic semantic);

        /**
         * @brief 添加一个顶点属性
         * @param [in] vertexAttribute : 顶点属性对象
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult addAttribute(const VertexAttribute &vertexAttribute);

        /**
         * @brief 插入一个顶点属性
         * @param [in] pos : 插入的位置
         * @param [in] vertexAttribute : 顶点属性对象
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult insertAttribute(size_t pos, 
            const VertexAttribute &vertexAttribute);

        /**
         * @brief 移除指定位置的顶点属性
         * @param [in] pos : 数组位置
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult removeAttribute(size_t pos);

        /**
         * @brief 移除对应语法解释的顶点属性
         * @param [in] semantic : 语法解释
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult removeAttribute(VertexAttribute::Semantic semantic);

        /**
         * @brief 移除所有顶点属性
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult removeAllAttributes();

        /**
         * @brief 更新顶点属性
         * @param [in] pos : 更新对应位置的顶点属性
         * @param [in] stream : 数据流索引
         * @param [in] type : 顶点数据类型
         * @param [in] semantic : 顶点语法解释
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult updateAttribute(size_t pos, size_t stream, 
            size_t offset, VertexAttribute::Type type, 
            VertexAttribute::Semantic semantic);

        /**
         * @brief 根据语法解释查找对应顶点属性对象
         * @param [in] semantic : 语法解释枚举
         * @return 查找成功返回对应的顶点属性对象，失败返回nullptr
         */
        virtual const VertexAttribute *findAttributeBySemantic(
            VertexAttribute::Semantic semantic) const;
         
        /**
         * @brief 获取顶点字节大小
         * @param [in] source : 数据流索引
         * @return 返回字节大小
         */
        virtual size_t getVertexSize(size_t source) const;

        /**
         * @brief 克隆一个顶点声明对象
         * @return 返回一个新的顶点声明对象
         * @remarks 具体的渲染系统子类需要实现该接口
         */
        virtual VertexDeclarationPtr clone() const;

    protected:
        VertexAttriList mVertexAttributes;  /**< 顶点属性列表 */
    };
}


#endif  /*__T3D_HARDWARE_VERTEX_BUFFER_H__*/
