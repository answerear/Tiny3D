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


#ifndef __T3D_NULL_PREREQUISITES_H__
#define __T3D_NULL_PREREQUISITES_H__


#include <Tiny3D.h>


#include <algorithm>
#undef min


#if defined NULLRENDERER_EXPORT
    #define T3D_NULLRENDERER_API        T3D_EXPORT_API
#else
    #define T3D_NULLRENDERER_API        T3D_IMPORT_API
#endif



namespace Tiny3D
{
    #define LOG_TAG_NULLRENDERER        "NullRenderer"

    class NullRenderer;
    class NullContext;
    class NullWindow;
    class NullVertexDeclaration;
    class NullVertexBuffer;
    class NullIndexBuffer;
    class NullConstantBuffer;
    class NullPixelBuffer1D;
    class NullPixelBuffer2D;
    class NullPixelBuffer3D;
    class NullBlendState;
    class NullRasterizerState;
    class NullDepthStencilState;
    class NullSamplerState;
    class NullVertexShader;
    class NullHullShader;
    class NullDomainShader;
    class NullGeometryShader;
    class NullPixelShader;
    class NullComputeShader;

    T3D_DECLARE_SMART_PTR(NullRenderer);
    T3D_DECLARE_SMART_PTR(NullContext);
    T3D_DECLARE_SMART_PTR(NullWindow);
    T3D_DECLARE_SMART_PTR(NullVertexDeclaration);
    T3D_DECLARE_SMART_PTR(NullVertexBuffer);
    T3D_DECLARE_SMART_PTR(NullIndexBuffer);
    T3D_DECLARE_SMART_PTR(NullConstantBuffer);
    T3D_DECLARE_SMART_PTR(NullPixelBuffer1D);
    T3D_DECLARE_SMART_PTR(NullPixelBuffer2D);
    T3D_DECLARE_SMART_PTR(NullPixelBuffer3D);
    T3D_DECLARE_SMART_PTR(NullBlendState);
    T3D_DECLARE_SMART_PTR(NullRasterizerState);
    T3D_DECLARE_SMART_PTR(NullDepthStencilState);
    T3D_DECLARE_SMART_PTR(NullSamplerState);
    T3D_DECLARE_SMART_PTR(NullVertexShader);
    T3D_DECLARE_SMART_PTR(NullHullShader);
    T3D_DECLARE_SMART_PTR(NullDomainShader);
    T3D_DECLARE_SMART_PTR(NullGeometryShader);
    T3D_DECLARE_SMART_PTR(NullPixelShader);
    T3D_DECLARE_SMART_PTR(NullComputeShader);
}


#endif  /*__T3D_NULL_PREREQUISITES_H__*/
