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
    
    RHIPixelBuffer2DPtr NullContext::createPixelBuffer2D(PixelBuffer2DPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer3DPtr NullContext::createPixelBuffer3D(PixelBuffer3DPtr buffer)
    {
        return nullptr;
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

    TResult NullContext::setVSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, const ConstantBufferPtr *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setVSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, const PixelBufferPtr *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setVSSampler(uint32_t startSlot, uint32_t numOfSamplers, const SamplerStatePtr *samplers)
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

    TResult NullContext::setPSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, const ConstantBufferPtr *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setPSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, const PixelBufferPtr *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setPSSampler(uint32_t startSlot, uint32_t numOfSamplers, const SamplerStatePtr *samplers)
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

    TResult NullContext::setHSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, const ConstantBufferPtr *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setHSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, const PixelBufferPtr *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setHSSampler(uint32_t startSlot, uint32_t numOfSamplers, const SamplerStatePtr *samplers)
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

    TResult NullContext::setDSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, const ConstantBufferPtr *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------d

    TResult NullContext::setDSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, const PixelBufferPtr *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setDSSampler(uint32_t startSlot, uint32_t numOfSamplers, const SamplerStatePtr *samplers)
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

    TResult NullContext::setGSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, const ConstantBufferPtr *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setGSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, const PixelBufferPtr *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setGSSampler(uint32_t startSlot, uint32_t numOfSamplers, const SamplerStatePtr *samplers)
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

    TResult NullContext::setCSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, const ConstantBufferPtr *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setCSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, const PixelBufferPtr *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setCSSampler(uint32_t startSlot, uint32_t numOfSamplers, const SamplerStatePtr *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::compileShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::reflectShaderAllBindings(ShaderVariantPtr shader, ShaderConstantBindings &constantBindings, ShaderTexSamplerBindings texSamplerBindings)
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

