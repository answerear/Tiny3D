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


#ifndef __T3D_GL4_CONTEXT_H__
#define __T3D_GL4_CONTEXT_H__


#include "T3DGL4Prerequisites.h"
#include "T3DGL4ContextBase.h"


namespace Tiny3D
{
    class GL4Context : public GL4ContextBase
    {
    public:
        static GL4ContextPtr create();

        TResult init();

        TResult swapBackBuffer(GL4RenderWindow *renderWindow);

        TResult resizeRenderWindow(GL4RenderWindow *rw, uint32_t w, uint32_t h);

        //-------------------------------------------------------------------
        // RHIContext overrides
        //-------------------------------------------------------------------

        TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat) override;

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
        TResult clearStencil(uint32_t stencil) override;

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

        RHIStructuredBufferPtr createStructuredBuffer(StructuredBuffer *buffer) override;
        TResult setVSStructuredBuffers(uint32_t startSlot, const ShaderResourceBuffers &buffers) override;
        TResult setPSStructuredBuffers(uint32_t startSlot, const ShaderResourceBuffers &buffers) override;
        TResult setCSStructuredBuffers(uint32_t startSlot, const ShaderResourceBuffers &buffers) override;
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

        /// GPU → CPU 读回
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
        GL4Context();
        ~GL4Context() override;

        TResult stageConstantBuffers(const ConstantBuffers &buffers);
        TResult bindPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers);
        TResult bindSamplers(uint32_t startSlot, const Samplers &samplers);
        TResult bindStructuredBuffers(uint32_t startSlot, const ShaderResourceBuffers &buffers);

        void bindPendingUniformBlocks(GLuint program);
        void setupSamplerBindings(GLuint program);

        /// 解开 program 上与 shader 同阶段的旧 shader，避免同一阶段挂两个对象
        void detachShaderStage(GLuint program, GLuint shader);

        /// 解开所有常用纹理单元上的 texture / sampler，避免同一张纹理既被采样
        /// 又当 FBO 附件，以及上一 pass 留下的 comparison sampler 影响下一 pass。
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

        /**
         * \brief draw call 之前确保当前 graphics program 已链接并激活
         * \return 链接失败返回 T3D_ERR_GL4_LINK_PROGRAM
         */
        TResult ensureProgramLinked();

        /**
         * \brief dispatch 之前确保当前 compute program 已链接并激活
         */
        TResult ensureComputeProgramLinked();

        TResult compileGLSLShader(GLenum shaderType, const String &source, GLuint &outHandle, const char *stageName);
        TResult attachGraphicsShader(ShaderVariant *shader, ShaderVariantPtr &currentVariant);

        void releaseRenderTextureResources(GL4PixelBuffer2D *pb);
        TResult buildRenderTextureResources(PixelBuffer2D *buffer, GL4PixelBuffer2D *pb);

        GLuint getGLBufferHandle(RenderBuffer *buffer) const;
        bool getGLImageInfo(RenderBuffer *buffer, GLuint &outTex, GLenum &outFormat, GLboolean &outLayered) const;
        TResult validateIndirectArgs(RenderBuffer *argsBuffer, size_t argsOffset, size_t argsSize);

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
        /// 当前激活的 graphics GL Program
        GLuint  mCurrentProgram {0};
        /// 当前激活的 compute GL Program（与 graphics 分离）
        GLuint  mCurrentComputeProgram {0};
        bool    mComputeProgramDirty {false};
        /// 当前图元类型
        GLenum  mPrimitiveType {GL_TRIANGLES};
        /// 当前索引类型
        GLenum  mIndexType {GL_UNSIGNED_SHORT};
        /// 当前索引大小
        uint32_t mIndexSize {2};
        /// 当前渲染目标
        RenderTargetPtr mCurrentRenderTarget {nullptr};
        /// 当前绑定的 VAO
        GLuint  mCurrentVAO {0};
        /// 延迟配置顶点属性用的 VertexDeclaration 缓存
        VertexDeclarationPtr mPendingVertexDecl {nullptr};
        /// 延迟绑定 UBO：cbuffer 名 -> GL buffer handle（render() link 后统一绑定）
        TMap<String, GLuint> mPendingUBOs;
        /// mPendingUBOs 的修订号，每次 stageConstantBuffers 递增
        uint32_t mPendingUBORevision {0};
        /// 上一次真正执行 UBO 绑定时的 program 与修订号。
        /// glBindBufferBase(GL_UNIFORM_BUFFER) 是 context 状态而非 program 状态，
        /// graphics 与 compute 两个 program 会抢同一批 binding point，
        /// 所以换 program 或换 UBO 都必须重绑，只有两者都没变才能跳过。
        GLuint   mUBOBoundProgram {0};
        uint32_t mUBOBoundRevision {0};
        /// program 是否需要重新 link（setVertexShader/setPixelShader 后置 true，link 后置 false）
        bool    mProgramDirty {false};

        /// 当前是否渲染到 FBO（非 backbuffer），用于 Y 翻转判断
        bool    mRenderingToFBO {false};

        /// 当前绑定的各阶段 ShaderVariant（用于 setupSamplerBindings 查找 slot）
        ShaderVariantPtr mCurrentVSVariant {nullptr};
        ShaderVariantPtr mCurrentPSVariant {nullptr};
        ShaderVariantPtr mCurrentHSVariant {nullptr};
        ShaderVariantPtr mCurrentDSVariant {nullptr};
        ShaderVariantPtr mCurrentGSVariant {nullptr};
        ShaderVariantPtr mCurrentCSVariant {nullptr};

        /// 当前绑定的 CS Image Unit 槽位 -> GL 纹理对象（供 uavBarrier 解绑）
        TMap<uint32_t, GLuint> mBoundCSImages;

        /// blit 用的临时 FBO（给没有自带 FBO 的普通纹理）
        GLuint  mScratchReadFBO {0};
        GLuint  mScratchDrawFBO {0};

        uint32_t mNextReadbackIndex {0};
        uint32_t mReadbackGeneration {1};
        TMap<uint32_t, ReadbackRequest> mPendingReadbacks;

        /// 主窗口 GL context 句柄（用于 multi-viewport 恢复）
#if defined(T3D_OS_WINDOWS)
        HGLRC   mSavedGLContext {nullptr};
        HDC     mSavedGLDC {nullptr};
#elif defined(T3D_OS_LINUX)
        GLXContext  mSavedGLContext {nullptr};
        Display    *mSavedGLDisplay {nullptr};
        ::Window    mSavedGLWindow {0};
#endif
    };
}


#endif  /*__T3D_GL4_CONTEXT_H__*/
