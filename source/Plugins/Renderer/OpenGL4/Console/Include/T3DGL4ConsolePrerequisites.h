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


#ifndef __T3D_GL4CONSOLE_PREREQUISITES_H__
#define __T3D_GL4CONSOLE_PREREQUISITES_H__


#include "T3DGL4PrerequisitesBase.h"


#if defined T3D_GL4CONSOLE_EXPORT
    #define T3D_GL4CONSOLE_API        T3D_EXPORT_API
#else
    #define T3D_GL4CONSOLE_API        T3D_IMPORT_API
#endif


namespace Tiny3D
{
    #define LOG_TAG_GL4CONSOLERENDERER        "GL4RendererConsole"

    class GL4RendererConsole;
    class GL4ConsoleContext;
    class GL4ConsoleWindow;
    class GL4ConsoleVertexDeclaration;
    class GL4ConsoleVertexBuffer;
    class GL4ConsoleIndexBuffer;
    class GL4ConsoleConstantBuffer;
    class GL4ConsolePixelBuffer1D;
    class GL4ConsolePixelBuffer2D;
    class GL4ConsolePixelBuffer3D;
    class GL4ConsoleBlendState;
    class GL4ConsoleRasterizerState;
    class GL4ConsoleDepthStencilState;
    class GL4ConsoleSamplerState;
    class GL4ConsoleShader;
    class GL4ConsoleVertexShader;
    class GL4ConsoleHullShader;
    class GL4ConsoleDomainShader;
    class GL4ConsoleGeometryShader;
    class GL4ConsolePixelShader;
    class GL4ConsoleComputeShader;

    T3D_DECLARE_SMART_PTR(GL4RendererConsole);
    T3D_DECLARE_SMART_PTR(GL4ConsoleContext);
    T3D_DECLARE_SMART_PTR(GL4ConsoleWindow);
    T3D_DECLARE_SMART_PTR(GL4ConsoleVertexDeclaration);
    T3D_DECLARE_SMART_PTR(GL4ConsoleVertexBuffer);
    T3D_DECLARE_SMART_PTR(GL4ConsoleIndexBuffer);
    T3D_DECLARE_SMART_PTR(GL4ConsoleConstantBuffer);
    T3D_DECLARE_SMART_PTR(GL4ConsolePixelBuffer1D);
    T3D_DECLARE_SMART_PTR(GL4ConsolePixelBuffer2D);
    T3D_DECLARE_SMART_PTR(GL4ConsolePixelBuffer3D);
    T3D_DECLARE_SMART_PTR(GL4ConsoleBlendState);
    T3D_DECLARE_SMART_PTR(GL4ConsoleRasterizerState);
    T3D_DECLARE_SMART_PTR(GL4ConsoleDepthStencilState);
    T3D_DECLARE_SMART_PTR(GL4ConsoleSamplerState);
    T3D_DECLARE_SMART_PTR(GL4ConsoleShader);
    T3D_DECLARE_SMART_PTR(GL4ConsoleVertexShader);
    T3D_DECLARE_SMART_PTR(GL4ConsoleHullShader);
    T3D_DECLARE_SMART_PTR(GL4ConsoleDomainShader);
    T3D_DECLARE_SMART_PTR(GL4ConsoleGeometryShader);
    T3D_DECLARE_SMART_PTR(GL4ConsolePixelShader);
    T3D_DECLARE_SMART_PTR(GL4ConsoleComputeShader);
}


#endif  /*__T3D_GL4CONSOLE_PREREQUISITES_H__*/
