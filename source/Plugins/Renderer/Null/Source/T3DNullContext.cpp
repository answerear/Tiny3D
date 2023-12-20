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


#include "T3DNullContext.h"
#include "T3DNullWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullContextPtr NullContext::create()
    {
        NullContextPtr renderer = new NullContext();
        // renderer->release();
        return renderer;
    }

    //--------------------------------------------------------------------------

    NullContext::NullContext()
    {
        
    }

    //--------------------------------------------------------------------------

    NullContext::~NullContext()
    {
        
    }

    //--------------------------------------------------------------------------

    RHIRenderTargetPtr NullContext::createRenderWindow(RenderWindowPtr renderWindow)
    {
        return NullWindow::create();
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr NullContext::createRenderTexture(PixelBuffer2DPtr buffer)
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    TResult NullContext::setRenderTarget(RenderWindowPtr renderWindow)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setRenderTarget(RenderTexturePtr renderTexture)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setRenderTarget(RenderTargetPtr renderTarget)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::resetRenderTarget()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setViewport(const Viewport &viewport)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::clearColor(const ColorRGB &color)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::clearDepthStencil(Real depth, uint32_t stencil)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIBlendStatePtr NullContext::createBlendState(BlendStatePtr state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIDepthStencilStatePtr NullContext::createDepthStencilState(DepthStencilStatePtr state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIRasterizerStatePtr NullContext::createRasterizerState(RasterizerStatePtr state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHISamplerStatePtr NullContext::createSamplerState(SamplerStatePtr state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setBlendState(BlendStatePtr state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setDepthStencilState(DepthStencilStatePtr state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setRasterizerState(RasterizerStatePtr state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setSamplerState(SamplerStatePtr state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr NullContext::createVertexDeclaration(VertexDeclarationPtr decl)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setVertexDeclaration(VertexDeclarationPtr decl)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    RHIVertexBufferPtr NullContext::createVertexBuffer(VertexBufferPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setVertexBuffer(VertexBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIIndexBufferPtr NullContext::createIndexBuffer(IndexBufferPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setIndexBuffer(IndexBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIConstantBufferPtr NullContext::createConstantBuffer(ConstantBufferPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer1DPtr NullContext::createPixelBuffer1D(PixelBuffer1DPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setPixelBuffer1D(PixelBuffer1DPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr NullContext::createPixelBuffer2D(PixelBuffer2DPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setPixelBuffer2D(PixelBuffer2DPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer3DPtr NullContext::createPixelBuffer3D(PixelBuffer3DPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setPixelBuffer3D(PixelBuffer3DPtr buffer)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    RHIShaderPtr NullContext::createVertexShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setVertexShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setVSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setVSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr NullContext::createPixelShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setPixelShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setPSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setPSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr NullContext::createHullShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setHullShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setHSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setHSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr NullContext::createDomainShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setDomainShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setDSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setDSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr NullContext::createGeometryShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setGeometryShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setGSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setGSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr NullContext::createComputeShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setComputeShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setCSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setCSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::render()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::reset()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::blit(RenderTargetPtr src, RenderTargetPtr dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::blit(TexturePtr src, RenderTargetPtr dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::blit(RenderTargetPtr src, TexturePtr dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::blit(TexturePtr src, TexturePtr dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::copyBuffer(RenderBufferPtr src, RenderBufferPtr dst, size_t srcOffset, size_t size, size_t dstOffset)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
}

