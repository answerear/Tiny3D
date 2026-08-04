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


#ifndef __T3D_RHI_VERTEX_DECLARATION_H__
#define __T3D_RHI_VERTEX_DECLARATION_H__


#include "RHI/T3DRHIResource.h"


namespace Tiny3D
{
    /**
     * \brief RHI 层顶点声明（输入布局）对象，描述顶点属性到 shader 输入的映射
     */
    class T3D_ENGINE_API RHIVertexDeclaration : public RHIResource
    {
    public:
        /**
         * \brief 返回资源类型 kVertexDeclaration
         * \return ResourceType::kVertexDeclaration
         */
        ResourceType getResourceType() const override { return ResourceType::kVertexDeclaration; }

    protected:
        /// 受保护构造，由 RHIContext::createVertexDeclaration 及后端派生类创建
        RHIVertexDeclaration();
    };
}


#endif  /*__T3D_RHI_VERTEX_DECLARATION_H__*/
