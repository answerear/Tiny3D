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


#ifndef __T3D_NULLD3D11_PREREQUISITES_H__
#define __T3D_NULLD3D11_PREREQUISITES_H__


#include <Tiny3D.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <algorithm>
#undef min


#if defined T3D_NULLD3D11_EXPORT
    #define T3D_NULLD3D11_API        T3D_EXPORT_API
#else
    #define T3D_NULLD3D11_API        T3D_IMPORT_API
#endif



namespace Tiny3D
{
    #define D3D_SAFE_RELEASE(p) \
        if ((p) != nullptr)   \
        {   \
            (p)->Release();   \
            (p) = nullptr;    \
        }

    #define LOG_TAG_NULLD3D11RENDERER        "NullD3D11Renderer"

    class NullD3D11Renderer;
    class NullD3D11Context;
    class NullD3D11Window;
    class NullD3D11VertexDeclaration;
    class NullD3D11VertexBuffer;
    class NullD3D11IndexBuffer;
    class NullD3D11ConstantBuffer;
    class NullD3D11PixelBuffer1D;
    class NullD3D11PixelBuffer2D;
    class NullD3D11PixelBuffer3D;
    class NullD3D11BlendState;
    class NullD3D11RasterizerState;
    class NullD3D11DepthStencilState;
    class NullD3D11SamplerState;
    class NullD3D11VertexShader;
    class NullD3D11HullShader;
    class NullD3D11DomainShader;
    class NullD3D11GeometryShader;
    class NullD3D11PixelShader;
    class NullD3D11ComputeShader;

    T3D_DECLARE_SMART_PTR(NullD3D11Renderer);
    T3D_DECLARE_SMART_PTR(NullD3D11Context);
    T3D_DECLARE_SMART_PTR(NullD3D11Window);
    T3D_DECLARE_SMART_PTR(NullD3D11VertexDeclaration);
    T3D_DECLARE_SMART_PTR(NullD3D11VertexBuffer);
    T3D_DECLARE_SMART_PTR(NullD3D11IndexBuffer);
    T3D_DECLARE_SMART_PTR(NullD3D11ConstantBuffer);
    T3D_DECLARE_SMART_PTR(NullD3D11PixelBuffer1D);
    T3D_DECLARE_SMART_PTR(NullD3D11PixelBuffer2D);
    T3D_DECLARE_SMART_PTR(NullD3D11PixelBuffer3D);
    T3D_DECLARE_SMART_PTR(NullD3D11BlendState);
    T3D_DECLARE_SMART_PTR(NullD3D11RasterizerState);
    T3D_DECLARE_SMART_PTR(NullD3D11DepthStencilState);
    T3D_DECLARE_SMART_PTR(NullD3D11SamplerState);
    T3D_DECLARE_SMART_PTR(NullD3D11VertexShader);
    T3D_DECLARE_SMART_PTR(NullD3D11HullShader);
    T3D_DECLARE_SMART_PTR(NullD3D11DomainShader);
    T3D_DECLARE_SMART_PTR(NullD3D11GeometryShader);
    T3D_DECLARE_SMART_PTR(NullD3D11PixelShader);
    T3D_DECLARE_SMART_PTR(NullD3D11ComputeShader);
}


#endif  /*__T3D_NULLD3D11_PREREQUISITES_H__*/
