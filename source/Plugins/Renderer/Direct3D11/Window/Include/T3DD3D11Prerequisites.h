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


#ifndef __T3D_D3D11_PREREQUISITES_H__
#define __T3D_D3D11_PREREQUISITES_H__


#include "T3DD3D11PrerequisitesBase.h"


#if defined D3D11RENDERER_EXPORT
    #define T3D_D3D11RENDERER_API        T3D_EXPORT_API
#else
    #define T3D_D3D11RENDERER_API        T3D_IMPORT_API
#endif



namespace Tiny3D
{
    class D3D11Renderer;
    class D3D11Context;
    class D3D11Capabilities;
    class D3D11BlendState;
    class D3D11DepthStencilState;
    class D3D11RasterizerState;
    class D3D11SamplerState;
    class D3D11RenderWindow;
    class D3D11BufferManager;
    class D3D11Buffer;
    class D3D11VertexBuffer;
    class D3D11IndexBuffer;
    class D3D11PixelBuffer1D;
    class D3D11PixelBuffer2D;
    class D3D11PixelBuffer3D;
    class D3D11PixelBufferCubemap;
    class D3D11ConstantBuffer;
    class D3D11VertexDeclaration;
    class D3D11Shader;
    class D3D11VertexShader;
    class D3D11PixelShader;
    class D3D11HullShader;
    class D3D11DomainShader;
    class D3D11GeometryShader;
    class D3D11ComputeShader;
    class D3D11BlendState;
    class D3D11DepthStencilState;
    class D3D11RasterizerState;
    class D3D11SamplerState;

    T3D_DECLARE_SMART_PTR(D3D11Renderer);
    T3D_DECLARE_SMART_PTR(D3D11Context);
    T3D_DECLARE_SMART_PTR(D3D11Capabilities);
    T3D_DECLARE_SMART_PTR(D3D11BlendState);
    T3D_DECLARE_SMART_PTR(D3D11DepthStencilState);
    T3D_DECLARE_SMART_PTR(D3D11RasterizerState);
    T3D_DECLARE_SMART_PTR(D3D11SamplerState);
    T3D_DECLARE_SMART_PTR(D3D11RenderWindow);
    T3D_DECLARE_SMART_PTR(D3D11BufferManager);
    T3D_DECLARE_SMART_PTR(D3D11Buffer);
    T3D_DECLARE_SMART_PTR(D3D11VertexBuffer);
    T3D_DECLARE_SMART_PTR(D3D11IndexBuffer);
    T3D_DECLARE_SMART_PTR(D3D11PixelBuffer1D);
    T3D_DECLARE_SMART_PTR(D3D11PixelBuffer2D);
    T3D_DECLARE_SMART_PTR(D3D11PixelBuffer3D);
    T3D_DECLARE_SMART_PTR(D3D11PixelBufferCubemap);
    T3D_DECLARE_SMART_PTR(D3D11ConstantBuffer);
    T3D_DECLARE_SMART_PTR(D3D11VertexDeclaration);
    T3D_DECLARE_SMART_PTR(D3D11Shader);
    T3D_DECLARE_SMART_PTR(D3D11VertexShader);
    T3D_DECLARE_SMART_PTR(D3D11PixelShader);
    T3D_DECLARE_SMART_PTR(D3D11HullShader);
    T3D_DECLARE_SMART_PTR(D3D11DomainShader);
    T3D_DECLARE_SMART_PTR(D3D11GeometryShader);
    T3D_DECLARE_SMART_PTR(D3D11ComputeShader);
    T3D_DECLARE_SMART_PTR(D3D11BlendState);
    T3D_DECLARE_SMART_PTR(D3D11DepthStencilState);
    T3D_DECLARE_SMART_PTR(D3D11RasterizerState);
    T3D_DECLARE_SMART_PTR(D3D11SamplerState);
}


#endif  /*__T3D_D3D11_PREREQUISITES_H__*/
