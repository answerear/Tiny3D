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

    RHIRenderTargetPtr NullContext::createRenderWindow(RenderWindow *renderWindow)
    {
        return NullWindow::create();
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr NullContext::createRenderTexture(PixelBuffer2D *buffer)
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    TResult NullContext::setRenderTarget(RenderWindow *renderWindow)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setRenderTarget(RenderTexture *renderTexture)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setRenderTarget(RenderTarget *renderTarget)
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
    
    RHIBlendStatePtr NullContext::createBlendState(BlendState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIDepthStencilStatePtr NullContext::createDepthStencilState(DepthStencilState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIRasterizerStatePtr NullContext::createRasterizerState(RasterizerState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHISamplerStatePtr NullContext::createSamplerState(SamplerState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setBlendState(BlendState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setDepthStencilState(DepthStencilState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setRasterizerState(RasterizerState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr NullContext::createVertexDeclaration(VertexDeclaration *decl)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setVertexDeclaration(VertexDeclaration *decl)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    RHIVertexBufferPtr NullContext::createVertexBuffer(VertexBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setVertexBuffers(uint32_t startSlot, uint32_t numOfBuffers, VertexBuffer * const *buffers, const uint32_t *strides, const uint32_t *offsets)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIIndexBufferPtr NullContext::createIndexBuffer(IndexBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setIndexBuffer(IndexBuffer *buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIConstantBufferPtr NullContext::createConstantBuffer(ConstantBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer1DPtr NullContext::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr NullContext::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer3DPtr NullContext::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr NullContext::createVertexShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setVertexShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setVSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setVSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setVSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr NullContext::createPixelShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setPixelShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setPSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setPSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setPSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    RHIShaderPtr NullContext::createHullShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setHullShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setHSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setHSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setHSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr NullContext::createDomainShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setDomainShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setDSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setDSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setDSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr NullContext::createGeometryShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setGeometryShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setGSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setGSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setGSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr NullContext::createComputeShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult NullContext::setComputeShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setCSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullContext::setCSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult NullContext::setCSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }


    //--------------------------------------------------------------------------

    TResult NullContext::compileShader(ShaderVariant *shader)
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

