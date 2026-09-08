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


#include "T3DGL4Context.h"
#include "T3DGL4RenderWindow.h"
#include "T3DGL4Error.h"
#include "T3DGL4RenderBuffer.h"
#include "T3DGL4Mapping.h"
#include "T3DGL4RenderState.h"
#include "T3DGL4Shader.h"
#include "T3DGL4Renderer.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GL4ContextPtr GL4Context::create()
    {
        GL4ContextPtr ctx = T3D_NEW GL4Context();
        if (ctx && T3D_FAILED(ctx->init()))
        {
            ctx = nullptr;
        }
        return ctx;
    }

    //--------------------------------------------------------------------------

    GL4Context::GL4Context()
    {
    }

    //--------------------------------------------------------------------------

    GL4Context::~GL4Context()
    {
        mCurrentRenderTarget = nullptr;

        GL_SAFE_DELETE_PROGRAM(mCurrentProgram);
        GL_SAFE_DELETE_PROGRAM(mCurrentComputeProgram);
        GL_SAFE_DELETE_FBO(mScratchReadFBO);
        GL_SAFE_DELETE_FBO(mScratchDrawFBO);
        GL_SAFE_DELETE_VAO(mCurrentVAO);

        for (auto &pair : mPendingReadbacks)
        {
            GL_SAFE_DELETE_BUFFER(pair.second.Staging);
        }
        mPendingReadbacks.clear();

        if (mGlslangInitialized)
        {
            glslang::FinalizeProcess();
            mGlslangInitialized = false;
        }

        destroyDummyContext();
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::init()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = initDummyContext();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Failed to init dummy GL context !");
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

    void GL4Context::fillCapabilities()
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
            }

            GLint sharedMemory = 0;
            glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &sharedMemory);
            mCapabilities.maxComputeSharedMemory = (uint32_t)sharedMemory;

            GLint storageBuffers = 0;
            glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &storageBuffers);
            mCapabilities.maxUnorderedAccessSlots = (uint32_t)storageBuffers;
        }

        while (glGetError() != GL_NO_ERROR) {}
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::initDummyContext()
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
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Failed to create dummy window !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            mDummyHDC = GetDC(mDummyHWND);
            if (mDummyHDC == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Failed to get dummy HDC !");
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
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "ChoosePixelFormat failed for dummy window !");
                ret = T3D_ERR_GL4_SET_PIXEL_FORMAT;
                break;
            }

            if (!SetPixelFormat(mDummyHDC, pixelFormat, &pfd))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "SetPixelFormat failed for dummy window !");
                ret = T3D_ERR_GL4_SET_PIXEL_FORMAT;
                break;
            }

            // 创建 GL 上下文
            mDummyHGLRC = wglCreateContext(mDummyHDC);
            if (mDummyHGLRC == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "wglCreateContext failed for dummy window !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            if (!wglMakeCurrent(mDummyHDC, mDummyHGLRC))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "wglMakeCurrent failed for dummy window !");
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
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "gladLoadGLLoader failed !");
                    ret = T3D_ERR_GL4_GLAD_LOAD;
                    break;
                }
            }

#elif defined(T3D_OS_LINUX)
            mDummyDisplay = XOpenDisplay(nullptr);
            if (mDummyDisplay == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "XOpenDisplay failed for dummy !");
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
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "glXChooseVisual failed for dummy !");
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
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "glXCreateContext failed for dummy !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            if (!glXMakeCurrent(mDummyDisplay, mDummyWindow, mDummyGLXContext))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "glXMakeCurrent failed for dummy !");
                ret = T3D_ERR_GL4_CREATE_CONTEXT;
                break;
            }

            if (!gladLoadGLLoader((GLADloadproc)glXGetProcAddress))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "gladLoadGLLoader failed !");
                ret = T3D_ERR_GL4_GLAD_LOAD;
                break;
            }
#endif

            mGLADLoaded = true;

#if defined(T3D_OS_WINDOWS)
            // 获取 WGL 扩展函数指针（用于 MSAA 像素格式选择）
            mWglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
            mWglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
            mWglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

            if (mWglChoosePixelFormatARB != nullptr)
            {
                T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "wglChoosePixelFormatARB is available, MSAA pixel format selection enabled.");
            }
            else
            {
                T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "wglChoosePixelFormatARB is NOT available, MSAA will be disabled for render windows.");
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
                    T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "Dummy context upgraded to OpenGL 4.5 Core Profile (RenderDoc compatible).");
                }
                else
                {
                    T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "wglCreateContextAttribsARB failed for Core Profile 4.5, keeping legacy context.");
                }
            }
            else
            {
                T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "wglCreateContextAttribsARB is NOT available, RenderDoc may not work.");
            }
