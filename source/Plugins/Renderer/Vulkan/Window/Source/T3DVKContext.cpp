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


#include "T3DVKContext.h"
#include "T3DVKError.h"
#include "T3DVKMapping.h"
#include "T3DVKRenderBuffer.h"
#include "T3DVKRenderState.h"
#include "T3DVKRenderWindow.h"
#include "T3DVKShader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKContextPtr VKContext::create()
    {
        return T3D_NEW VKContext();
    }

    //--------------------------------------------------------------------------

    VKContext::VKContext()
    {
    }

    //--------------------------------------------------------------------------

    VKContext::~VKContext()
    {
    }

    //--------------------------------------------------------------------------

    TResult VKContext::init()
    {
        TResult ret = T3D_OK;
        // TODO: Initialize Vulkan instance, device, queues
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::swapBackBuffer(VKRenderWindow *renderWindow)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIRenderTargetPtr VKContext::createRenderWindow(RenderWindow *renderWindow)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr VKContext::createRenderTexture(PixelBuffer2D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setRenderTarget(RenderTarget *renderTarget)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::resetRenderTarget()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setViewport(const Viewport &viewport)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::clearColor(const ColorRGB &color)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::clearDepth(Real depth)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::clearDepthStencil(Real depth, uint32_t stencil)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIBlendStatePtr VKContext::createBlendState(BlendState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIDepthStencilStatePtr VKContext::createDepthStencilState(DepthStencilState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIRasterizerStatePtr VKContext::createRasterizerState(RasterizerState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHISamplerStatePtr VKContext::createSamplerState(SamplerState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setBlendState(BlendState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setDepthStencilState(DepthStencilState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setRasterizerState(RasterizerState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr VKContext::createVertexDeclaration(VertexDeclaration *decl)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVertexDeclaration(VertexDeclaration *decl)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexBufferPtr VKContext::createVertexBuffer(VertexBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers,
        const VertexStrides &strides, const VertexOffsets &offsets)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIIndexBufferPtr VKContext::createIndexBuffer(IndexBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setIndexBuffer(IndexBuffer *buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIConstantBufferPtr VKContext::createConstantBuffer(ConstantBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer1DPtr VKContext::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr VKContext::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer3DPtr VKContext::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createVertexShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVertexShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createPixelShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setPixelShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setPSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createHullShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setHullShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setHSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createDomainShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setDomainShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setDSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createGeometryShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setGeometryShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setGSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createComputeShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setComputeShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setCSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setPrimitiveType(PrimitiveType primitive)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::render(uint32_t vertexCount, uint32_t startVertex)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::reset()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::blit(RenderTarget *src, RenderTarget *dst,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::blit(Texture *src, RenderTarget *dst,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::blit(RenderTarget *src, Texture *dst,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::blit(Texture *src, Texture *dst,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::copyBuffer(RenderBuffer *src, RenderBuffer *dst,
        size_t srcOffset, size_t size, size_t dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::writeBuffer(RenderBuffer *renderBuffer,
        const Buffer &buffer, bool discardWholeBuffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
