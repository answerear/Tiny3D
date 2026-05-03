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


#ifndef __T3D_D3D11CONSOLE_PREREQUISITES_H__
#define __T3D_D3D11CONSOLE_PREREQUISITES_H__


#include <Tiny3D.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <algorithm>
#undef min


#if defined T3D_D3D11CONSOLE_EXPORT
    #define T3D_D3D11CONSOLE_API        T3D_EXPORT_API
#else
    #define T3D_D3D11CONSOLE_API        T3D_IMPORT_API
#endif



namespace Tiny3D
{
    #define D3D_SAFE_RELEASE(p) \
        if ((p) != nullptr)   \
        {   \
            (p)->Release();   \
            (p) = nullptr;    \
        }

    #define LOG_TAG_D3D11CONSOLERENDERER        "D3D11RendererConsole"

    class D3D11RendererConsole;
    class D3D11ConsoleContext;
    class D3D11ConsoleWindow;
    class D3D11ConsoleVertexDeclaration;
    class D3D11ConsoleVertexBuffer;
    class D3D11ConsoleIndexBuffer;
    class D3D11ConsoleConstantBuffer;
    class D3D11ConsolePixelBuffer1D;
    class D3D11ConsolePixelBuffer2D;
    class D3D11ConsolePixelBuffer3D;
    class D3D11ConsoleBlendState;
    class D3D11ConsoleRasterizerState;
    class D3D11ConsoleDepthStencilState;
    class D3D11ConsoleSamplerState;
    class D3D11ConsoleVertexShader;
    class D3D11ConsoleHullShader;
    class D3D11ConsoleDomainShader;
    class D3D11ConsoleGeometryShader;
    class D3D11ConsolePixelShader;
    class D3D11ConsoleComputeShader;

    T3D_DECLARE_SMART_PTR(D3D11RendererConsole);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleContext);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleWindow);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleVertexDeclaration);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleVertexBuffer);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleIndexBuffer);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleConstantBuffer);
    T3D_DECLARE_SMART_PTR(D3D11ConsolePixelBuffer1D);
    T3D_DECLARE_SMART_PTR(D3D11ConsolePixelBuffer2D);
    T3D_DECLARE_SMART_PTR(D3D11ConsolePixelBuffer3D);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleBlendState);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleRasterizerState);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleDepthStencilState);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleSamplerState);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleVertexShader);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleHullShader);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleDomainShader);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleGeometryShader);
    T3D_DECLARE_SMART_PTR(D3D11ConsolePixelShader);
    T3D_DECLARE_SMART_PTR(D3D11ConsoleComputeShader);
}


#endif  /*__T3D_D3D11CONSOLE_PREREQUISITES_H__*/
