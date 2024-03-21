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
        static VertexDeclarationPtr create(const VertexAttributes &attributes);

        ~VertexDeclaration() override;

        Type getType() const override;

        size_t getAttributeCount() const  {  return mVertexAttributes.size(); }

        uint32_t hash();
        
        uint32_t getVertexSize(uint32_t source) const;

    protected:
        /**
         * @brief   构造函数
         */
        VertexDeclaration(const VertexAttributes &attributes);

        bool onLoad() override;

        bool onUnload() override;

    protected:
        /// 顶点属性列表
        const VertexAttributes &mVertexAttributes;
        /// 数据 hash 值
        uint32_t    mHash {0};
    };
}


#endif  /*__T3D_VERTEX_DECLARATION_H__*/
