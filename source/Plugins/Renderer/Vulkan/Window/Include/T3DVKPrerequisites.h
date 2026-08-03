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


#ifndef __T3D_VK_PREREQUISITES_H__
#define __T3D_VK_PREREQUISITES_H__


#include "T3DVKPrerequisitesBase.h"


#if defined VKRENDERER_EXPORT
    #define T3D_VKRENDERER_API        T3D_EXPORT_API
#else
    #define T3D_VKRENDERER_API        T3D_IMPORT_API
#endif



namespace Tiny3D
{
    class VKRenderer;
    class VKContext;
    class VKBlendState;
    class VKDepthStencilState;
    class VKRasterizerState;
    class VKSamplerState;
    class VKRenderWindow;
    class VKVertexDeclaration;
    class VKVertexBuffer;
    class VKIndexBuffer;
    class VKConstantBuffer;
    class VKPixelBuffer1D;
    class VKPixelBuffer2D;
    class VKPixelBuffer3D;
    class VKPixelBufferCubemap;
    class VKShader;
    class VKVertexShader;
    class VKPixelShader;
    class VKHullShader;
    class VKDomainShader;
    class VKGeometryShader;
    class VKComputeShader;

    T3D_DECLARE_SMART_PTR(VKRenderer);
    T3D_DECLARE_SMART_PTR(VKContext);
    T3D_DECLARE_SMART_PTR(VKBlendState);
    T3D_DECLARE_SMART_PTR(VKDepthStencilState);
    T3D_DECLARE_SMART_PTR(VKRasterizerState);
    T3D_DECLARE_SMART_PTR(VKSamplerState);
    T3D_DECLARE_SMART_PTR(VKRenderWindow);
    T3D_DECLARE_SMART_PTR(VKVertexDeclaration);
    T3D_DECLARE_SMART_PTR(VKVertexBuffer);
    T3D_DECLARE_SMART_PTR(VKIndexBuffer);
    T3D_DECLARE_SMART_PTR(VKConstantBuffer);
    T3D_DECLARE_SMART_PTR(VKPixelBuffer1D);
    T3D_DECLARE_SMART_PTR(VKPixelBuffer2D);
    T3D_DECLARE_SMART_PTR(VKPixelBuffer3D);
    T3D_DECLARE_SMART_PTR(VKPixelBufferCubemap);
    T3D_DECLARE_SMART_PTR(VKShader);
    T3D_DECLARE_SMART_PTR(VKVertexShader);
    T3D_DECLARE_SMART_PTR(VKPixelShader);
    T3D_DECLARE_SMART_PTR(VKHullShader);
    T3D_DECLARE_SMART_PTR(VKDomainShader);
    T3D_DECLARE_SMART_PTR(VKGeometryShader);
    T3D_DECLARE_SMART_PTR(VKComputeShader);
}


#endif  /*__T3D_VK_PREREQUISITES_H__*/
