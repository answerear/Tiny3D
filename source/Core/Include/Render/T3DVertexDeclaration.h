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
    class T3D_ENGINE_API VertexDeclaration : public RenderResource
    {
    public:
        using VertexAttriList = TArray<VertexAttribute>;
        using VertexAttriListItr = VertexAttriList::iterator;
        using VertexAttriListConstItr = VertexAttriList::const_iterator;

        static VertexDeclarationPtr create();

        virtual ~VertexDeclaration();

        Type getType() const override;

        size_t getAttributeCount() const  {  return mVertexAttributes.size(); }

        const VertexAttriList &getAttributes() const { return mVertexAttributes; }

        const VertexAttribute &getAttribute(size_t index) const;

        const VertexAttribute &addAttribute(uint32_t stream, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        const VertexAttribute &insertAttribute(uint32_t pos, uint32_t stream, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        TResult addAttribute(const VertexAttribute &vertexAttribute);

        TResult insertAttribute(uint32_t pos, const VertexAttribute &vertexAttribute);

        TResult removeAttribute(uint32_t pos);

        TResult removeAttribute(VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        TResult removeAllAttributes();

        TResult updateAttribute(uint32_t pos, uint32_t stream, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex);

        uint32_t hash();

        const VertexAttribute *findAttributeBySemantic(VertexAttribute::Semantic semantic, uint32_t semanticIndex) const;
        
        uint32_t getVertexSize(uint32_t source) const;

    protected:
        /**
         * @brief   构造函数
         */
        VertexDeclaration();

        bool onLoad() override;

        bool onUnload() override;

    protected:
        /// 顶点属性列表
        VertexAttriList mVertexAttributes {};
        /// 数据 hash 值
        uint32_t    mHash {0};
        /// 数据是否被修改，用于重新计算 hash
        bool    mIsDirty {false};
    };
}


#endif  /*__T3D_VERTEX_DECLARATION_H__*/
