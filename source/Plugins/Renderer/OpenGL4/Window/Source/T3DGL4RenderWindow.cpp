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


#include "T3DGL4RenderWindow.h"
#include "T3DGL4Error.h"
#include "T3DGL4Renderer.h"
#include "T3DGL4Context.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GL4RenderWindowPtr GL4RenderWindow::create(RenderWindow *renderWindow)
    {
        GL4RenderWindowPtr window = T3D_NEW GL4RenderWindow();
        if (window && !window->init(renderWindow))
        {
            window = nullptr;
        }
        return window;
    }

    //--------------------------------------------------------------------------

    GL4RenderWindow::GL4RenderWindow()
    {
    }

    //--------------------------------------------------------------------------

    GL4RenderWindow::~GL4RenderWindow()
    {
        GL_SAFE_DELETE_RBO(GLDepthRBO);

#if defined(T3D_OS_WINDOWS)
        if (GLContext != nullptr)
        {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(GLContext);
            GLContext = nullptr;
        }
        GLDeviceContext = nullptr;
#elif defined(T3D_OS_LINUX)
        if (GLContext != nullptr)
        {
            glXMakeCurrent(GLDisplay, None, nullptr);
            glXDestroyContext(GLDisplay, GLContext);
            GLContext = nullptr;
        }
        GLDisplay = nullptr;
        GLWindow = 0;
#endif
    }

    //--------------------------------------------------------------------------

    bool GL4RenderWindow::init(RenderWindow *renderWindow)
    {
        bool ret = true;

        do
        {
            SysWMInfo info;
            renderWindow->getSystemInfo(info);

            const RenderWindowDesc &desc = renderWindow->getDescriptor();
            mWidth = desc.Width;
            mHeight = desc.Height;

#if defined(T3D_OS_WINDOWS)
            GLDeviceContext = (HDC)info.hDC;
            if (GLDeviceContext == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Invalid HDC from SysWMInfo !");
                ret = false;
                break;
            }

            {
                int pixelFormat = 0;
                bool msaaEnabled = false;

                // 尝试使用 wglChoosePixelFormatARB 获取 MSAA 像素格式
                auto wglChoosePF = GL4_CONTEXT->getWglChoosePixelFormatARB();
                uint32_t requestedMSAA = desc.MSAA.Count;

                if (wglChoosePF != nullptr && requestedMSAA > 1)
                {
                    int attribs[] = {
                        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                        WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
                        WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
                        WGL_COLOR_BITS_ARB,     32,
                        WGL_DEPTH_BITS_ARB,     24,
                        WGL_STENCIL_BITS_ARB,   8,
                        WGL_SAMPLE_BUFFERS_ARB, 1,
                        WGL_SAMPLES_ARB,        (int)requestedMSAA,
                        0
                    };

                    UINT numFormats = 0;
                    if (wglChoosePF(GLDeviceContext, attribs, nullptr, 1, &pixelFormat, &numFormats)
                        && numFormats > 0 && pixelFormat != 0)
                    {
                        msaaEnabled = true;
                        mMSAACount = requestedMSAA;
                        T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "MSAA pixel format selected: %dx MSAA (format=%d)", requestedMSAA, pixelFormat);
                    }
                    else
                    {
                        T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "wglChoosePixelFormatARB failed for %dx MSAA, falling back to non-MSAA.", requestedMSAA);
                        pixelFormat = 0;
                    }
                }

                // 回退到传统 ChoosePixelFormat（无 MSAA）
                if (pixelFormat == 0)
                {
                    PIXELFORMATDESCRIPTOR pfd = {};
                    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
                    pfd.nVersion = 1;
                    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
                    pfd.iPixelType = PFD_TYPE_RGBA;
                    pfd.cColorBits = 32;
                    pfd.cDepthBits = 24;
                    pfd.cStencilBits = 8;
                    pfd.iLayerType = PFD_MAIN_PLANE;

                    pixelFormat = ChoosePixelFormat(GLDeviceContext, &pfd);
                    if (pixelFormat == 0)
                    {
                        T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "ChoosePixelFormat failed !");
                        ret = false;
                        break;
                    }
                    mMSAACount = 1;
                }

                PIXELFORMATDESCRIPTOR pfd = {};
                DescribePixelFormat(GLDeviceContext, pixelFormat, sizeof(pfd), &pfd);
                if (!SetPixelFormat(GLDeviceContext, pixelFormat, &pfd))
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "SetPixelFormat failed !");
                    ret = false;
                    break;
                }
            }

            // 创建 GL 上下文
            HGLRC dummyRC = GL4_CONTEXT->getDummyGLRC();
            auto wglCreateCtxAttribs = GL4_CONTEXT->getWglCreateContextAttribsARB();

            if (wglCreateCtxAttribs != nullptr)
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

                GLContext = wglCreateCtxAttribs(GLDeviceContext, dummyRC, attribs);
                if (GLContext != nullptr)
                {
                    T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "GL context created via wglCreateContextAttribsARB (Core Profile 4.5, RenderDoc compatible).");
                }
                else
                {
                    T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "wglCreateContextAttribsARB failed, falling back to wglCreateContext.");
                }
            }

            if (GLContext == nullptr)
            {
                GLContext = wglCreateContext(GLDeviceContext);
                if (GLContext == nullptr)
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "wglCreateContext failed !");
                    ret = false;
                    break;
                }

                // 回退路径需要 wglShareLists 来共享 GL 对象
                if (dummyRC != nullptr)
                {
                    if (!wglShareLists(dummyRC, GLContext))
                    {
                        T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "wglShareLists failed, GL resources may not be shared !");
                    }
                }
            }

            if (!wglMakeCurrent(GLDeviceContext, GLContext))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "wglMakeCurrent failed !");
                ret = false;
                break;
            }

