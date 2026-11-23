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

        TResult init() override;

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

        TResult stageConstantBuffers(TextureUnitStage stage, uint32_t startSlot, const ConstantBuffers &buffers);
        TResult bindPixelBuffers(TextureUnitStage stage, uint32_t startSlot, const PixelBuffers &buffers);
        TResult bindSamplers(TextureUnitStage stage, uint32_t startSlot, const Samplers &samplers);
        TResult bindStructuredBuffers(TextureUnitStage stage, uint32_t startSlot, const ShaderResourceBuffers &buffers);

        /// 校验 [startSlot, startSlot + count) 落在该阶段的纹理单元区间内
        TResult checkTextureSlots(TextureUnitStage stage, uint32_t startSlot, size_t count, const char *func) const;

        /// 阶段内 slot -> GL 纹理单元号，并记下该阶段用到的最高单元供 unbindTextureUnits 解绑
        GLuint toTextureUnit(TextureUnitStage stage, uint32_t slot);

        void bindPendingUniformBlocks(GLuint program, bool isCompute);
        void setupSamplerBindings(GLuint program, bool isCompute);

        /// draw / dispatch 前按 program 的 sampler uniform 表，把各阶段槽位上的纹理与采样器绑到对应单元
        void bindPendingTextures(GLuint program);

        /// 链接后枚举 program 的 SSBO 块，建立 mProgramStorageBlocks；HLSLCross 生成的块重新分配绑定点
        void setupStorageBlockBindings(GLuint program, bool isCompute);

        /// draw / dispatch 前按 program 的 SSBO 块表，把各槽位上的缓冲（或其计数器）绑到对应绑定点
        void bindPendingStorageBuffers(GLuint program);

        /// 记录 SSBO 绑定点 slot 上的缓冲（手写 GLSL 的块按绑定点查这张表）
        void setLegacyStorageBuffer(uint32_t slot, const RenderBufferPtr &buffer);

        /// 链接后记录 graphics program 的 VS 输入与输入签名，建立 mProgramVertexInputs
        void setupVertexInputs(GLuint program);

        /// draw 前按当前 program 的 VS 输入配置当前 VAO 的 attribute 指针。
        /// D3D11 按语义把输入布局对到 VS 输入，GL 只按 location 读取，所以 HLSLCross 生成的 VS
        /// 按语义查 location；手写 GLSL 拿不到语义，属性下标即 location
        void applyVertexLayout(GLuint program);

        /// draw 前对照当前顶点声明与 program 的 VS 输入，不匹配时打 warning（Debug 构建）。
        /// 每个顶点声明对每个 program 只校验一次
        void validateVertexInputs(GLuint program);

        /// 链接 program，成功后建立 uniform block 绑定与 sampler 绑定并记录 block 名单
        TResult linkProgram(GLuint program, const char *tag, bool isCompute);

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
        TResult attachGraphicsShader(ShaderVariant *shader, ShaderVariantPtr &currentVariant, GLenum stageType);

        void releaseRenderTextureResources(GL4PixelBuffer2D *pb);
        TResult buildRenderTextureResources(PixelBuffer2D *buffer, GL4PixelBuffer2D *pb);

        /// 窗口目标挂外部深度时的前置校验（默认帧缓冲不能挂外部附件，只能改画到中间 FBO）
        TResult checkWindowDepthStencil(RenderTarget *renderTarget) const;

        /// 以下 window proxy 相关函数只在 RHI 线程调用。
        /// 把窗口目标改画到中间 FBO（颜色 + 外部深度），首次切入时把窗口内容拷进来
        TResult bindWindowProxy(GL4RenderWindow *window, uint32_t width, uint32_t height, GL4PixelBuffer2D *depth);
        /// 按窗口尺寸与采样数（重）建中间 FBO 的颜色附件，MSAA 时另建单采样的中转纹理
        TResult ensureWindowProxyColor(uint32_t width, uint32_t height, uint32_t samples);
        /// 在中间 FBO 与窗口 backbuffer 之间翻转 Y 整张拷贝，不改变当前 FBO 绑定。
        /// MSAA 缓冲之间的 blit 要求矩形一致、不能翻转，所以 MSAA 时先 resolve 到中转纹理再翻转
        void copyWindowProxy(bool toWindow);
        /// 把中间 FBO 的内容拷回窗口并停用中间 FBO
        void releaseWindowProxy();

        GLuint getGLBufferHandle(RenderBuffer *buffer) const;
        bool getGLImageInfo(RenderBuffer *buffer, GLuint &outTex, GLenum &outFormat, GLboolean &outLayered) const;
        TResult validateIndirectArgs(RenderBuffer *argsBuffer, size_t argsOffset, size_t argsSize);

        struct BlitEndpoint
        {
            GLuint      fbo {0};
            GLuint      texture {0};
            GLenum      target {GL_TEXTURE_2D};
            GLenum      internalFormat {GL_RGBA8};
            GLuint      resolveFbo {0};
            GLuint      resolveTex {0};
            uint32_t    width {0};
            uint32_t    height {0};
            uint32_t    depth {1};
            uint32_t    arraySize {1};
            uint32_t    sampleCount {1};
            bool        isWindow {false};
            bool        isDepth {false};
            bool        hasStencil {false};
            bool        needsScratchFbo {false};
            /// isWindow 时所属的窗口
            GL4RenderWindow *window {nullptr};
        };

        void attachScratchFBO(GLuint fbo, const BlitEndpoint &ep, GLint layer);
        TResult resolveBlitEndpoint(Texture *tex, bool asSource, BlitEndpoint &out);
        TResult resolveBlitEndpoint(RenderTarget *rt, bool asSource, BlitEndpoint &out);
        TResult doBlit(const BlitEndpoint &srcIn, const BlitEndpoint &dstIn,
            const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset);

        /// blit 端点是当前画到中间 FBO 的窗口时，改指向中间 FBO（只在 RHI 线程调用）
        void redirectToWindowProxy(BlitEndpoint &ep) const;

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

        /**
         * \brief 读回中转缓冲池中的一块缓冲
         * \remarks 缓冲读回与纹理读回（PIXEL_PACK）都用 GL buffer 中转，所以只有一种条目。
         *       RHI 线程在 map 的拷贝命令里租借；主线程在 finishReadback 的
         *       syncRHIThread 之后归还，此时 RHI 线程空闲，与 D3D11 的做法一致。
         *       闲置条目由 trimStagingPool 在 RHI 线程上按超时与总量淘汰
         */
        struct StagingEntry
        {
            /// GL 缓冲名
            GLuint      Buffer {0};
            /// 缓冲容量（字节）
            size_t      Capacity {0};
            /// 是否已被某个未完成的请求占用
            bool        InUse {false};
            /// false 表示超过入池上限的一次性缓冲，归还时直接删除
            bool        Pooled {true};
            /// 最近一次归还的时间（steady clock 毫秒）
            uint64_t    LastReleaseMs {0};
        };

        /**
         * \brief 从池里租一块容量不小于 bytes 的 staging 缓冲，没有就新建
         * \param [in] bytes : 需要的字节数
         * \return 租到的缓冲名；创建失败返回 0
         * \note 只能在 RHI 线程调用。取能装下的最小一块，容量超过需要的 kStagingMaxReuseRatio 倍时不复用；
         *       新建时容量向上取到 2 的幂，便于不同大小的读回复用。超过 kStagingMaxPooledBytes 的请求
         *       按实际大小建一次性缓冲，不进池
         */
        GLuint acquireStagingBuffer(size_t bytes);

        /**
         * \brief 归还 staging：入池的标记为闲置，一次性的直接删除
         * \param [in] buffer : acquireStagingBuffer 租出去的缓冲，0 时不做任何事
         */
        void releaseStaging(GLuint buffer);

        /**
         * \brief 淘汰闲置的 staging：闲置超过 kStagingIdleTimeoutMs 的删除，闲置总量超过
         *       kStagingMaxIdleBytes 时从最久没用的开始删除
         * \note 只能在 RHI 线程（有 current context）调用，每次 present 时执行
         */
        void trimStagingPool();

        /**
         * \brief 删除池里所有 staging 缓冲
         * \note 只能在有 current context 且 RHI 线程空闲时调用
         */
        void destroyStagingPool();

    protected:
        /// graphics 管线的 shader 阶段下标，同时作为 program 缓存键的分量顺序
        enum GraphicsStage : uint32_t
        {
            kStageVS = 0,
            kStageHS,
            kStageDS,
            kStageGS,
            kStagePS,
            kStageCount
        };

        using GraphicsProgramKey = std::array<GLuint, kStageCount>;

        static uint32_t getGraphicsStageIndex(GLenum stageType);

        /// 当前激活的 graphics GL Program（归 mGraphicsProgramCache 所有，不能单独删除）
        GLuint  mCurrentProgram {0};
        /// 当前各阶段 shader 句柄，draw 前据此查找 program
        GraphicsProgramKey mStageShaders {};
        /// shader 组合 -> 已链接的 program，链接失败记为 0 以免每帧重复链接。
        /// 缓存里的 program 从不 detach shader，所以 shader 被删除后句柄名仍被占用，
        /// 不会被新 shader 复用，句柄可以安全地当作键。
        TMap<GraphicsProgramKey, GLuint> mGraphicsProgramCache;
        /// 当前 shader 组合链接失败，draw 直接跳过
        bool    mProgramLinkFailed {false};
        /// 当前激活的 compute GL Program（归 mComputeProgramCache 所有）
        GLuint  mCurrentComputeProgram {0};
        GLuint  mCurrentComputeShader {0};
        TMap<GLuint, GLuint> mComputeProgramCache;
        bool    mComputeProgramDirty {false};
        struct ProgramUBO
        {
            /// cbuffer 名
            String      name;
            /// 引用该 block 的阶段，按 TextureUnitStage 置位
            uint32_t    stageMask {0};
            /// 已报过多个阶段绑了不同 buffer 的警告
            bool        conflictWarned {false};
        };
        /// program -> 按 block 下标排列的 cbuffer（binding point 即下标）
        TMap<GLuint, TArray<ProgramUBO>> mProgramUBONames;
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
        /// 当前绑定的顶点声明，只在 RHI 线程访问
        GL4VertexDeclarationPtr mCurrentVertexDecl {nullptr};

        /// 一个 slot 上设置的顶点缓冲
        struct VertexBufferBinding
        {
            VertexBufferPtr buffer {nullptr};
            uint32_t        stride {0};
            uint32_t        offset {0};
        };
        /// 各 slot 上设置的顶点缓冲，只在 RHI 线程访问。与 D3D11 一样独立于顶点声明，
        /// set*VertexBuffers 只记录，draw 时再按当前声明与 program 配置 VAO
        TArray<VertexBufferBinding> mVertexBufferBindings;
        /// mVertexBufferBindings 的修订号，内容有变化时递增
        uint32_t mVertexBufferRevision {1};

        /// program 的一个 VS 输入（不含 gl_ 内建变量）
        struct VertexInput
        {
            String  name;
            /// HLSLCross 生成的输入名为 in_var_<语义>，hasSemantic 为 true 时以下两项有效（语义名为大写）
            bool        hasSemantic {false};
            String      semanticName;
            uint32_t    semanticIndex {0};
            GLint   location {0};
            /// 占用的 location 数，矩阵按列数、数组按元素数累计，对应依次递增的语义序号
            GLint   locationCount {1};
            GLenum  type {GL_FLOAT};
        };
        struct ProgramVertexInputs
        {
            TArray<VertexInput> inputs;
            /// 有任一输入带语义时按语义映射 location，否则属性下标即 location
            bool        bySemantic {false};
            /// 输入签名，相同的签名对同一个声明得到相同的 location 映射，从 1 开始
            uint32_t    signature {0};
        };
        /// graphics program -> VS 输入表，链接时由 setupVertexInputs 建立
        TMap<GLuint, ProgramVertexInputs> mProgramVertexInputs;
        /// VS 输入签名的规范串 -> 签名号
        TMap<String, uint32_t> mVertexInputSignatures;
        /// GL_MAX_VERTEX_ATTRIBS，首次配置 VAO 时查询
        GLint   mMaxVertexAttribs {0};
        struct PendingUBO
        {
            String  name;
            GLuint  ubo {0};
        };
        /// 延迟绑定 UBO：各阶段 CB 槽位 -> cbuffer 名与 GL buffer handle。
        /// GL 只能按 cbuffer 名把 UBO 对到 program 的 block 上，槽位用于覆盖与解绑；
        /// draw / dispatch 前 link 后按 block 名在本管线的各阶段里查找并统一绑定
        TMap<uint32_t, PendingUBO> mPendingUBOs[kTexStageCount];
        /// mPendingUBOs 的修订号，内容有变化时递增
        uint32_t mPendingUBORevision {0};
        /// 上一次真正执行 UBO 绑定时的 program 与修订号。
        /// glBindBufferBase(GL_UNIFORM_BUFFER) 是 context 状态而非 program 状态，
        /// graphics 与 compute 两个 program 会抢同一批 binding point，
        /// 所以换 program 或换 UBO 都必须重绑，只有两者都没变才能跳过。
        GLuint   mUBOBoundProgram {0};
        uint32_t mUBOBoundRevision {0};
        /// shader 组合是否变化（set*Shader 换了句柄后置 true，draw 前查缓存后置 false）
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

        /// 各阶段绑定过的 slot 上界（不含），unbindTextureUnits 只解绑这些单元
        uint32_t mTexUnitUsed[kTexStageCount] {};

        /// 各阶段 t# / s# 上设置的纹理与采样器，只在 RHI 线程访问。
        /// D3D11 的纹理与采样器槽位相互独立，同一张纹理可以配多个采样器，而 GL 的一个纹理单元
        /// 只有一个 sampler object，所以 set*PixelBuffers / set*Samplers 只记录，draw / dispatch 时
        /// 再按当前 program 的 mProgramSamplerUnits 绑定。持有引用与 D3D11 一致，绑定期间资源不会被释放
        PixelBuffers mStageTextures[kTexStageCount];
        Samplers mStageSamplers[kTexStageCount];

        /// 一个 sampler uniform 占用的纹理单元，以及它读取的纹理槽位与采样器槽位
        struct SamplerUnit
        {
            TextureUnitStage    stage {kTexStagePS};
            uint32_t            texSlot {0};
            /// 没有采样器（只做 texelFetch）时为 UINT32_MAX
            uint32_t            samplerSlot {UINT32_MAX};
            GLuint              unit {0};
            /// 纹理槽位为空时按 sampler 类型解绑的 target
            GLenum              target {GL_TEXTURE_2D};
        };
        /// program -> sampler uniform 表，链接时由 setupSamplerBindings 建立
        TMap<GLuint, TArray<SamplerUnit>> mProgramSamplerUnits;

        /// 各阶段 t# 上设置的结构化缓冲与 CS 的 u# 上设置的 UAV，只在 RHI 线程访问。
        /// D3D11 的 t# 与 u# 是两套编号，而 GL 只有一套 SSBO 绑定点，所以只记录，draw / dispatch 时
        /// 再按当前 program 的 mProgramStorageBlocks 绑定
        ShaderResourceBuffers mStageSRVs[kTexStageCount];
        UnorderedAccessBuffers mCSUAVs;
        /// 按 SSBO 绑定点记录的缓冲，SRV 与 UAV 写同一张表，后设置的覆盖先设置的。
        /// 手写 GLSL 的块直接按自己的 binding 查这张表，调用方须把 UAV 槽号排在 SRV 之后（附录 D.3.2）
        TArray<RenderBufferPtr> mLegacySSBOs;

        /// program 中一个 SSBO 块的数据来源
        struct StorageBlock
        {
            enum class Source : uint32_t
            {
                /// 手写 GLSL：按 binding 查 mLegacySSBOs
                kLegacy,
                /// HLSLCross 生成的 SRV：查 mStageSRVs[stage][slot]
                kSRV,
                /// HLSLCross 生成的 UAV：查 mCSUAVs[slot]
                kUAV,
            };

            GLuint              binding {0};
            Source              source {Source::kLegacy};
            TextureUnitStage    stage {kTexStageCS};
            uint32_t            slot {0};
            /// 为 true 时绑定该槽位缓冲的隐藏计数器
            bool                isCounter {false};
        };
        /// program -> SSBO 块表，链接时由 setupStorageBlockBindings 建立
        TMap<GLuint, TArray<StorageBlock>> mProgramStorageBlocks;

        /// 当前绑定的 CS Image Unit 槽位 -> GL 纹理对象（供 uavBarrier 解绑）
        TMap<uint32_t, GLuint> mBoundCSImages;

        /// blit 用的临时 FBO（给没有自带 FBO 的普通纹理）
        GLuint  mScratchReadFBO {0};
        GLuint  mScratchDrawFBO {0};
        /// depth-only RenderTarget 用的 FBO
        GLuint  mDepthOnlyFBO {0};

        /// 窗口目标挂外部深度时代替默认帧缓冲的中间 FBO，以下成员只在 RHI 线程访问
        GLuint  mWindowProxyFBO {0};
        GLuint  mWindowProxyColor {0};
        /// MSAA 时与 backbuffer 互拷用的单采样中转 FBO / 纹理
        GLuint  mWindowProxyResolveFBO {0};
        GLuint  mWindowProxyResolveTex {0};
        uint32_t mWindowProxyWidth {0};
        uint32_t mWindowProxyHeight {0};
        uint32_t mWindowProxySamples {1};
        /// 正在画到中间 FBO 的窗口；非空时该窗口的最新内容在中间 FBO 里，尚未拷回 backbuffer
        GL4RenderWindow *mWindowProxyOwner {nullptr};

        uint32_t mNextReadbackIndex {0};
        uint32_t mReadbackGeneration {1};
        TMap<uint32_t, ReadbackRequest> mPendingReadbacks;
        TArray<StagingEntry> mStagingPool;

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
