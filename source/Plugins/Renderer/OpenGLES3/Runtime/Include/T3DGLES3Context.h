/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GLES3_CONTEXT_H__
#define __T3D_GLES3_CONTEXT_H__


#include "T3DGLES3Prerequisites.h"
#include "T3DGLES3ContextBase.h"


namespace Tiny3D
{
    class GLES3Context : public GLES3ContextBase
    {
    public:
        static GLES3ContextPtr create();

        TResult init();

        TResult swapBackBuffer(GLES3RenderWindow *renderWindow);

        TResult resizeRenderWindow(GLES3RenderWindow *rw, uint32_t w, uint32_t h);

        /// eglMakeCurrent 之后二次确认 GLES 版本并刷新能力位
        void confirmDeviceVersion();

        //-------------------------------------------------------------------
        // RHIContext overrides
        //-------------------------------------------------------------------

        TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat) override;

        const Matrix4& getDepthRemapMatrix() const override;

        RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow) override;

        RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer) override;

        TResult setRenderTarget(RenderTarget *renderTarget) override;

        TResult resetRenderTarget() override;

        TResult resizeRenderTexture(RenderTexture *rt, uint32_t width, uint32_t height) override;
        TResult resizeRenderTarget(RenderTarget *rt, uint32_t width, uint32_t height) override;

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

        TResult compileShader(ShaderVariant *shader) override;
        TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams) override;
        TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams) override;

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

        ReadbackHandle map(RenderBuffer *src, size_t offset, size_t size) override;
        ReadbackHandle map(RenderBuffer *src, const ReadbackRegion &region) override;
        TResult unmap(ReadbackHandle handle, Buffer &dst) override;

        void* getNativeContext() const override;
        void restoreNativeContext() override;

        TResult beginRender() override { return T3D_OK; }
        TResult endRender() override { return T3D_OK; }

        TResult beginPass() override { return T3D_OK; }
        TResult endPass() override { return T3D_OK; }

    protected:
        GLES3Context();
        ~GLES3Context() override;

        TResult stageConstantBuffers(const ConstantBuffers &buffers);
        TResult bindPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers);
        TResult bindSamplers(uint32_t startSlot, const Samplers &samplers);
        TResult bindStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers);

        void bindPendingUniformBlocks(GLuint program);
        void setupSamplerBindings(GLuint program);

        void unbindTextureUnits();

        /// glClear 被写掩码和 scissor 约束，而 D3D11 的 ClearXXXView 无视管线
        /// 状态。RHI 层要对齐 D3D11 语义，clear 前后必须自己开关掩码。
        struct ClearMaskGuard
        {
            explicit ClearMaskGuard(GLbitfield mask);
            ~ClearMaskGuard();

            GLbitfield  mMask {0};
            GLboolean   mColorMask[4] {GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE};
            GLboolean   mDepthMask {GL_TRUE};
            GLint       mStencilMaskFront {~0};
            GLint       mStencilMaskBack {~0};
            GLboolean   mScissorEnabled {GL_FALSE};
        };

        /// 按 GLES 版本填充 mCapabilities，必须在 GL 上下文就绪后调用
        void fillCapabilities();

        /**
         * \brief draw call 之前确保当前 graphics program 已链接并激活
         * \return 链接失败返回 T3D_ERR_GLES3_LINK_PROGRAM
         */
        TResult ensureProgramLinked();

        /**
         * \brief dispatch 之前确保当前 compute program 已链接并激活
         */
        TResult ensureComputeProgramLinked();

        TResult compileGLSLShader(GLenum shaderType, const String &source, GLuint &outHandle, const char *stageName);
        /// 挂新 shader 之前，把程序上同一阶段的旧 shader 摘下来
        void detachShaderStage(GLuint program, GLuint shader);
        TResult attachGraphicsShader(ShaderVariant *shader, ShaderVariant *&currentVariant);

        void releaseRenderTextureResources(GLES3PixelBuffer2D *pb);
        TResult buildRenderTextureResources(PixelBuffer2D *buffer, GLES3PixelBuffer2D *glPixelBuffer);

        GLuint getGLBufferHandle(RenderBuffer *buffer) const;
        TResult validateIndirectArgs(RenderBuffer *argsBuffer, size_t argsOffset, size_t argsSize);

        bool supportsTessellation() const;
        bool supportsIndexedBlend() const;

        struct BlitEndpoint
        {
            GLuint      fbo {0};
            GLuint      texture {0};
            GLuint      resolveFbo {0};
            GLuint      resolveTex {0};
            uint32_t    width {0};
            uint32_t    height {0};
            uint32_t    sampleCount {1};
            bool        isWindow {false};
            bool        isDepth {false};
            bool        needsScratchFbo {false};
        };

        TResult resolveBlitEndpoint(Texture *tex, bool asSource, BlitEndpoint &out);
        TResult resolveBlitEndpoint(RenderTarget *rt, bool asSource, BlitEndpoint &out);
        TResult doBlit(const BlitEndpoint &src, const BlitEndpoint &dst,
            const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset);

        struct ReadbackRequest
        {
            ReadbackHandle      Handle {};
            RenderBufferPtr     Src {nullptr};
            bool                IsTexture {false};
            bool                CopyRecorded {false};
            TResult             CopyResult {T3D_OK};
            GLuint              Staging {0};
            size_t              TotalBytes {0};
            uint32_t            TightRowPitch {0};
            uint32_t            TightSlicePitch {0};
            uint32_t            CopyWidth {1};
            uint32_t            CopyHeight {1};
            uint32_t            CopyDepth {1};
            size_t              BufferOffset {0};
            size_t              BufferSize {0};
            ReadbackRegion      Region {};
        };

        ReadbackHandle allocReadbackRequest(RenderBuffer *src, bool isTexture, ReadbackRequest *&outRequest);
        TResult finishReadback(ReadbackHandle handle, Buffer &dst);

    protected:
        GLuint  mCurrentProgram {0};
        GLuint  mCurrentComputeProgram {0};
        bool    mComputeProgramDirty {false};
        GLenum  mPrimitiveType {GL_TRIANGLES};
        GLenum  mIndexType {GL_UNSIGNED_SHORT};
        uint32_t mIndexSize {2};
        RenderTargetPtr mCurrentRenderTarget {nullptr};
        GLuint  mCurrentVAO {0};
        VertexDeclaration *mPendingVertexDecl {nullptr};
        TMap<String, GLuint> mPendingUBOs;
        bool    mProgramDirty {false};
        bool    mRenderingToFBO {false};

        ShaderVariant *mCurrentVSVariant {nullptr};
        ShaderVariant *mCurrentPSVariant {nullptr};
        ShaderVariant *mCurrentHSVariant {nullptr};
        ShaderVariant *mCurrentDSVariant {nullptr};
        ShaderVariant *mCurrentGSVariant {nullptr};
        ShaderVariant *mCurrentCSVariant {nullptr};

        /// 被 doBlit 与 map(texture) 共用：每次使用前必须重新 attach，不能按附件缓存跳过
        GLuint  mScratchReadFBO {0};
        GLuint  mScratchDrawFBO {0};

        uint32_t mNextReadbackIndex {0};
        uint32_t mReadbackGeneration {1};
        TMap<uint32_t, ReadbackRequest> mPendingReadbacks;

        uint32_t mMaxTextureImageUnits {32};
        uint32_t mMaxVertexShaderStorageBlocks {0};
        bool    mSupportsIndexedBlend {false};
    };
}


#endif  /*__T3D_GLES3_CONTEXT_H__*/