#elif defined(T3D_OS_LINUX)
            GLDisplay = (Display *)info.display;
            GLWindow = (::Window)info.window;

            if (GLDisplay == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Invalid Display from SysWMInfo !");
                ret = false;
                break;
            }

            int screen = DefaultScreen(GLDisplay);

            // 先尝试带 MSAA 的属性列表
            uint32_t requestedMSAA = desc.MSAA.Count;
            int attribsMSAA[] = {
                GLX_RGBA,
                GLX_RED_SIZE, 8,
                GLX_GREEN_SIZE, 8,
                GLX_BLUE_SIZE, 8,
                GLX_ALPHA_SIZE, 8,
                GLX_DEPTH_SIZE, 24,
                GLX_STENCIL_SIZE, 8,
                GLX_DOUBLEBUFFER,
                GLX_SAMPLE_BUFFERS, 1,
                GLX_SAMPLES, (int)requestedMSAA,
                None
            };

            int attribsFallback[] = {
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

            XVisualInfo *visualInfo = nullptr;

            if (requestedMSAA > 1)
            {
                visualInfo = glXChooseVisual(GLDisplay, screen, attribsMSAA);
                if (visualInfo != nullptr)
                {
                    mMSAACount = requestedMSAA;
                    T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "GLX MSAA visual selected: %dx MSAA", requestedMSAA);
                }
                else
                {
                    T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "glXChooseVisual failed for %dx MSAA, falling back to non-MSAA.", requestedMSAA);
                }
            }

            if (visualInfo == nullptr)
            {
                visualInfo = glXChooseVisual(GLDisplay, screen, attribsFallback);
                mMSAACount = 1;
            }
            if (visualInfo == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "glXChooseVisual failed !");
                ret = false;
                break;
            }

            // 与 dummy 上下文共享（通过第三个参数传入）
            GLXContext dummyCtx = GL4_CONTEXT->getDummyGLXContext();
            GLContext = glXCreateContext(GLDisplay, visualInfo, dummyCtx, GL_TRUE);
            XFree(visualInfo);

            if (GLContext == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "glXCreateContext failed !");
                ret = false;
                break;
            }

            if (!glXMakeCurrent(GLDisplay, GLWindow, GLContext))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "glXMakeCurrent failed !");
                ret = false;
                break;
            }
#endif

            // GLAD 只加载一次（init dummy context 时已加载）
            if (!GL4_CONTEXT->isGLADLoaded())
            {
#if defined(T3D_OS_WINDOWS)
                auto combinedLoader = [](const char *name) -> void*
                {
                    void *proc = (void *)wglGetProcAddress(name);
                    if (proc == nullptr
                        || proc == (void *)0x1 || proc == (void *)0x2
                        || proc == (void *)0x3 || proc == (void *)-1)
                    {
                        static HMODULE hGL = LoadLibrary(TEXT("opengl32.dll"));
                        proc = (void *)GetProcAddress(hGL, name);
                    }
                    return proc;
                };
                if (!gladLoadGLLoader((GLADloadproc)+combinedLoader))
#elif defined(T3D_OS_LINUX)
                if (!gladLoadGLLoader((GLADloadproc)glXGetProcAddress))
#endif
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "gladLoadGLLoader failed !");
                    ret = false;
                    break;
                }
            }

            T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "GL4RenderWindow created: %ux%u", mWidth, mHeight);

            // 默认帧缓冲
            GLDefaultFBO = 0;

            // 初始化默认 GL 状态
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);

            // 启用多重采样抗锯齿
            if (mMSAACount > 1)
            {
                glEnable(GL_MULTISAMPLE);
                T3D_LOG_INFO(LOG_TAG_GL4RENDERER, "GL_MULTISAMPLE enabled (%dx)", mMSAACount);
            }

            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4RenderWindow::init");

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4RenderWindow::swapBuffers()
    {
        return GL4_CONTEXT->swapBackBuffer(this);
    }

    //--------------------------------------------------------------------------

    TResult GL4RenderWindow::resize(uint32_t w, uint32_t h)
    {
        return GL4_CONTEXT->resizeRenderWindow(this, w, h);
    }

    //--------------------------------------------------------------------------

    void *GL4RenderWindow::getNativeObject() const
    {
#if defined(T3D_OS_WINDOWS)
        return (void *)GLContext;
#elif defined(T3D_OS_LINUX)
        return (void *)GLContext;
#else
        return nullptr;
#endif
    }

    //--------------------------------------------------------------------------
}
