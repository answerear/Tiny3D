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


#ifndef __T3D_VKCONSOLE_PREREQUISITES_H__
#define __T3D_VKCONSOLE_PREREQUISITES_H__


#include <Tiny3D.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


#if defined T3D_VKCONSOLE_EXPORT
    #define T3D_VKCONSOLE_API        T3D_EXPORT_API
#else
    #define T3D_VKCONSOLE_API        T3D_IMPORT_API
#endif



namespace Tiny3D
{
    #define LOG_TAG_VKRENDERCONSOLE        "VKRendererConsole"

    class VKRendererConsole;
    class VKContextBase;
    class VKConsoleContext;
    class VKConsoleWindow;
    class VKConsoleVertexDeclaration;
    class VKConsoleVertexBuffer;
    class VKConsoleIndexBuffer;
    class VKConsoleConstantBuffer;
    class VKConsolePixelBuffer1D;
    class VKConsolePixelBuffer2D;
    class VKConsolePixelBuffer3D;
    class VKConsoleBlendState;
    class VKConsoleRasterizerState;
    class VKConsoleDepthStencilState;
    class VKConsoleSamplerState;
    class VKConsoleVertexShader;
    class VKConsoleHullShader;
    class VKConsoleDomainShader;
    class VKConsoleGeometryShader;
    class VKConsolePixelShader;
    class VKConsoleComputeShader;

    T3D_DECLARE_SMART_PTR(VKRendererConsole);
    T3D_DECLARE_SMART_PTR(VKContextBase);
    T3D_DECLARE_SMART_PTR(VKConsoleContext);
    T3D_DECLARE_SMART_PTR(VKConsoleWindow);
    T3D_DECLARE_SMART_PTR(VKConsoleVertexDeclaration);
    T3D_DECLARE_SMART_PTR(VKConsoleVertexBuffer);
    T3D_DECLARE_SMART_PTR(VKConsoleIndexBuffer);
    T3D_DECLARE_SMART_PTR(VKConsoleConstantBuffer);
    T3D_DECLARE_SMART_PTR(VKConsolePixelBuffer1D);
    T3D_DECLARE_SMART_PTR(VKConsolePixelBuffer2D);
    T3D_DECLARE_SMART_PTR(VKConsolePixelBuffer3D);
    T3D_DECLARE_SMART_PTR(VKConsoleBlendState);
    T3D_DECLARE_SMART_PTR(VKConsoleRasterizerState);
    T3D_DECLARE_SMART_PTR(VKConsoleDepthStencilState);
    T3D_DECLARE_SMART_PTR(VKConsoleSamplerState);
    T3D_DECLARE_SMART_PTR(VKConsoleVertexShader);
    T3D_DECLARE_SMART_PTR(VKConsoleHullShader);
    T3D_DECLARE_SMART_PTR(VKConsoleDomainShader);
    T3D_DECLARE_SMART_PTR(VKConsoleGeometryShader);
    T3D_DECLARE_SMART_PTR(VKConsolePixelShader);
    T3D_DECLARE_SMART_PTR(VKConsoleComputeShader);
}


#endif  /*__T3D_VKCONSOLE_PREREQUISITES_H__*/
