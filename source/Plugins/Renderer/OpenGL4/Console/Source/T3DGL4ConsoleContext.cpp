/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGL4ConsoleContext.h"
#include "T3DGL4Error.h"
#include "T3DGL4ConsoleWindow.h"
#include "T3DGL4ConsoleRenderBuffer.h"
#include "T3DGL4ConsoleRenderState.h"
#include "T3DGL4ConsoleShader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GL4ConsoleContextPtr GL4ConsoleContext::create()
    {
        GL4ConsoleContextPtr ctx = T3D_NEW GL4ConsoleContext();
        return ctx;
    }

    //--------------------------------------------------------------------------

    GL4ConsoleContext::GL4ConsoleContext()
    {
    }

    //--------------------------------------------------------------------------

    GL4ConsoleContext::~GL4ConsoleContext()
    {
    }

    //--------------------------------------------------------------------------
    // Shader creation (real GL compilation, moved from GL4ContextBase)
    //--------------------------------------------------------------------------

    RHIShaderPtr GL4ConsoleContext::createVertexShader(ShaderVariant *shader)
    {
        GL4ConsoleVertexShaderPtr glShader = GL4ConsoleVertexShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        glShader->GLShaderHandle = glCreateShader(GL_VERTEX_SHADER);
        GLint len = static_cast<GLint>(bytecodeLength);
        glShaderSource(glShader->GLShaderHandle, 1, &bytecode, &len);
        glCompileShader(glShader->GLShaderHandle);

        GLint compiled = 0;
        glGetShaderiv(glShader->GLShaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint logLen = 0;
            glGetShaderiv(glShader->GLShaderHandle, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen > 0)
            {
                TArray<char> log(logLen + 1, 0);
                glGetShaderInfoLog(glShader->GLShaderHandle, logLen, nullptr, log.data());
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "Vertex shader compile error: %s", log.data());
            }
            GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
            return nullptr;
        }

        return glShader;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr GL4ConsoleContext::createPixelShader(ShaderVariant *shader)
    {
        GL4ConsolePixelShaderPtr glShader = GL4ConsolePixelShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        glShader->GLShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
        GLint len = static_cast<GLint>(bytecodeLength);
        glShaderSource(glShader->GLShaderHandle, 1, &bytecode, &len);
        glCompileShader(glShader->GLShaderHandle);

        GLint compiled = 0;
        glGetShaderiv(glShader->GLShaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint logLen = 0;
            glGetShaderiv(glShader->GLShaderHandle, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen > 0)
            {
                TArray<char> log(logLen + 1, 0);
                glGetShaderInfoLog(glShader->GLShaderHandle, logLen, nullptr, log.data());
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "Pixel shader compile error: %s", log.data());
            }
            GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
            return nullptr;
        }

        return glShader;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr GL4ConsoleContext::createGeometryShader(ShaderVariant *shader)
    {
        GL4ConsoleGeometryShaderPtr glShader = GL4ConsoleGeometryShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        glShader->GLShaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
        GLint len = static_cast<GLint>(bytecodeLength);
        glShaderSource(glShader->GLShaderHandle, 1, &bytecode, &len);
        glCompileShader(glShader->GLShaderHandle);

        GLint compiled = 0;
        glGetShaderiv(glShader->GLShaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint logLen = 0;
            glGetShaderiv(glShader->GLShaderHandle, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen > 0)
            {
                TArray<char> log(logLen + 1, 0);
                glGetShaderInfoLog(glShader->GLShaderHandle, logLen, nullptr, log.data());
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "Geometry shader compile error: %s", log.data());
            }
            GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
            return nullptr;
        }

        return glShader;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr GL4ConsoleContext::createHullShader(ShaderVariant *shader) { return GL4ConsoleHullShader::create(); }
    RHIShaderPtr GL4ConsoleContext::createDomainShader(ShaderVariant *shader) { return GL4ConsoleDomainShader::create(); }
    RHIShaderPtr GL4ConsoleContext::createComputeShader(ShaderVariant *shader) { return GL4ConsoleComputeShader::create(); }

    //--------------------------------------------------------------------------
    // Empty implementations for Console
    //--------------------------------------------------------------------------

    TResult GL4ConsoleContext::setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat) { return T3D_OK; }
    RHIRenderTargetPtr GL4ConsoleContext::createRenderWindow(RenderWindow *renderWindow) { return GL4ConsoleWindow::create(); }
    RHIPixelBuffer2DPtr GL4ConsoleContext::createRenderTexture(PixelBuffer2D *buffer) { return nullptr; }
    TResult GL4ConsoleContext::setRenderTarget(RenderTarget *renderTarget) { return T3D_OK; }
    TResult GL4ConsoleContext::resetRenderTarget() { return T3D_OK; }
    TResult GL4ConsoleContext::setViewport(const Viewport &viewport) { return T3D_OK; }
    TResult GL4ConsoleContext::setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height) { return T3D_OK; }
    TResult GL4ConsoleContext::clearColor(const ColorRGB &color) { return T3D_OK; }
    TResult GL4ConsoleContext::clearDepth(Real depth) { return T3D_OK; }
    TResult GL4ConsoleContext::clearDepthStencil(Real depth, uint32_t stencil) { return T3D_OK; }
    RHIBlendStatePtr GL4ConsoleContext::createBlendState(BlendState *state) { return GL4ConsoleBlendState::create(); }
    RHIDepthStencilStatePtr GL4ConsoleContext::createDepthStencilState(DepthStencilState *state) { return GL4ConsoleDepthStencilState::create(); }
    RHIRasterizerStatePtr GL4ConsoleContext::createRasterizerState(RasterizerState *state) { return GL4ConsoleRasterizerState::create(); }
    RHISamplerStatePtr GL4ConsoleContext::createSamplerState(SamplerState *state) { return GL4ConsoleSamplerState::create(); }
    TResult GL4ConsoleContext::setBlendState(BlendState *state) { return T3D_OK; }
    TResult GL4ConsoleContext::setDepthStencilState(DepthStencilState *state) { return T3D_OK; }
    TResult GL4ConsoleContext::setRasterizerState(RasterizerState *state) { return T3D_OK; }
    RHIVertexDeclarationPtr GL4ConsoleContext::createVertexDeclaration(VertexDeclaration *decl) { return GL4ConsoleVertexDeclaration::create(); }
    TResult GL4ConsoleContext::setVertexDeclaration(VertexDeclaration *decl) { return T3D_OK; }
    RHIVertexBufferPtr GL4ConsoleContext::createVertexBuffer(VertexBuffer *buffer) { return GL4ConsoleVertexBuffer::create(); }
    TResult GL4ConsoleContext::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets) { return T3D_OK; }
    RHIIndexBufferPtr GL4ConsoleContext::createIndexBuffer(IndexBuffer *buffer) { return GL4ConsoleIndexBuffer::create(); }
    TResult GL4ConsoleContext::setIndexBuffer(IndexBuffer *buffer) { return T3D_OK; }
    RHIConstantBufferPtr GL4ConsoleContext::createConstantBuffer(ConstantBuffer *buffer) { return GL4ConsoleConstantBuffer::create(); }
    RHIPixelBuffer1DPtr GL4ConsoleContext::createPixelBuffer1D(PixelBuffer1D *buffer) { return GL4ConsolePixelBuffer1D::create(); }
    RHIPixelBuffer2DPtr GL4ConsoleContext::createPixelBuffer2D(PixelBuffer2D *buffer) { return GL4ConsolePixelBuffer2D::create(); }
    RHIPixelBuffer3DPtr GL4ConsoleContext::createPixelBuffer3D(PixelBuffer3D *buffer) { return nullptr; }
    RHIPixelBufferCubemapPtr GL4ConsoleContext::createPixelBufferCubemap(PixelBufferCubemap *buffer) { return nullptr; }
    TResult GL4ConsoleContext::setVertexShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setVSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult GL4ConsoleContext::setPixelShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setPSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult GL4ConsoleContext::setHullShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setHSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult GL4ConsoleContext::setDomainShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setDSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult GL4ConsoleContext::setGeometryShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setGSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult GL4ConsoleContext::setComputeShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setCSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    RHIStructuredBufferPtr GL4ConsoleContext::createStructuredBuffer(StructuredBuffer *buffer) { T3D_RHI_UNSUPPORTED_PTR(supportsStructuredBuffer); }
    TResult GL4ConsoleContext::setVSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) { T3D_RHI_UNSUPPORTED(supportsStructuredBuffer); }
    TResult GL4ConsoleContext::setPSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) { T3D_RHI_UNSUPPORTED(supportsStructuredBuffer); }
    TResult GL4ConsoleContext::setCSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) { T3D_RHI_UNSUPPORTED(supportsStructuredBuffer); }
    TResult GL4ConsoleContext::setCSUnorderedAccessBuffers(uint32_t startSlot, const UnorderedAccessBuffers &buffers, const UAVInitialCounts &initialCounts) { T3D_RHI_UNSUPPORTED(supportsUnorderedAccess); }
    TResult GL4ConsoleContext::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) { T3D_RHI_UNSUPPORTED(supportsCompute); }
    TResult GL4ConsoleContext::dispatchIndirect(RenderBuffer *argsBuffer, size_t argsOffset) { T3D_RHI_UNSUPPORTED(supportsIndirectDispatch); }
    TResult GL4ConsoleContext::uavBarrier(const UnorderedAccessBuffers &buffers) { T3D_RHI_UNSUPPORTED(supportsUnorderedAccess); }
    TResult GL4ConsoleContext::copyStructureCount(RenderBuffer *dstBuffer, size_t dstOffset, RenderBuffer *srcBuffer) { T3D_RHI_UNSUPPORTED(supportsAppendConsumeBuffer); }
    TResult GL4ConsoleContext::setPrimitiveType(PrimitiveType primitive) { return T3D_OK; }
    TResult GL4ConsoleContext::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) { return T3D_OK; }
    TResult GL4ConsoleContext::render(uint32_t vertexCount, uint32_t startVertex) { return T3D_OK; }
    TResult GL4ConsoleContext::renderIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance) { T3D_RHI_UNSUPPORTED(supportsInstancing); }
    TResult GL4ConsoleContext::renderInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance) { T3D_RHI_UNSUPPORTED(supportsInstancing); }
    TResult GL4ConsoleContext::renderIndexedIndirect(RenderBuffer *argsBuffer, size_t argsOffset) { T3D_RHI_UNSUPPORTED(supportsIndirectDraw); }
    TResult GL4ConsoleContext::renderIndirect(RenderBuffer *argsBuffer, size_t argsOffset) { T3D_RHI_UNSUPPORTED(supportsIndirectDraw); }
    TResult GL4ConsoleContext::reset() { return T3D_OK; }
    TResult GL4ConsoleContext::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult GL4ConsoleContext::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult GL4ConsoleContext::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult GL4ConsoleContext::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult GL4ConsoleContext::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset) { return T3D_OK; }
    TResult GL4ConsoleContext::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer) { return T3D_OK; }
}
