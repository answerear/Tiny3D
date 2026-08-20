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


#ifndef __T3D_METAL_PREREQUISITES_H__
#define __T3D_METAL_PREREQUISITES_H__


#include <Tiny3D.h>


#include <algorithm>
#undef min


#if defined METALRENDERER_EXPORT
    #define T3D_METALRENDERER_API        T3D_EXPORT_API
#else
    #define T3D_METALRENDERER_API        T3D_IMPORT_API
#endif


namespace Tiny3D
{
    #define LOG_TAG_METALRENDERER        "MetalRenderer"

    class MetalRenderer;
    class MetalContext;
    class MetalWindow;
    class MetalVertexDeclaration;
    class MetalVertexBuffer;
    class MetalIndexBuffer;
    class MetalConstantBuffer;
    class MetalPixelBuffer1D;
    class MetalPixelBuffer2D;
    class MetalPixelBuffer3D;
    class MetalPixelBufferCubemap;
    class MetalBlendState;
    class MetalRasterizerState;
    class MetalDepthStencilState;
    class MetalSamplerState;
    class MetalVertexShader;
    class MetalHullShader;
    class MetalDomainShader;
    class MetalGeometryShader;
    class MetalPixelShader;
    class MetalComputeShader;

    T3D_DECLARE_SMART_PTR(MetalRenderer);
    T3D_DECLARE_SMART_PTR(MetalContext);
    T3D_DECLARE_SMART_PTR(MetalWindow);
    T3D_DECLARE_SMART_PTR(MetalVertexDeclaration);
    T3D_DECLARE_SMART_PTR(MetalVertexBuffer);
    T3D_DECLARE_SMART_PTR(MetalIndexBuffer);
    T3D_DECLARE_SMART_PTR(MetalConstantBuffer);
    T3D_DECLARE_SMART_PTR(MetalPixelBuffer1D);
    T3D_DECLARE_SMART_PTR(MetalPixelBuffer2D);
    T3D_DECLARE_SMART_PTR(MetalPixelBuffer3D);
    T3D_DECLARE_SMART_PTR(MetalPixelBufferCubemap);
    T3D_DECLARE_SMART_PTR(MetalBlendState);
    T3D_DECLARE_SMART_PTR(MetalRasterizerState);
    T3D_DECLARE_SMART_PTR(MetalDepthStencilState);
    T3D_DECLARE_SMART_PTR(MetalSamplerState);
    T3D_DECLARE_SMART_PTR(MetalVertexShader);
    T3D_DECLARE_SMART_PTR(MetalHullShader);
    T3D_DECLARE_SMART_PTR(MetalDomainShader);
    T3D_DECLARE_SMART_PTR(MetalGeometryShader);
    T3D_DECLARE_SMART_PTR(MetalPixelShader);
    T3D_DECLARE_SMART_PTR(MetalComputeShader);
}


#endif  /*__T3D_METAL_PREREQUISITES_H__*/
