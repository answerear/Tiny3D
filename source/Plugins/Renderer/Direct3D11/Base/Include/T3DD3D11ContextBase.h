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


#ifndef __T3D_D3D11_CONTEXT_BASE_H__
#define __T3D_D3D11_CONTEXT_BASE_H__


#include <Tiny3D.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <algorithm>
#undef min


namespace Tiny3D
{
    class D3D11ContextBase;
    T3D_DECLARE_SMART_PTR(D3D11ContextBase);

    class D3D11ContextBase : public RHIContext
    {
    public:
        virtual ~D3D11ContextBase();

        // D3D11 shared: depth range conversion [0,1]
        TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat) override;

        // Shader compilation and reflection (shared by Console and Runtime)
        TResult compileShader(ShaderVariant *shader) override;
        TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams) override;
        TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams) override;

    protected:
        D3D11ContextBase();
    };
}


#endif  /*__T3D_D3D11_CONTEXT_BASE_H__*/
