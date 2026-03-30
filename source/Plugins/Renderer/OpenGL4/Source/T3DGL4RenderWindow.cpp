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

            // 设置像素格式
            PIXELFORMATDESCRIPTOR pfd = {};
            pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.cColorBits = 32;
            pfd.cDepthBits = 24;
            pfd.cStencilBits = 8;
            pfd.iLayerType = PFD_MAIN_PLANE;

            int pixelFormat = ChoosePixelFormat(GLDeviceContext, &pfd);
            if (pixelFormat == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "ChoosePixelFormat failed !");
                ret = false;
                break;
            }

            if (!SetPixelFormat(GLDeviceContext, pixelFormat, &pfd))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "SetPixelFormat failed !");
                ret = false;
                break;
            }

            // 创建 GL 上下文
            GLContext = wglCreateContext(GLDeviceContext);
            if (GLContext == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "wglCreateContext failed !");
                ret = false;
                break;
            }

            // 与 dummy 上下文共享 GL 对象（纹理、FBO、UBO 等）
            HGLRC dummyRC = GL4_CONTEXT->getDummyGLRC();
            if (dummyRC != nullptr)
            {
                if (!wglShareLists(dummyRC, GLContext))
                {
                    T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "wglShareLists failed, GL resources may not be shared !");
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

            XVisualInfo *visualInfo = glXChooseVisual(GLDisplay, screen, attribs);
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
        mWidth = w;
        mHeight = h;
        glViewport(0, 0, (GLsizei)w, (GLsizei)h);
        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4RenderWindow::resize");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4RenderWindow::clear(const ColorRGB &clrFill, uint32_t clearFlags, Real depth, uint32_t stencil)
    {
        TResult ret = T3D_OK;

        GLbitfield mask = 0;

        if (clearFlags & 0x01) // Color
        {
            glClearColor(clrFill.red(), clrFill.green(), clrFill.blue(), 1.0f);
            mask |= GL_COLOR_BUFFER_BIT;
        }

        if (clearFlags & 0x02) // Depth
        {
            glClearDepth((GLdouble)depth);
            mask |= GL_DEPTH_BUFFER_BIT;
        }

        if (clearFlags & 0x04) // Stencil
        {
            glClearStencil((GLint)stencil);
            mask |= GL_STENCIL_BUFFER_BIT;
        }

        if (mask != 0)
        {
            glClear(mask);
        }

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4RenderWindow::clear");

        return ret;
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
