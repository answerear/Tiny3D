/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

        D3D11PixelBuffer2D() = default;

        virtual ~D3D11PixelBuffer2D();
        
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
