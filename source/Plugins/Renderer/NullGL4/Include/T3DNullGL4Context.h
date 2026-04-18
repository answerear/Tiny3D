/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_NULLGL4_CONTEXT_H__
#define __T3D_NULLGL4_CONTEXT_H__


#include "T3DNullGL4Prerequisites.h"


namespace Tiny3D
{
    class NullGL4Context : public RHIContext
    {
    public:
        static NullGL4ContextPtr create();

        virtual ~NullGL4Context();

        TResult init();

        // ===== Core methods with real implementation =====

        TResult compileShader(ShaderVariant *shader) override;
        TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams) override;

        RHIShaderPtr createVertexShader(ShaderVariant *shader) override;
        RHIShaderPtr createPixelShader(ShaderVariant *shader) override;
        RHIShaderPtr createHullShader(ShaderVariant *shader) override;
        RHIShaderPtr createDomainShader(ShaderVariant *shader) override;
        RHIShaderPtr createGeometryShader(ShaderVariant *shader) override;
        RHIShaderPtr createComputeShader(ShaderVariant *shader) override;

        // ===== Empty implementations =====

        TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat) override;
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
        NullGL4Context();

        TResult initDummyContext();
        void destroyDummyContext();

#if defined(T3D_OS_WINDOWS)
        HWND  mDummyHWND {nullptr};
        HDC   mDummyHDC {nullptr};
        HGLRC mDummyHGLRC {nullptr};
#elif defined(T3D_OS_LINUX)
        Display      *mDummyDisplay {nullptr};
        ::Window      mDummyWindow {0};
        GLXContext    mDummyGLXContext {nullptr};
#endif
        bool mGLADLoaded {false};
    };
}


#endif  /*__T3D_NULLGL4_CONTEXT_H__*/
