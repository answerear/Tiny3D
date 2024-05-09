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


#ifndef __T3D_RHI_RESOURCE_H__
#define __T3D_RHI_RESOURCE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 渲染硬件层的颜色混合状态
     */
    class T3D_ENGINE_API RHIResource : public Object
    {
    public:
        RHIResource() = default;
        ~RHIResource() override = default;

        enum class ResourceType : uint32_t
        {
            kNone = 0,
            kBlendState,
            kDepthStencilState,
            kRasterizerState,
            kSamplerState,
            kVertexBuffer,
            kIndexBuffer,
            kPixelBuffer1D,
            kPixelBuffer2D,
            kPixelBuffer3D,
            kPixelBufferCubemap,
            kConstantBuffer,
            kVertexDeclaration,
            kRenderWindow,
            kRenderTexture,
            kShader,
        };
        
        virtual ResourceType getResourceType() const = 0;

        virtual void *getNativeObject() const = 0;
    };
}


#endif  /*__T3D_RHI_RESOURCE_H__*/
