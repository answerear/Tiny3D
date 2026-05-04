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


#include "T3DVKConsoleContext.h"
#include "T3DVKConsoleRenderBuffer.h"
#include "T3DVKConsoleRenderState.h"
#include "T3DVKConsoleShader.h"
#include "T3DVKConsoleWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKConsoleContextPtr VKConsoleContext::create()
    {
        return T3D_NEW VKConsoleContext();
    }

    //--------------------------------------------------------------------------

    VKConsoleContext::VKConsoleContext()
    {
    }

    //--------------------------------------------------------------------------

    VKConsoleContext::~VKConsoleContext()
    {
    }

    //--------------------------------------------------------------------------

    RHIRenderTargetPtr VKConsoleContext::createRenderWindow(RenderWindow *renderWindow)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr VKConsoleContext::createRenderTexture(PixelBuffer2D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setRenderTarget(RenderTarget *renderTarget)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::resetRenderTarget()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setViewport(const Viewport &viewport)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::clearColor(const ColorRGB &color)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::clearDepth(Real depth)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::clearDepthStencil(Real depth, uint32_t stencil)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIBlendStatePtr VKConsoleContext::createBlendState(BlendState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIDepthStencilStatePtr VKConsoleContext::createDepthStencilState(DepthStencilState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIRasterizerStatePtr VKConsoleContext::createRasterizerState(RasterizerState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHISamplerStatePtr VKConsoleContext::createSamplerState(SamplerState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setBlendState(BlendState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setDepthStencilState(DepthStencilState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setRasterizerState(RasterizerState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr VKConsoleContext::createVertexDeclaration(VertexDeclaration *decl)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setVertexDeclaration(VertexDeclaration *decl)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexBufferPtr VKConsoleContext::createVertexBuffer(VertexBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setVertexBuffers(uint32_t startSlot,
        const VertexBuffers &buffers, const VertexStrides &strides,
        const VertexOffsets &offsets)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIIndexBufferPtr VKConsoleContext::createIndexBuffer(IndexBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setIndexBuffer(IndexBuffer *buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIConstantBufferPtr VKConsoleContext::createConstantBuffer(ConstantBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer1DPtr VKConsoleContext::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr VKConsoleContext::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer3DPtr VKConsoleContext::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKConsoleContext::createVertexShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setVertexShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setVSConstantBuffers(uint32_t startSlot,
        const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setVSPixelBuffers(uint32_t startSlot,
        const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setVSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKConsoleContext::createPixelShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setPixelShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setPSConstantBuffers(uint32_t startSlot,
        const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setPSPixelBuffers(uint32_t startSlot,
        const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setPSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKConsoleContext::createHullShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setHullShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setHSConstantBuffers(uint32_t startSlot,
        const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setHSPixelBuffers(uint32_t startSlot,
        const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setHSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKConsoleContext::createDomainShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setDomainShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setDSConstantBuffers(uint32_t startSlot,
        const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setDSPixelBuffers(uint32_t startSlot,
        const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setDSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKConsoleContext::createGeometryShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setGeometryShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setGSConstantBuffers(uint32_t startSlot,
        const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setGSPixelBuffers(uint32_t startSlot,
        const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setGSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKConsoleContext::createComputeShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setComputeShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setCSConstantBuffers(uint32_t startSlot,
        const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setCSPixelBuffers(uint32_t startSlot,
        const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setCSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::setPrimitiveType(PrimitiveType primitive)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::render(uint32_t indexCount, uint32_t startIndex,
        uint32_t baseVertex)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::render(uint32_t vertexCount, uint32_t startVertex)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::reset()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::blit(RenderTarget *src, RenderTarget *dst,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::blit(Texture *src, RenderTarget *dst,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::blit(RenderTarget *src, Texture *dst,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::blit(Texture *src, Texture *dst,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::copyBuffer(RenderBuffer *src, RenderBuffer *dst,
        size_t srcOffset, size_t size, size_t dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKConsoleContext::writeBuffer(RenderBuffer *renderBuffer,
        const Buffer &buffer, bool discardWholeBuffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
