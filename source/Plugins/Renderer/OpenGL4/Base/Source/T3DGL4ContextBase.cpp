/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGL4ContextBase.h"
#include "T3DGL4Error.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/Include/Types.h>


namespace Tiny3D
{
    #define LOG_TAG_GL4CONTEXTBASE        "GL4ContextBase"

    //--------------------------------------------------------------------------

    GL4ContextBase::GL4ContextBase()
    {
    }

    //--------------------------------------------------------------------------

    GL4ContextBase::~GL4ContextBase()
    {
        if (mGlslangInitialized)
        {
            glslang::FinalizeProcess();
            mGlslangInitialized = false;
        }
        destroyDummyContext();
    }

    //--------------------------------------------------------------------------

    TResult GL4ContextBase::init()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = initDummyContext();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "Failed to init dummy GL context !");
                break;
            }

            if (!mGlslangInitialized)
            {
                glslang::InitializeProcess();
                mGlslangInitialized = true;
            }

            fillCapabilities();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    const Matrix4& GL4ContextBase::getDepthRemapMatrix() const
    {
        static Matrix4 zRemapMat(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.5f,
            0.0f, 0.0f, 0.0f, 1.0f
            );
        return zRemapMat;
    }

    //--------------------------------------------------------------------------

    void GL4ContextBase::fillCapabilities()
    {
        GLint major = 0;
        GLint minor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        const bool has40 = (major > 4) || (major == 4 && minor >= 0);
        const bool has42 = (major > 4) || (major == 4 && minor >= 2);
        const bool has43 = (major > 4) || (major == 4 && minor >= 3);

        // 实例化与 divisor 是 GL 3.3 核心功能；base instance 需要 GL 4.2
        mCapabilities.supportsInstancing = true;
        mCapabilities.supportsBaseInstance = has42;
        // GL 4.0 有 indirect draw，compute / SSBO / UAV 从 4.3 起才齐
        mCapabilities.supportsIndirectDraw = has40;
        mCapabilities.supportsCompute = has43;
        mCapabilities.supportsUnorderedAccess = has43;
        mCapabilities.supportsStructuredBuffer = has43;
        mCapabilities.supportsIndirectDispatch = has43;
        mCapabilities.supportsAppendConsumeBuffer = has43;
        mCapabilities.supportsReadback = true;

        if (has43)
        {
            for (GLuint i = 0; i < 3; ++i)
            {
                GLint count = 0;
                GLint size = 0;
                glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, i, &count);
                glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, i, &size);
                mCapabilities.maxDispatchGroupCount[i] = (uint32_t)count;
                mCapabilities.maxComputeGroupSize[i] = (uint32_t)size;
                mGLSLangLimits.maxComputeWorkGroupCount[i] = count;
                mGLSLangLimits.maxComputeWorkGroupSize[i] = size;
            }

            GLint sharedMemory = 0;
            glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &sharedMemory);
            mCapabilities.maxComputeSharedMemory = (uint32_t)sharedMemory;

            GLint storageBuffers = 0;
            glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &storageBuffers);
            mCapabilities.maxUnorderedAccessSlots = (uint32_t)storageBuffers;

            glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &mGLSLangLimits.computeAtomicCounters);
            glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &mGLSLangLimits.computeAtomicCounterBuffers);
            glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, &mGLSLangLimits.combinedShaderOutputResources);
        }

        if (has42)
        {
            glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &mGLSLangLimits.atomicCounterBindings);
            glGetIntegerv(GL_MAX_IMAGE_UNITS, &mGLSLangLimits.imageUnits);
        }

        // 一个 UAV 槽同时对应 SSBO 绑定点、image unit 与 atomic counter 绑定点，
        // 纹理 / Typed 缓冲与带计数器的缓冲还要受后两者的上限约束
        {
            const uint32_t imageUnits = mGLSLangLimits.imageUnits > 0 ? (uint32_t)mGLSLangLimits.imageUnits : 0u;
            const uint32_t counterBindings = mGLSLangLimits.atomicCounterBindings > 0 ? (uint32_t)mGLSLangLimits.atomicCounterBindings : 0u;
            mCapabilities.maxImageUnorderedAccessSlots = std::min(imageUnits, mCapabilities.maxUnorderedAccessSlots);
            mCapabilities.maxCounterUnorderedAccessSlots = std::min(counterBindings, mCapabilities.maxUnorderedAccessSlots);
        }

        // 纹理单元按阶段分区：PS 拿满片元阶段上限，其余阶段平分剩下的单元，
        // 再各自受本阶段上限约束。总数不超过 GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS，
        // 它同时也是 glActiveTexture 能用的单元总数。
        {
            auto queryLimit = [](GLenum pname) -> uint32_t
            {
                GLint value = 0;
                glGetIntegerv(pname, &value);
                return value > 0 ? static_cast<uint32_t>(value) : 0u;
            };

            const uint32_t combined = queryLimit(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
            uint32_t stageMax[kTexStageCount] = {};
            stageMax[kTexStagePS] = queryLimit(GL_MAX_TEXTURE_IMAGE_UNITS);
            stageMax[kTexStageVS] = queryLimit(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
            stageMax[kTexStageGS] = queryLimit(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);
            stageMax[kTexStageHS] = has40 ? queryLimit(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS) : 0u;
            stageMax[kTexStageDS] = has40 ? queryLimit(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS) : 0u;
            stageMax[kTexStageCS] = has43 ? queryLimit(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS) : 0u;

            const uint32_t psCount = std::min(stageMax[kTexStagePS], combined);
            const uint32_t perStage = (combined - psCount) / (kTexStageCount - 1);

            uint32_t base = 0;
            for (uint32_t stage = 0; stage < kTexStageCount; ++stage)
            {
                const uint32_t count = (stage == kTexStagePS) ? psCount : std::min(stageMax[stage], perStage);
                mTexUnitBase[stage] = base;
                mTexUnitCount[stage] = count;
                base += count;
            }

            T3D_LOG_INFO(LOG_TAG_GL4CONTEXTBASE,
                "Texture units: combined=%u, PS=%u@%u, VS=%u@%u, GS=%u@%u, HS=%u@%u, DS=%u@%u, CS=%u@%u",
                combined,
                mTexUnitCount[kTexStagePS], mTexUnitBase[kTexStagePS],
                mTexUnitCount[kTexStageVS], mTexUnitBase[kTexStageVS],
                mTexUnitCount[kTexStageGS], mTexUnitBase[kTexStageGS],
                mTexUnitCount[kTexStageHS], mTexUnitBase[kTexStageHS],
                mTexUnitCount[kTexStageDS], mTexUnitBase[kTexStageDS],
                mTexUnitCount[kTexStageCS], mTexUnitBase[kTexStageCS]);
        }

        while (glGetError() != GL_NO_ERROR) {}
    }

    //--------------------------------------------------------------------------

    TResult GL4ContextBase::initDummyContext()
    {
        TResult ret = T3D_OK;

        do
        {
#if defined(T3D_OS_WINDOWS)
            // 注册一个不可见的 dummy 窗口类
            const wchar_t *className = L"Tiny3DGL4DummyWindow";
            WNDCLASSW wc = {};
            wc.lpfnWndProc = DefWindowProcW;
            wc.hInstance = GetModuleHandleW(nullptr);
            wc.lpszClassName = className;
            RegisterClassW(&wc);

            // 创建不可见的 dummy 窗口
            mDummyHWND = CreateWindowExW(
                0, className, L"", WS_POPUP,
                0, 0, 1, 1,
                nullptr, nullptr, wc.hInstance, nullptr);
            if (mDummyHWND == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "Failed to create dummy window !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            mDummyHDC = GetDC(mDummyHWND);
            if (mDummyHDC == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "Failed to get dummy HDC !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            // 设置像素格式（必须与正式窗口兼容）
            PIXELFORMATDESCRIPTOR pfd = {};
            pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.cColorBits = 32;
            pfd.cDepthBits = 24;
            pfd.cStencilBits = 8;
            pfd.iLayerType = PFD_MAIN_PLANE;

            int pixelFormat = ChoosePixelFormat(mDummyHDC, &pfd);
            if (pixelFormat == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "ChoosePixelFormat failed for dummy window !");
                ret = T3D_ERR_GL4_SET_PIXEL_FORMAT;
                break;
            }

            if (!SetPixelFormat(mDummyHDC, pixelFormat, &pfd))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "SetPixelFormat failed for dummy window !");
                ret = T3D_ERR_GL4_SET_PIXEL_FORMAT;
                break;
            }

            // 创建 GL 上下文
            mDummyHGLRC = wglCreateContext(mDummyHDC);
            if (mDummyHGLRC == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "wglCreateContext failed for dummy window !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            if (!wglMakeCurrent(mDummyHDC, mDummyHGLRC))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "wglMakeCurrent failed for dummy window !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            // 加载 GLAD
            // 注意：wglGetProcAddress 只能加载 GL 1.2+ 扩展函数，
            // GL 1.1 核心函数（glGenTextures 等）需要从 opengl32.dll 获取。
            // 这里用组合 loader 解决此问题。
            {
                static HMODULE hOpenGL32 = LoadLibrary(TEXT("opengl32.dll"));
                auto combinedLoader = [](const char *name) -> void*
                {
                    // 优先尝试 wglGetProcAddress（GL 1.2+ 扩展）
                    void *proc = (void *)wglGetProcAddress(name);
                    if (proc == nullptr
                        || proc == (void *)0x1 || proc == (void *)0x2
                        || proc == (void *)0x3 || proc == (void *)-1)
                    {
                        // 回退到从 opengl32.dll 加载（GL 1.0/1.1 核心函数）
                        static HMODULE hGL = LoadLibrary(TEXT("opengl32.dll"));
                        proc = (void *)GetProcAddress(hGL, name);
                    }
                    return proc;
                };

                if (!gladLoadGLLoader((GLADloadproc)+combinedLoader))
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "gladLoadGLLoader failed !");
                    ret = T3D_ERR_GL4_GLAD_LOAD;
                    break;
                }
            }

#elif defined(T3D_OS_LINUX)
            mDummyDisplay = XOpenDisplay(nullptr);
            if (mDummyDisplay == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "XOpenDisplay failed for dummy !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            int screen = DefaultScreen(mDummyDisplay);
            int attribs[] = {
                GLX_RGBA,
                GLX_RED_SIZE, 8,
                GLX_GREEN_SIZE, 8,
                GLX_BLUE_SIZE, 8,
                GLX_ALPHA_SIZE, 8,
                GLX_DEPTH_SIZE, 24,
                GLX_STENCIL_SIZE, 8,
                GLX_DOUBLEBUFFER,
                None
            };

            XVisualInfo *vi = glXChooseVisual(mDummyDisplay, screen, attribs);
            if (vi == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "glXChooseVisual failed for dummy !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            ::Window rootWin = RootWindow(mDummyDisplay, screen);
            XSetWindowAttributes swa = {};
            swa.colormap = XCreateColormap(mDummyDisplay, rootWin, vi->visual, AllocNone);
            mDummyWindow = XCreateWindow(mDummyDisplay, rootWin,
                0, 0, 1, 1, 0, vi->depth, InputOutput, vi->visual,
                CWColormap, &swa);

            mDummyGLXContext = glXCreateContext(mDummyDisplay, vi, nullptr, GL_TRUE);
            XFree(vi);

            if (mDummyGLXContext == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "glXCreateContext failed for dummy !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            if (!glXMakeCurrent(mDummyDisplay, mDummyWindow, mDummyGLXContext))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "glXMakeCurrent failed for dummy !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            if (!gladLoadGLLoader((GLADloadproc)glXGetProcAddress))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "gladLoadGLLoader failed !");
                ret = T3D_ERR_GL4_GLAD_LOAD;
                break;
            }
#endif

            mGLADLoaded = true;

#if defined(T3D_OS_WINDOWS)
            // 获取 WGL 扩展函数指针（用于 MSAA 像素格式选择与 Core Profile 创建）
            mWglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
            mWglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
            mWglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

            if (mWglChoosePixelFormatARB != nullptr)
            {
                T3D_LOG_INFO(LOG_TAG_GL4CONTEXTBASE, "wglChoosePixelFormatARB is available, MSAA pixel format selection enabled.");
            }
            else
            {
                T3D_LOG_WARNING(LOG_TAG_GL4CONTEXTBASE, "wglChoosePixelFormatARB is NOT available, MSAA will be disabled for render windows.");
            }

            // 升级 dummy 上下文为 Core Profile（RenderDoc 要求通过 wglCreateContextAttribsARB 创建）
            if (mWglCreateContextAttribsARB != nullptr)
            {
                int contextFlags = 0;
#if defined(T3D_DEBUG)
                contextFlags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif
                int attribs[] = {
                    WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                    WGL_CONTEXT_MINOR_VERSION_ARB, 5,
                    WGL_CONTEXT_FLAGS_ARB, contextFlags,
                    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                    0
                };

                HGLRC coreRC = mWglCreateContextAttribsARB(mDummyHDC, nullptr, attribs);
                if (coreRC != nullptr)
                {
                    wglMakeCurrent(mDummyHDC, coreRC);
                    wglDeleteContext(mDummyHGLRC);
                    mDummyHGLRC = coreRC;
                    T3D_LOG_INFO(LOG_TAG_GL4CONTEXTBASE, "Dummy context upgraded to OpenGL 4.5 Core Profile (RenderDoc compatible).");
                }
                else
                {
                    T3D_LOG_WARNING(LOG_TAG_GL4CONTEXTBASE, "wglCreateContextAttribsARB failed for Core Profile 4.5, keeping legacy context.");
                }
            }
            else
            {
                T3D_LOG_WARNING(LOG_TAG_GL4CONTEXTBASE, "wglCreateContextAttribsARB is NOT available, RenderDoc may not work.");
            }
#endif

            T3D_LOG_INFO(LOG_TAG_GL4CONTEXTBASE, "OpenGL Vendor: %s", glGetString(GL_VENDOR));
            T3D_LOG_INFO(LOG_TAG_GL4CONTEXTBASE, "OpenGL Renderer: %s", glGetString(GL_RENDERER));
            T3D_LOG_INFO(LOG_TAG_GL4CONTEXTBASE, "OpenGL Version: %s", glGetString(GL_VERSION));
            T3D_LOG_INFO(LOG_TAG_GL4CONTEXTBASE, "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
            T3D_LOG_INFO(LOG_TAG_GL4CONTEXTBASE, "GL4 dummy context initialized.");

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void GL4ContextBase::makeDummyCurrent()
    {
#if defined(T3D_OS_WINDOWS)
        if (mDummyHDC != nullptr && mDummyHGLRC != nullptr)
        {
            wglMakeCurrent(mDummyHDC, mDummyHGLRC);
        }
#elif defined(T3D_OS_LINUX)
        if (mDummyDisplay != nullptr && mDummyWindow != 0 && mDummyGLXContext != nullptr)
        {
            glXMakeCurrent(mDummyDisplay, mDummyWindow, mDummyGLXContext);
        }
#endif
    }

    //--------------------------------------------------------------------------

    void GL4ContextBase::destroyDummyContext()
    {
#if defined(T3D_OS_WINDOWS)
        if (mDummyHGLRC != nullptr)
        {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(mDummyHGLRC);
            mDummyHGLRC = nullptr;
        }

        if (mDummyHDC != nullptr && mDummyHWND != nullptr)
        {
            ReleaseDC(mDummyHWND, mDummyHDC);
            mDummyHDC = nullptr;
        }

        if (mDummyHWND != nullptr)
        {
            DestroyWindow(mDummyHWND);
            mDummyHWND = nullptr;
            UnregisterClassW(L"Tiny3DGL4DummyWindow", GetModuleHandleW(nullptr));
        }
#elif defined(T3D_OS_LINUX)
        if (mDummyGLXContext != nullptr)
        {
            glXMakeCurrent(mDummyDisplay, None, nullptr);
            glXDestroyContext(mDummyDisplay, mDummyGLXContext);
            mDummyGLXContext = nullptr;
        }

        if (mDummyWindow != 0)
        {
            XDestroyWindow(mDummyDisplay, mDummyWindow);
            mDummyWindow = 0;
        }

        if (mDummyDisplay != nullptr)
        {
            XCloseDisplay(mDummyDisplay);
            mDummyDisplay = nullptr;
        }
#endif
    }

    //--------------------------------------------------------------------------

    TResult GL4ContextBase::compileShader(ShaderVariant *shader)
    {
        size_t bytesLength = 0;
        const char *bytes = shader->getBytesCode(bytesLength);
        shader->setBytesCode(bytes, bytesLength);

        return glslangCompileAndReflect(shader);
    }

    //--------------------------------------------------------------------------

    TResult GL4ContextBase::glslangCompileAndReflect(ShaderVariant *shader)
    {
        TResult ret = T3D_OK;

        do
        {
            EShLanguage glslangStage;
            switch (shader->getShaderStage())
            {
            case SHADER_STAGE::kVertex:   glslangStage = EShLangVertex; break;
            case SHADER_STAGE::kPixel:    glslangStage = EShLangFragment; break;
            case SHADER_STAGE::kGeometry: glslangStage = EShLangGeometry; break;
            case SHADER_STAGE::kHull:     glslangStage = EShLangTessControl; break;
            case SHADER_STAGE::kDomain:   glslangStage = EShLangTessEvaluation; break;
            case SHADER_STAGE::kCompute:  glslangStage = EShLangCompute; break;
            default:
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "glslangCompileAndReflect: unsupported shader stage !");
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            if (T3D_FAILED(ret))
                break;

            size_t bytesLength = 0;
            const char *source = shader->getBytesCode(bytesLength);

            glslang::TShader glslangShader(glslangStage);
            int sourceLen = static_cast<int>(bytesLength);
            glslangShader.setStringsWithLengths(&source, &sourceLen, 1);

            // 用实际 GL 上限覆盖 glslang 的规范最低值，否则合法 shader 会被误判。
            // 查不到（能力位不支持）就留着默认值，别用 0 去卡。
            TBuiltInResource resources = *GetDefaultResources();
            auto raise = [](int &dst, int actual)
            {
                if (actual > dst) { dst = actual; }
            };
            raise(resources.maxAtomicCounterBindings, mGLSLangLimits.atomicCounterBindings);
            raise(resources.maxComputeAtomicCounters, mGLSLangLimits.computeAtomicCounters);
            raise(resources.maxComputeAtomicCounterBuffers, mGLSLangLimits.computeAtomicCounterBuffers);
            raise(resources.maxCombinedShaderOutputResources, mGLSLangLimits.combinedShaderOutputResources);
            raise(resources.maxImageUnits, mGLSLangLimits.imageUnits);
            raise(resources.maxComputeWorkGroupCountX, mGLSLangLimits.maxComputeWorkGroupCount[0]);
            raise(resources.maxComputeWorkGroupCountY, mGLSLangLimits.maxComputeWorkGroupCount[1]);
            raise(resources.maxComputeWorkGroupCountZ, mGLSLangLimits.maxComputeWorkGroupCount[2]);
            raise(resources.maxComputeWorkGroupSizeX, mGLSLangLimits.maxComputeWorkGroupSize[0]);
            raise(resources.maxComputeWorkGroupSizeY, mGLSLangLimits.maxComputeWorkGroupSize[1]);
            raise(resources.maxComputeWorkGroupSizeZ, mGLSLangLimits.maxComputeWorkGroupSize[2]);

            const int glslVersion = (shader->getShaderStage() == SHADER_STAGE::kCompute) ? 430 : 400;
            if (!glslangShader.parse(&resources, glslVersion, false, EShMsgDefault))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "glslang parse error:\n%s", glslangShader.getInfoLog());
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            glslang::TProgram program;
            program.addShader(&glslangShader);

            if (!program.link(EShMsgDefault))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "glslang link error:\n%s", program.getInfoLog());
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            if (!program.buildReflection(EShReflectionAllBlockVariables | EShReflectionSeparateBuffers))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "glslang buildReflection failed !");
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            GlslangReflectionData data;

            if (shader->getShaderStage() == SHADER_STAGE::kCompute)
            {
                data.localSize[0] = program.getLocalSize(0);
                data.localSize[1] = program.getLocalSize(1);
                data.localSize[2] = program.getLocalSize(2);
            }

            int numBlocks = program.getNumUniformBlocks();
            data.blocks.resize(numBlocks);
            for (int i = 0; i < numBlocks; ++i)
            {
                const auto &block = program.getUniformBlock(i);
                data.blocks[i].name = block.name.c_str();
                data.blocks[i].size = block.size;
            }

            int numUniforms = program.getNumUniformVariables();
            data.uniforms.resize(numUniforms);
            for (int i = 0; i < numUniforms; ++i)
            {
                const auto &uniform = program.getUniform(i);
                data.uniforms[i].name = uniform.name.c_str();
                data.uniforms[i].glDefineType = uniform.glDefineType;
                data.uniforms[i].offset = uniform.offset;
                data.uniforms[i].size = uniform.size;
                data.uniforms[i].blockIndex = uniform.index;
                data.uniforms[i].arrayStride = uniform.arrayStride;
                data.uniforms[i].binding = uniform.getBinding();
            }

            int numBufferBlocks = program.getNumBufferBlocks();
            data.bufferBlocks.resize(numBufferBlocks);
            for (int i = 0; i < numBufferBlocks; ++i)
            {
                const auto &block = program.getBufferBlock(i);
                data.bufferBlocks[i].name = block.name.c_str();
                data.bufferBlocks[i].binding = block.getBinding();
                data.bufferBlocks[i].size = block.size;
                const glslang::TType *blockType = block.getType();
                data.bufferBlocks[i].isReadOnly = (blockType != nullptr && blockType->getQualifier().isReadOnly());
            }

            int numBufVars = program.getNumBufferVariables();
            for (int i = 0; i < numBufVars; ++i)
            {
                const auto &var = program.getBufferVariable(i);
                if (var.index >= 0 && var.index < numBufferBlocks)
                {
                    uint32_t stride = 0;
                    if (var.topLevelArrayStride > 0)
                    {
                        stride = static_cast<uint32_t>(var.topLevelArrayStride);
                    }
                    else if (var.arrayStride > 0)
                    {
                        stride = static_cast<uint32_t>(var.arrayStride);
                    }
                    if (stride > 0 && data.bufferBlocks[var.index].elementStride == 0)
                    {
                        data.bufferBlocks[var.index].elementStride = stride;
                    }
                }
            }

            int numAtomicCounters = program.getNumAtomicCounters();
            data.atomicCounterBindings.resize(numAtomicCounters);
            for (int i = 0; i < numAtomicCounters; ++i)
            {
                data.atomicCounterBindings[i] = program.getAtomicCounter(i).getBinding();
            }

            mReflectionCache[shader] = std::move(data);

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool GL4ContextBase::mapSamplerGLType(GLenum glDefineType, TEXTURE_TYPE &outTexType)
    {
        switch (glDefineType)
        {
        case GL_SAMPLER_1D:
        case GL_SAMPLER_1D_SHADOW:
        case GL_SAMPLER_1D_ARRAY:
        case GL_SAMPLER_1D_ARRAY_SHADOW:
        case GL_INT_SAMPLER_1D:
        case GL_INT_SAMPLER_1D_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_1D:
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
            outTexType = TEXTURE_TYPE::TT_1D;
            return true;
        case GL_SAMPLER_2D:
        case GL_SAMPLER_2D_SHADOW:
        case GL_SAMPLER_2D_MULTISAMPLE:
        case GL_INT_SAMPLER_2D:
        case GL_INT_SAMPLER_2D_MULTISAMPLE:
        case GL_UNSIGNED_INT_SAMPLER_2D:
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
            outTexType = TEXTURE_TYPE::TT_2D;
            return true;
        case GL_SAMPLER_2D_ARRAY:
        case GL_SAMPLER_2D_ARRAY_SHADOW:
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_INT_SAMPLER_2D_ARRAY:
        case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
            outTexType = TEXTURE_TYPE::TT_2D_ARRAY;
            return true;
        case GL_SAMPLER_3D:
        case GL_INT_SAMPLER_3D:
        case GL_UNSIGNED_INT_SAMPLER_3D:
            outTexType = TEXTURE_TYPE::TT_3D;
            return true;
        case GL_SAMPLER_CUBE:
        case GL_SAMPLER_CUBE_SHADOW:
        case GL_INT_SAMPLER_CUBE:
        case GL_UNSIGNED_INT_SAMPLER_CUBE:
            outTexType = TEXTURE_TYPE::TT_CUBE;
            return true;
        case GL_SAMPLER_CUBE_MAP_ARRAY_ARB:
        case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW_ARB:
        case GL_INT_SAMPLER_CUBE_MAP_ARRAY_ARB:
        case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY_ARB:
            outTexType = TEXTURE_TYPE::TT_CUBE_ARRAY;
            return true;
        default:
            break;
        }
        return false;
    }

    //--------------------------------------------------------------------------

    bool GL4ContextBase::isSamplerBufferGLType(GLenum glDefineType)
    {
        switch (glDefineType)
        {
        case GL_SAMPLER_BUFFER:
        case GL_INT_SAMPLER_BUFFER:
        case GL_UNSIGNED_INT_SAMPLER_BUFFER:
            return true;
        default:
            break;
        }
        return false;
    }

    //--------------------------------------------------------------------------

    bool GL4ContextBase::isImageGLType(GLenum glDefineType, bool &outIsBuffer)
    {
        outIsBuffer = false;
        switch (glDefineType)
        {
        case GL_IMAGE_BUFFER:
        case GL_INT_IMAGE_BUFFER:
        case GL_UNSIGNED_INT_IMAGE_BUFFER:
            outIsBuffer = true;
            return true;
        case GL_IMAGE_1D:
        case GL_IMAGE_2D:
        case GL_IMAGE_3D:
        case GL_IMAGE_2D_RECT:
        case GL_IMAGE_CUBE:
        case GL_IMAGE_1D_ARRAY:
        case GL_IMAGE_2D_ARRAY:
        case GL_IMAGE_CUBE_MAP_ARRAY:
        case GL_IMAGE_2D_MULTISAMPLE:
        case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
        case GL_INT_IMAGE_1D:
        case GL_INT_IMAGE_2D:
        case GL_INT_IMAGE_3D:
        case GL_INT_IMAGE_2D_RECT:
        case GL_INT_IMAGE_CUBE:
        case GL_INT_IMAGE_1D_ARRAY:
        case GL_INT_IMAGE_2D_ARRAY:
        case GL_INT_IMAGE_CUBE_MAP_ARRAY:
        case GL_INT_IMAGE_2D_MULTISAMPLE:
        case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
        case GL_UNSIGNED_INT_IMAGE_1D:
        case GL_UNSIGNED_INT_IMAGE_2D:
        case GL_UNSIGNED_INT_IMAGE_3D:
        case GL_UNSIGNED_INT_IMAGE_2D_RECT:
        case GL_UNSIGNED_INT_IMAGE_CUBE:
        case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
        case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
        case GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY:
        case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
        case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
            return true;
        default:
            break;
        }
        return false;
    }

    //--------------------------------------------------------------------------

    TResult GL4ContextBase::reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            auto itr = mReflectionCache.find(shader);
            if (itr == mReflectionCache.end())
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "reflectShaderAllBindings: no cached reflection data (compileShader not called?)");
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            const GlslangReflectionData &data = itr->second;

            if (shader->getShaderStage() == SHADER_STAGE::kCompute
                && data.localSize[0] > 0 && data.localSize[1] > 0 && data.localSize[2] > 0)
            {
                shader->setThreadGroupSize(data.localSize[0], data.localSize[1], data.localSize[2]);
            }

            // Reflect Uniform Blocks (cbuffers) — iterate block members
            for (int blockIdx = 0; blockIdx < static_cast<int>(data.blocks.size()); ++blockIdx)
            {
                for (const auto &uniform : data.uniforms)
                {
                    if (uniform.blockIndex != blockIdx)
                        continue;

                    TEXTURE_TYPE unusedTexType = TEXTURE_TYPE::TT_2D;
                    bool unusedIsBuf = false;
                    if (mapSamplerGLType(static_cast<GLenum>(uniform.glDefineType), unusedTexType)
                        || isSamplerBufferGLType(static_cast<GLenum>(uniform.glDefineType))
                        || isImageGLType(static_cast<GLenum>(uniform.glDefineType), unusedIsBuf)
                        || uniform.glDefineType == GL_UNSIGNED_INT_ATOMIC_COUNTER)
                    {
                        continue;
                    }

                    const uint32_t elemCount = static_cast<uint32_t>(std::max(1, uniform.size));
                    const bool isArray = (elemCount > 1);
                    auto calcSize = [&](uint32_t elemBytes) -> uint32_t
                    {
                        if (isArray && uniform.arrayStride > 0)
                        {
                            return static_cast<uint32_t>(uniform.arrayStride) * elemCount;
                        }
                        return elemBytes * elemCount;
                    };

                    uint32_t dataSize = 0;
                    ShaderConstantParam::DATA_TYPE dataType = ShaderConstantParam::DATA_TYPE::DT_FLOAT;

                    switch (uniform.glDefineType)
                    {
                    case GL_FLOAT:
                        dataSize = calcSize(sizeof(float));
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_FLOAT_VEC2:
                        dataSize = calcSize(sizeof(float) * 2);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_FLOAT_VEC3:
                        dataSize = calcSize(sizeof(float) * 3);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_FLOAT_VEC4:
                        dataSize = calcSize(sizeof(float) * 4);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY : ShaderConstantParam::DATA_TYPE::DT_VECTOR4;
                        break;
                    case GL_FLOAT_MAT2:
                        dataSize = calcSize(sizeof(float) * 4);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_FLOAT_MAT2x3:
                    case GL_FLOAT_MAT3x2:
                        dataSize = calcSize(sizeof(float) * 6);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_FLOAT_MAT2x4:
                    case GL_FLOAT_MAT4x2:
                        dataSize = calcSize(sizeof(float) * 8);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_FLOAT_MAT3:
                        dataSize = calcSize(sizeof(float) * 9);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_FLOAT_MAT3x4:
                    case GL_FLOAT_MAT4x3:
                        dataSize = calcSize(sizeof(float) * 12);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_FLOAT_MAT4:
                        dataSize = calcSize(sizeof(float) * 16);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_MATRIX4_ARRAY : ShaderConstantParam::DATA_TYPE::DT_MATRIX4;
                        break;
                    case GL_INT:
                    case GL_UNSIGNED_INT:
                        dataSize = calcSize(sizeof(int));
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY : ShaderConstantParam::DATA_TYPE::DT_INTEGER;
                        break;
                    case GL_INT_VEC2:
                    case GL_UNSIGNED_INT_VEC2:
                        dataSize = calcSize(sizeof(int) * 2);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY : ShaderConstantParam::DATA_TYPE::DT_INTEGER;
                        break;
                    case GL_INT_VEC3:
                    case GL_UNSIGNED_INT_VEC3:
                        dataSize = calcSize(sizeof(int) * 3);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY : ShaderConstantParam::DATA_TYPE::DT_INTEGER;
                        break;
                    case GL_INT_VEC4:
                    case GL_UNSIGNED_INT_VEC4:
                        dataSize = calcSize(sizeof(int) * 4);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY : ShaderConstantParam::DATA_TYPE::DT_INTEGER;
                        break;
                    case GL_BOOL:
                        dataSize = calcSize(sizeof(int));
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY : ShaderConstantParam::DATA_TYPE::DT_BOOL;
                        break;
                    case GL_BOOL_VEC2:
                        dataSize = calcSize(sizeof(int) * 2);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY : ShaderConstantParam::DATA_TYPE::DT_BOOL;
                        break;
                    case GL_BOOL_VEC3:
                        dataSize = calcSize(sizeof(int) * 3);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY : ShaderConstantParam::DATA_TYPE::DT_BOOL;
                        break;
                    case GL_BOOL_VEC4:
                        dataSize = calcSize(sizeof(int) * 4);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY : ShaderConstantParam::DATA_TYPE::DT_BOOL;
                        break;
                    case GL_DOUBLE:
                        dataSize = calcSize(sizeof(double));
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_DOUBLE_VEC2:
                        dataSize = calcSize(sizeof(double) * 2);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_DOUBLE_VEC3:
                        dataSize = calcSize(sizeof(double) * 3);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_DOUBLE_VEC4:
                        dataSize = calcSize(sizeof(double) * 4);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_DOUBLE_MAT2:
                        dataSize = calcSize(sizeof(double) * 4);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_DOUBLE_MAT2x3:
                    case GL_DOUBLE_MAT3x2:
                        dataSize = calcSize(sizeof(double) * 6);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_DOUBLE_MAT2x4:
                    case GL_DOUBLE_MAT4x2:
                        dataSize = calcSize(sizeof(double) * 8);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_DOUBLE_MAT3:
                        dataSize = calcSize(sizeof(double) * 9);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_DOUBLE_MAT3x4:
                    case GL_DOUBLE_MAT4x3:
                        dataSize = calcSize(sizeof(double) * 12);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    case GL_DOUBLE_MAT4:
                        dataSize = calcSize(sizeof(double) * 16);
                        dataType = isArray ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                        break;
                    default:
                        dataSize = uniform.arrayStride > 0
                            ? static_cast<uint32_t>(uniform.arrayStride) * elemCount
                            : static_cast<uint32_t>(std::max(1, data.blocks[blockIdx].size - std::max(0, uniform.offset)));
                        dataType = ShaderConstantParam::DATA_TYPE::DT_STRUCT;
                        break;
                    }

                    // Convert SPIRV-Cross generated names back to original names
                    String cbufferName = data.blocks[blockIdx].name;
                    String cname = uniform.name;

                    if (StringUtil::startsWith(cbufferName, "type_"))
                    {
                        cbufferName = cbufferName.substr(5);
                    }

                    String::size_type dotPos = cname.find('.');
                    if (dotPos != String::npos)
                    {
                        cname = cname.substr(dotPos + 1);
                    }

                    String::size_type bracketPos = cname.find('[');
                    if (bracketPos != String::npos)
                    {
                        cname = cname.substr(0, bracketPos);
                    }

                    ShaderConstantParamPtr param = ShaderConstantParam::create(cbufferName, cname, blockIdx, dataSize, uniform.offset, dataType);
                    constantParams.emplace(param->getName(), param);

                    T3D_LOG_DEBUG(LOG_TAG_GL4CONTEXTBASE, "Shader reflection - UBO: %s -> %s, uniform: %s -> %s, type: %u, size: %u, offset: %d",
                        data.blocks[blockIdx].name.c_str(), cbufferName.c_str(), uniform.name.c_str(), cname.c_str(), dataType, dataSize, uniform.offset);
                }
            }

            // 按 GL 类型识别采样器。glslang 的 uniform.index 语义随版本变化，
            // 用它反推 standalone / block 成员不可靠，直接看类型。
            assignSamplerBindings(data, samplerParams, true);

            ShaderResourceParams resourceParams;

            for (const auto &uniform : data.uniforms)
            {
                TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;
                if (mapSamplerGLType(static_cast<GLenum>(uniform.glDefineType), texType))
                {
                    continue;
                }

                if (isSamplerBufferGLType(static_cast<GLenum>(uniform.glDefineType)))
                {
                    const uint32_t binding = (uniform.binding >= 0) ? static_cast<uint32_t>(uniform.binding) : 0u;
                    ShaderResourceParamPtr param = ShaderResourceParam::create(
                        uniform.name, ShaderResourceParam::Kind::kTypedSRV, binding, 0, false);
                    resourceParams.emplace(uniform.name, param);

                    T3D_LOG_DEBUG(LOG_TAG_GL4CONTEXTBASE,
                        "Shader reflection - samplerBuffer resource: %s, kind: %u, binding: %u",
                        uniform.name.c_str(), static_cast<uint32_t>(ShaderResourceParam::Kind::kTypedSRV), binding);
                    continue;
                }

                bool isImageBuf = false;
                if (isImageGLType(static_cast<GLenum>(uniform.glDefineType), isImageBuf))
                {
                    const uint32_t binding = (uniform.binding >= 0) ? static_cast<uint32_t>(uniform.binding) : 0u;
                    const ShaderResourceParam::Kind kind = isImageBuf
                        ? ShaderResourceParam::Kind::kTypedUAV
                        : ShaderResourceParam::Kind::kTextureUAV;
                    ShaderResourceParamPtr param = ShaderResourceParam::create(uniform.name, kind, binding, 0, false);
                    resourceParams.emplace(uniform.name, param);

                    T3D_LOG_DEBUG(LOG_TAG_GL4CONTEXTBASE,
                        "Shader reflection - image resource: %s, kind: %u, binding: %u",
                        uniform.name.c_str(), static_cast<uint32_t>(kind), binding);
                }
            }

            // HLSLCross 生成的计数器是单独的 SSBO 块，不作为资源上报，只给对应的缓冲置 hasCounter
            TSet<String> counterOwners;
            for (const auto &block : data.bufferBlocks)
            {
                BufferBlockName parsed;
                if (parseBufferBlockName(block.name, parsed) && parsed.isCounter)
                {
                    counterOwners.insert(parsed.name);
                }
            }

            for (const auto &block : data.bufferBlocks)
            {
                String name = block.name;
                ShaderResourceParam::Kind kind = block.isReadOnly
                    ? ShaderResourceParam::Kind::kStructuredSRV
                    : ShaderResourceParam::Kind::kStructuredUAV;
                uint32_t elementStride = block.elementStride;
                bool hasCounter = false;

                BufferBlockName parsed;
                if (parseBufferBlockName(block.name, parsed))
                {
                    if (parsed.isCounter)
                    {
                        continue;
                    }

                    name = parsed.name;
                    kind = parsed.kind;
                    hasCounter = (kind == ShaderResourceParam::Kind::kAppendUAV)
                        || (kind == ShaderResourceParam::Kind::kConsumeUAV)
                        || (counterOwners.count(name) > 0);
                }
                else
                {
                    // 手写 GLSL：没有种类信息，只能按 readonly 区分 SRV / UAV，计数器写成与缓冲同 binding 的 atomic_uint
                    for (int acBinding : data.atomicCounterBindings)
                    {
                        if (acBinding == block.binding)
                        {
                            hasCounter = true;
                            break;
                        }
                    }
                }

                // 与 D3D11 反射一致，字节地址缓冲的元素字节数为 0
                if (kind == ShaderResourceParam::Kind::kByteAddressSRV || kind == ShaderResourceParam::Kind::kByteAddressUAV)
                {
                    elementStride = 0;
                }

                const uint32_t binding = (block.binding >= 0) ? static_cast<uint32_t>(block.binding) : 0u;
                ShaderResourceParamPtr param = ShaderResourceParam::create(
                    name, kind, binding, elementStride, hasCounter);
                resourceParams.emplace(name, param);

                T3D_LOG_DEBUG(LOG_TAG_GL4CONTEXTBASE,
                    "Shader reflection - buffer block: %s -> %s, kind: %u, binding: %u, stride: %u, counter: %d",
                    block.name.c_str(), name.c_str(), static_cast<uint32_t>(kind), binding, elementStride, hasCounter ? 1 : 0);
            }

            shader->setShaderResourceParams(resourceParams);

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4ContextBase::reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            auto itr = mReflectionCache.find(shader);
            if (itr == mReflectionCache.end())
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONTEXTBASE, "reflectSamplerBindings: no cached reflection data (compileShader not called?)");
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            // 只回填已有的 sampler 参数，不新建 —— 新建是 reflectShaderAllBindings 的职责
            assignSamplerBindings(itr->second, samplerParams, false);

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool GL4ContextBase::parseRegisterCombinedName(const String &uniformName, CombinedSamplerName &result)
    {
        static const String kPrefix = "T3D_Combined_t";
        static const String kSamplerPrefix = "sampler";

        if (!StringUtil::startsWith(uniformName, kPrefix, false))
        {
            return false;
        }

        String::size_type pos = kPrefix.size();
        auto readNumber = [&uniformName, &pos](uint32_t &value) -> bool
        {
            const String::size_type start = pos;
            uint64_t v = 0;
            while (pos < uniformName.size() && uniformName[pos] >= '0' && uniformName[pos] <= '9')
            {
                v = v * 10 + static_cast<uint64_t>(uniformName[pos] - '0');
                if (v >= UINT32_MAX)
                {
                    return false;
                }
                ++pos;
            }
            value = static_cast<uint32_t>(v);
            return pos > start;
        };
        auto expect = [&uniformName, &pos](char c) -> bool
        {
            if (pos < uniformName.size() && uniformName[pos] == c)
            {
                ++pos;
                return true;
            }
            return false;
        };

        uint32_t texSlot = 0;
        uint32_t samplerSlot = UINT32_MAX;
        uint32_t texNameLen = 0;
        if (!readNumber(texSlot) || !expect('s'))
        {
            return false;
        }
        if (!expect('x') && !readNumber(samplerSlot))
        {
            return false;
        }
        if (!expect('l') || !readNumber(texNameLen) || texNameLen == 0 || pos + texNameLen > uniformName.size())
        {
            return false;
        }

        const String samplerName = uniformName.substr(pos + texNameLen);
        if ((samplerSlot == UINT32_MAX) != samplerName.empty())
        {
            return false;
        }

        result.texName = uniformName.substr(pos, texNameLen);
        result.texSlot = texSlot;
        result.samplerSlot = samplerSlot;
        if (samplerName.empty())
        {
            result.samplerKey.clear();
        }
        else if (samplerName.size() > kSamplerPrefix.size() && StringUtil::startsWith(samplerName, kSamplerPrefix, false))
        {
            result.samplerKey = samplerName.substr(kSamplerPrefix.size());
        }
        else
        {
            // D3D11 反射不接受这种采样器名，这里保持整名作键
            result.samplerKey = samplerName;
        }
        return true;
    }

    //--------------------------------------------------------------------------

    bool GL4ContextBase::parseBufferBlockName(const String &blockName, BufferBlockName &result)
    {
        static const String kPrefix = "T3D_Buffer_";

        struct KindTag
        {
            const char *tag;
            ShaderResourceParam::Kind kind;
            bool isCounter;
        };

        static const KindTag kTags[] =
        {
            { "sb",     ShaderResourceParam::Kind::kStructuredSRV,  false },
            { "rwsb",   ShaderResourceParam::Kind::kStructuredUAV,  false },
            { "bab",    ShaderResourceParam::Kind::kByteAddressSRV, false },
            { "rwbab",  ShaderResourceParam::Kind::kByteAddressUAV, false },
            { "asb",    ShaderResourceParam::Kind::kAppendUAV,      false },
            { "csb",    ShaderResourceParam::Kind::kConsumeUAV,     false },
            { "cnt",    ShaderResourceParam::Kind::kStructuredUAV,  true  },
        };

        if (!StringUtil::startsWith(blockName, kPrefix, false))
        {
            return false;
        }

        const String::size_type tagEnd = blockName.find('l', kPrefix.size());
        if (tagEnd == String::npos)
        {
            return false;
        }

        const String tag = blockName.substr(kPrefix.size(), tagEnd - kPrefix.size());
        const KindTag *kindTag = nullptr;
        for (const KindTag &t : kTags)
        {
            if (tag == t.tag)
            {
                kindTag = &t;
                break;
            }
        }
        if (kindTag == nullptr)
        {
            return false;
        }

        String::size_type pos = tagEnd + 1;
        uint64_t nameLen = 0;
        const String::size_type digitsStart = pos;
        while (pos < blockName.size() && blockName[pos] >= '0' && blockName[pos] <= '9')
        {
            nameLen = nameLen * 10 + static_cast<uint64_t>(blockName[pos] - '0');
            if (nameLen > blockName.size())
            {
                return false;
            }
            ++pos;
        }
        if (pos == digitsStart || nameLen == 0 || pos + nameLen != blockName.size())
        {
            return false;
        }

        result.name = blockName.substr(pos);
        result.kind = kindTag->kind;
        result.isCounter = kindTag->isCounter;
        return true;
    }

    //--------------------------------------------------------------------------

    GL4ContextBase::CombinedSamplerName GL4ContextBase::parseCombinedSamplerName(const String &uniformName)
    {
        static const String kCombinedPrefix = "SPIRV_Cross_Combined";
        static const String kSamplerPrefix = "sampler";
        static const String kDummySampler = "SPIRV_Cross_DummySampler";

        CombinedSamplerName result;
        result.texName = uniformName;
        result.samplerKey = uniformName;

        if (parseRegisterCombinedName(uniformName, result))
        {
            return result;
        }

        if (!StringUtil::startsWith(uniformName, kCombinedPrefix, false))
        {
            return result;
        }

        const String rest = uniformName.substr(kCombinedPrefix.size());

        if (rest.size() > kDummySampler.size() && StringUtil::endsWith(rest, kDummySampler, false))
        {
            result.texName = rest.substr(0, rest.size() - kDummySampler.size());
            result.samplerKey.clear();
            return result;
        }

        String::size_type split = String::npos;
        for (String::size_type pos = rest.find(kSamplerPrefix, 1); pos != String::npos;
            pos = rest.find(kSamplerPrefix, pos + 1))
        {
            if (rest.compare(pos + kSamplerPrefix.size(), String::npos, rest, 0, pos) == 0)
            {
                split = pos;
                break;
            }
        }
        if (split == String::npos)
        {
            split = rest.find(kSamplerPrefix, 1);
        }

        // 采样器名不以 "sampler" 开头时拆不开，D3D11 反射同样不接受这种命名，保持整名
        if (split != String::npos && split + kSamplerPrefix.size() < rest.size())
        {
            result.texName = rest.substr(0, split);
            result.samplerKey = rest.substr(split + kSamplerPrefix.size());
        }

        return result;
    }

    //--------------------------------------------------------------------------

    void GL4ContextBase::assignSamplerBindings(const GlslangReflectionData &data, ShaderSamplerParams &samplerParams, bool createMissing)
    {
        auto findParam = [&samplerParams, createMissing](const String &name) -> ShaderSamplerParam*
        {
            const auto it = samplerParams.find(name);
            if (it != samplerParams.end())
            {
                return it->second.get();
            }
            if (!createMissing)
            {
                return nullptr;
            }
            ShaderSamplerParamPtr param = ShaderSamplerParam::create(name);
            param->clearUsage();
            samplerParams.emplace(name, param);
            return param.get();
        };

        struct SlotTable
        {
            TMap<String, uint32_t> slots;
            TSet<uint32_t> used;

            void reserve(const String &name, uint32_t slot)
            {
                if (slots.emplace(name, slot).second)
                {
                    used.insert(slot);
                }
            }

            uint32_t slotOf(const String &name)
            {
                const auto it = slots.find(name);
                if (it != slots.end())
                {
                    return it->second;
                }
                uint32_t slot = 0;
                while (used.count(slot) > 0)
                {
                    ++slot;
                }
                slots.emplace(name, slot);
                used.insert(slot);
                return slot;
            }
        };

        SlotTable texSlots;
        SlotTable samplerSlots;
        TArray<std::pair<const GlslangUniformInfo*, CombinedSamplerName>> samplers;

        for (auto &item : samplerParams)
        {
            item.second->clearUsage();
        }

        // 先登记名字里带的寄存器号，不带寄存器号的旧名字再按顺序取剩下的槽位，避免两者撞号
        for (const auto &uniform : data.uniforms)
        {
            TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;
            if (!mapSamplerGLType(static_cast<GLenum>(uniform.glDefineType), texType))
            {
                continue;
            }

            samplers.emplace_back(&uniform, parseCombinedSamplerName(uniform.name));
            const CombinedSamplerName &parsed = samplers.back().second;
            if (parsed.texSlot != UINT32_MAX)
            {
                texSlots.reserve(parsed.texName, parsed.texSlot);
            }
            if (parsed.samplerSlot != UINT32_MAX)
            {
                samplerSlots.reserve(parsed.samplerKey, parsed.samplerSlot);
            }
        }

        for (const auto &item : samplers)
        {
            const GlslangUniformInfo &uniform = *item.first;
            const CombinedSamplerName &parsed = item.second;

            TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;
            mapSamplerGLType(static_cast<GLenum>(uniform.glDefineType), texType);

            const uint32_t texSlot = texSlots.slotOf(parsed.texName);
            ShaderSamplerParam *texParam = findParam(parsed.texName);
            if (texParam != nullptr)
            {
                texParam->setTexBinding(texSlot);
                texParam->setTextureType(texType);
                texParam->setHasTexture(true);
            }

            int32_t samplerSlot = -1;
            if (!parsed.samplerKey.empty())
            {
                samplerSlot = static_cast<int32_t>(samplerSlots.slotOf(parsed.samplerKey));
                ShaderSamplerParam *samplerParam = findParam(parsed.samplerKey);
                if (samplerParam != nullptr)
                {
                    samplerParam->setSamplerBinding(static_cast<uint32_t>(samplerSlot));
                    samplerParam->setHasSampler(true);
                }
            }

            T3D_LOG_DEBUG(LOG_TAG_GL4CONTEXTBASE,
                "Shader reflection - sampler: %s -> texture '%s' t%u, sampler '%s' s%d, type: %d",
                uniform.name.c_str(), parsed.texName.c_str(), texSlot, parsed.samplerKey.c_str(), samplerSlot, texType);
        }
    }

    //--------------------------------------------------------------------------
}
