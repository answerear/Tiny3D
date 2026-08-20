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


#ifndef __T3D_METAL_SHADER_H__
#define __T3D_METAL_SHADER_H__


#include "T3DMetalPrerequisites.h"


namespace Tiny3D
{
    class MetalShader : public RHIShader
    {
    public:
        void *getNativeObject() const override { return mNative; }

        void setNativeObject(void *function);

    protected:
        MetalShader() = default;

        ~MetalShader() override;

        void *mNative {nullptr};
    };

    class MetalVertexShader : public MetalShader
    {
    public:
        static MetalVertexShaderPtr create();

    protected:
        MetalVertexShader() = default;
    };

    class MetalHullShader : public MetalShader
    {
    public:
        static MetalHullShaderPtr create();

    protected:
        MetalHullShader() = default;
    };

    class MetalDomainShader : public MetalShader
    {
    public:
        static MetalDomainShaderPtr create();

    protected:
        MetalDomainShader() = default;
    };

    class MetalGeometryShader : public MetalShader
    {
    public:
        static MetalGeometryShaderPtr create();

    protected:
        MetalGeometryShader() = default;
    };

    class MetalPixelShader : public MetalShader
    {
    public:
        static MetalPixelShaderPtr create();

    protected:
        MetalPixelShader() = default;
    };

    class MetalComputeShader : public MetalShader
    {
    public:
        static MetalComputeShaderPtr create();

    protected:
        MetalComputeShader() = default;
    };
}


#endif  /*__T3D_METAL_SHADER_H__*/
