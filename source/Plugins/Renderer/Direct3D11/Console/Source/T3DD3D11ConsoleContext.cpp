/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DD3D11ConsoleContext.h"
#include "T3DD3D11ConsoleWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11ConsoleContextPtr D3D11ConsoleContext::create()
    {
        return T3D_NEW D3D11ConsoleContext();
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleContext::D3D11ConsoleContext()
    {
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleContext::~D3D11ConsoleContext()
    {
    }

    //--------------------------------------------------------------------------

    RHIRenderTargetPtr D3D11ConsoleContext::createRenderWindow(RenderWindow *renderWindow)
    {
        return D3D11ConsoleWindow::create();
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr D3D11ConsoleContext::createRenderTexture(PixelBuffer2D *buffer) { return nullptr; }
    TResult D3D11ConsoleContext::setRenderTarget(RenderTarget *renderTarget) { return T3D_OK; }
    TResult D3D11ConsoleContext::resetRenderTarget() { return T3D_OK; }
    TResult D3D11ConsoleContext::setViewport(const Viewport &viewport) { return T3D_OK; }
    TResult D3D11ConsoleContext::setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height) { return T3D_OK; }
    TResult D3D11ConsoleContext::clearColor(const ColorRGB &color) { return T3D_OK; }
    TResult D3D11ConsoleContext::clearDepth(Real depth) { return T3D_OK; }
    TResult D3D11ConsoleContext::clearDepthStencil(Real depth, uint32_t stencil) { return T3D_OK; }
    RHIBlendStatePtr D3D11ConsoleContext::createBlendState(BlendState *state) { return nullptr; }
    RHIDepthStencilStatePtr D3D11ConsoleContext::createDepthStencilState(DepthStencilState *state) { return nullptr; }
    RHIRasterizerStatePtr D3D11ConsoleContext::createRasterizerState(RasterizerState *state) { return nullptr; }
    RHISamplerStatePtr D3D11ConsoleContext::createSamplerState(SamplerState *state) { return nullptr; }
    TResult D3D11ConsoleContext::setBlendState(BlendState *state) { return T3D_OK; }
    TResult D3D11ConsoleContext::setDepthStencilState(DepthStencilState *state) { return T3D_OK; }
    TResult D3D11ConsoleContext::setRasterizerState(RasterizerState *state) { return T3D_OK; }
    RHIVertexDeclarationPtr D3D11ConsoleContext::createVertexDeclaration(VertexDeclaration *decl) { return nullptr; }
    TResult D3D11ConsoleContext::setVertexDeclaration(VertexDeclaration *decl) { return T3D_OK; }
    RHIVertexBufferPtr D3D11ConsoleContext::createVertexBuffer(VertexBuffer *buffer) { return nullptr; }
    TResult D3D11ConsoleContext::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets) { return T3D_OK; }
    RHIIndexBufferPtr D3D11ConsoleContext::createIndexBuffer(IndexBuffer *buffer) { return nullptr; }
    TResult D3D11ConsoleContext::setIndexBuffer(IndexBuffer *buffer) { return T3D_OK; }
    RHIConstantBufferPtr D3D11ConsoleContext::createConstantBuffer(ConstantBuffer *buffer) { return nullptr; }
    RHIPixelBuffer1DPtr D3D11ConsoleContext::createPixelBuffer1D(PixelBuffer1D *buffer) { return nullptr; }
    RHIPixelBuffer2DPtr D3D11ConsoleContext::createPixelBuffer2D(PixelBuffer2D *buffer) { return nullptr; }
    RHIPixelBuffer3DPtr D3D11ConsoleContext::createPixelBuffer3D(PixelBuffer3D *buffer) { return nullptr; }
    RHIPixelBufferCubemapPtr D3D11ConsoleContext::createPixelBufferCubemap(PixelBufferCubemap *buffer) { return nullptr; }
    RHIShaderPtr D3D11ConsoleContext::createVertexShader(ShaderVariant *shader) { return nullptr; }
    TResult D3D11ConsoleContext::setVertexShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setVSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIShaderPtr D3D11ConsoleContext::createPixelShader(ShaderVariant *shader) { return nullptr; }
    TResult D3D11ConsoleContext::setPixelShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setPSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIShaderPtr D3D11ConsoleContext::createHullShader(ShaderVariant *shader) { return nullptr; }
    TResult D3D11ConsoleContext::setHullShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setHSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIShaderPtr D3D11ConsoleContext::createDomainShader(ShaderVariant *shader) { return nullptr; }
    TResult D3D11ConsoleContext::setDomainShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setDSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIShaderPtr D3D11ConsoleContext::createGeometryShader(ShaderVariant *shader) { return nullptr; }
    TResult D3D11ConsoleContext::setGeometryShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setGSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIShaderPtr D3D11ConsoleContext::createComputeShader(ShaderVariant *shader) { return nullptr; }
    TResult D3D11ConsoleContext::setComputeShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setCSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setPrimitiveType(PrimitiveType primitive) { return T3D_OK; }
    TResult D3D11ConsoleContext::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) { return T3D_OK; }
    TResult D3D11ConsoleContext::render(uint32_t vertexCount, uint32_t startVertex) { return T3D_OK; }
    TResult D3D11ConsoleContext::reset() { return T3D_OK; }
    TResult D3D11ConsoleContext::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult D3D11ConsoleContext::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult D3D11ConsoleContext::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult D3D11ConsoleContext::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult D3D11ConsoleContext::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset) { return T3D_OK; }
    TResult D3D11ConsoleContext::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer) { return T3D_OK; }
}
