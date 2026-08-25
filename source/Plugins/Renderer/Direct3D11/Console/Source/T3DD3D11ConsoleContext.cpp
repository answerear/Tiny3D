/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DD3D11ConsoleContext.h"
#include "T3DD3D11ConsoleWindow.h"


namespace Tiny3D
{
    #define LOG_TAG_D3D11CONSOLE    "D3D11Console"

    /// 资源创建接口在离线 null backend 上不可用，静默返回 nullptr 会让误用在别处才崩，
    /// 所以这里留一条警告，把问题定位在调用现场
    #define T3D_CONSOLE_UNAVAILABLE()   T3D_LOG_WARNING(LOG_TAG_D3D11CONSOLE, "%s is not available in console context !", __FUNCTION__)

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

    RHIPixelBuffer2DPtr D3D11ConsoleContext::createRenderTexture(PixelBuffer2D *buffer) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    TResult D3D11ConsoleContext::setRenderTarget(RenderTarget *renderTarget) { return T3D_OK; }
    TResult D3D11ConsoleContext::resetRenderTarget() { return T3D_OK; }
    TResult D3D11ConsoleContext::setViewport(const Viewport &viewport) { return T3D_OK; }
    TResult D3D11ConsoleContext::setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height) { return T3D_OK; }
    TResult D3D11ConsoleContext::clearColor(const ColorRGB &color) { return T3D_OK; }
    TResult D3D11ConsoleContext::clearDepth(Real depth) { return T3D_OK; }
    TResult D3D11ConsoleContext::clearDepthStencil(Real depth, uint32_t stencil) { return T3D_OK; }
    RHIBlendStatePtr D3D11ConsoleContext::createBlendState(BlendState *state) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    RHIDepthStencilStatePtr D3D11ConsoleContext::createDepthStencilState(DepthStencilState *state) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    RHIRasterizerStatePtr D3D11ConsoleContext::createRasterizerState(RasterizerState *state) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    RHISamplerStatePtr D3D11ConsoleContext::createSamplerState(SamplerState *state) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    TResult D3D11ConsoleContext::setBlendState(BlendState *state) { return T3D_OK; }
    TResult D3D11ConsoleContext::setDepthStencilState(DepthStencilState *state) { return T3D_OK; }
    TResult D3D11ConsoleContext::setRasterizerState(RasterizerState *state) { return T3D_OK; }
    RHIVertexDeclarationPtr D3D11ConsoleContext::createVertexDeclaration(VertexDeclaration *decl) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    TResult D3D11ConsoleContext::setVertexDeclaration(VertexDeclaration *decl) { return T3D_OK; }
    RHIVertexBufferPtr D3D11ConsoleContext::createVertexBuffer(VertexBuffer *buffer) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    TResult D3D11ConsoleContext::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets) { return T3D_OK; }
    RHIIndexBufferPtr D3D11ConsoleContext::createIndexBuffer(IndexBuffer *buffer) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    TResult D3D11ConsoleContext::setIndexBuffer(IndexBuffer *buffer) { return T3D_OK; }
    RHIConstantBufferPtr D3D11ConsoleContext::createConstantBuffer(ConstantBuffer *buffer) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    RHIPixelBuffer1DPtr D3D11ConsoleContext::createPixelBuffer1D(PixelBuffer1D *buffer) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    RHIPixelBuffer2DPtr D3D11ConsoleContext::createPixelBuffer2D(PixelBuffer2D *buffer) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    RHIPixelBuffer3DPtr D3D11ConsoleContext::createPixelBuffer3D(PixelBuffer3D *buffer) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    RHIPixelBufferCubemapPtr D3D11ConsoleContext::createPixelBufferCubemap(PixelBufferCubemap *buffer) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    RHIShaderPtr D3D11ConsoleContext::createVertexShader(ShaderVariant *shader) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    TResult D3D11ConsoleContext::setVertexShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setVSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIShaderPtr D3D11ConsoleContext::createPixelShader(ShaderVariant *shader) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    TResult D3D11ConsoleContext::setPixelShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setPSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIShaderPtr D3D11ConsoleContext::createHullShader(ShaderVariant *shader) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    TResult D3D11ConsoleContext::setHullShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setHSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIShaderPtr D3D11ConsoleContext::createDomainShader(ShaderVariant *shader) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    TResult D3D11ConsoleContext::setDomainShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setDSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIShaderPtr D3D11ConsoleContext::createGeometryShader(ShaderVariant *shader) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    TResult D3D11ConsoleContext::setGeometryShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setGSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIShaderPtr D3D11ConsoleContext::createComputeShader(ShaderVariant *shader) { T3D_CONSOLE_UNAVAILABLE(); return nullptr; }
    TResult D3D11ConsoleContext::setComputeShader(ShaderVariant *shader) { return T3D_OK; }
    TResult D3D11ConsoleContext::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult D3D11ConsoleContext::setCSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIStructuredBufferPtr D3D11ConsoleContext::createStructuredBuffer(StructuredBuffer *buffer) { T3D_RHI_UNSUPPORTED_PTR(supportsStructuredBuffer); }
    TResult D3D11ConsoleContext::setVSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) { T3D_RHI_UNSUPPORTED(supportsStructuredBuffer); }
    TResult D3D11ConsoleContext::setPSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) { T3D_RHI_UNSUPPORTED(supportsStructuredBuffer); }
    TResult D3D11ConsoleContext::setCSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) { T3D_RHI_UNSUPPORTED(supportsStructuredBuffer); }
    TResult D3D11ConsoleContext::setCSUnorderedAccessBuffers(uint32_t startSlot, const UnorderedAccessBuffers &buffers, const UAVInitialCounts &initialCounts) { T3D_RHI_UNSUPPORTED(supportsUnorderedAccess); }
    TResult D3D11ConsoleContext::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) { T3D_RHI_UNSUPPORTED(supportsCompute); }
    TResult D3D11ConsoleContext::dispatchIndirect(RenderBuffer *argsBuffer, size_t argsOffset) { T3D_RHI_UNSUPPORTED(supportsIndirectDispatch); }
    TResult D3D11ConsoleContext::uavBarrier(const UnorderedAccessBuffers &buffers) { T3D_RHI_UNSUPPORTED(supportsUnorderedAccess); }
    TResult D3D11ConsoleContext::copyStructureCount(RenderBuffer *dstBuffer, size_t dstOffset, RenderBuffer *srcBuffer) { T3D_RHI_UNSUPPORTED(supportsAppendConsumeBuffer); }
    TResult D3D11ConsoleContext::setPrimitiveType(PrimitiveType primitive) { return T3D_OK; }
    TResult D3D11ConsoleContext::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) { return T3D_OK; }
    TResult D3D11ConsoleContext::render(uint32_t vertexCount, uint32_t startVertex) { return T3D_OK; }
    TResult D3D11ConsoleContext::renderIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance) { T3D_RHI_UNSUPPORTED(supportsInstancing); }
    TResult D3D11ConsoleContext::renderInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance) { T3D_RHI_UNSUPPORTED(supportsInstancing); }
    TResult D3D11ConsoleContext::renderIndexedIndirect(RenderBuffer *argsBuffer, size_t argsOffset) { T3D_RHI_UNSUPPORTED(supportsIndirectDraw); }
    TResult D3D11ConsoleContext::renderIndirect(RenderBuffer *argsBuffer, size_t argsOffset) { T3D_RHI_UNSUPPORTED(supportsIndirectDraw); }
    TResult D3D11ConsoleContext::reset() { return T3D_OK; }
    TResult D3D11ConsoleContext::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult D3D11ConsoleContext::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult D3D11ConsoleContext::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult D3D11ConsoleContext::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult D3D11ConsoleContext::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset) { return T3D_OK; }
    TResult D3D11ConsoleContext::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer) { return T3D_OK; }
}
