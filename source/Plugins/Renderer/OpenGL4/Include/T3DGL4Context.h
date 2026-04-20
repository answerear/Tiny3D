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


namespace Tiny3D
{
    class GL4Context : public RHIContext
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

        TResult compileShader(ShaderVariant *shader) override;
        TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams) override;

        TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams) override;


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

        /// GLAD 是否已加载（供 GL4RenderWindow 检查，避免重复加载）
        bool isGLADLoaded() const { return mGLADLoaded; }

        /// 获取 dummy GL 上下文（供 GL4RenderWindow 做 wglShareLists 共享）
#if defined(T3D_OS_WINDOWS)
        HGLRC getDummyGLRC() const { return mDummyHGLRC; }

        /// 获取 wglChoosePixelFormatARB 函数指针（供 GL4RenderWindow 使用 MSAA 像素格式）
        PFNWGLCHOOSEPIXELFORMATARBPROC getWglChoosePixelFormatARB() const { return mWglChoosePixelFormatARB; }

        /// 获取 wglCreateContextAttribsARB 函数指针（供 GL4RenderWindow 创建 Core Profile 上下文）
        PFNWGLCREATECONTEXTATTRIBSARBPROC getWglCreateContextAttribsARB() const { return mWglCreateContextAttribsARB; }
#elif defined(T3D_OS_LINUX)
        GLXContext getDummyGLXContext() const { return mDummyGLXContext; }
#endif

    protected:
        GL4Context();
        ~GL4Context() override;

        TResult initDummyContext();
        void destroyDummyContext();

        TResult stageConstantBuffers(const ConstantBuffers &buffers);
        TResult bindPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers);
        TResult bindSamplers(uint32_t startSlot, const Samplers &samplers);

        void bindPendingUniformBlocks(GLuint program);
        void setupSamplerBindings(GLuint program);

        //-------------------------------------------------------------------
        // glslang CPU-side reflection cache
        //-------------------------------------------------------------------
        struct GlslangUniformInfo
        {
            String name;        // e.g. "Tiny3DPerDraw.tiny3d_ObjectToWorld"
            int glDefineType;   // GL_FLOAT_MAT4, GL_SAMPLER_2D, etc.
            int offset;         // byte offset within the uniform block
            int size;           // array size (1 for non-array)
            int blockIndex;     // index of owning uniform block (-1 for standalone)
            int arrayStride;
        };

        struct GlslangBlockInfo
        {
            String name;        // e.g. "type_Tiny3DPerDraw"
            int size;           // block data size in bytes
        };

        struct GlslangReflectionData
        {
            TArray<GlslangBlockInfo> blocks;
            TArray<GlslangUniformInfo> uniforms;
        };

        TResult glslangCompileAndReflect(ShaderVariant *shader);

        TMap<ShaderVariant*, GlslangReflectionData> mReflectionCache;
        bool mGlslangInitialized {false};

    protected:
        /// 当前激活的 GL Program
        GLuint  mCurrentProgram {0};
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
        VertexDeclaration *mPendingVertexDecl {nullptr};
        /// 延迟绑定 UBO：cbuffer 名 -> GL buffer handle（render() link 后统一绑定）
        TMap<String, GLuint> mPendingUBOs;
        /// program 是否需要重新 link（setVertexShader/setPixelShader 后置 true，link 后置 false）
        bool    mProgramDirty {false};
        /// GLAD 是否已加载
        bool    mGLADLoaded {false};

        /// 当前是否渲染到 FBO（非 backbuffer），用于 Y 翻转判断
        bool    mRenderingToFBO {false};

        /// 当前绑定的 VS/PS ShaderVariant（用于 setupSamplerBindings 查找 slot）
        ShaderVariant *mCurrentVSVariant {nullptr};
        ShaderVariant *mCurrentPSVariant {nullptr};

        //-------------------------------------------------------------------
        // Dummy window/context 用于在 createRenderWindow 之前初始化 GLAD
        //-------------------------------------------------------------------
#if defined(T3D_OS_WINDOWS)
        HWND    mDummyHWND {nullptr};
        HDC     mDummyHDC {nullptr};
        HGLRC   mDummyHGLRC {nullptr};

        /// WGL 扩展函数指针（在 initDummyContext 中获取）
        PFNWGLCHOOSEPIXELFORMATARBPROC  mWglChoosePixelFormatARB {nullptr};
        PFNWGLGETEXTENSIONSSTRINGARBPROC mWglGetExtensionsStringARB {nullptr};
        PFNWGLCREATECONTEXTATTRIBSARBPROC mWglCreateContextAttribsARB {nullptr};
#elif defined(T3D_OS_LINUX)
        Display     *mDummyDisplay {nullptr};
        ::Window    mDummyWindow {0};
        GLXContext  mDummyGLXContext {nullptr};
#endif
    };
}


#endif  /*__T3D_GL4_CONTEXT_H__*/
