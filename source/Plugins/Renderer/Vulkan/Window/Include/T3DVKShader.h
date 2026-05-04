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

#ifndef __T3D_VK_SHADER_H__
#define __T3D_VK_SHADER_H__


#include "T3DVKPrerequisites.h"


namespace Tiny3D
{
    class VKShader : public RHIShader
    {
    protected:
        VKShader() = default;
    };

    class VKVertexShader : public VKShader
    {
    public:
        static VKVertexShaderPtr create();

        ~VKVertexShader() override;

        void *getNativeObject() const override;
        
        VkShaderModule  VkModule {VK_NULL_HANDLE};
    protected:
        VKVertexShader() = default;
    };

    class VKHullShader : public VKShader
    {
    public:
        static VKHullShaderPtr create();

        ~VKHullShader() override;

        void *getNativeObject() const override;
        
        VkShaderModule  VkModule {VK_NULL_HANDLE};
    protected:
        VKHullShader() = default;
    };

    class VKDomainShader : public VKShader
    {
    public:
        static VKDomainShaderPtr create();

        ~VKDomainShader() override;

        void *getNativeObject() const override;
        
        VkShaderModule  VkModule {VK_NULL_HANDLE};
    protected:
        VKDomainShader() = default;
    };

    class VKGeometryShader : public VKShader
    {
    public:
        static VKGeometryShaderPtr create();

        ~VKGeometryShader() override;

        void *getNativeObject() const override;
        
        VkShaderModule  VkModule {VK_NULL_HANDLE};
    protected:
        VKGeometryShader() = default;
    };

    class VKPixelShader : public VKShader
    {
    public:
        static VKPixelShaderPtr create();

        ~VKPixelShader() override;

        void *getNativeObject() const override;
        
        VkShaderModule  VkModule {VK_NULL_HANDLE};
    protected:
        VKPixelShader() = default;
    };

    class VKComputeShader : public VKShader
    {
    public:
        static VKComputeShaderPtr create();

        ~VKComputeShader() override;

        void *getNativeObject() const override;
        
        VkShaderModule  VkModule {VK_NULL_HANDLE};
    protected:
        VKComputeShader() = default;
    };
}


#endif  /*__T3D_VK_SHADER_H__*/
