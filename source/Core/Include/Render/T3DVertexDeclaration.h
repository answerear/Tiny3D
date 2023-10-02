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


#ifndef __T3D_VERTEX_DECLARATION_H__
#define __T3D_VERTEX_DECLARATION_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DVertexAttribute.h"
#include "Render/T3DRenderResource.h"


namespace Tiny3D
{
    /**
     * @brief   顶点声明
     */
    class T3D_ENGINE_API VertexDeclaration : public RenderResource
    {
    public:
        typedef TList<VertexAttribute>           VertexAttriList;
        typedef VertexAttriList::iterator           VertexAttriListItr;
        typedef VertexAttriList::const_iterator     VertexAttriListConstItr;

        /**
         * @brief   创建顶点声明对象
         * @returns 返回一个顶点声明对象.
         */
        static VertexDeclarationPtr create();

        /**
         * @brief   析构函数
         */
        virtual ~VertexDeclaration();

        Type getType() const override;

        /**
         * @brief   获取顶点属性数量
         * @returns 返回顶点属性数量.
         */
        size_t getAttributeCount() const  {  return mVertexAttributes.size(); }

        /**
         * @brief   获取顶点属性列表
         * @returns 返回顶点属性列表.
         */
        const VertexAttriList &getAttributes() const
        { 
            return mVertexAttributes; 
        }

        /**
         * @brief   获取顶点属性
         * @param [in]  index   顶点属性数组中的索引.
         * @returns 返回顶点属性.
         */
        const VertexAttribute &getAttribute(size_t index) const;

        /**
         * @brief   新增一个顶点属性
         * @param [in]  stream          数据流索引.
         * @param [in]  offset          相对一个顶点数据开始位置的偏移.
         * @param [in]  type            顶点数据类型.
         * @param [in]  semantic        顶点语义解释.
         * @param [in]  semanticIndex   顶点语义索引.
         * @returns 返回一个新的顶点属性.
         */
        virtual const VertexAttribute &addAttribute(size_t stream,
            size_t offset, VertexAttribute::Type type,
            VertexAttribute::Semantic semantic, size_t semanticIndex);

        /**
         * @brief   新增插入一个顶点属性
         * @param [in]  pos             插入位置.
         * @param [in]  stream          数据流索引.
         * @param [in]  offset          相对一个顶点数据开始位置的偏移.
         * @param [in]  type            顶点数据类型.
         * @param [in]  semantic        顶点语义解释.
         * @param [in]  semanticIndex   顶点语义索引.
         * @returns 返回一个新的顶点属性.
         */
        virtual const VertexAttribute &insertAttribute(size_t pos, 
            size_t stream, size_t offset, VertexAttribute::Type type, 
            VertexAttribute::Semantic semantic, size_t semanticIndex);

        /**
         * @brief   添加一个顶点属性
         * @param [in]  vertexAttribute 顶点属性对象.
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult addAttribute(const VertexAttribute &vertexAttribute);

        /**
         * @brief   插入一个顶点属性
         * @param [in]  pos             插入的位置.
         * @param [in]  vertexAttribute 顶点属性对象.
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult insertAttribute(size_t pos, const VertexAttribute &vertexAttribute);

        /**
         * @brief   移除指定位置的顶点属性
         * @param [in]  pos 数组位置.
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult removeAttribute(size_t pos);

        /**
         * @brief   移除对应语义解释的顶点属性
         * @param [in]  semantic        语义解释.
         * @param       semanticIndex   Zero-based index of the semantic.
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult removeAttribute(VertexAttribute::Semantic semantic, size_t semanticIndex);

        /**
         * @brief   移除所有顶点属性
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult removeAllAttributes();

        /**
         * @brief   更新顶点属性
         * @param [in]  pos             更新对应位置的顶点属性.
         * @param [in]  stream          数据流索引.
         * @param [in]  offset          The offset.
         * @param [in]  type            顶点数据类型.
         * @param [in]  semantic        顶点语义解释.
         * @param [in]  semanticIndex   语义索引.
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult updateAttribute(size_t pos, size_t stream, 
            size_t offset, VertexAttribute::Type type, 
            VertexAttribute::Semantic semantic, size_t semanticIndex);

        /**
         * @brief   根据语义解释查找对应顶点属性对象
         * @param [in]  semantic        语义解释枚举.
         * @param [in]  semanticIndex   顶点语义索引.
         * @returns 查找成功返回对应的顶点属性对象，失败返回nullptr.
         */
        virtual const VertexAttribute *findAttributeBySemantic(
            VertexAttribute::Semantic semantic, size_t semanticIndex) const;
         
        /**
         * @brief   获取顶点字节大小
         * @param [in]  source  数据流索引.
         * @returns 返回字节大小.
         */
        virtual size_t getVertexSize(size_t source) const;

    protected:
        /**
         * @brief   构造函数
         */
        VertexDeclaration();

        bool onLoad() override;

        bool onUnload() override;

    protected:
        VertexAttriList mVertexAttributes;  /**< 顶点属性列表 */
    };
}


#endif  /*__T3D_VERTEX_DECLARATION_H__*/
