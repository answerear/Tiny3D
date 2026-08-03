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


#include "T3DVKRenderBuffer.h"
#include "T3DVKRenderer.h"
#include "T3DVKContext.h"

#define VK_DEVICE()  (VK_CONTEXT->getVkDevice())


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKVertexDeclarationPtr VKVertexDeclaration::create()
    {
        return T3D_NEW VKVertexDeclaration();
    }

    //--------------------------------------------------------------------------

    VKVertexDeclaration::~VKVertexDeclaration()
    {
    }

    //--------------------------------------------------------------------------

    void *VKVertexDeclaration::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKVertexBufferPtr VKVertexBuffer::create()
    {
        return T3D_NEW VKVertexBuffer();
    }

    //--------------------------------------------------------------------------

    VKVertexBuffer::~VKVertexBuffer()
    {
        VkDevice device = VK_DEVICE();
        if (device != VK_NULL_HANDLE)
        {
            if (VkBuf != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(device, VkBuf, nullptr);
                VkBuf = VK_NULL_HANDLE;
            }
            if (VkBufMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, VkBufMemory, nullptr);
                VkBufMemory = VK_NULL_HANDLE;
            }
        }
    }

    //--------------------------------------------------------------------------

    void *VKVertexBuffer::getNativeObject() const
    {
        return (void *)VkBuf;
    }

    //--------------------------------------------------------------------------

    VKIndexBufferPtr VKIndexBuffer::create()
    {
        return T3D_NEW VKIndexBuffer();
    }

    //--------------------------------------------------------------------------

    VKIndexBuffer::~VKIndexBuffer()
    {
        VkDevice device = VK_DEVICE();
        if (device != VK_NULL_HANDLE)
        {
            if (VkBuf != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(device, VkBuf, nullptr);
                VkBuf = VK_NULL_HANDLE;
            }
            if (VkBufMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, VkBufMemory, nullptr);
                VkBufMemory = VK_NULL_HANDLE;
            }
        }
    }

    //--------------------------------------------------------------------------

    void *VKIndexBuffer::getNativeObject() const
    {
        return (void *)VkBuf;
    }

    //--------------------------------------------------------------------------

    VKConstantBufferPtr VKConstantBuffer::create()
    {
        return T3D_NEW VKConstantBuffer();
    }

    //--------------------------------------------------------------------------

    VKConstantBuffer::~VKConstantBuffer()
    {
        VkDevice device = VK_DEVICE();
        if (device != VK_NULL_HANDLE)
        {
            if (VkBuf != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(device, VkBuf, nullptr);
                VkBuf = VK_NULL_HANDLE;
            }
            if (VkBufMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, VkBufMemory, nullptr);
                VkBufMemory = VK_NULL_HANDLE;
            }
        }
    }

    //--------------------------------------------------------------------------

    void *VKConstantBuffer::getNativeObject() const
    {
        return (void *)VkBuf;
    }

    //--------------------------------------------------------------------------

    VKPixelBuffer1DPtr VKPixelBuffer1D::create()
    {
        return T3D_NEW VKPixelBuffer1D();
    }

    //--------------------------------------------------------------------------

    VKPixelBuffer1D::~VKPixelBuffer1D()
    {
        VkDevice device = VK_DEVICE();
        if (device != VK_NULL_HANDLE)
        {
            if (VkTexView != VK_NULL_HANDLE)
            {
                vkDestroyImageView(device, VkTexView, nullptr);
                VkTexView = VK_NULL_HANDLE;
            }
            if (VkTex != VK_NULL_HANDLE)
            {
                vkDestroyImage(device, VkTex, nullptr);
                VkTex = VK_NULL_HANDLE;
            }
            if (VkTexMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, VkTexMemory, nullptr);
                VkTexMemory = VK_NULL_HANDLE;
            }
        }
    }

    //--------------------------------------------------------------------------

    void *VKPixelBuffer1D::getNativeObject() const
    {
        return (void *)VkTex;
    }

    //--------------------------------------------------------------------------

    VKPixelBuffer2DPtr VKPixelBuffer2D::create()
    {
        return T3D_NEW VKPixelBuffer2D();
    }

    //--------------------------------------------------------------------------

    VKPixelBuffer2D::~VKPixelBuffer2D()
    {
        VkDevice device = VK_DEVICE();
        if (device != VK_NULL_HANDLE)
        {
            if (VkDSView != VK_NULL_HANDLE)
            {
                vkDestroyImageView(device, VkDSView, nullptr);
                if (VkTexView == VkDSView)
                    VkTexView = VK_NULL_HANDLE;
                VkDSView = VK_NULL_HANDLE;
            }
            if (VkRTView != VK_NULL_HANDLE)
            {
                vkDestroyImageView(device, VkRTView, nullptr);
                if (VkTexView == VkRTView)
                    VkTexView = VK_NULL_HANDLE;
                VkRTView = VK_NULL_HANDLE;
            }
            if (VkTexView != VK_NULL_HANDLE)
            {
                vkDestroyImageView(device, VkTexView, nullptr);
                VkTexView = VK_NULL_HANDLE;
            }
            if (VkTex != VK_NULL_HANDLE)
            {
                vkDestroyImage(device, VkTex, nullptr);
                VkTex = VK_NULL_HANDLE;
            }
            if (VkTexMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, VkTexMemory, nullptr);
                VkTexMemory = VK_NULL_HANDLE;
            }
        }
    }

    //--------------------------------------------------------------------------

    void *VKPixelBuffer2D::getNativeObject() const
    {
        return (void *)VkTex;
    }

    //--------------------------------------------------------------------------

    void *VKPixelBuffer3D::getNativeObject() const
    {
        return (void *)VkTex;
    }

    //--------------------------------------------------------------------------

    VKPixelBufferCubemapPtr VKPixelBufferCubemap::create()
    {
        return T3D_NEW VKPixelBufferCubemap();
    }

    //--------------------------------------------------------------------------

    VKPixelBufferCubemap::~VKPixelBufferCubemap()
    {
        VkDevice device = VK_DEVICE();
        if (device != VK_NULL_HANDLE)
        {
            if (VkTexView != VK_NULL_HANDLE)
            {
                vkDestroyImageView(device, VkTexView, nullptr);
                VkTexView = VK_NULL_HANDLE;
            }
            if (VkTex != VK_NULL_HANDLE)
            {
                vkDestroyImage(device, VkTex, nullptr);
                VkTex = VK_NULL_HANDLE;
            }
            if (VkTexMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, VkTexMemory, nullptr);
                VkTexMemory = VK_NULL_HANDLE;
            }
        }
    }

    //--------------------------------------------------------------------------

    void *VKPixelBufferCubemap::getNativeObject() const
    {
        return (void *)VkTex;
    }
}
