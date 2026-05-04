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


#ifndef __T3D_VK_CONSOLE_CONTEXT_H__
#define __T3D_VK_CONSOLE_CONTEXT_H__


#include "T3DVKContextBase.h"
#include "T3DVKConsolePrerequisites.h"


namespace Tiny3D
{
    class VKConsoleContext : public VKContextBase
    {
    public:
        static VKConsoleContextPtr create();

        ~VKConsoleContext() override;

        RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow) override;
        RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer) override;
        TResult setRenderTarget(RenderTarget *renderTarget) override;
        TResult resetRenderTarget() override;
        TResult setViewport(const Viewport &viewport) override;
        TResult clearColor(const ColorRGB &color) override;
        TResult clearDepth(Real depth) override;
        TResult clearDepthStencil(Real depth, uint32_t stencil) override;
        RHIBlendStatePtr createBlendState(BlendState *state) override;
        RHIDepthStencilStatePtr createDepthStencilState(DepthStencilState *state) override;
        RHIRasterizerStatePtr createRasterizerState(RasterizerState *state) override;
        RHISamplerStatePtr createSamplerState(SamplerState *state) override;
        TResult setBlendState(BlendState *state) override;
        TResult setDepthStencilState(DepthStencilState *state) override;
        TResult setRasterizerState(RasterizerState *state) override;
        RHIVertexDeclarationPtr createVertexDeclaration(VertexDeclaration *decl) override;
        TResult setVertexDeclaration(VertexDeclaration *decl) override;
        RHIVertexBufferPtr createVertexBuffer(VertexBuffer *buffer) override;
        TResult setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets) override;
        RHIIndexBufferPtr createIndexBuffer(IndexBuffer *buffer) override;
        TResult setIndexBuffer(IndexBuffer *buffer) override;
        RHIConstantBufferPtr createConstantBuffer(ConstantBuffer *buffer) override;
        RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1D *buffer) override;
        RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2D *buffer) override;
        RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3D *buffer) override;
        RHIShaderPtr createVertexShader(ShaderVariant *shader) override;
        TResult setVertexShader(ShaderVariant *shader) override;
        TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createPixelShader(ShaderVariant *shader) override;
        TResult setPixelShader(ShaderVariant *shader) override;
        TResult setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setPSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createHullShader(ShaderVariant *shader) override;
        TResult setHullShader(ShaderVariant *shader) override;
        TResult setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setHSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createDomainShader(ShaderVariant *shader) override;
        TResult setDomainShader(ShaderVariant *shader) override;
        TResult setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setDSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createGeometryShader(ShaderVariant *shader) override;
        TResult setGeometryShader(ShaderVariant *shader) override;
        TResult setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setGSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createComputeShader(ShaderVariant *shader) override;
        TResult setComputeShader(ShaderVariant *shader) override;
        TResult setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setCSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        TResult setPrimitiveType(PrimitiveType primitive) override;
        TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) override;
        TResult render(uint32_t vertexCount, uint32_t startVertex) override;
        TResult reset() override;
        TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(Texture *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset = 0, size_t size = 0, size_t dstOffset = 0) override;
        TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer = false) override;

    protected:
        VKConsoleContext();
    };
}


#endif  /*__T3D_VK_CONSOLE_CONTEXT_H__*/
