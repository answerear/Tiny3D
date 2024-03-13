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

#ifndef __T3D_D3D11_RENDER_BUFFER_H__
#define __T3D_D3D11_RENDER_BUFFER_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    class D3D11VertexDeclaration : public RHIVertexDeclaration
    {
    public:
    };
    
    class D3D11VertexBuffer : public RHIVertexBuffer
    {
    public:
    };

    class D3D11IndexBuffer : public RHIIndexBuffer
    {
    public:
    };

    class D3D11ConstantBuffer : public RHIConstantBuffer
    {
        
    };

    class D3D11PixelBuffer1D : public RHIPixelBuffer1D
    {
        
    };

    class D3D11PixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static D3D11PixelBuffer2DPtr create();

        D3D11PixelBuffer2D();

        ~D3D11PixelBuffer2D() override;
        
        ID3D11Texture2D             *D3DTexture {nullptr};
        ID3D11RenderTargetView      *D3DRTView {nullptr};
        
        ID3D11Texture2D             *D3DDSTexture {nullptr};
        ID3D11DepthStencilView      *D3DDSView {nullptr};
        
        ID3D11ShaderResourceView    *D3DSRView {nullptr};
    };

    class D3D11PixelBuffer3D : public RHIPixelBuffer3D
    {
        
    };
}


#endif    /*__T3D_D3D11_RENDER_BUFFER_H__*/
