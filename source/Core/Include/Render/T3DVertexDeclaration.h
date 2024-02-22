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
