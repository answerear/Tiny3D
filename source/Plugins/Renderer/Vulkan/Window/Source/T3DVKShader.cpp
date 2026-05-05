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


#include "T3DVKShader.h"
#include "T3DVKRenderer.h"
#include "T3DVKContext.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKVertexShaderPtr VKVertexShader::create()
    {
        return T3D_NEW VKVertexShader();
    }

    //--------------------------------------------------------------------------

    VKVertexShader::~VKVertexShader()
    {
        VkDevice device = VK_CONTEXT->getVkDevice();
        if (device != VK_NULL_HANDLE && VkModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, VkModule, nullptr);
            VkModule = VK_NULL_HANDLE;
        }
    }

    //--------------------------------------------------------------------------

    void *VKVertexShader::getNativeObject() const
    {
        return (void *)VkModule;
    }

    //--------------------------------------------------------------------------

    VKHullShaderPtr VKHullShader::create()
    {
        return T3D_NEW VKHullShader();
    }

    //--------------------------------------------------------------------------

    VKHullShader::~VKHullShader()
    {
        VkDevice device = VK_CONTEXT->getVkDevice();
        if (device != VK_NULL_HANDLE && VkModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, VkModule, nullptr);
            VkModule = VK_NULL_HANDLE;
        }
    }

    //--------------------------------------------------------------------------

    void *VKHullShader::getNativeObject() const
    {
        return (void *)VkModule;
    }

    //--------------------------------------------------------------------------

    VKDomainShaderPtr VKDomainShader::create()
    {
        return T3D_NEW VKDomainShader();
    }

    //--------------------------------------------------------------------------

    VKDomainShader::~VKDomainShader()
    {
        VkDevice device = VK_CONTEXT->getVkDevice();
        if (device != VK_NULL_HANDLE && VkModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, VkModule, nullptr);
            VkModule = VK_NULL_HANDLE;
        }
    }

    //--------------------------------------------------------------------------

    void *VKDomainShader::getNativeObject() const
    {
        return (void *)VkModule;
    }

    //--------------------------------------------------------------------------

    VKGeometryShaderPtr VKGeometryShader::create()
    {
        return T3D_NEW VKGeometryShader();
    }

    //--------------------------------------------------------------------------

    VKGeometryShader::~VKGeometryShader()
    {
        VkDevice device = VK_CONTEXT->getVkDevice();
        if (device != VK_NULL_HANDLE && VkModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, VkModule, nullptr);
            VkModule = VK_NULL_HANDLE;
        }
    }

    //--------------------------------------------------------------------------

    void *VKGeometryShader::getNativeObject() const
    {
        return (void *)VkModule;
    }

    //--------------------------------------------------------------------------

    VKPixelShaderPtr VKPixelShader::create()
    {
        return T3D_NEW VKPixelShader();
    }

    //--------------------------------------------------------------------------

    VKPixelShader::~VKPixelShader()
    {
        VkDevice device = VK_CONTEXT->getVkDevice();
        if (device != VK_NULL_HANDLE && VkModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, VkModule, nullptr);
            VkModule = VK_NULL_HANDLE;
        }
    }

    //--------------------------------------------------------------------------

    void *VKPixelShader::getNativeObject() const
    {
        return (void *)VkModule;
    }

    //--------------------------------------------------------------------------

    VKComputeShaderPtr VKComputeShader::create()
    {
        return T3D_NEW VKComputeShader();
    }

    //--------------------------------------------------------------------------

    VKComputeShader::~VKComputeShader()
    {
        VkDevice device = VK_CONTEXT->getVkDevice();
        if (device != VK_NULL_HANDLE && VkModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, VkModule, nullptr);
            VkModule = VK_NULL_HANDLE;
        }
    }

    //--------------------------------------------------------------------------

    void *VKComputeShader::getNativeObject() const
    {
        return (void *)VkModule;
    }
}
