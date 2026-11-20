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

        /// sampler uniform 名拆出的纹理与采样器
        struct CombinedSamplerName
        {
            /// 纹理名，即纹理所在 ShaderSamplerParam 的键
            String texName;
            /// 采样器所在 ShaderSamplerParam 的键（与 D3D11 一致，采样器名去掉 "sampler" 前缀）；
            /// 为空表示只做 texelFetch、没有采样器（SPIRV-Cross 的 dummy sampler）
            String samplerKey;
            /// 名字里带的 HLSL t# 寄存器号，没有时为 UINT32_MAX
            uint32_t texSlot {UINT32_MAX};
            /// 名字里带的 HLSL s# 寄存器号，没有或没有采样器时为 UINT32_MAX
            uint32_t samplerSlot {UINT32_MAX};
        };

        /**
         * \brief 拆解 sampler uniform 名
         * \remarks HLSLCross 把 HLSL 的纹理 + 采样器合并成
         *          "T3D_Combined_t<t#>s<s#|x>l<纹理名长度><纹理名><采样器名>"，x 表示没有采样器，
         *          纹理名与寄存器号都能直接读出。
         *          旧资源是 "SPIRV_Cross_Combined<纹理名><采样器名>"，不带寄存器号，采样器名按 D3D11 约定以
         *          "sampler" 开头。纹理名里也可能有 "sampler"，所以优先按 "<T>sampler<T>"（纹理自带的采样器）
         *          拆分，其次取第一个 "sampler"。
         *          手写 GLSL 的 sampler 没有前缀，纹理与采样器同名。
         */
        static CombinedSamplerName parseCombinedSamplerName(const String &uniformName);

        /// 解析带寄存器号的 "T3D_Combined_..." 名字，格式不符时返回 false 且不改 result
        static bool parseRegisterCombinedName(const String &uniformName, CombinedSamplerName &result);

        /// 存储缓冲块名拆出的原名与种类
        struct BufferBlockName
        {
            /// HLSL 中的资源名
            String name;
            /// 绑定形态，isCounter 为 true 时无意义
            ShaderResourceParam::Kind kind {ShaderResourceParam::Kind::kStructuredSRV};
            /// 是否为 name 的隐藏计数器块
            bool isCounter {false};
        };

        /**
         * \brief 解析存储缓冲块名
         * \remarks GLSL 的 SSBO 不区分 StructuredBuffer、ByteAddressBuffer、Append、Consume，
         *          HLSLCross 把种类与原名写进块名："T3D_Buffer_<种类>l<原名长度><原名>"，
         *          种类为 sb / rwsb / bab / rwbab / asb / csb，cnt 表示 <原名> 的隐藏计数器。
         *          手写 GLSL 与旧资源不带这个格式，返回 false 且不改 result
         */
        static bool parseBufferBlockName(const String &blockName, BufferBlockName &result);

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

        /**
         * \brief 按 D3D11 的编号方式给 sampler uniform 分配纹理与采样器槽位
         * \remarks 名字带寄存器号时直接使用，与 D3D11 的 t# / s# 一致。旧名字不带寄存器号，
         *          纹理按纹理名、采样器按采样器键各自去重，按首次出现的顺序取未被占用的最小槽位，
         *          同一张纹理配多个采样器时纹理只占一个槽位。纹理槽位写入纹理名对应参数的 texBinding，
         *          采样器槽位写入采样器键对应参数的 samplerBinding。
         * \param [in] createMissing : 为 true 时新建缺少的参数，否则只回填已有参数
         */
        static void assignSamplerBindings(const GlslangReflectionData &data, ShaderSamplerParams &samplerParams, bool createMissing);

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

        /// D3D11 每个阶段的 t# / s# 是独立的编号空间，而 GL 的纹理单元由所有阶段
        /// （包括 compute）共享。这里给每个阶段划一段互不重叠的纹理单元，
        /// 阶段内的 slot 加上该阶段的起点才是 GL 纹理单元号。PS 排在最前，从 0 开始。
        enum TextureUnitStage : uint32_t
        {
            kTexStagePS = 0,
            kTexStageVS,
            kTexStageGS,
            kTexStageHS,
            kTexStageDS,
            kTexStageCS,
            kTexStageCount
        };

        /// 各阶段纹理单元区间的起点与长度，在 fillCapabilities() 里按驱动上限划分
        uint32_t mTexUnitBase[kTexStageCount] {};
        uint32_t mTexUnitCount[kTexStageCount] {};

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
