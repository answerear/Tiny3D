/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GL4_CONSOLE_CONTEXT_H__
#define __T3D_GL4_CONSOLE_CONTEXT_H__


#include "T3DGL4ContextBase.h"
#include "T3DGL4ConsolePrerequisites.h"


namespace Tiny3D
{
    class GL4ConsoleContext : public GL4ContextBase
    {
    public:
        static GL4ConsoleContextPtr create();

        virtual ~GL4ConsoleContext();

        // Shader creation (real GL compilation)
        RHIShaderPtr createVertexShader(ShaderVariant *shader) override;
        RHIShaderPtr createPixelShader(ShaderVariant *shader) override;
        RHIShaderPtr createHullShader(ShaderVariant *shader) override;
        RHIShaderPtr createDomainShader(ShaderVariant *shader) override;
        RHIShaderPtr createGeometryShader(ShaderVariant *shader) override;
        RHIShaderPtr createComputeShader(ShaderVariant *shader) override;

        // Empty implementations for Console
        TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat) override;
        RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow) override;
        RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer) override;
        TResult setRenderTarget(RenderTarget *renderTarget) override;
        TResult resetRenderTarget() override;
        TResult setViewport(const Viewport &viewport) override;
        TResult setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height) override;
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
        RHIPixelBufferCubemapPtr createPixelBufferCubemap(PixelBufferCubemap *buffer) override;
        TResult setVertexShader(ShaderVariant *shader) override;
        TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        TResult setPixelShader(ShaderVariant *shader) override;
        TResult setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setPSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        TResult setHullShader(ShaderVariant *shader) override;
        TResult setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setHSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        TResult setDomainShader(ShaderVariant *shader) override;
        TResult setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setDSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        TResult setGeometryShader(ShaderVariant *shader) override;
        TResult setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setGSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        TResult setComputeShader(ShaderVariant *shader) override;
        TResult setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setCSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIStructuredBufferPtr createStructuredBuffer(StructuredBuffer *buffer) override;
        TResult setVSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) override;
        TResult setPSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) override;
        TResult setCSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) override;
        TResult setCSUnorderedAccessBuffers(uint32_t startSlot, const UnorderedAccessBuffers &buffers, const UAVInitialCounts &initialCounts = UAVInitialCounts()) override;
        TResult dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
        TResult dispatchIndirect(RenderBuffer *argsBuffer, size_t argsOffset = 0) override;
        TResult uavBarrier(const UnorderedAccessBuffers &buffers) override;
        TResult copyStructureCount(RenderBuffer *dstBuffer, size_t dstOffset, RenderBuffer *srcBuffer) override;
        TResult setPrimitiveType(PrimitiveType primitive) override;
        TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) override;
        TResult render(uint32_t vertexCount, uint32_t startVertex) override;
        TResult renderIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance) override;
        TResult renderInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance) override;
        TResult renderIndexedIndirect(RenderBuffer *argsBuffer, size_t argsOffset = 0) override;
        TResult renderIndirect(RenderBuffer *argsBuffer, size_t argsOffset = 0) override;
        TResult reset() override;
        TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(Texture *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset = 0, size_t size = 0, size_t dstOffset = 0) override;
        TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer = false) override;

        /// GPU 读回未实现
        ReadbackHandle beginReadBuffer(RenderBuffer *src, size_t offset, size_t size) override;
        TResult endReadBuffer(ReadbackHandle handle, Buffer &dst) override;
        ReadbackHandle beginReadTexture(RenderBuffer *src, const ReadbackRegion &region) override;
        TResult endReadTexture(ReadbackHandle handle, Buffer &dst) override;

        TResult beginRender() override { return T3D_OK; }
        TResult endRender() override { return T3D_OK; }

        TResult beginPass() override { return T3D_OK; }
        TResult endPass() override { return T3D_OK; }

    protected:
        GL4ConsoleContext();
    };
}


#endif  /*__T3D_GL4_CONSOLE_CONTEXT_H__*/
