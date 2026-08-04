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


#ifndef __T3D_RHI_PIXEL_BUFFER_H__
#define __T3D_RHI_PIXEL_BUFFER_H__


#include "RHI/T3DRHIResource.h"


namespace Tiny3D
{
    /**
     * \brief RHI 层一维像素缓冲（纹理）对象
     */
    class T3D_ENGINE_API RHIPixelBuffer1D : public RHIResource
    {
    public:
        /**
         * \brief 返回资源类型 kPixelBuffer1D
         * \return ResourceType::kPixelBuffer1D
         */
        ResourceType getResourceType() const override { return ResourceType::kPixelBuffer1D; }

    protected:
        /// 受保护构造，由 RHIContext::createPixelBuffer1D 及后端派生类创建
        RHIPixelBuffer1D();
    };

    /**
     * \brief RHI 层二维像素缓冲（纹理）对象
     */
    class T3D_ENGINE_API RHIPixelBuffer2D : public RHIResource
    {
    public:
        /**
         * \brief 返回资源类型 kPixelBuffer2D
         * \return ResourceType::kPixelBuffer2D
         */
        ResourceType getResourceType() const override { return ResourceType::kPixelBuffer2D; }

    protected:
        /// 受保护构造，由 RHIContext::createPixelBuffer2D / createRenderTexture 及后端派生类创建
        RHIPixelBuffer2D();
    };

    /**
     * \brief RHI 层三维像素缓冲（体积纹理）对象
     */
    class T3D_ENGINE_API RHIPixelBuffer3D : public RHIResource
    {
    public:
        /**
         * \brief 返回资源类型 kPixelBuffer3D
         * \return ResourceType::kPixelBuffer3D
         */
        ResourceType getResourceType() const override { return ResourceType::kPixelBuffer3D; }

    protected:
        /// 受保护构造，由 RHIContext::createPixelBuffer3D 及后端派生类创建
        RHIPixelBuffer3D();
    };

    /**
     * \brief RHI 层立方体贴图像素缓冲对象
     */
    class T3D_ENGINE_API RHIPixelBufferCubemap : public RHIResource
    {
    public:
        /**
         * \brief 返回资源类型 kPixelBufferCubemap
         * \return ResourceType::kPixelBufferCubemap
         */
        ResourceType getResourceType() const override { return ResourceType::kPixelBufferCubemap; }

    protected:
        /// 受保护构造，由 RHIContext::createPixelBufferCubemap 及后端派生类创建
        RHIPixelBufferCubemap();
    };
}


#endif  /*__T3D_RHI_PIXEL_BUFFER_H__*/
