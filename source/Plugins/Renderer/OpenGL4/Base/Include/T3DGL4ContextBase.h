/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GL4_CONTEXT_BASE_H__
#define __T3D_GL4_CONTEXT_BASE_H__


#include "T3DGL4PrerequisitesBase.h"


namespace Tiny3D
{
    class GL4ContextBase : public RHIContext
    {
    public:
        virtual ~GL4ContextBase();

        virtual TResult init();

        // Depth remap matrix [-1, 1] -> [0, 1] (shared by Console and Runtime)
        const Matrix4& getDepthRemapMatrix() const override;

        // Shader compilation and reflection (shared by Console and Runtime)
        TResult compileShader(ShaderVariant *shader) override;
        TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams) override;
        TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams) override;

        /// GLAD 是否已加载（供 GL4RenderWindow 检查，避免重复加载）
        bool isGLADLoaded() const { return mGLADLoaded; }

        /// 将 dummy GL 上下文设为当前线程上下文（主窗口销毁后供资源清理使用）
        void makeDummyCurrent();

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
        GL4ContextBase();

        TResult initDummyContext();
        void destroyDummyContext();

        /// 按 GL 版本与实现限制填充 mCapabilities 与 mGLSLangLimits，必须在 GLAD 加载后调用
        void fillCapabilities();

        //-------------------------------------------------------------------
        // glslang CPU-side reflection cache
        //-------------------------------------------------------------------
        static bool mapSamplerGLType(GLenum glDefineType, TEXTURE_TYPE &outTexType);
        static bool isSamplerBufferGLType(GLenum glDefineType);
        static bool isImageGLType(GLenum glDefineType, bool &outIsBuffer);

        struct GlslangUniformInfo
        {
            String name;        // e.g. "Tiny3DPerDraw.tiny3d_ObjectToWorld"
            int glDefineType;   // GL_FLOAT_MAT4, GL_SAMPLER_2D, etc.
            int offset;         // byte offset within the uniform block
            int size;           // array size (1 for non-array)
            int blockIndex;     // index of owning uniform block (-1 for standalone)
            int arrayStride;
            int binding {-1};   // layout(binding = ...) for samplers/images
        };

        struct GlslangBlockInfo
        {
            String name;        // e.g. "type_Tiny3DPerDraw"
            int size;           // block data size in bytes
        };

        struct GlslangBufferBlockInfo
        {
            String name;
            int binding {0};
            int size {0};
            uint32_t elementStride {0};
            bool isReadOnly {false};
        };

        struct GlslangReflectionData
        {
            TArray<GlslangBlockInfo> blocks;
            TArray<GlslangUniformInfo> uniforms;
            TArray<GlslangBufferBlockInfo> bufferBlocks;
            TArray<int> atomicCounterBindings;
            uint32_t localSize[3] {0, 0, 0};
        };

        TResult glslangCompileAndReflect(ShaderVariant *shader);

        TMap<ShaderVariant*, GlslangReflectionData> mReflectionCache;
        bool mGlslangInitialized {false};

        /// glslang 校验 shader 用的实际 GL 上限。
        /// GetDefaultResources() 给的是 GL 规范的最低保证值，比真实驱动紧得多 ——
        /// 比如 maxAtomicCounterBindings 只有 1，于是 binding = 1 的 atomic_uint
        /// 会被判成 "binding is too large"，而驱动本身支持 8 个。
        /// 这些值在 fillCapabilities() 里趁 GL 上下文可用时查好，编译时只读不查
        /// （compileShader 跑在主线程，那里不保证有 GL 上下文）。
        struct GLSLangLimits
        {
            int atomicCounterBindings {0};
            int computeAtomicCounters {0};
            int computeAtomicCounterBuffers {0};
            int combinedShaderOutputResources {0};
            int imageUnits {0};
            int maxComputeWorkGroupCount[3] {0, 0, 0};
            int maxComputeWorkGroupSize[3] {0, 0, 0};
        };
        GLSLangLimits mGLSLangLimits;

#if defined(T3D_OS_WINDOWS)
        HWND  mDummyHWND {nullptr};
        HDC   mDummyHDC {nullptr};
        HGLRC mDummyHGLRC {nullptr};

        /// WGL 扩展函数指针（在 initDummyContext 中获取）
        PFNWGLCHOOSEPIXELFORMATARBPROC    mWglChoosePixelFormatARB {nullptr};
        PFNWGLGETEXTENSIONSSTRINGARBPROC  mWglGetExtensionsStringARB {nullptr};
        PFNWGLCREATECONTEXTATTRIBSARBPROC mWglCreateContextAttribsARB {nullptr};
#elif defined(T3D_OS_LINUX)
        Display      *mDummyDisplay {nullptr};
        ::Window      mDummyWindow {0};
        GLXContext    mDummyGLXContext {nullptr};
#endif
        bool mGLADLoaded {false};
    };
}


#endif  /*__T3D_GL4_CONTEXT_BASE_H__*/
