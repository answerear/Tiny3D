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

#ifndef __T3D_VK_RENDER_BUFFER_H__
#define __T3D_VK_RENDER_BUFFER_H__


#include "T3DVKPrerequisites.h"


namespace Tiny3D
{
    class VKVertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static VKVertexDeclarationPtr create();

        ~VKVertexDeclaration() override;

        void *getNativeObject() const override;

    protected:
        VKVertexDeclaration() = default;
    };
    
    class VKVertexBuffer : public RHIVertexBuffer
    {
    public:
        static VKVertexBufferPtr create();

        ~VKVertexBuffer() override;

        void *getNativeObject() const override;
        
        VkBuffer        VkBuf {VK_NULL_HANDLE};
        VkDeviceMemory  VkBufMemory {VK_NULL_HANDLE};
        
    protected:
        VKVertexBuffer() = default;
    };

    class VKIndexBuffer : public RHIIndexBuffer
    {
    public:
        static VKIndexBufferPtr create();

        ~VKIndexBuffer() override;

        void *getNativeObject() const override;
        
        VkBuffer        VkBuf {VK_NULL_HANDLE};
        VkDeviceMemory  VkBufMemory {VK_NULL_HANDLE};
        
    protected:
        VKIndexBuffer() = default;
    };

    class VKConstantBuffer : public RHIConstantBuffer
    {
    public:
        static VKConstantBufferPtr create();

        ~VKConstantBuffer() override;

        void *getNativeObject() const override;
        
        VkBuffer        VkBuf {VK_NULL_HANDLE};
        VkDeviceMemory  VkBufMemory {VK_NULL_HANDLE};

    protected:
        VKConstantBuffer() = default;
    };

    class VKPixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static VKPixelBuffer1DPtr create();

        ~VKPixelBuffer1D() override;

        void *getNativeObject() const override;
        
        VkImage         VkTex {VK_NULL_HANDLE};
        VkDeviceMemory  VkTexMemory {VK_NULL_HANDLE};
        VkImageView     VkTexView {VK_NULL_HANDLE};
        
    protected:
        VKPixelBuffer1D() = default;
    };

    class VKPixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static VKPixelBuffer2DPtr create();

        ~VKPixelBuffer2D() override;

        void *getNativeObject() const override;

        VkImage         VkTex {VK_NULL_HANDLE};
        VkDeviceMemory  VkTexMemory {VK_NULL_HANDLE};
        VkImageView     VkTexView {VK_NULL_HANDLE};
        /// Render target view (for render texture)
        VkImageView     VkRTView {VK_NULL_HANDLE};
        /// Depth stencil view
        VkImageView     VkDSView {VK_NULL_HANDLE};
        /// Current image layout (tracked for correct barrier transitions)
        VkImageLayout   VkCurrentLayout {VK_IMAGE_LAYOUT_UNDEFINED};

    protected:
        VKPixelBuffer2D() = default;
    };

    class VKPixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        void *getNativeObject() const override;

        VkImage         VkTex {VK_NULL_HANDLE};
        VkDeviceMemory  VkTexMemory {VK_NULL_HANDLE};
        VkImageView     VkTexView {VK_NULL_HANDLE};
    };

    class VKPixelBufferCubemap : public RHIPixelBufferCubemap
    {
    public:
        static VKPixelBufferCubemapPtr create();

        ~VKPixelBufferCubemap() override;

        void *getNativeObject() const override;

        VkImage         VkTex {VK_NULL_HANDLE};
        VkDeviceMemory  VkTexMemory {VK_NULL_HANDLE};
        VkImageView     VkTexView {VK_NULL_HANDLE};
        /// Current image layout (tracked for correct barrier transitions)
        VkImageLayout   VkCurrentLayout {VK_IMAGE_LAYOUT_UNDEFINED};

    protected:
        VKPixelBufferCubemap() = default;
    };
}


#endif    /*__T3D_VK_RENDER_BUFFER_H__*/