#endif

            T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "OpenGL Vendor: %s", glGetString(GL_VENDOR));
            T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "OpenGL Renderer: %s", glGetString(GL_RENDERER));
            T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "OpenGL Version: %s", glGetString(GL_VERSION));
            T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

            T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "Dummy GL context initialized, GLAD loaded successfully.");

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void GL4Context::destroyDummyContext()
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

    TResult GL4Context::swapBackBuffer(GL4RenderWindow *renderWindow)
    {
        auto lambda = [this](GL4RenderWindowPtr renderWindow)
        {
            TResult ret = T3D_OK;

            do
            {
#if defined(T3D_OS_WINDOWS)
                if (!::SwapBuffers(renderWindow->GLDeviceContext))
                {
                    ret = T3D_ERR_GL4_PRESENT;
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "SwapBuffers failed !");
                    break;
                }
#elif defined(T3D_OS_LINUX)
                glXSwapBuffers(renderWindow->GLDisplay, renderWindow->GLWindow);
#endif
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, GL4RenderWindowPtr(renderWindow));
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::resizeRenderWindow(GL4RenderWindow *rw, uint32_t w, uint32_t h)
    {
        rw->mWidth = w;
        rw->mHeight = h;

        auto lambda = [this](uint32_t w, uint32_t h)
        {
            TResult ret = T3D_OK;

            do
            {
                glViewport(0, 0, (GLsizei)w, (GLsizei)h);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::resizeRenderWindow");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, w, h);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)
    {
        // GL4 now uses native OpenGL NDC with Z in [-1, 1] (no glClipControl).
        // When rendering to FBO, flip Y to match D3D orientation.
        static Matrix4 flipYMat(
            1.0f,  0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 0.0f,
            0.0f,  0.0f, 1.0f, 0.0f,
            0.0f,  0.0f, 0.0f, 1.0f
            );

        mViewMatrix = viewMat;

        if (mRenderingToFBO)
        {
            mProjMatrix = flipYMat * projMat;
            mProjectionFlipped = true;
        }
        else
        {
            mProjMatrix = projMat;
            mProjectionFlipped = false;
        }

        mProjViewMatrix = mProjMatrix * mViewMatrix;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    const Matrix4& GL4Context::getDepthRemapMatrix() const
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

    RHIRenderTargetPtr GL4Context::createRenderWindow(RenderWindow *renderWindow)
    {
        GL4RenderWindowPtr glRenderWindow = GL4RenderWindow::create(renderWindow);

        // 保存主窗口 GL context/DC，供 getNativeContext()/restoreNativeContext() 使用
#if defined(T3D_OS_WINDOWS)
        mSavedGLContext = glRenderWindow->GLContext;
        mSavedGLDC = glRenderWindow->GLDeviceContext;
#elif defined(T3D_OS_LINUX)
        mSavedGLContext = glRenderWindow->GLContext;
        mSavedGLDisplay = glRenderWindow->GLDisplay;
        mSavedGLWindow = glRenderWindow->GLWindow;
#endif

        // GL4RenderWindow::init() 在主线程完成了 GL context 创建和初始化。
        // 当 RHI 线程启用时，需要将 GL context 从主线程转移到 RHI 线程：
        // 1. 主线程 release GL context
        // 2. 设置 RHI 线程初始化回调，在回调中 acquire GL context
        // 这样保证在所有排队的 GL 命令执行之前，RHI 线程已拥有 GL context。
        if (T3D_RHI_THREAD.isRunning())
        {
#if defined(T3D_OS_WINDOWS)
            wglMakeCurrent(nullptr, nullptr);
            T3D_RHI_THREAD.setThreadInitCallback([glRenderWindow]()
            {
                if (!wglMakeCurrent(glRenderWindow->GLDeviceContext, glRenderWindow->GLContext))
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "RHI thread: wglMakeCurrent failed !");
                }
                else
                {
                    T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "RHI thread: GL context acquired successfully.");
                }
            });
#elif defined(T3D_OS_LINUX)
            glXMakeCurrent(glRenderWindow->GLDisplay, None, nullptr);
            T3D_RHI_THREAD.setThreadInitCallback([glRenderWindow]()
            {
                if (!glXMakeCurrent(glRenderWindow->GLDisplay, glRenderWindow->GLWindow, glRenderWindow->GLContext))
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "RHI thread: glXMakeCurrent failed !");
                }
                else
                {
                    T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "RHI thread: GL context acquired successfully.");
                }
            });
#endif
        }

        return glRenderWindow;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr GL4Context::createRenderTexture(PixelBuffer2D *buffer)
    {
        GL4PixelBuffer2DPtr glPixelBuffer = GL4PixelBuffer2D::create();

        auto lambda = [this](const PixelBuffer2DPtr &buffer, const GL4PixelBuffer2DPtr &glPixelBuffer)
        {
            return buildRenderTextureResources(buffer.get(), glPixelBuffer.get());
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, PixelBuffer2DPtr(buffer), glPixelBuffer);
        if (T3D_FAILED(ret))
        {
            glPixelBuffer = nullptr;
        }

        return glPixelBuffer;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setRenderTarget(RenderTarget *renderTarget)
    {
        TResult ret = T3D_OK;

        auto lambda = [this](RenderTargetPtr renderTarget)
        {
            TResult ret = T3D_OK;

            do
            {
                // 入口处清空残留 GL error
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget: ENTRY (stale error from outside)");

                switch (renderTarget->getType())
                {
                case RenderTarget::Type::E_RT_WINDOW:
                    {
                        glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    }
                    break;
                case RenderTarget::Type::E_RT_TEXTURE:
                    {
                        if (renderTarget->getNumOfRenderTextures() > 0)
                        {
                            GL4PixelBuffer2D *glPB = static_cast<GL4PixelBuffer2D*>(
                                renderTarget->getRenderTexture()->getPixelBuffer()->getRHIResource().get());
                            if (glPB != nullptr && glPB->GLFBO != 0)
                            {
                                glBindFramebuffer(GL_FRAMEBUFFER, glPB->GLFBO);
                                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget: glBindFramebuffer(color FBO)");

                                if (renderTarget->getDepthStencil() != nullptr)
                                {
                                    if (glPB->GLDepthRBO != 0)
                                    {
                                        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                            GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
                                        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget: glFramebufferRenderbuffer(detach RBO)");
                                    }

                                    GL4PixelBuffer2D *glDS = static_cast<GL4PixelBuffer2D*>(
                                        renderTarget->getDepthStencil()->getPixelBuffer()->getRHIResource().get());
                                    if (glDS != nullptr)
                                    {
                                        GLenum dsTexTarget = (glDS->GLMSAACount > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
                                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                            dsTexTarget, glDS->GLTexture, 0);
                                        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget: glFramebufferTexture2D(depth texture)");
                                    }
                                }

                                uint32_t numRT = renderTarget->getNumOfRenderTextures();
                                if (numRT > 1)
                                {
                                    GLenum drawBuffers[8];
                                    for (uint32_t i = 0; i < numRT && i < 8; ++i)
                                    {
                                        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
                                        if (i > 0)
                                        {
                                            GL4PixelBuffer2D *pb = static_cast<GL4PixelBuffer2D*>(
                                                renderTarget->getRenderTexture(i)->getPixelBuffer()->getRHIResource().get());
                                            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                                GL_TEXTURE_2D, pb->GLTexture, 0);
                                        }
                                    }
                                    glDrawBuffers(numRT, drawBuffers);
                                }
                            }
                        }
                        else if (renderTarget->getDepthStencil() != nullptr)
                        {
                            GL4PixelBuffer2D *glDS = static_cast<GL4PixelBuffer2D*>(
                                renderTarget->getDepthStencil()->getPixelBuffer()->getRHIResource().get());
                            if (glDS != nullptr)
                            {
                                static GLuint sDepthOnlyFBO = 0;
                                if (sDepthOnlyFBO == 0)
                                {
                                    glGenFramebuffers(1, &sDepthOnlyFBO);
                                }

                                glBindFramebuffer(GL_FRAMEBUFFER, sDepthOnlyFBO);
                                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                    GL_TEXTURE_2D, glDS->GLTexture, 0);
                                glDrawBuffer(GL_NONE);
                                glReadBuffer(GL_NONE);
                                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget: depth-only FBO");
                            }
                        }
                    }
                    break;
                default:
                    T3D_ASSERT(false);
                    break;
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setRenderTarget");
            } while (false);

            return ret;
        };

        ret = ENQUEUE_UNIQUE_COMMAND(lambda, RenderTargetPtr(renderTarget));

        if (T3D_SUCCEEDED(ret))
        {
            mCurrentRenderTarget = renderTarget;
            mRenderingToFBO = (renderTarget->getType() == RenderTarget::Type::E_RT_TEXTURE);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::resetRenderTarget()
    {
        mCurrentRenderTarget = nullptr;
        mRenderingToFBO = false;

        auto lambda = [this]()
        {
            TResult ret = T3D_OK;

            do
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    //--------------------------------------------------------------------------

    void GL4Context::releaseRenderTextureResources(GL4PixelBuffer2D *pb)
    {
        if (pb == nullptr)
        {
            return;
        }

        GL_SAFE_DELETE_FBO(pb->GLResolveFBO);
        GL_SAFE_DELETE_TEXTURE(pb->GLResolveTex);
        GL_SAFE_DELETE_RBO(pb->GLDepthRBO);
        GL_SAFE_DELETE_FBO(pb->GLFBO);
        GL_SAFE_DELETE_TEXTURE(pb->GLTexture);
        pb->GLMSAACount = 1;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::buildRenderTextureResources(PixelBuffer2D *buffer, GL4PixelBuffer2D *glPixelBuffer)
    {
        TResult ret = T3D_OK;

        do
        {
            const auto &desc = buffer->getDescriptor();
            bool isColorRT = true;
            if (desc.format >= PixelFormat::E_PF_D24_UNORM_S8_UINT
                && desc.format <= PixelFormat::E_PF_D16_UNORM)
            {
                isColorRT = false;
            }

            uint32_t msaaCount = desc.sampleDesc.Count;
            if (msaaCount < 1) msaaCount = 1;
            glPixelBuffer->GLMSAACount = msaaCount;

            const uint32_t width = desc.width;
            const uint32_t height = desc.height;
            const GLenum internalFmt = GL4Mapping::getInternalFormat(desc.format);
            const GLenum pixelFmt = GL4Mapping::get(desc.format);
            const GLenum pixelType = GL4Mapping::getPixelType(desc.format);
            const bool hasStencil = (desc.format == PixelFormat::E_PF_D24_UNORM_S8_UINT
                || desc.format == PixelFormat::E_PF_D32_FLOAT_S8X24_UINT);

            if (isColorRT)
            {
                if (msaaCount > 1)
                {
                    glGenTextures(1, &glPixelBuffer->GLTexture);
                    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, glPixelBuffer->GLTexture);
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaCount,
                        internalFmt, width, height, GL_TRUE);
                    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

                    glGenFramebuffers(1, &glPixelBuffer->GLFBO);
                    glBindFramebuffer(GL_FRAMEBUFFER, glPixelBuffer->GLFBO);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                        GL_TEXTURE_2D_MULTISAMPLE, glPixelBuffer->GLTexture, 0);

                    glGenRenderbuffers(1, &glPixelBuffer->GLDepthRBO);
                    glBindRenderbuffer(GL_RENDERBUFFER, glPixelBuffer->GLDepthRBO);
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaCount,
                        GL_DEPTH24_STENCIL8, width, height);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                        GL_RENDERBUFFER, glPixelBuffer->GLDepthRBO);

                    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    {
                        T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "MSAA color render texture FBO is not complete !");
                        ret = T3D_ERR_GL4_CREATE_FBO;
                    }

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    glGenTextures(1, &glPixelBuffer->GLResolveTex);
                    glBindTexture(GL_TEXTURE_2D, glPixelBuffer->GLResolveTex);
                    glTexImage2D(GL_TEXTURE_2D, 0, internalFmt,
                        width, height, 0, pixelFmt, pixelType, nullptr);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    // RT 只有 mip 0，不封顶的话 MIPMAP 类过滤会因 mip 链不完整恒采到黑
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glBindTexture(GL_TEXTURE_2D, 0);

                    glGenFramebuffers(1, &glPixelBuffer->GLResolveFBO);
                    glBindFramebuffer(GL_FRAMEBUFFER, glPixelBuffer->GLResolveFBO);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                        GL_TEXTURE_2D, glPixelBuffer->GLResolveTex, 0);

                    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    {
                        T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "MSAA resolve FBO is not complete !");
                        ret = T3D_ERR_GL4_CREATE_FBO;
                    }

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "Created MSAA render texture: %ux%u, %dx MSAA", width, height, msaaCount);
                }
                else
                {
                    glGenTextures(1, &glPixelBuffer->GLTexture);
                    glBindTexture(GL_TEXTURE_2D, glPixelBuffer->GLTexture);
                    glTexImage2D(GL_TEXTURE_2D, 0, internalFmt,
                        width, height, 0, pixelFmt, pixelType, nullptr);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    // RT 只有 mip 0，不封顶的话 MIPMAP 类过滤会因 mip 链不完整恒采到黑
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glBindTexture(GL_TEXTURE_2D, 0);

                    glGenFramebuffers(1, &glPixelBuffer->GLFBO);
                    glBindFramebuffer(GL_FRAMEBUFFER, glPixelBuffer->GLFBO);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glPixelBuffer->GLTexture, 0);

                    glGenRenderbuffers(1, &glPixelBuffer->GLDepthRBO);
                    glBindRenderbuffer(GL_RENDERBUFFER, glPixelBuffer->GLDepthRBO);
                    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glPixelBuffer->GLDepthRBO);

                    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    {
                        T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Color render texture FBO is not complete !");
                        ret = T3D_ERR_GL4_CREATE_FBO;
                    }

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                }
            }
            else
            {
                GLenum texFormat = hasStencil ? GL_DEPTH_STENCIL : GL_DEPTH_COMPONENT;
                GLenum texTarget = (msaaCount > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

                glGenTextures(1, &glPixelBuffer->GLTexture);
                glBindTexture(texTarget, glPixelBuffer->GLTexture);

                if (msaaCount > 1)
                {
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaCount,
                        internalFmt, width, height, GL_TRUE);
                    GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "Depth: glTexImage2DMultisample");
                }
                else
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, internalFmt,
                        width, height, 0, texFormat, pixelType, nullptr);
                    GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "Depth: glTexImage2D");
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }

                glBindTexture(texTarget, 0);

                glGenFramebuffers(1, &glPixelBuffer->GLFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, glPixelBuffer->GLFBO);

                GLenum attachment = hasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
                glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texTarget, glPixelBuffer->GLTexture, 0);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "Depth: glFramebufferTexture2D");

                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);

                GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER,
                        "Depth render texture FBO is not complete ! status=0x%04X, "
                        "size=%ux%u, internalFmt=0x%04X, texFormat=0x%04X, pixelType=0x%04X, "
                        "attachment=0x%04X, texTarget=0x%04X, msaa=%u, tex=%u, fbo=%u",
                        fboStatus, width, height, internalFmt, texFormat, pixelType,
                        attachment, texTarget, msaaCount,
                        glPixelBuffer->GLTexture, glPixelBuffer->GLFBO);
                    ret = T3D_ERR_GL4_CREATE_FBO;
                }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::buildRenderTextureResources");
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::resizeRenderTexture(RenderTexture *rt, uint32_t width, uint32_t height)
    {
        if (rt == nullptr || width == 0 || height == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "resizeRenderTexture : invalid render texture or size [%u x %u] !", width, height);
            return T3D_ERR_INVALID_PARAM;
        }

        PixelBuffer2D *pixelBuffer = static_cast<PixelBuffer2D *>(rt->getPixelBuffer());
        if (pixelBuffer == nullptr || pixelBuffer->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "resizeRenderTexture : render texture [%s] has no RHI resource !", rt->getName().c_str());
            return T3D_ERR_INVALID_POINTER;
        }

        if (pixelBuffer->getDescriptor().width != width || pixelBuffer->getDescriptor().height != height)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "resizeRenderTexture : descriptor [%u x %u] does not match requested size [%u x %u] !",
                pixelBuffer->getDescriptor().width, pixelBuffer->getDescriptor().height, width, height);
            return T3D_ERR_INVALID_PARAM;
        }

        GL4PixelBuffer2D *glPixelBuffer = static_cast<GL4PixelBuffer2D *>(pixelBuffer->getRHIResource().get());

        auto lambda = [this](const PixelBuffer2DPtr &pixelBuffer, const GL4PixelBuffer2DPtr &glPixelBuffer)
        {
            releaseRenderTextureResources(glPixelBuffer.get());
            return buildRenderTextureResources(pixelBuffer.get(), glPixelBuffer.get());
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, PixelBuffer2DPtr(pixelBuffer), GL4PixelBuffer2DPtr(glPixelBuffer));
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::resizeRenderTarget(RenderTarget *rt, uint32_t width, uint32_t height)
    {
        if (rt == nullptr || width == 0 || height == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "resizeRenderTarget : invalid render target or size [%u x %u] !", width, height);
            return T3D_ERR_INVALID_PARAM;
        }

        if (rt->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            GL4RenderWindow *glWindow = static_cast<GL4RenderWindow *>(rt->getRenderWindow()->getRHIRenderWindow());
            if (glWindow == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "resizeRenderTarget : render window has no RHI resource !");
                return T3D_ERR_INVALID_POINTER;
            }

            return resizeRenderWindow(glWindow, width, height);
        }

        const uint32_t numOfTextures = rt->getNumOfRenderTextures();
        for (uint32_t i = 0; i < numOfTextures; ++i)
        {
            TResult ret = rt->getRenderTexture(i)->resize(width, height);
            if (T3D_FAILED(ret))
            {
                return ret;
            }
        }

        RenderTexturePtr depthStencil = rt->getDepthStencil();
        if (depthStencil != nullptr)
        {
            TResult ret = depthStencil->resize(width, height);
            if (T3D_FAILED(ret))
            {
                return ret;
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setViewport(const Viewport &viewport)
    {
        Real width = 0, height = 0;

        if (mCurrentRenderTarget != nullptr)
        {
            if (mCurrentRenderTarget->getType() == RenderTarget::Type::E_RT_WINDOW)
            {
                width = static_cast<Real>(mCurrentRenderTarget->getRenderWindow()->getDescriptor().Width);
                height = static_cast<Real>(mCurrentRenderTarget->getRenderWindow()->getDescriptor().Height);
            }
            else
            {
                if (mCurrentRenderTarget->getNumOfRenderTextures() > 0)
                {
                    width = static_cast<Real>(mCurrentRenderTarget->getRenderTexture()->getWidth());
                    height = static_cast<Real>(mCurrentRenderTarget->getRenderTexture()->getHeight());
                }
                else if (mCurrentRenderTarget->getDepthStencil() != nullptr)
                {
                    width = static_cast<Real>(mCurrentRenderTarget->getDepthStencil()->getWidth());
                    height = static_cast<Real>(mCurrentRenderTarget->getDepthStencil()->getHeight());
                }
                else
                {
                    T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "GL4Context::setViewport: no color texture and depth stencil texture !");
                    return T3D_OK;
                }
            }
        }
        else
        {
            // 无引擎 RenderTarget 绑定（如 ImGui 子 viewport 渲染到默认 FBO），
            // 将比例值传入 lambda，在 RHI 线程中通过 GL API 查询当前 viewport 尺寸。
            // 这样无论同步还是异步模式都能正确工作。
            auto lambda = [this](Viewport vp)
            {
                TResult ret = T3D_OK;
                do
                {
                    GLint currentVP[4];
                    glGetIntegerv(GL_VIEWPORT, currentVP);
                    Real w = static_cast<Real>(currentVP[2]);
                    Real h = static_cast<Real>(currentVP[3]);
                    if (w <= 0 || h <= 0)
                    {
                        T3D_LOG_WARNING(LOG_TAG_GL4RENDERER,
                            "GL4Context::setViewport: no render target and GL viewport is zero");
                        break;
                    }
                    GLint x = static_cast<GLint>(vp.Left * w);
                    GLint y = static_cast<GLint>(vp.Top * h);
                    GLsizei gw = static_cast<GLsizei>(vp.Width * w);
                    GLsizei gh = static_cast<GLsizei>(vp.Height * h);
                    glViewport(x, y, gw, gh);
                    GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setViewport(fallback)");
                } while (false);
                return ret;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda, viewport);
        }

        GLint x = static_cast<GLint>(viewport.Left * width);
        GLint y = static_cast<GLint>(viewport.Top * height);
        GLsizei w = static_cast<GLsizei>(viewport.Width * width);
        GLsizei h = static_cast<GLsizei>(viewport.Height * height);

        auto lambda = [this](GLint x, GLint y, GLsizei w, GLsizei h)
        {
            TResult ret = T3D_OK;

            do
            {
                glViewport(x, y, w, h);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setViewport");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, x, y, w, h);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        int32_t fbHeight = 0;

        if (mCurrentRenderTarget != nullptr)
        {
            if (mCurrentRenderTarget->getType() == RenderTarget::Type::E_RT_WINDOW)
            {
                fbHeight = static_cast<int32_t>(mCurrentRenderTarget->getRenderWindow()->getDescriptor().Height);
            }
            else if (mCurrentRenderTarget->getNumOfRenderTextures() > 0)
            {
                fbHeight = static_cast<int32_t>(mCurrentRenderTarget->getRenderTexture()->getHeight());
            }
            else if (mCurrentRenderTarget->getDepthStencil() != nullptr)
            {
                fbHeight = static_cast<int32_t>(mCurrentRenderTarget->getDepthStencil()->getHeight());
            }
        }

        if (fbHeight > 0)
        {
            // 左上原点 → GL 左下原点
            const GLint glY = fbHeight - (y + static_cast<int32_t>(height));
            auto lambda = [this](GLint x, GLint y, GLsizei w, GLsizei h)
            {
                glScissor(x, y, w, h);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setScissorRect");
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda,
                static_cast<GLint>(x), glY,
                static_cast<GLsizei>(width), static_cast<GLsizei>(height));
        }

        // 无 RenderTarget（如 ImGui 子 viewport）：在 RHI 线程查当前 viewport 高度再翻转
        auto lambda = [this](int32_t x, int32_t y, uint32_t width, uint32_t height)
        {
            GLint currentVP[4];
            glGetIntegerv(GL_VIEWPORT, currentVP);
            const int32_t fbH = currentVP[3];
            if (fbH <= 0)
            {
                T3D_LOG_WARNING(LOG_TAG_GL4RENDERER,
                    "GL4Context::setScissorRect: no render target and GL viewport height is zero");
                return T3D_OK;
            }
            const GLint glY = fbH - (y + static_cast<int32_t>(height));
            glScissor(static_cast<GLint>(x), glY,
                static_cast<GLsizei>(width), static_cast<GLsizei>(height));
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setScissorRect(fallback)");
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, x, y, width, height);
    }

    //--------------------------------------------------------------------------

    GL4Context::ClearMaskGuard::ClearMaskGuard(GLbitfield mask)
        : mMask(mask)
    {
        // scissor 对所有 buffer bit 都生效，写掩码则各管各的
        mScissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
        if (mScissorEnabled)
        {
            glDisable(GL_SCISSOR_TEST);
        }

        if (mMask & GL_COLOR_BUFFER_BIT)
        {
            glGetBooleanv(GL_COLOR_WRITEMASK, mColorMask);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        }

        if (mMask & GL_DEPTH_BUFFER_BIT)
        {
            glGetBooleanv(GL_DEPTH_WRITEMASK, &mDepthMask);
            glDepthMask(GL_TRUE);
        }

        if (mMask & GL_STENCIL_BUFFER_BIT)
        {
            glGetIntegerv(GL_STENCIL_WRITEMASK, &mStencilMaskFront);
            glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &mStencilMaskBack);
            glStencilMask(~0u);
        }
    }

    //--------------------------------------------------------------------------

    GL4Context::ClearMaskGuard::~ClearMaskGuard()
    {
        if (mMask & GL_STENCIL_BUFFER_BIT)
        {
            glStencilMaskSeparate(GL_FRONT, static_cast<GLuint>(mStencilMaskFront));
            glStencilMaskSeparate(GL_BACK, static_cast<GLuint>(mStencilMaskBack));
        }

        if (mMask & GL_DEPTH_BUFFER_BIT)
        {
            glDepthMask(mDepthMask);
        }

        if (mMask & GL_COLOR_BUFFER_BIT)
        {
            glColorMask(mColorMask[0], mColorMask[1], mColorMask[2], mColorMask[3]);
        }

        if (mScissorEnabled)
        {
            glEnable(GL_SCISSOR_TEST);
        }
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::clearColor(const ColorRGB &color)
    {
        auto lambda = [this](ColorRGB color)
        {
            TResult ret = T3D_OK;

            do
            {
                ClearMaskGuard guard(GL_COLOR_BUFFER_BIT);
                glClearColor(color.red(), color.green(), color.blue(), 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::clearColor");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, color);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::clearDepth(Real depth)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        auto lambda = [this](Real depth)
        {
            TResult ret = T3D_OK;

            do
            {
                ClearMaskGuard guard(GL_DEPTH_BUFFER_BIT);
                glClearDepth((GLdouble)depth);
                glClear(GL_DEPTH_BUFFER_BIT);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::clearDepth");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, depth);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::clearDepthStencil(Real depth, uint32_t stencil)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        auto lambda = [this](Real depth, uint32_t stencil)
        {
            TResult ret = T3D_OK;

            do
            {
                ClearMaskGuard guard(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                glClearDepth((GLdouble)depth);
                glClearStencil((GLint)stencil);
                glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::clearDepthStencil");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, depth, stencil);
    }

    //--------------------------------------------------------------------------

    RHIBlendStatePtr GL4Context::createBlendState(BlendState *state)
    {
        GL4BlendStatePtr glState = GL4BlendState::create();

        const BlendDesc &desc = state->getStateDesc();

        GL4BlendStateData d {};
        d.independentBlend = desc.IndependentBlendEnable;
        d.alphaToCoverage = desc.AlphaToCoverageEnable;

        const uint32_t rtCount = d.independentBlend ? BlendDesc::kMaxRenderTarget : 1;
        for (uint32_t i = 0; i < rtCount; ++i)
        {
            const auto &src = desc.RenderTargetStates[i];
            auto &dst = d.targets[i];
            dst.enabled = src.BlendEnable;
            dst.srcRGB = GL4Mapping::get(src.SrcBlend);
            dst.dstRGB = GL4Mapping::get(src.DestBlend);
            dst.opRGB = GL4Mapping::get(src.BlendOp);
            dst.srcAlpha = GL4Mapping::get(src.SrcBlendAlpha);
            dst.dstAlpha = GL4Mapping::get(src.DstBlendAlpha);
            dst.opAlpha = GL4Mapping::get(src.BlendOpAlpha);
            dst.colorMask[0] = (src.ColorMask & kWriteMaskRed) ? GL_TRUE : GL_FALSE;
            dst.colorMask[1] = (src.ColorMask & kWriteMaskGreen) ? GL_TRUE : GL_FALSE;
            dst.colorMask[2] = (src.ColorMask & kWriteMaskBlue) ? GL_TRUE : GL_FALSE;
            dst.colorMask[3] = (src.ColorMask & kWriteMaskAlpha) ? GL_TRUE : GL_FALSE;
        }

        auto lambda = [this](const GL4BlendStatePtr &glState, GL4BlendStateData d)
        {
            TResult ret = T3D_OK;

            do
            {
                glState->data = d;
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glState, d);
        if (T3D_FAILED(ret))
        {
            glState = nullptr;
        }

        return glState;
    }

    //--------------------------------------------------------------------------

    RHIDepthStencilStatePtr GL4Context::createDepthStencilState(DepthStencilState *state)
    {
        GL4DepthStencilStatePtr glState = GL4DepthStencilState::create();

        const DepthStencilDesc &desc = state->getStateDesc();

        // 主线程提取所有描述符数据（POD）
        GL4DepthStencilStateData d {};
        d.depthTestEnabled = desc.DepthTestEnable;
        d.depthWriteEnabled = desc.DepthWriteEnable;
        d.depthFunc = GL4Mapping::get(desc.DepthFunc);
        d.stencilEnabled = desc.StencilEnable;
        d.stencilReadMask = desc.StencilReadMask;
        d.stencilWriteMask = desc.StencilWriteMask;
        d.frontStencilFail = GL4Mapping::get(desc.FrontFace.StencilFailOp);
        d.frontDepthFail = GL4Mapping::get(desc.FrontFace.StencilDepthFailOp);
        d.frontStencilPass = GL4Mapping::get(desc.FrontFace.StencilPassOp);
        d.frontStencilFunc = GL4Mapping::get(desc.FrontFace.StencilFunc);
        d.backStencilFail = GL4Mapping::get(desc.BackFace.StencilFailOp);
        d.backDepthFail = GL4Mapping::get(desc.BackFace.StencilDepthFailOp);
        d.backStencilPass = GL4Mapping::get(desc.BackFace.StencilPassOp);
        d.backStencilFunc = GL4Mapping::get(desc.BackFace.StencilFunc);

        auto lambda = [this](const GL4DepthStencilStatePtr &glState, GL4DepthStencilStateData d)
        {
            TResult ret = T3D_OK;

            do
            {
                glState->data = d;
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glState, d);
        if (T3D_FAILED(ret))
        {
            glState = nullptr;
        }

        return glState;
    }

    //--------------------------------------------------------------------------

    RHIRasterizerStatePtr GL4Context::createRasterizerState(RasterizerState *state)
    {
        GL4RasterizerStatePtr glState = GL4RasterizerState::create();

        const RasterizerDesc &desc = state->getStateDesc();

        // 主线程提取所有描述符数据（POD）
        GL4RasterizerStateData d {};
        d.fillMode = GL4Mapping::get(desc.FillMode);
        d.cullMode = GL4Mapping::get(desc.CullMode);
        d.cullEnabled = (desc.CullMode != CullingMode::kNone);
        d.frontCCW = desc.FrontAnticlockwise;
        d.scissorEnabled = desc.ScissorEnable;
        d.depthClipEnabled = desc.DepthClipEnable;
        d.depthBias = static_cast<GLfloat>(desc.DepthBias);
        d.slopeScaledDepthBias = static_cast<GLfloat>(desc.SlopeScaledDepthBias);
        d.multisampleEnabled = desc.MultisampleEnable;

        auto lambda = [this](const GL4RasterizerStatePtr &glState, GL4RasterizerStateData d)
        {
            TResult ret = T3D_OK;

            do
            {
                glState->data = d;
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glState, d);
        if (T3D_FAILED(ret))
        {
            glState = nullptr;
        }

        return glState;
    }

    //--------------------------------------------------------------------------

    RHISamplerStatePtr GL4Context::createSamplerState(SamplerState *state)
    {
        GL4SamplerStatePtr glState = GL4SamplerState::create();

        const SamplerDesc &desc = state->getStateDesc();

        GLenum wrapS = GL4Mapping::get(desc.AddressU);
        GLenum wrapT = GL4Mapping::get(desc.AddressV);
        GLenum wrapR = GL4Mapping::get(desc.AddressW);
        GLenum minFilter = GL4Mapping::getMinFilter(desc.MinFilter, desc.MipFilter);
        GLenum magFilter = GL4Mapping::getMagFilter(desc.MagFilter);
        GLfloat maxAniso = static_cast<GLfloat>(desc.MaxAnisotropy);
        GLfloat lodBias = desc.MipLODBias;
        GLfloat minLOD = desc.MinLOD;
        GLfloat maxLOD = (desc.MaxLOD > 1000.0f) ? 1000.0f : desc.MaxLOD;
        GLfloat borderColor[4] = {
            desc.BorderColor.blue(), desc.BorderColor.green(),
            desc.BorderColor.red(), desc.BorderColor.alpha()
        };
        bool isComparison = desc.IsComparison;
        GLenum compareFunc = GL4Mapping::get(desc.CompareFunc);

        auto lambda = [this](const GL4SamplerStatePtr &glState,
            GLenum wrapS, GLenum wrapT, GLenum wrapR,
            GLenum minFilter, GLenum magFilter, GLfloat maxAniso,
            GLfloat lodBias, GLfloat minLOD, GLfloat maxLOD,
            bool isComparison, GLenum compareFunc)
        {
            TResult ret = T3D_OK;

            do
            {
                glGenSamplers(1, &glState->GLSampler);

                glSamplerParameteri(glState->GLSampler, GL_TEXTURE_WRAP_S, wrapS);
                glSamplerParameteri(glState->GLSampler, GL_TEXTURE_WRAP_T, wrapT);
                glSamplerParameteri(glState->GLSampler, GL_TEXTURE_WRAP_R, wrapR);
                glSamplerParameteri(glState->GLSampler, GL_TEXTURE_MIN_FILTER, minFilter);
                glSamplerParameteri(glState->GLSampler, GL_TEXTURE_MAG_FILTER, magFilter);
                glSamplerParameterf(glState->GLSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
                glSamplerParameterf(glState->GLSampler, GL_TEXTURE_LOD_BIAS, lodBias);
                glSamplerParameterf(glState->GLSampler, GL_TEXTURE_MIN_LOD, minLOD);
                glSamplerParameterf(glState->GLSampler, GL_TEXTURE_MAX_LOD, maxLOD);

                if (isComparison)
                {
                    glSamplerParameteri(glState->GLSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                    glSamplerParameteri(glState->GLSampler, GL_TEXTURE_COMPARE_FUNC, compareFunc);
                }
                else
                {
                    glSamplerParameteri(glState->GLSampler, GL_TEXTURE_COMPARE_MODE, GL_NONE);
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createSamplerState");
            } while (false);

            return ret;
        };

        // borderColor 需要单独处理（数组不能直接传递给 ENQUEUE_UNIQUE_COMMAND）
        // 由于它已经在主线程计算好了，这里直接通过 lambda 捕获的 glState 在渲染线程设置
        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glState,
            wrapS, wrapT, wrapR, minFilter, magFilter, maxAniso,
            lodBias, minLOD, maxLOD, isComparison, compareFunc);

        // borderColor 单独入队设置
        auto lambdaBorder = [this](const GL4SamplerStatePtr &glState,
            GLfloat b0, GLfloat b1, GLfloat b2, GLfloat b3)
        {
            TResult ret = T3D_OK;

            do
            {
                GLfloat bc[4] = { b0, b1, b2, b3 };
                glSamplerParameterfv(glState->GLSampler, GL_TEXTURE_BORDER_COLOR, bc);
            } while (false);

            return ret;
        };

        ENQUEUE_UNIQUE_COMMAND(lambdaBorder, glState,
            borderColor[0], borderColor[1], borderColor[2], borderColor[3]);

        T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "createSamplerState: IsComparison=%d CompareFunc=%d",
            isComparison ? 1 : 0, desc.CompareFunc);

        if (T3D_FAILED(ret))
        {
            glState = nullptr;
        }

        return glState;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setBlendState(BlendState *state)
    {
        GL4BlendState *glState = static_cast<GL4BlendState*>(state->getRHIState().get());
        const auto &d = glState->data;

        auto lambda = [this](GL4BlendStateData d)
        {
            TResult ret = T3D_OK;

            do
            {
                if (d.alphaToCoverage)
                {
                    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
                }
                else
                {
                    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
                }

                if (d.independentBlend)
                {
                    for (uint32_t i = 0; i < BlendDesc::kMaxRenderTarget; ++i)
                    {
                        const auto &rt = d.targets[i];
                        if (rt.enabled)
                        {
                            glEnablei(GL_BLEND, i);
                            glBlendFuncSeparateiARB(i, rt.srcRGB, rt.dstRGB, rt.srcAlpha, rt.dstAlpha);
                            glBlendEquationSeparateiARB(i, rt.opRGB, rt.opAlpha);
                        }
                        else
                        {
                            glDisablei(GL_BLEND, i);
                        }

                        glColorMaski(i, rt.colorMask[0], rt.colorMask[1], rt.colorMask[2], rt.colorMask[3]);
                    }
                }
                else
                {
                    const auto &rt = d.targets[0];
                    if (rt.enabled)
                    {
                        glEnable(GL_BLEND);
                        glBlendFuncSeparate(rt.srcRGB, rt.dstRGB, rt.srcAlpha, rt.dstAlpha);
                        glBlendEquationSeparate(rt.opRGB, rt.opAlpha);
                    }
                    else
                    {
                        glDisable(GL_BLEND);
                    }

                    glColorMask(rt.colorMask[0], rt.colorMask[1], rt.colorMask[2], rt.colorMask[3]);
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setBlendState");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, d);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setDepthStencilState(DepthStencilState *state)
    {
        GL4DepthStencilState *glState = static_cast<GL4DepthStencilState*>(state->getRHIState().get());
        const auto &d = glState->data;

        auto lambda = [this](GL4DepthStencilStateData d)
        {
            TResult ret = T3D_OK;

            do
            {
                if (d.depthTestEnabled)
                {
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(d.depthFunc);
                }
                else
                {
                    glDisable(GL_DEPTH_TEST);
                }

                glDepthMask(d.depthWriteEnabled ? GL_TRUE : GL_FALSE);

                if (d.stencilEnabled)
                {
                    glEnable(GL_STENCIL_TEST);
                    glStencilMaskSeparate(GL_FRONT, d.stencilWriteMask);
                    glStencilMaskSeparate(GL_BACK, d.stencilWriteMask);
                    glStencilFuncSeparate(GL_FRONT, d.frontStencilFunc, 1, d.stencilReadMask);
                    glStencilFuncSeparate(GL_BACK, d.backStencilFunc, 1, d.stencilReadMask);
                    glStencilOpSeparate(GL_FRONT, d.frontStencilFail, d.frontDepthFail, d.frontStencilPass);
                    glStencilOpSeparate(GL_BACK, d.backStencilFail, d.backDepthFail, d.backStencilPass);
                }
                else
                {
                    glDisable(GL_STENCIL_TEST);
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setDepthStencilState");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, d);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setRasterizerState(RasterizerState *state)
    {
        GL4RasterizerState *glState = static_cast<GL4RasterizerState*>(state->getRHIState().get());
        const auto &d = glState->data;
        bool projFlipped = mProjectionFlipped;

        auto lambda = [this](GL4RasterizerStateData d, bool projFlipped)
        {
            TResult ret = T3D_OK;

            do
            {
                glPolygonMode(GL_FRONT_AND_BACK, d.fillMode);

                if (d.cullEnabled)
                {
                    glEnable(GL_CULL_FACE);

                    GLenum cullMode = d.cullMode;
                    if (projFlipped)
                    {
                        if (cullMode == GL_FRONT)
                            cullMode = GL_BACK;
                        else if (cullMode == GL_BACK)
                            cullMode = GL_FRONT;
                    }
                    glCullFace(cullMode);
                }
                else
                {
                    glDisable(GL_CULL_FACE);
                }

                glFrontFace(d.frontCCW ? GL_CCW : GL_CW);

                if (d.scissorEnabled)
                    glEnable(GL_SCISSOR_TEST);
                else
                    glDisable(GL_SCISSOR_TEST);

                if (d.depthClipEnabled)
                    glEnable(GL_DEPTH_CLAMP);
                else
                    glDisable(GL_DEPTH_CLAMP);

                if (d.depthBias != 0.0f || d.slopeScaledDepthBias != 0.0f)
                {
                    glEnable(GL_POLYGON_OFFSET_FILL);
                    glEnable(GL_POLYGON_OFFSET_LINE);
                    glPolygonOffset(d.slopeScaledDepthBias, d.depthBias);
                }
                else
                {
                    glDisable(GL_POLYGON_OFFSET_FILL);
                    glDisable(GL_POLYGON_OFFSET_LINE);
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setRasterizerState");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, d, projFlipped);
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr GL4Context::createVertexDeclaration(VertexDeclaration *decl)
    {
        GL4VertexDeclarationPtr glDecl = GL4VertexDeclaration::create();

        auto lambda = [this](const GL4VertexDeclarationPtr &glDecl)
        {
            TResult ret = T3D_OK;

            do
            {
                glGenVertexArrays(1, &glDecl->GLVAO);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createVertexDeclaration");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glDecl);
        if (T3D_FAILED(ret)) { glDecl = nullptr; }
        return glDecl;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setVertexDeclaration(VertexDeclaration *decl)
    {
        GL4VertexDeclarationPtr glDecl = static_cast<GL4VertexDeclaration*>(decl->getRHIResource().get());
        mPendingVertexDecl = decl;

        auto lambda = [this](const GL4VertexDeclarationPtr &glDecl)
        {
            TResult ret = T3D_OK;

            do
            {
                GLuint vao = glDecl->GLVAO;
                mCurrentVAO = vao;
                glBindVertexArray(vao);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setVertexDeclaration");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glDecl);
    }

    //--------------------------------------------------------------------------

    RHIVertexBufferPtr GL4Context::createVertexBuffer(VertexBuffer *buffer)
    {
        GL4VertexBufferPtr glBuffer = GL4VertexBuffer::create();

        GLenum usage = GL4Mapping::getBufferUsage(buffer->getUsage());

        auto lambda = [this](const GL4VertexBufferPtr &glBuffer, const VertexBufferPtr &buffer, GLenum usage)
        {
            TResult ret = T3D_OK;

            do
            {
                glGenBuffers(1, &glBuffer->GLBuffer);
                glBindBuffer(GL_ARRAY_BUFFER, glBuffer->GLBuffer);
                glBufferData(GL_ARRAY_BUFFER,
                    (GLsizeiptr)buffer->getBufferSize(),
                    buffer->getBuffer().Data,
                    usage);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createVertexBuffer");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, VertexBufferPtr(buffer), usage);
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)
    {
        VertexDeclaration *pendingDecl = mPendingVertexDecl;
        mPendingVertexDecl = nullptr;

        auto lambda = [this](uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets, VertexDeclaration *pendingDecl)
        {
            TResult ret = T3D_OK;

            do
            {
                for (uint32_t i = 0; i < buffers.size(); ++i)
                {
                    GL4VertexBuffer *glVB = static_cast<GL4VertexBuffer*>(buffers[i]->getRHIResource().get());
                    glBindBuffer(GL_ARRAY_BUFFER, glVB->GLBuffer);

                    if (pendingDecl != nullptr)
                    {
                        for (uint32_t j = 0; j < pendingDecl->getAttributeCount(); ++j)
                        {
                            const VertexAttribute &attrib = pendingDecl->getAttributes()[j];
                            if (attrib.getSlot() != startSlot + i)
                                continue;

                            GLint size = GL4Mapping::getVertexAttribSize(attrib.getType());
                            GLenum type = GL4Mapping::getVertexAttribType(attrib.getType());
                            GLboolean normalized = GL4Mapping::getVertexAttribNormalized(attrib.getType());

                            glEnableVertexAttribArray(j);
                            if (GL4Mapping::isIntegerAttrib(attrib.getType()))
                            {
                                glVertexAttribIPointer(j, size, type,
                                    (GLsizei)strides[i],
                                    reinterpret_cast<const void*>((uintptr_t)attrib.getOffset()));
                            }
                            else
                            {
                                glVertexAttribPointer(j, size, type, normalized,
                                    (GLsizei)strides[i],
                                    reinterpret_cast<const void*>((uintptr_t)attrib.getOffset()));
                            }

                            if (attrib.getInputRate() == VertexAttribute::InputRate::kPerInstance)
                            {
                                const uint32_t stepRate = attrib.getInstanceStepRate();
                                glVertexAttribDivisor(j, (GLuint)(stepRate > 0 ? stepRate : 1));
                            }
                            else
                            {
                                glVertexAttribDivisor(j, 0);
                            }
                        }
                    }
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setVertexBuffers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, buffers, strides, offsets, pendingDecl);
    }

    //--------------------------------------------------------------------------

    RHIIndexBufferPtr GL4Context::createIndexBuffer(IndexBuffer *buffer)
    {
        GL4IndexBufferPtr glBuffer = GL4IndexBuffer::create();

        GLenum usage = GL4Mapping::getBufferUsage(buffer->getUsage());

        auto lambda = [this](const GL4IndexBufferPtr &glBuffer, const IndexBufferPtr &buffer, GLenum usage)
        {
            TResult ret = T3D_OK;

            do
            {
                glGenBuffers(1, &glBuffer->GLBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer->GLBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    (GLsizeiptr)buffer->getBufferSize(),
                    buffer->getBuffer().Data,
                    usage);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createIndexBuffer");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, IndexBufferPtr(buffer), usage);
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setIndexBuffer(IndexBuffer *buffer)
    {
        GL4IndexBufferPtr glIB = static_cast<GL4IndexBuffer*>(buffer->getRHIResource().get());

        mIndexType = GL4Mapping::get(buffer->getIndexType());
        mIndexSize = (buffer->getIndexType() == IndexType::E_IT_16BITS) ? 2 : 4;

        auto lambda = [this](const GL4IndexBufferPtr &glIB)
        {
            TResult ret = T3D_OK;

            do
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIB->GLBuffer);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setIndexBuffer");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glIB);
    }

    //--------------------------------------------------------------------------

    RHIConstantBufferPtr GL4Context::createConstantBuffer(ConstantBuffer *buffer)
    {
        GL4ConstantBufferPtr glBuffer = GL4ConstantBuffer::create();

        GLenum usage = GL4Mapping::getBufferUsage(buffer->getUsage());

        auto lambda = [this](const GL4ConstantBufferPtr &glBuffer, const ConstantBufferPtr &buffer, GLenum usage)
        {
            TResult ret = T3D_OK;

            do
            {
                glGenBuffers(1, &glBuffer->GLBuffer);
                glBindBuffer(GL_UNIFORM_BUFFER, glBuffer->GLBuffer);
                glBufferData(GL_UNIFORM_BUFFER,
                    (GLsizeiptr)buffer->getBufferSize(),
                    buffer->getBuffer().Data,
                    usage);
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createConstantBuffer");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, ConstantBufferPtr(buffer), usage);
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer1DPtr GL4Context::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        GL4PixelBuffer1DPtr glBuffer = GL4PixelBuffer1D::create();

        auto lambda = [this](const GL4PixelBuffer1DPtr &glBuffer, const PixelBuffer1DPtr &buffer)
        {
            TResult ret = T3D_OK;

            do
            {
                const auto &desc = buffer->getDescriptor();

                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(GL_TEXTURE_1D, glBuffer->GLTexture);
                glTexImage1D(GL_TEXTURE_1D, 0,
                    GL4Mapping::getInternalFormat(desc.format),
                    desc.width, 0,
                    GL4Mapping::get(desc.format),
                    GL4Mapping::getPixelType(desc.format),
                    buffer->getBuffer().Data);
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_1D, 0);

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createPixelBuffer1D");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, PixelBuffer1DPtr(buffer));
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr GL4Context::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        GL4PixelBuffer2DPtr glBuffer = GL4PixelBuffer2D::create();

        auto lambda = [this](const GL4PixelBuffer2DPtr &glBuffer, const PixelBuffer2DPtr &buffer)
        {
            TResult ret = T3D_OK;

            do
            {
                const auto &desc = buffer->getDescriptor();

                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(GL_TEXTURE_2D, glBuffer->GLTexture);
                glTexImage2D(GL_TEXTURE_2D, 0,
                    GL4Mapping::getInternalFormat(desc.format),
                    desc.width, desc.height, 0,
                    GL4Mapping::get(desc.format),
                    GL4Mapping::getPixelType(desc.format),
                    buffer->getBuffer().Data);

                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createPixelBuffer2D");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, PixelBuffer2DPtr(buffer));
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer3DPtr GL4Context::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        GL4PixelBuffer3DPtr glBuffer = GL4PixelBuffer3D::create();

        auto lambda = [this](const GL4PixelBuffer3DPtr &glBuffer, const PixelBuffer3DPtr &buffer)
        {
            TResult ret = T3D_OK;

            do
            {
                const auto &desc = buffer->getDescriptor();

                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(GL_TEXTURE_3D, glBuffer->GLTexture);
                glTexImage3D(GL_TEXTURE_3D, 0,
                    GL4Mapping::getInternalFormat(desc.format),
                    desc.width, desc.height, desc.depth, 0,
                    GL4Mapping::get(desc.format),
                    GL4Mapping::getPixelType(desc.format),
                    buffer->getBuffer().Data);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_3D, 0);

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createPixelBuffer3D");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, PixelBuffer3DPtr(buffer));
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBufferCubemapPtr GL4Context::createPixelBufferCubemap(PixelBufferCubemap *buffer)
    {
        GL4PixelBufferCubemapPtr glBuffer = GL4PixelBufferCubemap::create();

        auto lambda = [this](const GL4PixelBufferCubemapPtr &glBuffer, const PixelBufferCubemapPtr &buffer)
        {
            TResult ret = T3D_OK;

            do
            {
                const auto &desc = buffer->getDescriptor();
                const Buffer &src = buffer->getBuffer();

                const size_t bpp = Image::getBPP(desc.format) / 8;
                const size_t faceSize = (size_t)desc.width * desc.height * bpp;
                const uint8_t *data = static_cast<const uint8_t*>(src.Data);

                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(GL_TEXTURE_CUBE_MAP, glBuffer->GLTexture);

                for (uint32_t face = 0; face < PixelBufferCubemap::FACE_COUNT; ++face)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
                        GL4Mapping::getInternalFormat(desc.format),
                        desc.width, desc.height, 0,
                        GL4Mapping::get(desc.format),
                        GL4Mapping::getPixelType(desc.format),
                        data != nullptr ? data + face * faceSize : nullptr);
                }

                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createPixelBufferCubemap");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, PixelBufferCubemapPtr(buffer));
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------
    // Vertex Shader
    //--------------------------------------------------------------------------

    RHIShaderPtr GL4Context::createVertexShader(ShaderVariant *shader)
    {
        GL4VertexShaderPtr glShader = GL4VertexShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        // 复制 shader 源码，确保渲染线程执行时数据有效
        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GL4VertexShaderPtr &glShader, String shaderSource)
        {
            TResult ret = T3D_OK;

            do
            {
                const char *src = shaderSource.c_str();
                GLint len = static_cast<GLint>(shaderSource.size());

                glShader->GLShaderHandle = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(glShader->GLShaderHandle, 1, &src, &len);
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
                        T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Vertex shader compile error: %s", log.data());
                    }
                    GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
                    ret = T3D_ERR_GL4_COMPILE_SHADER;
                    break;
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createVertexShader");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setVertexShader(ShaderVariant *shader)
    {
        if (shader == nullptr)
        {
            auto lambda = [this]()
            {
                mCurrentVSVariant = nullptr;
                if (mCurrentProgram != 0)
                {
                    GL_SAFE_DELETE_PROGRAM(mCurrentProgram);
                }
                mProgramDirty = false;
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

        GL4Shader *glShader = static_cast<GL4Shader*>(shader->getRHIShader());
        GLuint shaderHandle = glShader->GLShaderHandle;

        auto lambda = [this](GLuint shaderHandle, ShaderVariant *variant)
        {
            TResult ret = T3D_OK;

            do
            {
                mCurrentVSVariant = variant;

                if (mCurrentProgram != 0)
                {
                    GL_SAFE_DELETE_PROGRAM(mCurrentProgram);
                }
                mCurrentProgram = glCreateProgram();
                glAttachShader(mCurrentProgram, shaderHandle);
                mProgramDirty = true;
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setVertexShader");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, shaderHandle, shader);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(buffers);
    }

    TResult GL4Context::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GL4Context::setVSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    // Pixel Shader
    //--------------------------------------------------------------------------

    RHIShaderPtr GL4Context::createPixelShader(ShaderVariant *shader)
    {
        GL4PixelShaderPtr glShader = GL4PixelShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GL4PixelShaderPtr &glShader, String shaderSource)
        {
            TResult ret = T3D_OK;

            do
            {
                const char *src = shaderSource.c_str();
                GLint len = static_cast<GLint>(shaderSource.size());

                glShader->GLShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(glShader->GLShaderHandle, 1, &src, &len);
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
                        T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Pixel shader compile error: %s", log.data());
                    }
                    GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
                    ret = T3D_ERR_GL4_COMPILE_SHADER;
                    break;
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createPixelShader");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setPixelShader(ShaderVariant *shader)
    {
        if (shader == nullptr)
        {
            auto lambda = [this]()
            {
                mCurrentPSVariant = nullptr;
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

        GL4Shader *glShader = static_cast<GL4Shader*>(shader->getRHIShader());
        GLuint shaderHandle = glShader->GLShaderHandle;

        auto lambda = [this](GLuint shaderHandle, ShaderVariant *variant)
        {
            TResult ret = T3D_OK;

            do
            {
                mCurrentPSVariant = variant;

                if (mCurrentProgram == 0)
                {
                    mCurrentProgram = glCreateProgram();
                }
                glAttachShader(mCurrentProgram, shaderHandle);
                mProgramDirty = true;
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setPixelShader");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, shaderHandle, shader);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(buffers);
    }

    TResult GL4Context::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GL4Context::setPSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    // Hull Shader (Tessellation Control, GL 4.0+)
    //--------------------------------------------------------------------------

    RHIShaderPtr GL4Context::createHullShader(ShaderVariant *shader)
    {
        GL4HullShaderPtr glShader = GL4HullShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);
        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GL4HullShaderPtr &glShader, String shaderSource)
        {
            return compileGLSLShader(GL_TESS_CONTROL_SHADER, shaderSource, glShader->GLShaderHandle, "Hull");
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    TResult GL4Context::setHullShader(ShaderVariant *shader)
    {
        return attachGraphicsShader(shader, mCurrentHSVariant);
    }

    TResult GL4Context::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(buffers);
    }

    TResult GL4Context::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GL4Context::setHSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    // Domain Shader (Tessellation Evaluation, GL 4.0+)
    //--------------------------------------------------------------------------

    RHIShaderPtr GL4Context::createDomainShader(ShaderVariant *shader)
    {
        GL4DomainShaderPtr glShader = GL4DomainShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);
        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GL4DomainShaderPtr &glShader, String shaderSource)
        {
            return compileGLSLShader(GL_TESS_EVALUATION_SHADER, shaderSource, glShader->GLShaderHandle, "Domain");
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    TResult GL4Context::setDomainShader(ShaderVariant *shader)
    {
        return attachGraphicsShader(shader, mCurrentDSVariant);
    }

    TResult GL4Context::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(buffers);
    }

    TResult GL4Context::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GL4Context::setDSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    // Geometry Shader
    //--------------------------------------------------------------------------

    RHIShaderPtr GL4Context::createGeometryShader(ShaderVariant *shader)
    {
        GL4GeometryShaderPtr glShader = GL4GeometryShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GL4GeometryShaderPtr &glShader, String shaderSource)
        {
            TResult ret = T3D_OK;

            do
            {
                const char *src = shaderSource.c_str();
                GLint len = static_cast<GLint>(shaderSource.size());

                glShader->GLShaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
                glShaderSource(glShader->GLShaderHandle, 1, &src, &len);
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
                        T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Geometry shader compile error: %s", log.data());
                    }
                    GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
                    ret = T3D_ERR_GL4_COMPILE_SHADER;
                    break;
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createGeometryShader");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setGeometryShader(ShaderVariant *shader)
    {
        return attachGraphicsShader(shader, mCurrentGSVariant);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(buffers);
    }

    TResult GL4Context::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GL4Context::setGSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    // Compute Shader (GL 4.3+)
    //--------------------------------------------------------------------------

    RHIShaderPtr GL4Context::createComputeShader(ShaderVariant *shader)
    {
        if (!mCapabilities.supportsCompute)
        {
            T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "Compute shader requires OpenGL 4.3+");
            return nullptr;
        }

        GL4ComputeShaderPtr glShader = GL4ComputeShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);
        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GL4ComputeShaderPtr &glShader, String shaderSource)
        {
            return compileGLSLShader(GL_COMPUTE_SHADER, shaderSource, glShader->GLShaderHandle, "Compute");
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    TResult GL4Context::setComputeShader(ShaderVariant *shader)
    {
        if (shader == nullptr)
        {
            auto lambda = [this]()
            {
                mCurrentCSVariant = nullptr;
                if (mCurrentComputeProgram != 0)
                {
                    glUseProgram(0);
                    GL_SAFE_DELETE_PROGRAM(mCurrentComputeProgram);
                }
                mComputeProgramDirty = false;
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

        GL4Shader *glShader = static_cast<GL4Shader*>(shader->getRHIShader());
        GLuint shaderHandle = glShader->GLShaderHandle;

        auto lambda = [this](GLuint shaderHandle, ShaderVariant *variant)
        {
            TResult ret = T3D_OK;

            do
            {
                mCurrentCSVariant = variant;

                if (mCurrentComputeProgram != 0)
                {
                    GL_SAFE_DELETE_PROGRAM(mCurrentComputeProgram);
                }
                mCurrentComputeProgram = glCreateProgram();
                glAttachShader(mCurrentComputeProgram, shaderHandle);
                mComputeProgramDirty = true;
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setComputeShader");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, shaderHandle, shader);
    }

    TResult GL4Context::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(buffers);
    }

    TResult GL4Context::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GL4Context::setCSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(startSlot, samplers);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::compileShader(ShaderVariant *shader)
    {
        size_t bytesLength = 0;
        const char *bytes = shader->getBytesCode(bytesLength);
        shader->setBytesCode(bytes, bytesLength);

        return glslangCompileAndReflect(shader);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::glslangCompileAndReflect(ShaderVariant *shader)
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
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "glslangCompileAndReflect: unsupported shader stage !");
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

            const TBuiltInResource *resources = GetDefaultResources();
            const int glslVersion = (shader->getShaderStage() == SHADER_STAGE::kCompute) ? 430 : 400;
            if (!glslangShader.parse(resources, glslVersion, false, EShMsgDefault))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "glslang parse error:\n%s", glslangShader.getInfoLog());
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            glslang::TProgram program;
            program.addShader(&glslangShader);

            if (!program.link(EShMsgDefault))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "glslang link error:\n%s", program.getInfoLog());
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            if (!program.buildReflection(EShReflectionAllBlockVariables))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "glslang buildReflection failed !");
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            GlslangReflectionData data;

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
            }

            mReflectionCache[shader] = std::move(data);

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            auto itr = mReflectionCache.find(shader);
            if (itr == mReflectionCache.end())
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "reflectShaderAllBindings: no cached reflection data (compileShader not called?)");
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            const GlslangReflectionData &data = itr->second;

            // Reflect Uniform Blocks (cbuffers) — iterate block members
            for (int blockIdx = 0; blockIdx < static_cast<int>(data.blocks.size()); ++blockIdx)
            {
                for (const auto &uniform : data.uniforms)
                {
                    if (uniform.blockIndex != blockIdx)
                        continue;

                    if (uniform.glDefineType == GL_SAMPLER_1D
                        || uniform.glDefineType == GL_SAMPLER_2D
                        || uniform.glDefineType == GL_SAMPLER_3D
                        || uniform.glDefineType == GL_SAMPLER_CUBE
                        || uniform.glDefineType == GL_SAMPLER_2D_SHADOW)
                    {
                        continue;
                    }

                    uint32_t dataSize = 0;
                    ShaderConstantParam::DATA_TYPE dataType = ShaderConstantParam::DATA_TYPE::DT_FLOAT;

                    switch (uniform.glDefineType)
                    {
                    case GL_FLOAT:       dataSize = sizeof(float) * uniform.size; dataType = (uniform.size > 1) ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT; break;
                    case GL_FLOAT_VEC4:  dataSize = sizeof(float) * 4 * uniform.size; dataType = (uniform.size > 1) ? ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY : ShaderConstantParam::DATA_TYPE::DT_VECTOR4; break;
                    case GL_FLOAT_MAT4:  dataSize = sizeof(float) * 16 * uniform.size; dataType = (uniform.size > 1) ? ShaderConstantParam::DATA_TYPE::DT_MATRIX4_ARRAY : ShaderConstantParam::DATA_TYPE::DT_MATRIX4; break;
                    case GL_INT:         dataSize = sizeof(int) * uniform.size; dataType = (uniform.size > 1) ? ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY : ShaderConstantParam::DATA_TYPE::DT_INTEGER; break;
                    case GL_BOOL:        dataSize = sizeof(int) * uniform.size; dataType = (uniform.size > 1) ? ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY : ShaderConstantParam::DATA_TYPE::DT_BOOL; break;
                    default:             dataSize = uniform.arrayStride > 0 ? uniform.arrayStride * uniform.size : 4 * uniform.size; dataType = ShaderConstantParam::DATA_TYPE::DT_STRUCT; break;
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

                    T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "Shader reflection - UBO: %s -> %s, uniform: %s -> %s, type: %u, size: %u, offset: %d",
                        data.blocks[blockIdx].name.c_str(), cbufferName.c_str(), uniform.name.c_str(), cname.c_str(), dataType, dataSize, uniform.offset);
                }
            }

            // 按 GL 类型识别采样器。glslang 的 uniform.index 语义随版本变化，
            // 用它反推 standalone / block 成员不可靠，直接看类型。
            uint32_t samplerIndex = 0;
            for (const auto &uniform : data.uniforms)
            {
                bool isSampler = false;
                TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;

                switch (uniform.glDefineType)
                {
                case GL_SAMPLER_1D:         isSampler = true; texType = TEXTURE_TYPE::TT_1D; break;
                case GL_SAMPLER_2D:         isSampler = true; texType = TEXTURE_TYPE::TT_2D; break;
                case GL_SAMPLER_3D:         isSampler = true; texType = TEXTURE_TYPE::TT_3D; break;
                case GL_SAMPLER_CUBE:       isSampler = true; texType = TEXTURE_TYPE::TT_CUBE; break;
                case GL_SAMPLER_2D_SHADOW:  isSampler = true; texType = TEXTURE_TYPE::TT_2D; break;
                default: break;
                }

                if (isSampler)
                {
                    String name = uniform.name;

                    const String kSpirvPrefix = "SPIRV_Cross_Combined";
                    if (StringUtil::startsWith(name, kSpirvPrefix, false))
                    {
                        String remainder = name.substr(kSpirvPrefix.size());
                        String::size_type samplerPos = remainder.find("sampler");
                        if (samplerPos != String::npos && samplerPos > 0)
                        {
                            name = remainder.substr(0, samplerPos);
                        }
                    }

                    ShaderSamplerParamPtr param;
                    const auto it = samplerParams.find(name);
                    if (it == samplerParams.end())
                    {
                        param = ShaderSamplerParam::create(name);
                        samplerParams.emplace(name, param);
                    }
                    else
                    {
                        param = it->second;
                    }

                    param->setTexBinding(samplerIndex);
                    param->setSamplerBinding(samplerIndex);
                    param->setTextureType(texType);

                    T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "Shader reflection - sampler: %s -> %s, binding: %d, type: %d",
                        uniform.name.c_str(), name.c_str(), samplerIndex, texType);
                    samplerIndex++;
                }
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            auto itr = mReflectionCache.find(shader);
            if (itr == mReflectionCache.end())
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "reflectSamplerBindings: no cached reflection data (compileShader not called?)");
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            const GlslangReflectionData &data = itr->second;

            uint32_t samplerIndex = 0;
            for (const auto &uniform : data.uniforms)
            {
                bool isSampler = false;
                TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;

                switch (uniform.glDefineType)
                {
                case GL_SAMPLER_1D:         isSampler = true; texType = TEXTURE_TYPE::TT_1D; break;
                case GL_SAMPLER_2D:         isSampler = true; texType = TEXTURE_TYPE::TT_2D; break;
                case GL_SAMPLER_3D:         isSampler = true; texType = TEXTURE_TYPE::TT_3D; break;
                case GL_SAMPLER_CUBE:       isSampler = true; texType = TEXTURE_TYPE::TT_CUBE; break;
                case GL_SAMPLER_2D_SHADOW:  isSampler = true; texType = TEXTURE_TYPE::TT_2D; break;
                default: break;
                }

                if (isSampler)
                {
                    String name = uniform.name;

                    const String kSpirvPrefix = "SPIRV_Cross_Combined";
                    if (StringUtil::startsWith(name, kSpirvPrefix, false))
                    {
                        String remainder = name.substr(kSpirvPrefix.size());
                        String::size_type samplerPos = remainder.find("sampler");
                        if (samplerPos != String::npos && samplerPos > 0)
                        {
                            name = remainder.substr(0, samplerPos);
                        }
                    }

                    auto it = samplerParams.find(name);
                    if (it != samplerParams.end())
                    {
                        it->second->setTexBinding(samplerIndex);
                        it->second->setSamplerBinding(samplerIndex);
                        it->second->setTextureType(texType);
                    }

                    samplerIndex++;
                }
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setPrimitiveType(PrimitiveType primitive)
    {
        GLenum glPrimitive = GL4Mapping::get(primitive);

        auto lambda = [this](GLenum glPrimitive)
        {
            TResult ret = T3D_OK;

            do
            {
                mPrimitiveType = glPrimitive;
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glPrimitive);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::ensureProgramLinked()
    {
        if (mCurrentProgram == 0 || !mProgramDirty)
            return T3D_OK;

        glLinkProgram(mCurrentProgram);

        GLint linked = 0;
        glGetProgramiv(mCurrentProgram, GL_LINK_STATUS, &linked);
        if (!linked)
        {
            GLint logLen = 0;
            glGetProgramiv(mCurrentProgram, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen > 0)
            {
                TArray<char> log(logLen + 1, 0);
                glGetProgramInfoLog(mCurrentProgram, logLen, nullptr, log.data());
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Program link error: %s", log.data());
            }
            return T3D_ERR_GL4_LINK_PROGRAM;
        }

        glUseProgram(mCurrentProgram);
        bindPendingUniformBlocks(mCurrentProgram);
        setupSamplerBindings(mCurrentProgram);
        mProgramDirty = false;

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::ensureProgramLinked");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::ensureComputeProgramLinked()
    {
        if (mCurrentComputeProgram == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "ensureComputeProgramLinked: no compute program bound !");
            return T3D_ERR_GL4_LINK_PROGRAM;
        }

        if (mComputeProgramDirty)
        {
            glLinkProgram(mCurrentComputeProgram);

            GLint linked = 0;
            glGetProgramiv(mCurrentComputeProgram, GL_LINK_STATUS, &linked);
            if (!linked)
            {
                GLint logLen = 0;
                glGetProgramiv(mCurrentComputeProgram, GL_INFO_LOG_LENGTH, &logLen);
                if (logLen > 0)
                {
                    TArray<char> log(logLen + 1, 0);
                    glGetProgramInfoLog(mCurrentComputeProgram, logLen, nullptr, log.data());
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Compute program link error: %s", log.data());
                }
                return T3D_ERR_GL4_LINK_PROGRAM;
            }

            mComputeProgramDirty = false;
        }

        glUseProgram(mCurrentComputeProgram);
        bindPendingUniformBlocks(mCurrentComputeProgram);
        setupSamplerBindings(mCurrentComputeProgram);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::ensureComputeProgramLinked");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
    {
        auto lambda = [this](uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
        {
            TResult ret = T3D_OK;

            do
            {
                ret = ensureProgramLinked();
                if (T3D_FAILED(ret))
                    break;

                const void *offset = reinterpret_cast<const void*>((uintptr_t)(startIndex * mIndexSize));

                // Safety check: verify EBO is bound before issuing indexed draw
                GLint boundEBO = 0;
                glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundEBO);
                if (boundEBO == 0)
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::render(indexed): No EBO bound! Skipping draw to avoid crash.");
                    ret = T3D_ERR_GL4_INVALID_USAGE;
                    break;
                }

                glDrawElementsBaseVertex(mPrimitiveType, indexCount, mIndexType, offset, baseVertex);

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::render(indexed)");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, indexCount, startIndex, baseVertex);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::render(uint32_t vertexCount, uint32_t startVertex)
    {
        auto lambda = [this](uint32_t vertexCount, uint32_t startVertex)
        {
            TResult ret = T3D_OK;

            do
            {
                ret = ensureProgramLinked();
                if (T3D_FAILED(ret))
                    break;

                glDrawArrays(mPrimitiveType, startVertex, vertexCount);

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::render(non-indexed)");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, vertexCount, startVertex);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::renderIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
        uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)
    {
        if (startInstance != 0 && !mCapabilities.supportsBaseInstance)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER,
                "renderIndexedInstanced() with non-zero startInstance requires GL 4.2+ !");
            return T3D_ERR_NOT_IMPLEMENT;
        }

        auto lambda = [this](uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)
        {
            TResult ret = T3D_OK;

            do
            {
                ret = ensureProgramLinked();
                if (T3D_FAILED(ret))
                    break;

                const void *offset = reinterpret_cast<const void*>((uintptr_t)(startIndex * mIndexSize));

                GLint boundEBO = 0;
                glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundEBO);
                if (boundEBO == 0)
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::renderIndexedInstanced(): No EBO bound! Skipping draw to avoid crash.");
                    ret = T3D_ERR_GL4_INVALID_USAGE;
                    break;
                }

                if (startInstance != 0)
                {
                    glDrawElementsInstancedBaseVertexBaseInstance(mPrimitiveType, indexCount,
                        mIndexType, offset, instanceCount, baseVertex, startInstance);
                }
                else
                {
                    glDrawElementsInstancedBaseVertex(mPrimitiveType, indexCount, mIndexType,
                        offset, instanceCount, baseVertex);
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::renderIndexedInstanced");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, indexCount, instanceCount, startIndex, baseVertex, startInstance);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::renderInstanced(uint32_t vertexCount, uint32_t instanceCount,
        uint32_t startVertex, uint32_t startInstance)
    {
        if (startInstance != 0 && !mCapabilities.supportsBaseInstance)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER,
                "renderInstanced() with non-zero startInstance requires GL 4.2+ !");
            return T3D_ERR_NOT_IMPLEMENT;
        }

        auto lambda = [this](uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)
        {
            TResult ret = T3D_OK;

            do
            {
                ret = ensureProgramLinked();
                if (T3D_FAILED(ret))
                    break;

                if (startInstance != 0)
                {
                    glDrawArraysInstancedBaseInstance(mPrimitiveType, startVertex, vertexCount,
                        instanceCount, startInstance);
                }
                else
                {
                    glDrawArraysInstanced(mPrimitiveType, startVertex, vertexCount, instanceCount);
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::renderInstanced");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, vertexCount, instanceCount, startVertex, startInstance);
    }

    //--------------------------------------------------------------------------

    RHIStructuredBufferPtr GL4Context::createStructuredBuffer(StructuredBuffer *buffer)
    {
        if (!mCapabilities.supportsStructuredBuffer)
        {
            T3D_RHI_UNSUPPORTED_PTR(supportsStructuredBuffer);
        }

        if (buffer == nullptr)
        {
            return nullptr;
        }

        GL4StructuredBufferPtr glBuffer = GL4StructuredBuffer::create();
        const StructuredBufferDesc &desc = buffer->getDescriptor();
        const size_t byteWidth = buffer->getGPUSizeInBytes();
        const GLenum usage = GL4Mapping::getBufferUsage(buffer->getUsage());
        const bool hasCounter = desc.hasCounter || desc.isAppendConsume;

        glBuffer->ElementCount = desc.elementCount;
        glBuffer->ElementSize = desc.elementSize;
        glBuffer->HasCounter = hasCounter;

        auto lambda = [this](const GL4StructuredBufferPtr &glBuffer, const StructuredBufferPtr &buffer,
            size_t byteWidth, GLenum usage, bool hasCounter)
        {
            TResult ret = T3D_OK;

            do
            {
                glGenBuffers(1, &glBuffer->GLBuffer);
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, glBuffer->GLBuffer);
                glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)byteWidth,
                    buffer->getBuffer().Data, usage);
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

                if (hasCounter)
                {
                    uint32_t zero = 0;
                    glGenBuffers(1, &glBuffer->GLCounterBuffer);
                    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, glBuffer->GLCounterBuffer);
                    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(uint32_t), &zero, GL_DYNAMIC_DRAW);
                    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createStructuredBuffer");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, StructuredBufferPtr(buffer), byteWidth, usage, hasCounter);
        if (T3D_FAILED(ret))
        {
            return nullptr;
        }

        return glBuffer;
    }

    TResult GL4Context::setVSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers)
    {
        return bindStructuredBuffers(startSlot, buffers);
    }

    TResult GL4Context::setPSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers)
    {
        return bindStructuredBuffers(startSlot, buffers);
    }

    TResult GL4Context::setCSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers)
    {
        return bindStructuredBuffers(startSlot, buffers);
    }

    TResult GL4Context::setCSUnorderedAccessBuffers(uint32_t startSlot, const UnorderedAccessBuffers &buffers, const UAVInitialCounts &initialCounts)
    {
        if (!mCapabilities.supportsUnorderedAccess)
        {
            T3D_RHI_UNSUPPORTED(supportsUnorderedAccess);
        }

        if (!initialCounts.empty() && initialCounts.size() != buffers.size())
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "setCSUnorderedAccessBuffers : initial count array "
                "size [%zu] does not match buffer count [%zu] !", initialCounts.size(), buffers.size());
            return T3D_ERR_INVALID_PARAM;
        }

        struct UAVBinding
        {
            GLuint ssbo {0};
            GLuint counter {0};
            uint32_t initialCount {kKeepUAVCounter};
            bool hasCounter {false};
        };

        TArray<UAVBinding> bindings;
        bindings.reserve(buffers.size());

        for (uint32_t i = 0; i < buffers.size(); ++i)
        {
            UAVBinding b {};
            if (!initialCounts.empty())
            {
                b.initialCount = initialCounts[i];
            }

            RenderBuffer *rb = buffers[i].get();
            if (rb != nullptr && rb->getRHIResource() != nullptr
                && rb->getRHIResource()->getResourceType() == RHIResource::ResourceType::kStructuredBuffer)
            {
                GL4StructuredBuffer *glSB = static_cast<GL4StructuredBuffer*>(rb->getRHIResource().get());
                b.ssbo = glSB->GLBuffer;
                b.counter = glSB->GLCounterBuffer;
                b.hasCounter = glSB->HasCounter;
            }

            bindings.push_back(b);
        }

        auto lambda = [this](uint32_t startSlot, TArray<UAVBinding> bindings)
        {
            TResult ret = T3D_OK;

            do
            {
                for (uint32_t i = 0; i < bindings.size(); ++i)
                {
                    const uint32_t slot = startSlot + i;
                    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, bindings[i].ssbo);

                    if (bindings[i].hasCounter && bindings[i].counter != 0)
                    {
                        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, slot, bindings[i].counter);
                        if (bindings[i].initialCount != kKeepUAVCounter)
                        {
                            uint32_t count = bindings[i].initialCount;
                            glNamedBufferSubData(bindings[i].counter, 0, sizeof(uint32_t), &count);
                        }
                    }
                    else
                    {
                        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, slot, 0);
                    }
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setCSUnorderedAccessBuffers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, bindings);
    }

    TResult GL4Context::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        if (!mCapabilities.supportsCompute)
        {
            T3D_RHI_UNSUPPORTED(supportsCompute);
        }

        if (groupCountX == 0 || groupCountY == 0 || groupCountZ == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "dispatch : group count [%u, %u, %u] must not contain zero !",
                groupCountX, groupCountY, groupCountZ);
            return T3D_ERR_INVALID_PARAM;
        }

        const uint32_t *maxGroups = mCapabilities.maxDispatchGroupCount;
        if (groupCountX > maxGroups[0] || groupCountY > maxGroups[1] || groupCountZ > maxGroups[2])
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "dispatch : group count [%u, %u, %u] exceeds device "
                "limit [%u, %u, %u] !", groupCountX, groupCountY, groupCountZ,
                maxGroups[0], maxGroups[1], maxGroups[2]);
            return T3D_ERR_INVALID_PARAM;
        }

        auto lambda = [this](uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) -> TResult
        {
            TResult ret = ensureComputeProgramLinked();
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            glDispatchCompute(groupCountX, groupCountY, groupCountZ);

            if (mCurrentProgram != 0)
            {
                glUseProgram(mCurrentProgram);
            }

            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::dispatch");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, groupCountX, groupCountY, groupCountZ);
    }

    TResult GL4Context::dispatchIndirect(RenderBuffer *argsBuffer, size_t argsOffset)
    {
        if (!mCapabilities.supportsIndirectDispatch)
        {
            T3D_RHI_UNSUPPORTED(supportsIndirectDispatch);
        }

        TResult ret = validateIndirectArgs(argsBuffer, argsOffset, sizeof(DispatchIndirectArgs));
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        GLuint glBuf = getGLBufferHandle(argsBuffer);

        auto lambda = [this](GLuint glBuf, GLintptr argsOffset) -> TResult
        {
            TResult ret = ensureComputeProgramLinked();
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, glBuf);
            glDispatchComputeIndirect(argsOffset);
            glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);

            if (mCurrentProgram != 0)
            {
                glUseProgram(mCurrentProgram);
            }

            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::dispatchIndirect");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glBuf, (GLintptr)argsOffset);
    }

    TResult GL4Context::uavBarrier(const UnorderedAccessBuffers &buffers)
    {
        if (!mCapabilities.supportsUnorderedAccess)
        {
            T3D_RHI_UNSUPPORTED(supportsUnorderedAccess);
        }

        auto lambda = [this]()
        {
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT
                | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
                | GL_TEXTURE_FETCH_BARRIER_BIT
                | GL_ATOMIC_COUNTER_BARRIER_BIT
                | GL_COMMAND_BARRIER_BIT
                | GL_BUFFER_UPDATE_BARRIER_BIT);
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::uavBarrier");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    TResult GL4Context::copyStructureCount(RenderBuffer *dstBuffer, size_t dstOffset, RenderBuffer *srcBuffer)
    {
        if (!mCapabilities.supportsAppendConsumeBuffer)
        {
            T3D_RHI_UNSUPPORTED(supportsAppendConsumeBuffer);
        }

        if (dstBuffer == nullptr || srcBuffer == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyStructureCount : null buffer !");
            return T3D_ERR_INVALID_POINTER;
        }

        if ((dstOffset % 4) != 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyStructureCount : destination offset [%zu] must be a multiple of 4 !", dstOffset);
            return T3D_ERR_INVALID_PARAM;
        }

        if (dstOffset + sizeof(uint32_t) > dstBuffer->getGPUSizeInBytes())
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyStructureCount : offset [%zu] + 4 exceeds destination buffer size [%zu] !",
                dstOffset, dstBuffer->getGPUSizeInBytes());
            return T3D_ERR_OUT_OF_BOUND;
        }

        if (srcBuffer->getType() != RenderResource::Type::kStructuredBuffer
            || !static_cast<StructuredBuffer*>(srcBuffer)->hasUAVCounter())
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyStructureCount : source must be a structured buffer with a hidden counter !");
            return T3D_ERR_INVALID_PARAM;
        }

        GLuint dstBuf = getGLBufferHandle(dstBuffer);
        GL4StructuredBuffer *glSrc = static_cast<GL4StructuredBuffer*>(srcBuffer->getRHIResource().get());
        GLuint srcCounter = (glSrc != nullptr) ? glSrc->GLCounterBuffer : 0;
        if (dstBuf == 0 || srcCounter == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyStructureCount : underlying GL objects are not ready !");
            return T3D_ERR_INVALID_POINTER;
        }

        auto lambda = [this](GLuint dstBuf, GLintptr dstOffset, GLuint srcCounter)
        {
            glCopyNamedBufferSubData(srcCounter, dstBuf, 0, dstOffset, sizeof(uint32_t));
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::copyStructureCount");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, dstBuf, (GLintptr)dstOffset, srcCounter);
    }

    TResult GL4Context::renderIndexedIndirect(RenderBuffer *argsBuffer, size_t argsOffset)
    {
        if (!mCapabilities.supportsIndirectDraw)
        {
            T3D_RHI_UNSUPPORTED(supportsIndirectDraw);
        }

        TResult ret = validateIndirectArgs(argsBuffer, argsOffset, sizeof(DrawIndexedIndirectArgs));
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        GLuint glBuf = getGLBufferHandle(argsBuffer);

        auto lambda = [this](GLuint glBuf, GLintptr argsOffset) -> TResult
        {
            TResult ret = ensureProgramLinked();
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, glBuf);
            glDrawElementsIndirect(mPrimitiveType, mIndexType, reinterpret_cast<const void *>(argsOffset));
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::renderIndexedIndirect");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glBuf, (GLintptr)argsOffset);
    }

    TResult GL4Context::renderIndirect(RenderBuffer *argsBuffer, size_t argsOffset)
    {
        if (!mCapabilities.supportsIndirectDraw)
        {
            T3D_RHI_UNSUPPORTED(supportsIndirectDraw);
        }

        TResult ret = validateIndirectArgs(argsBuffer, argsOffset, sizeof(DrawIndirectArgs));
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        GLuint glBuf = getGLBufferHandle(argsBuffer);

        auto lambda = [this](GLuint glBuf, GLintptr argsOffset) -> TResult
        {
            TResult ret = ensureProgramLinked();
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, glBuf);
            glDrawArraysIndirect(mPrimitiveType, reinterpret_cast<const void *>(argsOffset));
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::renderIndirect");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glBuf, (GLintptr)argsOffset);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::reset()
    {
        mCurrentRenderTarget = nullptr;

        auto lambda = [this]()
        {
            TResult ret = T3D_OK;

            do
            {
                mCurrentVSVariant = nullptr;
                mCurrentPSVariant = nullptr;
                mCurrentHSVariant = nullptr;
                mCurrentDSVariant = nullptr;
                mCurrentGSVariant = nullptr;
                mCurrentCSVariant = nullptr;

                GL_SAFE_DELETE_PROGRAM(mCurrentComputeProgram);
                mComputeProgramDirty = false;

                glUseProgram(0);
                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                unbindTextureUnits();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                GL_SAFE_DELETE_PROGRAM(mCurrentProgram);
                mPendingUBOs.clear();
                mProgramDirty = false;
                mCurrentVAO = 0;

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::reset");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    //--------------------------------------------------------------------------
    // Blit operations
    //--------------------------------------------------------------------------

    TResult GL4Context::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        BlitEndpoint srcEp {}, dstEp {};
        TResult ret = resolveBlitEndpoint(src, true, srcEp);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        ret = resolveBlitEndpoint(dst, false, dstEp);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        auto lambda = [this](BlitEndpoint srcEp, BlitEndpoint dstEp, Vector3 srcOffset, Vector3 size, Vector3 dstOffset)
        {
            return doBlit(srcEp, dstEp, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, srcEp, dstEp, srcOffset, size, dstOffset);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        BlitEndpoint srcEp {}, dstEp {};
        TResult ret = resolveBlitEndpoint(src, true, srcEp);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        ret = resolveBlitEndpoint(dst, false, dstEp);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        auto lambda = [this](BlitEndpoint srcEp, BlitEndpoint dstEp, Vector3 srcOffset, Vector3 size, Vector3 dstOffset)
        {
            return doBlit(srcEp, dstEp, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, srcEp, dstEp, srcOffset, size, dstOffset);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        BlitEndpoint srcEp {}, dstEp {};
        TResult ret = resolveBlitEndpoint(src, true, srcEp);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        ret = resolveBlitEndpoint(dst, false, dstEp);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        auto lambda = [this](BlitEndpoint srcEp, BlitEndpoint dstEp, Vector3 srcOffset, Vector3 size, Vector3 dstOffset)
        {
            return doBlit(srcEp, dstEp, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, srcEp, dstEp, srcOffset, size, dstOffset);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        BlitEndpoint srcEp {}, dstEp {};
        TResult ret = resolveBlitEndpoint(src, true, srcEp);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        ret = resolveBlitEndpoint(dst, false, dstEp);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        auto lambda = [this](BlitEndpoint srcEp, BlitEndpoint dstEp, Vector3 srcOffset, Vector3 size, Vector3 dstOffset)
        {
            return doBlit(srcEp, dstEp, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, srcEp, dstEp, srcOffset, size, dstOffset);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyBuffer : null buffer !");
            return T3D_ERR_INVALID_POINTER;
        }

        if (src == dst)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyBuffer : source and destination must be different !");
            return T3D_ERR_INVALID_PARAM;
        }

        auto isLinearBuffer = [](const RHIResourcePtr &resource)
        {
            const RHIResource::ResourceType type = resource->getResourceType();
            return type == RHIResource::ResourceType::kVertexBuffer
                || type == RHIResource::ResourceType::kIndexBuffer
                || type == RHIResource::ResourceType::kConstantBuffer
                || type == RHIResource::ResourceType::kStructuredBuffer;
        };

        if (src->getRHIResource() == nullptr || dst->getRHIResource() == nullptr
            || !isLinearBuffer(src->getRHIResource()) || !isLinearBuffer(dst->getRHIResource()))
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyBuffer : only linear buffers are supported, use blit for textures !");
            return T3D_ERR_GL4_UNSUPPORTED_OPERATION;
        }

        if (dst->getUsage() == Usage::kImmutable)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyBuffer : destination is immutable !");
            return T3D_ERR_GL4_INVALID_USAGE;
        }

        const size_t srcSize = src->getGPUSizeInBytes();
        const size_t dstSize = dst->getGPUSizeInBytes();
        const size_t copySize = (size == 0)
            ? (srcSize - std::min(srcOffset, srcSize))
            : size;

        if (copySize == 0)
        {
            return T3D_OK;
        }

        if (srcOffset + copySize > srcSize || dstOffset + copySize > dstSize)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyBuffer : out of range ! src [%zu + %zu / %zu] dst [%zu + %zu / %zu]",
                srcOffset, copySize, srcSize, dstOffset, copySize, dstSize);
            return T3D_ERR_OUT_OF_BOUND;
        }

        GLuint srcBuf = getGLBufferHandle(src);
        GLuint dstBuf = getGLBufferHandle(dst);
        if (srcBuf == 0 || dstBuf == 0)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        auto lambda = [this](GLuint srcBuf, GLuint dstBuf, GLintptr srcOffset, GLintptr dstOffset, GLsizeiptr copySize)
        {
            glCopyNamedBufferSubData(srcBuf, dstBuf, srcOffset, dstOffset, copySize);
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::copyBuffer");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, srcBuf, dstBuf, (GLintptr)srcOffset, (GLintptr)dstOffset, (GLsizeiptr)copySize);
    }

    //--------------------------------------------------------------------------

    ReadbackHandle GL4Context::map(RenderBuffer *src, size_t offset, size_t size)
    {
        ReadbackRequest *request = nullptr;
        ReadbackHandle handle = allocReadbackRequest(src, false, request);
        if (!handle.isValid())
        {
            return handle;
        }

        request->BufferOffset = offset;
        request->BufferSize = size;

        auto lambda = [this](ReadbackRequest *request, const RenderBufferPtr &src) -> TResult
        {
            request->CopyRecorded = true;

            GLuint srcBuf = getGLBufferHandle(src.get());
            if (srcBuf == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : failed to retrieve underlying GL buffer !");
                request->CopyResult = T3D_ERR_INVALID_POINTER;
                return request->CopyResult;
            }

            const size_t srcSize = src->getGPUSizeInBytes();
            const size_t offset = std::min(request->BufferOffset, srcSize);
            const size_t copySize = (request->BufferSize == 0) ? (srcSize - offset) : request->BufferSize;

            if (copySize == 0 || offset + copySize > srcSize)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : buffer out of range ! [%zu + %zu / %zu]", offset, copySize, srcSize);
                request->CopyResult = T3D_ERR_INVALID_PARAM;
                return request->CopyResult;
            }

            glGenBuffers(1, &request->Staging);
            glNamedBufferData(request->Staging, (GLsizeiptr)copySize, nullptr, GL_STREAM_READ);
            glCopyNamedBufferSubData(srcBuf, request->Staging, (GLintptr)offset, 0, (GLsizeiptr)copySize);

            request->TotalBytes = copySize;
            request->TightRowPitch = static_cast<uint32_t>(copySize);
            request->TightSlicePitch = static_cast<uint32_t>(copySize);
            request->CopyWidth = static_cast<uint32_t>(copySize);
            request->CopyResult = T3D_OK;
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::map(buffer)");
            return T3D_OK;
        };

        ENQUEUE_UNIQUE_COMMAND(lambda, request, RenderBufferPtr(src));
        return handle;
    }

    //--------------------------------------------------------------------------

    ReadbackHandle GL4Context::map(RenderBuffer *src, const ReadbackRegion &region)
    {
        ReadbackRequest *request = nullptr;
        ReadbackHandle handle = allocReadbackRequest(src, true, request);
        if (!handle.isValid())
        {
            return handle;
        }

        request->Region = region;

        auto lambda = [this](ReadbackRequest *request, const RenderBufferPtr &src) -> TResult
        {
            request->CopyRecorded = true;

            if (src->getRHIResource() == nullptr
                || src->getRHIResource()->getResourceType() != RHIResource::ResourceType::kPixelBuffer2D)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : only 2D textures are supported for texture readback !");
                request->CopyResult = T3D_ERR_GL4_UNSUPPORTED_OPERATION;
                return request->CopyResult;
            }

            PixelBuffer2D *pb = static_cast<PixelBuffer2D*>(src.get());
            const auto &desc = pb->getDescriptor();
            const uint32_t bpp = GL4Mapping::getBytesPerPixel(desc.format);
            if (bpp == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : format is not supported for readback !");
                request->CopyResult = T3D_ERR_GL4_UNSUPPORTED_OPERATION;
                return request->CopyResult;
            }

            GL4PixelBuffer2D *glPB = static_cast<GL4PixelBuffer2D*>(src->getRHIResource().get());
            GLuint tex = glPB->GLTexture;
            if (glPB->GLMSAACount > 1 && glPB->GLResolveTex != 0)
            {
                if (glPB->GLFBO != 0 && glPB->GLResolveFBO != 0)
                {
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, glPB->GLFBO);
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, glPB->GLResolveFBO);
                    glBlitFramebuffer(0, 0, (GLint)desc.width, (GLint)desc.height,
                        0, 0, (GLint)desc.width, (GLint)desc.height,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                }
                tex = glPB->GLResolveTex;
            }

            const uint32_t mipWidth = std::max<uint32_t>(1, desc.width >> request->Region.mipLevel);
            const uint32_t mipHeight = std::max<uint32_t>(1, desc.height >> request->Region.mipLevel);
            const uint32_t offsetX = static_cast<uint32_t>(request->Region.offset.x());
            const uint32_t offsetY = static_cast<uint32_t>(request->Region.offset.y());
            uint32_t copyWidth = static_cast<uint32_t>(request->Region.size.x());
            uint32_t copyHeight = static_cast<uint32_t>(request->Region.size.y());
            if (copyWidth == 0)
            {
                copyWidth = mipWidth - std::min(offsetX, mipWidth);
            }
            if (copyHeight == 0)
            {
                copyHeight = mipHeight - std::min(offsetY, mipHeight);
            }

            if (copyWidth == 0 || copyHeight == 0
                || offsetX + copyWidth > mipWidth || offsetY + copyHeight > mipHeight)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : region out of range !");
                request->CopyResult = T3D_ERR_INVALID_PARAM;
                return request->CopyResult;
            }

            request->CopyWidth = copyWidth;
            request->CopyHeight = copyHeight;
            request->CopyDepth = 1;
            request->TightRowPitch = copyWidth * bpp;
            request->TightSlicePitch = request->TightRowPitch * copyHeight;
            request->TotalBytes = request->TightSlicePitch;

            glGenBuffers(1, &request->Staging);
            glNamedBufferData(request->Staging, (GLsizeiptr)request->TotalBytes, nullptr, GL_STREAM_READ);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, request->Staging);
            glGetTextureSubImage(tex, (GLint)request->Region.mipLevel,
                (GLint)offsetX, (GLint)offsetY, 0,
                (GLsizei)copyWidth, (GLsizei)copyHeight, 1,
                GL4Mapping::get(desc.format), GL4Mapping::getPixelType(desc.format),
                (GLsizei)request->TotalBytes, nullptr);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

            request->CopyResult = T3D_OK;
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::map(texture)");
            return T3D_OK;
        };

        ENQUEUE_UNIQUE_COMMAND(lambda, request, RenderBufferPtr(src));
        return handle;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::unmap(ReadbackHandle handle, Buffer &dst)
    {
        return finishReadback(handle, dst);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)
    {
        TResult ret = T3D_OK;

        auto rhiRes = renderBuffer->getRHIResource();
        bool isTexture = false;

        switch (rhiRes->getResourceType())
        {
        case RHIResource::ResourceType::kVertexBuffer:
        case RHIResource::ResourceType::kIndexBuffer:
        case RHIResource::ResourceType::kConstantBuffer:
        case RHIResource::ResourceType::kStructuredBuffer:
            break;
        case RHIResource::ResourceType::kPixelBuffer2D:
            isTexture = true;
            break;
        default:
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Unsupported resource type for writeBuffer");
            return T3D_ERR_GL4_INVALID_USAGE;
        }

        // Deep copy Buffer.Data to avoid dangling pointer in multi-threaded mode.
        // The lambda executes on the RHI thread after the caller's stack frame is gone.
        Buffer ownedBuffer;
        ownedBuffer.DataSize = buffer.DataSize;
        ownedBuffer.Data = T3D_POD_NEW_ARRAY(uint8_t, buffer.DataSize);
        memcpy(ownedBuffer.Data, buffer.Data, buffer.DataSize);

        if (isTexture)
        {
            PixelBuffer2D *pb = static_cast<PixelBuffer2D*>(renderBuffer);
            const auto &desc = pb->getDescriptor();
            const GLenum pixelFmt = GL4Mapping::get(desc.format);
            const GLenum pixelType = GL4Mapping::getPixelType(desc.format);
            const GLsizei texW = static_cast<GLsizei>(desc.width);
            const GLsizei texH = static_cast<GLsizei>(desc.height);
            GL4PixelBuffer2DPtr glTex = static_cast<GL4PixelBuffer2D*>(rhiRes.get());

            auto lambda = [this](const GL4PixelBuffer2DPtr &glTex, Buffer ownedBuffer,
                GLsizei texW, GLsizei texH, GLenum pixelFmt, GLenum pixelType)
            {
                TResult ret = T3D_OK;

                do
                {
                    glBindTexture(GL_TEXTURE_2D, glTex->GLTexture);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        texW, texH, pixelFmt, pixelType, ownedBuffer.Data);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::writeBuffer(texture)");
                } while (false);

                T3D_POD_SAFE_DELETE_ARRAY(ownedBuffer.Data);
                return ret;
            };

            return ENQUEUE_UNIQUE_COMMAND(lambda, glTex, ownedBuffer, texW, texH, pixelFmt, pixelType);
        }

        // For buffer resources, use DSA (glNamedBufferData/glNamedBufferSubData) to avoid
        // polluting VAO's EBO binding when writing to GL_ELEMENT_ARRAY_BUFFER.
        auto rhiResPtr = rhiRes;  // capture smart pointer to keep alive

        auto lambda = [this](const RHIResourcePtr &rhiResPtr, Buffer ownedBuffer, bool discardWholeBuffer)
        {
            TResult ret = T3D_OK;

            do
            {
                GLuint glBuf = 0;

                switch (rhiResPtr->getResourceType())
                {
                case RHIResource::ResourceType::kVertexBuffer:
                    glBuf = static_cast<GL4VertexBuffer*>(rhiResPtr.get())->GLBuffer;
                    break;
                case RHIResource::ResourceType::kIndexBuffer:
                    glBuf = static_cast<GL4IndexBuffer*>(rhiResPtr.get())->GLBuffer;
                    break;
                case RHIResource::ResourceType::kConstantBuffer:
                    glBuf = static_cast<GL4ConstantBuffer*>(rhiResPtr.get())->GLBuffer;
                    break;
                case RHIResource::ResourceType::kStructuredBuffer:
                    glBuf = static_cast<GL4StructuredBuffer*>(rhiResPtr.get())->GLBuffer;
                    break;
                default:
                    break;
                }

                if (glBuf == 0)
                {
                    T3D_POD_SAFE_DELETE_ARRAY(ownedBuffer.Data);
                    break;
                }

                // Use DSA to avoid unbinding the current VAO's EBO
                if (discardWholeBuffer)
                {
                    glNamedBufferData(glBuf, (GLsizeiptr)ownedBuffer.DataSize, ownedBuffer.Data, GL_DYNAMIC_DRAW);
                }
                else
                {
                    glNamedBufferSubData(glBuf, 0, (GLsizeiptr)ownedBuffer.DataSize, ownedBuffer.Data);
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::writeBuffer");
            } while (false);

            T3D_POD_SAFE_DELETE_ARRAY(ownedBuffer.Data);
            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, rhiResPtr, ownedBuffer, discardWholeBuffer);
    }

    //--------------------------------------------------------------------------

    void* GL4Context::getNativeContext() const
    {
#if defined(T3D_OS_WINDOWS)
        return (void *)mSavedGLContext;
#elif defined(T3D_OS_LINUX)
        return (void *)mSavedGLContext;
#else
        return nullptr;
#endif
    }

    //--------------------------------------------------------------------------

    void GL4Context::restoreNativeContext()
    {
#if defined(T3D_OS_WINDOWS)
        if (mSavedGLContext != nullptr && mSavedGLDC != nullptr)
        {
            wglMakeCurrent(mSavedGLDC, mSavedGLContext);
        }
#elif defined(T3D_OS_LINUX)
        if (mSavedGLContext != nullptr && mSavedGLDisplay != nullptr)
        {
            glXMakeCurrent(mSavedGLDisplay, mSavedGLWindow, mSavedGLContext);
        }
#endif
    }

    //--------------------------------------------------------------------------
    // Helper methods for resource binding (OpenGL unified binding)
    //--------------------------------------------------------------------------

    void GL4Context::bindPendingUniformBlocks(GLuint program)
    {
        GLint numBlocks = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);

        T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "bindPendingUniformBlocks: program=%u numBlocks=%d pendingUBOs=%d",
            program, numBlocks, (int)mPendingUBOs.size());

        for (GLint i = 0; i < numBlocks; ++i)
        {
            char blockName[256] = {};
            GLsizei nameLen = 0;
            glGetActiveUniformBlockName(program, i, sizeof(blockName), &nameLen, blockName);

            String cbufferName(blockName);
            if (StringUtil::startsWith(cbufferName, "type_"))
            {
                cbufferName = cbufferName.substr(5);
            }

            GLuint bindingPoint = static_cast<GLuint>(i);
            glUniformBlockBinding(program, i, bindingPoint);

            auto it = mPendingUBOs.find(cbufferName);
            if (it != mPendingUBOs.end())
            {
                glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, it->second);
                T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "  UBO bound: block='%s' -> '%s' bindingPoint=%u glBuffer=%u",
                    blockName, cbufferName.c_str(), bindingPoint, it->second);
            }
            else
            {
                T3D_LOG_WARNING(LOG_TAG_GL4RENDERER,
                    "No pending UBO for uniform block '%s' (cbuffer name '%s')",
                    blockName, cbufferName.c_str());
            }
        }

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::bindPendingUniformBlocks");
    }

    //--------------------------------------------------------------------------

    void GL4Context::setupSamplerBindings(GLuint program)
    {
        GLint numUniforms = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

        // 解析 SPIRV-Cross 组合采样器名称，提取纹理名称
        // 格式: "SPIRV_Cross_Combined<texName>sampler<texName>" -> "<texName>"
        auto parseSpirvName = [](const String &uniformName) -> String
        {
            const String kSpirvPrefix = "SPIRV_Cross_Combined";
            if (StringUtil::startsWith(uniformName, kSpirvPrefix, false))
            {
                String remainder = uniformName.substr(kSpirvPrefix.size());
                String::size_type samplerPos = remainder.find("sampler");
                if (samplerPos != String::npos && samplerPos > 0)
                {
                    return remainder.substr(0, samplerPos);
                }
            }
            return uniformName;
        };

        // 从缓存的 ShaderVariant 中按名称查找反射阶段分配的 samplerBinding（即 texUnit slot）
        auto findSlot = [this](const String &texName) -> int32_t
        {
            ShaderVariant *variants[] = {
                mCurrentPSVariant, mCurrentVSVariant, mCurrentGSVariant,
                mCurrentHSVariant, mCurrentDSVariant, mCurrentCSVariant
            };
            for (ShaderVariant *variant : variants)
            {
                if (variant == nullptr)
                {
                    continue;
                }
                const auto &params = variant->getShaderSamplerParams();
                const auto itr = params.find(texName);
                if (itr != params.end())
                {
                    // GL 的 sampler uniform 值是 texture unit 号，语义上对应 texBinding。
                    // GL4 反射把 texBinding / samplerBinding 赋成同一个序号，两者等价。
                    return static_cast<int32_t>(itr->second->getTexBinding());
                }
            }
            return -1;
        };

        GLint fallbackTexUnit = 0;
        for (GLint i = 0; i < numUniforms; ++i)
        {
            char name[256] = {};
            GLsizei nameLen = 0;
            GLint uniformSize = 0;
            GLenum uniformType = 0;
            glGetActiveUniform(program, i, sizeof(name), &nameLen, &uniformSize, &uniformType, name);

            bool isSampler = (uniformType == GL_SAMPLER_1D || uniformType == GL_SAMPLER_2D
                || uniformType == GL_SAMPLER_3D || uniformType == GL_SAMPLER_CUBE
                || uniformType == GL_SAMPLER_2D_SHADOW);

            if (isSampler)
            {
                GLint loc = glGetUniformLocation(program, name);
                if (loc >= 0)
                {
                    String texName = parseSpirvName(String(name));
                    int32_t slot = findSlot(texName);

                    GLint texUnit;
                    if (slot >= 0)
                    {
                        texUnit = static_cast<GLint>(slot);
                    }
                    else
                    {
                        // 查找失败，回退到递增分配（不应该发生）
                        texUnit = fallbackTexUnit;
                        T3D_LOG_WARNING(LOG_TAG_GL4RENDERER,
                            "setupSamplerBindings: could not find slot for sampler='%s' texName='%s', fallback texUnit=%d",
                            name, texName.c_str(), texUnit);
                    }

                    glUniform1i(loc, texUnit);
                    T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "setupSamplerBindings: sampler='%s' texName='%s' loc=%d texUnit=%d",
                        name, texName.c_str(), loc, texUnit);
                    fallbackTexUnit++;
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::stageConstantBuffers(const ConstantBuffers &buffers)
    {
        // 主线程提取 UBO 名称和 GL 句柄到 POD 数组，避免 lambda 中访问引擎对象
        using UBOBinding = std::pair<String, GLuint>;
        TArray<UBOBinding> uboBindings;
        uboBindings.reserve(buffers.size());

        for (uint32_t i = 0; i < buffers.size(); ++i)
        {
            GL4ConstantBuffer *glCB = static_cast<GL4ConstantBuffer*>(buffers[i]->getRHIResource().get());
            uboBindings.push_back({buffers[i]->getName(), glCB->GLBuffer});
        }

        auto lambda = [this](TArray<UBOBinding> uboBindings)
        {
            TResult ret = T3D_OK;

            do
            {
                for (const auto &binding : uboBindings)
                {
                    mPendingUBOs[binding.first] = binding.second;
                }
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, uboBindings);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::bindPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "bindPixelBuffers: startSlot=%d bufferCount=%d",
            startSlot, (int)buffers.size());

        // 提取 GL 句柄和目标到 POD 数组，避免在 lambda 中访问引擎对象
        struct TexBinding { GLuint handle; GLenum target; };
        TArray<TexBinding> bindings;
        bindings.reserve(buffers.size());

        for (uint32_t i = 0; i < buffers.size(); ++i)
        {
            if (buffers[i] == nullptr)
            {
                T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "  pixelBuffer[%d]: NULL (skipped)", i);
                bindings.push_back({0, GL_TEXTURE_2D});
                continue;
            }

            GLuint texHandle = 0;
            GLenum texTarget = GL_TEXTURE_2D;

            switch (buffers[i]->getRHIResource()->getResourceType())
            {
            case RHIResource::ResourceType::kPixelBuffer1D:
                texHandle = static_cast<GL4PixelBuffer1D*>(buffers[i]->getRHIResource().get())->GLTexture;
                texTarget = GL_TEXTURE_1D;
                break;
            case RHIResource::ResourceType::kPixelBuffer2D:
                {
                    GL4PixelBuffer2D *glPB = static_cast<GL4PixelBuffer2D*>(buffers[i]->getRHIResource().get());
                    if (glPB->GLMSAACount > 1 && glPB->GLResolveTex != 0)
                    {
                        texHandle = glPB->GLResolveTex;
                        texTarget = GL_TEXTURE_2D;
                    }
                    else if (glPB->GLMSAACount > 1)
                    {
                        texHandle = glPB->GLTexture;
                        texTarget = GL_TEXTURE_2D_MULTISAMPLE;
                    }
                    else
                    {
                        texHandle = glPB->GLTexture;
                        texTarget = GL_TEXTURE_2D;
                    }
                }
                break;
            case RHIResource::ResourceType::kPixelBuffer3D:
                texHandle = static_cast<GL4PixelBuffer3D*>(buffers[i]->getRHIResource().get())->GLTexture;
                texTarget = GL_TEXTURE_3D;
                break;
            case RHIResource::ResourceType::kPixelBufferCubemap:
                texHandle = static_cast<GL4PixelBufferCubemap*>(buffers[i]->getRHIResource().get())->GLTexture;
                texTarget = GL_TEXTURE_CUBE_MAP;
                break;
            default:
                break;
            }

            T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "  pixelBuffer[%d]: texHandle=%u texTarget=0x%X -> GL_TEXTURE%d",
                i, texHandle, texTarget, startSlot + i);
            bindings.push_back({texHandle, texTarget});
        }

        auto lambda = [this](uint32_t startSlot, TArray<TexBinding> bindings)
        {
            TResult ret = T3D_OK;

            do
            {
                for (uint32_t i = 0; i < bindings.size(); ++i)
                {
                    glActiveTexture(GL_TEXTURE0 + startSlot + i);
                    glBindTexture(bindings[i].target, bindings[i].handle);
                }
                glActiveTexture(GL_TEXTURE0);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::bindPixelBuffers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, bindings);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::bindSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        // 提取 GL 句柄到 POD 数组
        TArray<GLuint> samplerHandles;
        samplerHandles.reserve(samplers.size());

        for (uint32_t i = 0; i < samplers.size(); ++i)
        {
            if (samplers[i] != nullptr)
            {
                GL4SamplerState *glSampler = static_cast<GL4SamplerState*>(samplers[i]->getRHIState().get());
                //T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "bindSamplers: slot=%u glSampler=%u", startSlot + i, glSampler->GLSampler);
                samplerHandles.push_back(glSampler->GLSampler);
            }
            else
            {
                //T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "bindSamplers: slot=%u sampler=NULL", startSlot + i);
                samplerHandles.push_back(0);
            }
        }

        auto lambda = [this](uint32_t startSlot, TArray<GLuint> samplerHandles)
        {
            TResult ret = T3D_OK;

            do
            {
                for (uint32_t i = 0; i < samplerHandles.size(); ++i)
                {
                    // handle==0 也要绑：sampler object 是全局状态，不显式解绑就会
                    // 把上一 pass 的采样参数（比如 comparison 模式）留给下一 pass。
                    glBindSampler(startSlot + i, samplerHandles[i]);
                }
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::bindSamplers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, samplerHandles);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::bindStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers)
    {
        if (!mCapabilities.supportsStructuredBuffer)
        {
            T3D_RHI_UNSUPPORTED(supportsStructuredBuffer);
        }

        TArray<GLuint> handles;
        handles.reserve(buffers.size());

        for (uint32_t i = 0; i < buffers.size(); ++i)
        {
            GLuint handle = 0;
            if (buffers[i] != nullptr && buffers[i]->getRHIResource() != nullptr)
            {
                handle = static_cast<GL4StructuredBuffer*>(buffers[i]->getRHIResource().get())->GLBuffer;
            }
            handles.push_back(handle);
        }

        auto lambda = [this](uint32_t startSlot, TArray<GLuint> handles)
        {
            for (uint32_t i = 0; i < handles.size(); ++i)
            {
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, startSlot + i, handles[i]);
            }
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::bindStructuredBuffers");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, handles);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::compileGLSLShader(GLenum shaderType, const String &source, GLuint &outHandle, const char *stageName)
    {
        TResult ret = T3D_OK;

        do
        {
            const char *src = source.c_str();
            GLint len = static_cast<GLint>(source.size());

            outHandle = glCreateShader(shaderType);
            glShaderSource(outHandle, 1, &src, &len);
            glCompileShader(outHandle);

            GLint compiled = 0;
            glGetShaderiv(outHandle, GL_COMPILE_STATUS, &compiled);
            if (!compiled)
            {
                GLint logLen = 0;
                glGetShaderiv(outHandle, GL_INFO_LOG_LENGTH, &logLen);
                if (logLen > 0)
                {
                    TArray<char> log(logLen + 1, 0);
                    glGetShaderInfoLog(outHandle, logLen, nullptr, log.data());
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "%s shader compile error: %s", stageName, log.data());
                }
                GL_SAFE_DELETE_SHADER(outHandle);
                ret = T3D_ERR_GL4_COMPILE_SHADER;
                break;
            }

            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::compileGLSLShader");
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::attachGraphicsShader(ShaderVariant *shader, ShaderVariant *&currentVariant)
    {
        ShaderVariant **slot = &currentVariant;

        if (shader == nullptr)
        {
            auto lambda = [this, slot]()
            {
                *slot = nullptr;
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

        GL4Shader *glShader = static_cast<GL4Shader*>(shader->getRHIShader());
        GLuint shaderHandle = glShader->GLShaderHandle;

        auto lambda = [this](GLuint shaderHandle, ShaderVariant *variant, ShaderVariant **slot)
        {
            TResult ret = T3D_OK;

            do
            {
                *slot = variant;

                if (mCurrentProgram == 0)
                {
                    mCurrentProgram = glCreateProgram();
                }
                glAttachShader(mCurrentProgram, shaderHandle);
                mProgramDirty = true;
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::attachGraphicsShader");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, shaderHandle, shader, slot);
    }

    //--------------------------------------------------------------------------

    GLuint GL4Context::getGLBufferHandle(RenderBuffer *buffer) const
    {
        if (buffer == nullptr || buffer->getRHIResource() == nullptr)
        {
            return 0;
        }

        switch (buffer->getRHIResource()->getResourceType())
        {
        case RHIResource::ResourceType::kVertexBuffer:
            return static_cast<GL4VertexBuffer*>(buffer->getRHIResource().get())->GLBuffer;
        case RHIResource::ResourceType::kIndexBuffer:
            return static_cast<GL4IndexBuffer*>(buffer->getRHIResource().get())->GLBuffer;
        case RHIResource::ResourceType::kConstantBuffer:
            return static_cast<GL4ConstantBuffer*>(buffer->getRHIResource().get())->GLBuffer;
        case RHIResource::ResourceType::kStructuredBuffer:
            return static_cast<GL4StructuredBuffer*>(buffer->getRHIResource().get())->GLBuffer;
        default:
            break;
        }

        return 0;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::validateIndirectArgs(RenderBuffer *argsBuffer, size_t argsOffset, size_t argsSize)
    {
        if (argsBuffer == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Indirect args buffer is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        if ((argsBuffer->getGPUAccess() & kGPUIndirectArgs) == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Indirect args buffer was not created with kGPUIndirectArgs !");
            return T3D_ERR_INVALID_PARAM;
        }

        if ((argsOffset % 4) != 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Indirect args offset [%zu] must be a multiple of 4 !", argsOffset);
            return T3D_ERR_INVALID_PARAM;
        }

        if (argsOffset + argsSize > argsBuffer->getGPUSizeInBytes())
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Indirect args offset [%zu] + size [%zu] exceeds buffer size [%zu] !",
                argsOffset, argsSize, argsBuffer->getGPUSizeInBytes());
            return T3D_ERR_OUT_OF_BOUND;
        }

        if (argsBuffer->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Indirect args buffer has no RHI resource, is it loaded ?");
            return T3D_ERR_INVALID_POINTER;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::resolveBlitEndpoint(Texture *tex, bool asSource, BlitEndpoint &out)
    {
        if (tex == nullptr || tex->getPixelBuffer() == nullptr
            || tex->getPixelBuffer()->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "blit : texture has no RHI resource !");
            return T3D_ERR_INVALID_POINTER;
        }

        const TEXTURE_TYPE type = tex->getTextureType();
        if (type != TEXTURE_TYPE::TT_2D && type != TEXTURE_TYPE::TT_RENDER_TEXTURE)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "blit : only 2D / render textures are supported !");
            return T3D_ERR_GL4_UNSUPPORTED_OPERATION;
        }

        Texture2D *tex2D = static_cast<Texture2D*>(tex);
        PixelBuffer2D *pb = static_cast<PixelBuffer2D*>(tex2D->getPixelBuffer());
        GL4PixelBuffer2D *glPB = static_cast<GL4PixelBuffer2D*>(pb->getRHIResource().get());

        out.texture = glPB->GLTexture;
        out.fbo = glPB->GLFBO;
        out.resolveFbo = glPB->GLResolveFBO;
        out.resolveTex = glPB->GLResolveTex;
        out.width = tex2D->getWidth();
        out.height = tex2D->getHeight();
        out.sampleCount = glPB->GLMSAACount;
        out.isWindow = false;
        out.isDepth = (pb->getDescriptor().format >= PixelFormat::E_PF_D24_UNORM_S8_UINT
            && pb->getDescriptor().format <= PixelFormat::E_PF_D16_UNORM);
        out.needsScratchFbo = (glPB->GLFBO == 0);

        if (asSource && glPB->GLMSAACount > 1 && glPB->GLResolveFBO != 0)
        {
            // doBlit 会先 resolve，再以 resolve FBO 为读源
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::resolveBlitEndpoint(RenderTarget *rt, bool asSource, BlitEndpoint &out)
    {
        if (rt == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        if (rt->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            out.fbo = 0;
            out.texture = 0;
            out.width = rt->getRenderWindow()->getDescriptor().Width;
            out.height = rt->getRenderWindow()->getDescriptor().Height;
            out.sampleCount = 1;
            out.isWindow = true;
            out.isDepth = false;
            out.needsScratchFbo = false;
            return T3D_OK;
        }

        if (rt->getNumOfRenderTextures() > 0)
        {
            return resolveBlitEndpoint(rt->getRenderTexture().get(), asSource, out);
        }

        if (rt->getDepthStencil() != nullptr)
        {
            return resolveBlitEndpoint(rt->getDepthStencil().get(), asSource, out);
        }

        T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "blit : render target has no color or depth attachment !");
        return T3D_ERR_INVALID_PARAM;
    }

    //--------------------------------------------------------------------------

    void GL4Context::unbindTextureUnits()
    {
        for (int i = 0; i < 16; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_1D, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glBindTexture(GL_TEXTURE_3D, 0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glBindSampler(i, 0);
        }
        glActiveTexture(GL_TEXTURE0);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::doBlit(const BlitEndpoint &src, const BlitEndpoint &dst,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
    {
        TResult ret = T3D_OK;

        do
        {
            // 池化临时 RT 上一帧可能还绑在某个 texture unit 上，这里先摘干净，
            // 避免同一张纹理既挂在 unit 上又当 FBO 附件写。
            unbindTextureUnits();

            GLint prevFBO = 0;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

            GLuint readFBO = src.fbo;
            GLuint drawFBO = dst.fbo;

            if (src.needsScratchFbo)
            {
                if (mScratchReadFBO == 0)
                {
                    glGenFramebuffers(1, &mScratchReadFBO);
                }
                glBindFramebuffer(GL_FRAMEBUFFER, mScratchReadFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src.texture, 0);
                readFBO = mScratchReadFBO;
            }

            if (dst.needsScratchFbo)
            {
                if (mScratchDrawFBO == 0)
                {
                    glGenFramebuffers(1, &mScratchDrawFBO);
                }
                glBindFramebuffer(GL_FRAMEBUFFER, mScratchDrawFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst.texture, 0);
                drawFBO = mScratchDrawFBO;
            }

            const uint32_t copyW = (size == Vector3::ZERO)
                ? (src.width - std::min(static_cast<uint32_t>(srcOffset.x()), src.width))
                : static_cast<uint32_t>(size.x());
            const uint32_t copyH = (size == Vector3::ZERO)
                ? (src.height - std::min(static_cast<uint32_t>(srcOffset.y()), src.height))
                : static_cast<uint32_t>(size.y());

            if (copyW == 0 || copyH == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "blit : copy size is zero !");
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            GLint srcX0 = static_cast<GLint>(srcOffset.x());
            GLint srcY0 = static_cast<GLint>(srcOffset.y());
            GLint srcX1 = srcX0 + static_cast<GLint>(copyW);
            GLint srcY1 = srcY0 + static_cast<GLint>(copyH);

            GLint dstX0 = static_cast<GLint>(dstOffset.x());
            GLint dstY0 = static_cast<GLint>(dstOffset.y());
            GLint dstX1 = dstX0 + static_cast<GLint>(copyW);
            GLint dstY1 = dstY0 + static_cast<GLint>(copyH);

            if (dst.isWindow && mProjectionFlipped)
            {
                GLint tmp = srcY0;
                srcY0 = srcY1;
                srcY1 = tmp;
            }

            GLbitfield mask = src.isDepth || dst.isDepth
                ? (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)
                : GL_COLOR_BUFFER_BIT;

            auto bindRead = [](GLuint fbo, bool isWindow, bool isDepth)
            {
                glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
                if (isDepth)
                {
                    return;
                }
                glReadBuffer(isWindow || fbo == 0 ? GL_BACK : GL_COLOR_ATTACHMENT0);
            };
            auto bindDraw = [](GLuint fbo, bool isWindow, bool isDepth)
            {
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
                if (isDepth)
                {
                    return;
                }
                glDrawBuffer(isWindow || fbo == 0 ? GL_BACK : GL_COLOR_ATTACHMENT0);
            };

            // MSAA 源直接 resolve 到目标，不要先写入 GLResolveTex 再拷一次。
            // 中间 resolve 贴图常被 bindPixelBuffers 绑在 unit 上，再当 FBO 写就是 feedback。
            if (src.sampleCount > 1 && src.fbo != 0)
            {
                readFBO = src.fbo;
            }

            bindRead(readFBO, src.isWindow, src.isDepth);
            bindDraw(drawFBO, dst.isWindow, dst.isDepth);
            glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1,
                dstX0, dstY0, dstX1, dstY1,
                mask, GL_NEAREST);

            glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::doBlit");
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    ReadbackHandle GL4Context::allocReadbackRequest(RenderBuffer *src, bool isTexture, ReadbackRequest *&outRequest)
    {
        outRequest = nullptr;

        if (src == nullptr || src->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "readback : source buffer is null or has no RHI resource !");
            return ReadbackHandle::invalid();
        }

        if ((src->getCPUAccessMode() & kCPURead) != kCPURead)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "readback : source was not created with kCPURead, "
                "readback is rejected. Declare kCPURead at creation time !");
            return ReadbackHandle::invalid();
        }

        const RHIResource::ResourceType type = src->getRHIResource()->getResourceType();
        const bool isTextureResource = (type == RHIResource::ResourceType::kPixelBuffer1D
            || type == RHIResource::ResourceType::kPixelBuffer2D
            || type == RHIResource::ResourceType::kPixelBuffer3D
            || type == RHIResource::ResourceType::kPixelBufferCubemap);
        const bool isLinearResource = (type == RHIResource::ResourceType::kVertexBuffer
            || type == RHIResource::ResourceType::kIndexBuffer
            || type == RHIResource::ResourceType::kConstantBuffer
            || type == RHIResource::ResourceType::kStructuredBuffer);

        if (isTexture && !isTextureResource)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : resource type [%d] is not a texture, use the buffer overload !", (int32_t)type);
            return ReadbackHandle::invalid();
        }

        if (!isTexture && !isLinearResource)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : resource type [%d] is not a linear buffer, use the texture overload !", (int32_t)type);
            return ReadbackHandle::invalid();
        }

        if (mNextReadbackIndex >= 0xFFFFFFFFu)
        {
            mNextReadbackIndex = 0;
        }

        ReadbackHandle handle;
        handle.index = mNextReadbackIndex++;
        handle.generation = mReadbackGeneration++;

        ReadbackRequest &request = mPendingReadbacks[handle.index];
        request = ReadbackRequest{};
        request.Handle = handle;
        request.Src = RenderBufferPtr(src);
        request.IsTexture = isTexture;

        outRequest = &request;
        return handle;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::finishReadback(ReadbackHandle handle, Buffer &dst)
    {
        auto itr = mPendingReadbacks.find(handle.index);
        if (!handle.isValid() || itr == mPendingReadbacks.end()
            || itr->second.Handle.generation != handle.generation)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "unmap : invalid or already consumed readback handle !");
            return T3D_ERR_INVALID_PARAM;
        }

        T3D_AGENT.syncRHIThread();

        ReadbackRequest &request = itr->second;
        TResult ret = request.CopyResult;

        if (T3D_OK == ret && !request.CopyRecorded)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "unmap : copy command has not been executed. "
                "map must be called inside onRender, unmap inside onPostRender !");
            ret = T3D_ERR_FAIL;
        }

        if (T3D_OK == ret)
        {
            auto lambda = [this](ReadbackRequest *request, Buffer *dst) -> TResult
            {
                void *mapped = glMapNamedBuffer(request->Staging, GL_READ_ONLY);
                if (mapped == nullptr)
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "unmap : failed to map staging buffer !");
                    request->CopyResult = T3D_ERR_GL4_MAP_BUFFER;
                    return request->CopyResult;
                }

                memcpy(dst->Data, mapped, request->TotalBytes);
                glUnmapNamedBuffer(request->Staging);
                request->CopyResult = T3D_OK;
                return T3D_OK;
            };

            dst.release();
            dst.DataSize = request.TotalBytes;
            dst.Data = T3D_POD_NEW_ARRAY(uint8_t, request.TotalBytes);

            ENQUEUE_UNIQUE_COMMAND(lambda, &request, &dst);
            T3D_AGENT.syncRHIThread();
            ret = request.CopyResult;
        }

        GL_SAFE_DELETE_BUFFER(request.Staging);
        mPendingReadbacks.erase(itr);

        if (T3D_FAILED(ret))
        {
            dst.release();
        }

        return ret;
    }

    //--------------------------------------------------------------------------
}
