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

#pragma once


#include "T3DNullPrerequisites.h"


namespace Tiny3D
{
    class NullShader : public RHIShader
    {
    protected:
        NullShader() = default;
    };

    class NullVertexShader : public NullShader
    {
    public:
        static NullVertexShaderPtr create();

        ~NullVertexShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullVertexShader() = default;
    };

    class NullHullShader : public NullShader
    {
    public:
        static NullHullShaderPtr create();

        ~NullHullShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullHullShader() = default;
    };

    class NullDomainShader : public NullShader
    {
    public:
        static NullDomainShaderPtr create();

        ~NullDomainShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullDomainShader() = default;
    };

    class NullGeometryShader : public NullShader
    {
    public:
        static NullGeometryShaderPtr create();

        ~NullGeometryShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullGeometryShader() = default;
    };

    class NullPixelShader : public NullShader
    {
    public:
        static NullPixelShaderPtr create();

        ~NullPixelShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullPixelShader() = default;
    };

    class NullComputeShader : public NullShader
    {
    public:
        static NullComputeShaderPtr create();

        ~NullComputeShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullComputeShader() = default;
    };
}
