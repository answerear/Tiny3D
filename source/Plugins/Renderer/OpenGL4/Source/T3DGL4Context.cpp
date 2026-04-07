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
        GL_SAFE_DELETE_VAO(mCurrentVAO);

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
        } while (false);

        return ret;
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
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::resizeRenderWindow(GL4RenderWindow *rw, uint32_t w, uint32_t h)
    {
        rw->mWidth = w;
        rw->mHeight = h;
        glViewport(0, 0, (GLsizei)w, (GLsizei)h);
        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::resizeRenderWindow");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)
    {
        mViewMatrix = viewMat;
        mProjMatrix = projMat;
        mProjViewMatrix = mProjMatrix * mViewMatrix;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIRenderTargetPtr GL4Context::createRenderWindow(RenderWindow *renderWindow)
    {
        GL4RenderWindowPtr glRenderWindow = GL4RenderWindow::create(renderWindow);
        return glRenderWindow;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr GL4Context::createRenderTexture(PixelBuffer2D *buffer)
    {
        GL4PixelBuffer2DPtr glPixelBuffer = GL4PixelBuffer2D::create();

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

            if (isColorRT)
            {
                // 创建颜色纹理
                glGenTextures(1, &glPixelBuffer->GLTexture);
                glBindTexture(GL_TEXTURE_2D, glPixelBuffer->GLTexture);
                glTexImage2D(GL_TEXTURE_2D, 0,
                    GL4Mapping::getInternalFormat(desc.format),
                    desc.width, desc.height, 0,
                    GL4Mapping::get(desc.format),
                    GL4Mapping::getPixelType(desc.format),
                    nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);

                // 创建 FBO
                glGenFramebuffers(1, &glPixelBuffer->GLFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, glPixelBuffer->GLFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glPixelBuffer->GLTexture, 0);

                // 创建深度 RBO
                glGenRenderbuffers(1, &glPixelBuffer->GLDepthRBO);
                glBindRenderbuffer(GL_RENDERBUFFER, glPixelBuffer->GLDepthRBO);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, desc.width, desc.height);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glPixelBuffer->GLDepthRBO);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Color render texture FBO is not complete !");
                    ret = T3D_ERR_GL4_CREATE_FBO;
                }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            else
            {
                // 创建深度纹理
                GLenum internalFormat = GL4Mapping::getInternalFormat(desc.format);
                GLenum texFormat = GL_DEPTH_COMPONENT;
                GLenum texType = GL4Mapping::getPixelType(desc.format);

                if (desc.format == PixelFormat::E_PF_D24_UNORM_S8_UINT
                    || desc.format == PixelFormat::E_PF_D32_FLOAT_S8X24_UINT)
                {
                    texFormat = GL_DEPTH_STENCIL;
                }

                glGenTextures(1, &glPixelBuffer->GLTexture);
                glBindTexture(GL_TEXTURE_2D, glPixelBuffer->GLTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
                    desc.width, desc.height, 0,
                    texFormat, texType, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glBindTexture(GL_TEXTURE_2D, 0);

                // 创建 FBO
                glGenFramebuffers(1, &glPixelBuffer->GLFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, glPixelBuffer->GLFBO);

                if (desc.format == PixelFormat::E_PF_D24_UNORM_S8_UINT
                    || desc.format == PixelFormat::E_PF_D32_FLOAT_S8X24_UINT)
                {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, glPixelBuffer->GLTexture, 0);
                }
                else
                {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, glPixelBuffer->GLTexture, 0);
                }

                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Depth render texture FBO is not complete !");
                    ret = T3D_ERR_GL4_CREATE_FBO;
                }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createRenderTexture");
        } while (false);

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

        // 入口处清空残留 GL error，判断错误是否来自外部
        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget: ENTRY (stale error from outside)");

        switch (renderTarget->getType())
        {
        case RenderTarget::Type::E_RT_WINDOW:
            {
                // 渲染到默认帧缓冲
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            break;
        case RenderTarget::Type::E_RT_TEXTURE:
            {
                if (renderTarget->getNumOfRenderTextures() > 0)
                {
                    // 获取第一个颜色纹理的 FBO
                    GL4PixelBuffer2D *glPB = static_cast<GL4PixelBuffer2D*>(
                        renderTarget->getRenderTexture()->getPixelBuffer()->getRHIResource().get());
                    if (glPB != nullptr && glPB->GLFBO != 0)
                    {
                        glBindFramebuffer(GL_FRAMEBUFFER, glPB->GLFBO);
                        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget: glBindFramebuffer(color FBO)");

                        // 如果有独立深度纹理，attach 到当前 FBO
                        if (renderTarget->getDepthStencil() != nullptr)
                        {
                            // 先 detach 颜色 FBO 自带的深度 RBO（如果有的话），
                            // 避免 RBO 和纹理同时 attach 到同一挂载点导致 GL_INVALID_OPERATION
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
                                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                    GL_TEXTURE_2D, glDS->GLTexture, 0);
                                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget: glFramebufferTexture2D(depth texture)");
                            }
                        }

                        // 设置多个颜色 attachment
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
                    // 只有深度纹理 (shadow map path)
                    // 使用运行时 FBO 并动态 attach 深度纹理
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

        if (T3D_SUCCEEDED(ret))
        {
            mCurrentRenderTarget = renderTarget;
        }

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setRenderTarget");
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::resetRenderTarget()
    {
        mCurrentRenderTarget = nullptr;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
            T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "GL4Context::setViewport: no render target");
            return T3D_OK;
        }

        GLint x = static_cast<GLint>(viewport.Left * width);
        GLint y = static_cast<GLint>(viewport.Top * height);
        GLsizei w = static_cast<GLsizei>(viewport.Width * width);
        GLsizei h = static_cast<GLsizei>(viewport.Height * height);
        glViewport(x, y, w, h);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setViewport");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::clearColor(const ColorRGB &color)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        glClearColor(color.red(), color.green(), color.blue(), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::clearColor");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::clearDepth(Real depth)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        glClearDepth((GLdouble)depth);
        glClear(GL_DEPTH_BUFFER_BIT);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::clearDepth");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::clearDepthStencil(Real depth, uint32_t stencil)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        glClearDepth((GLdouble)depth);
        glClearStencil((GLint)stencil);
        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::clearDepthStencil");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIBlendStatePtr GL4Context::createBlendState(BlendState *state)
    {
        GL4BlendStatePtr glState = GL4BlendState::create();

        const BlendDesc &desc = state->getStateDesc();
        const auto &rt0 = desc.RenderTargetStates[0];
        glState->data.enabled = rt0.BlendEnable;
        glState->data.srcRGB = GL4Mapping::get(rt0.SrcBlend);
        glState->data.dstRGB = GL4Mapping::get(rt0.DestBlend);
        glState->data.opRGB = GL4Mapping::get(rt0.BlendOp);
        glState->data.srcAlpha = GL4Mapping::get(rt0.SrcBlendAlpha);
        glState->data.dstAlpha = GL4Mapping::get(rt0.DstBlendAlpha);
        glState->data.opAlpha = GL4Mapping::get(rt0.BlendOpAlpha);
        glState->data.colorMask[0] = (rt0.ColorMask & kWriteMaskRed) ? GL_TRUE : GL_FALSE;
        glState->data.colorMask[1] = (rt0.ColorMask & kWriteMaskGreen) ? GL_TRUE : GL_FALSE;
        glState->data.colorMask[2] = (rt0.ColorMask & kWriteMaskBlue) ? GL_TRUE : GL_FALSE;
        glState->data.colorMask[3] = (rt0.ColorMask & kWriteMaskAlpha) ? GL_TRUE : GL_FALSE;

        return glState;
    }

    //--------------------------------------------------------------------------

    RHIDepthStencilStatePtr GL4Context::createDepthStencilState(DepthStencilState *state)
    {
        GL4DepthStencilStatePtr glState = GL4DepthStencilState::create();

        const DepthStencilDesc &desc = state->getStateDesc();
        glState->data.depthTestEnabled = desc.DepthTestEnable;
        glState->data.depthWriteEnabled = desc.DepthWriteEnable;
        glState->data.depthFunc = GL4Mapping::get(desc.DepthFunc);
        glState->data.stencilEnabled = desc.StencilEnable;
        glState->data.stencilReadMask = desc.StencilReadMask;
        glState->data.stencilWriteMask = desc.StencilWriteMask;
        glState->data.frontStencilFail = GL4Mapping::get(desc.FrontFace.StencilFailOp);
        glState->data.frontDepthFail = GL4Mapping::get(desc.FrontFace.StencilDepthFailOp);
        glState->data.frontStencilPass = GL4Mapping::get(desc.FrontFace.StencilPassOp);
        glState->data.frontStencilFunc = GL4Mapping::get(desc.FrontFace.StencilFunc);
        glState->data.backStencilFail = GL4Mapping::get(desc.BackFace.StencilFailOp);
        glState->data.backDepthFail = GL4Mapping::get(desc.BackFace.StencilDepthFailOp);
        glState->data.backStencilPass = GL4Mapping::get(desc.BackFace.StencilPassOp);
        glState->data.backStencilFunc = GL4Mapping::get(desc.BackFace.StencilFunc);

        return glState;
    }

    //--------------------------------------------------------------------------

    RHIRasterizerStatePtr GL4Context::createRasterizerState(RasterizerState *state)
    {
        GL4RasterizerStatePtr glState = GL4RasterizerState::create();

        const RasterizerDesc &desc = state->getStateDesc();
        glState->data.fillMode = GL4Mapping::get(desc.FillMode);
        glState->data.cullMode = GL4Mapping::get(desc.CullMode);
        glState->data.cullEnabled = (desc.CullMode != CullingMode::kNone);
        glState->data.frontCCW = desc.FrontAnticlockwise;
        glState->data.scissorEnabled = desc.ScissorEnable;
        glState->data.depthClipEnabled = desc.DepthClipEnable;

        return glState;
    }

    //--------------------------------------------------------------------------

    RHISamplerStatePtr GL4Context::createSamplerState(SamplerState *state)
    {
        GL4SamplerStatePtr glState = GL4SamplerState::create();

        const SamplerDesc &desc = state->getStateDesc();

        glGenSamplers(1, &glState->GLSampler);

        glSamplerParameteri(glState->GLSampler, GL_TEXTURE_WRAP_S, GL4Mapping::get(desc.AddressU));
        glSamplerParameteri(glState->GLSampler, GL_TEXTURE_WRAP_T, GL4Mapping::get(desc.AddressV));
        glSamplerParameteri(glState->GLSampler, GL_TEXTURE_WRAP_R, GL4Mapping::get(desc.AddressW));
        glSamplerParameteri(glState->GLSampler, GL_TEXTURE_MIN_FILTER, GL4Mapping::getMinFilter(desc.MinFilter, desc.MipFilter));
        glSamplerParameteri(glState->GLSampler, GL_TEXTURE_MAG_FILTER, GL4Mapping::getMagFilter(desc.MagFilter));
        glSamplerParameterf(glState->GLSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLfloat>(desc.MaxAnisotropy));
        glSamplerParameterf(glState->GLSampler, GL_TEXTURE_LOD_BIAS, desc.MipLODBias);
        glSamplerParameterf(glState->GLSampler, GL_TEXTURE_MIN_LOD, desc.MinLOD);
        glSamplerParameterf(glState->GLSampler, GL_TEXTURE_MAX_LOD, desc.MaxLOD);

        GLfloat borderColor[4] = {
            desc.BorderColor.blue(), desc.BorderColor.green(),
            desc.BorderColor.red(), desc.BorderColor.alpha()
        };
        glSamplerParameterfv(glState->GLSampler, GL_TEXTURE_BORDER_COLOR, borderColor);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createSamplerState");

        return glState;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setBlendState(BlendState *state)
    {
        GL4BlendState *glState = static_cast<GL4BlendState*>(state->getRHIState().get());
        const auto &d = glState->data;

        if (d.enabled)
        {
            glEnable(GL_BLEND);
            glBlendFuncSeparate(d.srcRGB, d.dstRGB, d.srcAlpha, d.dstAlpha);
            glBlendEquationSeparate(d.opRGB, d.opAlpha);
        }
        else
        {
            glDisable(GL_BLEND);
        }

        glColorMask(d.colorMask[0], d.colorMask[1], d.colorMask[2], d.colorMask[3]);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setBlendState");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setDepthStencilState(DepthStencilState *state)
    {
        GL4DepthStencilState *glState = static_cast<GL4DepthStencilState*>(state->getRHIState().get());
        const auto &d = glState->data;

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
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setRasterizerState(RasterizerState *state)
    {
        GL4RasterizerState *glState = static_cast<GL4RasterizerState*>(state->getRHIState().get());
        const auto &d = glState->data;

        glPolygonMode(GL_FRONT_AND_BACK, d.fillMode);

        if (d.cullEnabled)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(d.cullMode);
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

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setRasterizerState");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr GL4Context::createVertexDeclaration(VertexDeclaration *decl)
    {
        GL4VertexDeclarationPtr glDecl = GL4VertexDeclaration::create();

        glGenVertexArrays(1, &glDecl->GLVAO);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createVertexDeclaration");
        return glDecl;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setVertexDeclaration(VertexDeclaration *decl)
    {
        GL4VertexDeclaration *glDecl = static_cast<GL4VertexDeclaration*>(decl->getRHIResource().get());
        mCurrentVAO = glDecl->GLVAO;
        glBindVertexArray(mCurrentVAO);

        // 延迟顶点属性配置到 setVertexBuffers，因为 glVertexAttribPointer
        // 在 Core Profile 下要求当前有 GL_ARRAY_BUFFER 绑定
        mPendingVertexDecl = decl;

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setVertexDeclaration");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexBufferPtr GL4Context::createVertexBuffer(VertexBuffer *buffer)
    {
        GL4VertexBufferPtr glBuffer = GL4VertexBuffer::create();

        glGenBuffers(1, &glBuffer->GLBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, glBuffer->GLBuffer);
        glBufferData(GL_ARRAY_BUFFER,
            (GLsizeiptr)buffer->getBufferSize(),
            buffer->getBuffer().Data,
            GL4Mapping::getBufferUsage(buffer->getUsage()));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createVertexBuffer");
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)
    {
        for (uint32_t i = 0; i < buffers.size(); ++i)
        {
            GL4VertexBuffer *glVB = static_cast<GL4VertexBuffer*>(buffers[i]->getRHIResource().get());
            glBindBuffer(GL_ARRAY_BUFFER, glVB->GLBuffer);

            // VBO 已绑定，现在配置此 slot 对应的顶点属性
            if (mPendingVertexDecl != nullptr)
            {
                for (uint32_t j = 0; j < mPendingVertexDecl->getAttributeCount(); ++j)
                {
                    const VertexAttribute &attrib = mPendingVertexDecl->getAttributes()[j];
                    if (attrib.getSlot() != startSlot + i)
                        continue;

                    GLint size = GL4Mapping::getVertexAttribSize(attrib.getType());
                    GLenum type = GL4Mapping::getVertexAttribType(attrib.getType());
                    GLboolean normalized = GL4Mapping::getVertexAttribNormalized(attrib.getType());

                    glEnableVertexAttribArray(j);
                    glVertexAttribPointer(j, size, type, normalized,
                        (GLsizei)strides[i],
                        reinterpret_cast<const void*>((uintptr_t)attrib.getOffset()));
                }
            }
        }

        mPendingVertexDecl = nullptr;

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setVertexBuffers");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIIndexBufferPtr GL4Context::createIndexBuffer(IndexBuffer *buffer)
    {
        GL4IndexBufferPtr glBuffer = GL4IndexBuffer::create();

        glGenBuffers(1, &glBuffer->GLBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer->GLBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            (GLsizeiptr)buffer->getBufferSize(),
            buffer->getBuffer().Data,
            GL4Mapping::getBufferUsage(buffer->getUsage()));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createIndexBuffer");
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setIndexBuffer(IndexBuffer *buffer)
    {
        GL4IndexBuffer *glIB = static_cast<GL4IndexBuffer*>(buffer->getRHIResource().get());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIB->GLBuffer);

        mIndexType = GL4Mapping::get(buffer->getIndexType());
        mIndexSize = (buffer->getIndexType() == IndexType::E_IT_16BITS) ? 2 : 4;

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setIndexBuffer");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIConstantBufferPtr GL4Context::createConstantBuffer(ConstantBuffer *buffer)
    {
        GL4ConstantBufferPtr glBuffer = GL4ConstantBuffer::create();

        glGenBuffers(1, &glBuffer->GLBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, glBuffer->GLBuffer);
        glBufferData(GL_UNIFORM_BUFFER,
            (GLsizeiptr)buffer->getBufferSize(),
            buffer->getBuffer().Data,
            GL4Mapping::getBufferUsage(buffer->getUsage()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createConstantBuffer");
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer1DPtr GL4Context::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        GL4PixelBuffer1DPtr glBuffer = GL4PixelBuffer1D::create();

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
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr GL4Context::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        GL4PixelBuffer2DPtr glBuffer = GL4PixelBuffer2D::create();

        const auto &desc = buffer->getDescriptor();

        glGenTextures(1, &glBuffer->GLTexture);
        glBindTexture(GL_TEXTURE_2D, glBuffer->GLTexture);
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL4Mapping::getInternalFormat(desc.format),
            desc.width, desc.height, 0,
            GL4Mapping::get(desc.format),
            GL4Mapping::getPixelType(desc.format),
            buffer->getBuffer().Data);

        // 无论请求多少 mipmap，都生成完整 mipmap chain，
        // 确保 Sampler Object 的 mipmap filtering 模式正常工作
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createPixelBuffer2D");
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer3DPtr GL4Context::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        GL4PixelBuffer3DPtr glBuffer = GL4PixelBuffer3D::create();

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

        glShader->GLShaderHandle = glCreateShader(GL_VERTEX_SHADER);
        GLint len = static_cast<GLint>(bytecodeLength);
        glShaderSource(glShader->GLShaderHandle, 1, &bytecode, &len);
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
            return nullptr;
        }

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createVertexShader");
        return glShader;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setVertexShader(ShaderVariant *shader)
    {
        GL4Shader *glShader = static_cast<GL4Shader*>(shader->getRHIShader());

        if (mCurrentProgram != 0)
        {
            GL_SAFE_DELETE_PROGRAM(mCurrentProgram);
        }
        mCurrentProgram = glCreateProgram();
        glAttachShader(mCurrentProgram, glShader->GLShaderHandle);
        mProgramDirty = true;

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setVertexShader");
        return T3D_OK;
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

        glShader->GLShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
        GLint len = static_cast<GLint>(bytecodeLength);
        glShaderSource(glShader->GLShaderHandle, 1, &bytecode, &len);
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
            return nullptr;
        }

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createPixelShader");
        return glShader;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setPixelShader(ShaderVariant *shader)
    {
        GL4Shader *glShader = static_cast<GL4Shader*>(shader->getRHIShader());

        if (mCurrentProgram == 0)
        {
            mCurrentProgram = glCreateProgram();
        }
        glAttachShader(mCurrentProgram, glShader->GLShaderHandle);
        mProgramDirty = true;

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setPixelShader");
        return T3D_OK;
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
    // Hull Shader (not supported in GL 3.3)
    //--------------------------------------------------------------------------

    RHIShaderPtr GL4Context::createHullShader(ShaderVariant *shader)
    {
        T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "Hull shader is not supported in OpenGL 3.3");
        return GL4HullShader::create();
    }

    TResult GL4Context::setHullShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4Context::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4Context::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4Context::setHSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }

    //--------------------------------------------------------------------------
    // Domain Shader (not supported in GL 3.3)
    //--------------------------------------------------------------------------

    RHIShaderPtr GL4Context::createDomainShader(ShaderVariant *shader)
    {
        T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "Domain shader is not supported in OpenGL 3.3");
        return GL4DomainShader::create();
    }

    TResult GL4Context::setDomainShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4Context::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4Context::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4Context::setDSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }

    //--------------------------------------------------------------------------
    // Geometry Shader
    //--------------------------------------------------------------------------

    RHIShaderPtr GL4Context::createGeometryShader(ShaderVariant *shader)
    {
        GL4GeometryShaderPtr glShader = GL4GeometryShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        glShader->GLShaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
        GLint len = static_cast<GLint>(bytecodeLength);
        glShaderSource(glShader->GLShaderHandle, 1, &bytecode, &len);
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
            return nullptr;
        }

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createGeometryShader");
        return glShader;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setGeometryShader(ShaderVariant *shader)
    {
        GL4Shader *glShader = static_cast<GL4Shader*>(shader->getRHIShader());

        if (mCurrentProgram == 0)
        {
            mCurrentProgram = glCreateProgram();
        }

        glAttachShader(mCurrentProgram, glShader->GLShaderHandle);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setGeometryShader");
        return T3D_OK;
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
    // Compute Shader (not supported in GL 3.3)
    //--------------------------------------------------------------------------

    RHIShaderPtr GL4Context::createComputeShader(ShaderVariant *shader)
    {
        T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "Compute shader is not supported in OpenGL 3.3");
        return GL4ComputeShader::create();
    }

    TResult GL4Context::setComputeShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4Context::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4Context::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4Context::setCSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }

    //--------------------------------------------------------------------------

    TResult GL4Context::compileShader(ShaderVariant *shader)
    {
        // OpenGL 使用 GLSL，编译发生在 createXXXShader 中（glCompileShader）
        // compileShader 在 GL4 中只需标记已编译，源码即字节码
        size_t bytesLength = 0;
        const char *bytes = shader->getBytesCode(bytesLength);
        shader->setBytesCode(bytes, bytesLength);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            // 为了反射，需要先创建一个临时 program 并 link
            GLuint tempProgram = glCreateProgram();

            GL4Shader *glShader = static_cast<GL4Shader*>(shader->getRHIShader());
            if (glShader == nullptr || glShader->GLShaderHandle == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Cannot reflect shader: RHI shader is null !");
                glDeleteProgram(tempProgram);
                ret = T3D_ERR_GL4_SHADER_REFLECTION;
                break;
            }

            glAttachShader(tempProgram, glShader->GLShaderHandle);
            // Allow linking with a single shader stage for reflection purposes
            glProgramParameteri(tempProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
            glLinkProgram(tempProgram);

            GLint linked = 0;
            glGetProgramiv(tempProgram, GL_LINK_STATUS, &linked);
            if (!linked)
            {
                GLint logLen = 0;
                glGetProgramiv(tempProgram, GL_INFO_LOG_LENGTH, &logLen);
                if (logLen > 0)
                {
                    TArray<char> log(logLen + 1, 0);
                    glGetProgramInfoLog(tempProgram, logLen, nullptr, log.data());
                    T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Shader link error during reflection: %s", log.data());
                }
                glDeleteProgram(tempProgram);
                ret = T3D_ERR_GL4_LINK_PROGRAM;
                break;
            }

            // 反射 Uniform Blocks (对应 D3D11 的 cbuffer)
            GLint numBlocks = 0;
            glGetProgramiv(tempProgram, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);

            for (GLint i = 0; i < numBlocks; ++i)
            {
                char blockName[256] = {};
                GLsizei nameLen = 0;
                glGetActiveUniformBlockName(tempProgram, i, sizeof(blockName), &nameLen, blockName);

                GLint blockSize = 0;
                glGetActiveUniformBlockiv(tempProgram, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

                GLint numUniforms = 0;
                glGetActiveUniformBlockiv(tempProgram, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniforms);

                TArray<GLint> uniformIndices(numUniforms);
                glGetActiveUniformBlockiv(tempProgram, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformIndices.data());

                for (GLint j = 0; j < numUniforms; ++j)
                {
                    GLuint idx = static_cast<GLuint>(uniformIndices[j]);
                    char uniformName[256] = {};
                    GLsizei uniformNameLen = 0;
                    GLint uniformSize = 0;
                    GLenum uniformType = 0;
                    glGetActiveUniform(tempProgram, idx, sizeof(uniformName), &uniformNameLen, &uniformSize, &uniformType, uniformName);

                    GLint offset = 0;
                    glGetActiveUniformsiv(tempProgram, 1, &idx, GL_UNIFORM_OFFSET, &offset);

                    GLint arrayStride = 0;
                    glGetActiveUniformsiv(tempProgram, 1, &idx, GL_UNIFORM_ARRAY_STRIDE, &arrayStride);

                    // 计算 data size
                    uint32_t dataSize = 0;
                    ShaderConstantParam::DATA_TYPE dataType = ShaderConstantParam::DATA_TYPE::DT_FLOAT;

                    switch (uniformType)
                    {
                    case GL_FLOAT:       dataSize = sizeof(float) * uniformSize; dataType = (uniformSize > 1) ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT; break;
                    case GL_FLOAT_VEC4:  dataSize = sizeof(float) * 4 * uniformSize; dataType = (uniformSize > 1) ? ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY : ShaderConstantParam::DATA_TYPE::DT_VECTOR4; break;
                    case GL_FLOAT_MAT4:  dataSize = sizeof(float) * 16 * uniformSize; dataType = (uniformSize > 1) ? ShaderConstantParam::DATA_TYPE::DT_MATRIX4_ARRAY : ShaderConstantParam::DATA_TYPE::DT_MATRIX4; break;
                    case GL_INT:         dataSize = sizeof(int) * uniformSize; dataType = (uniformSize > 1) ? ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY : ShaderConstantParam::DATA_TYPE::DT_INTEGER; break;
                    case GL_BOOL:        dataSize = sizeof(int) * uniformSize; dataType = (uniformSize > 1) ? ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY : ShaderConstantParam::DATA_TYPE::DT_BOOL; break;
                    default:             dataSize = arrayStride > 0 ? arrayStride * uniformSize : 4 * uniformSize; dataType = ShaderConstantParam::DATA_TYPE::DT_STRUCT; break;
                    }

                    // Convert SPIRV-Cross generated names back to original names
                    // UBO block name: "type_Xxx" -> "Xxx" (strip "type_" prefix)
                    // Uniform name: "Instance.member" -> "member" (strip instance name + dot)
                    String cbufferName(blockName);
                    String cname(uniformName);

                    if (StringUtil::startsWith(cbufferName, "type_"))
                    {
                        cbufferName = cbufferName.substr(5);
                    }

                    String::size_type dotPos = cname.find('.');
                    if (dotPos != String::npos)
                    {
                        cname = cname.substr(dotPos + 1);
                    }

                    // Strip array suffix [0] that OpenGL adds to array uniform names
                    String::size_type bracketPos = cname.find('[');
                    if (bracketPos != String::npos)
                    {
                        cname = cname.substr(0, bracketPos);
                    }

                    ShaderConstantParamPtr param = ShaderConstantParam::create(cbufferName, cname, i, dataSize, offset, dataType);
                    constantParams.emplace(param->getName(), param);

                    T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "Shader reflection - UBO name: %s -> %s, uniform name: %s -> %s, type: %u, size: %u, offset: %d", blockName, cbufferName.c_str(), uniformName, cname.c_str(), dataType, dataSize, offset);
                }
            }

            // 反射独立 Uniform（纹理采样器）
            GLint numUniforms = 0;
            glGetProgramiv(tempProgram, GL_ACTIVE_UNIFORMS, &numUniforms);

            uint32_t samplerIndex = 0;
            for (GLint i = 0; i < numUniforms; ++i)
            {
                char uniformName[256] = {};
                GLsizei nameLen = 0;
                GLint uniformSize = 0;
                GLenum uniformType = 0;
                glGetActiveUniform(tempProgram, i, sizeof(uniformName), &nameLen, &uniformSize, &uniformType, uniformName);

                // 只处理采样器类型
                bool isSampler = false;
                TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;

                switch (uniformType)
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
                    GLint loc = glGetUniformLocation(tempProgram, uniformName);
                    if (loc < 0)
                        continue;

                    String name(uniformName);

                    // Convert SPIRV-Cross combined sampler name to original texture name
                    // Pattern: "SPIRV_Cross_Combined<texName>sampler<texName>" -> "<texName>"
                    const String kSpirvPrefix = "SPIRV_Cross_Combined";
                    if (StringUtil::startsWith(name, kSpirvPrefix, false))
                    {
                        String remainder = name.substr(kSpirvPrefix.size());
                        // Find "sampler" keyword in the remainder to extract texture name
                        String::size_type samplerPos = remainder.find("sampler");
                        if (samplerPos != String::npos && samplerPos > 0)
                        {
                            name = remainder.substr(0, samplerPos);
                        }
                    }

                    ShaderSamplerParamPtr param;
                    const auto itr = samplerParams.find(name);
                    if (itr == samplerParams.end())
                    {
                        param = ShaderSamplerParam::create(name);
                        samplerParams.emplace(name, param);
                    }
                    else
                    {
                        param = itr->second;
                    }

                    // 使用从 0 开始递增的索引作为 binding，
                    // 与 setupSamplerBindings() 中的 texUnit 递增顺序一致
                    param->setTexBinding(samplerIndex);
                    param->setSamplerBinding(samplerIndex);
                    param->setTextureType(texType);

                    T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "Shader reflection - sampler name: %s -> %s, binding: %d, type: %d", uniformName, name.c_str(), samplerIndex, texType);
                    samplerIndex++;
                }
            }

            glDetachShader(tempProgram, glShader->GLShaderHandle);
            glDeleteProgram(tempProgram);

            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::reflectShaderAllBindings");
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setPrimitiveType(PrimitiveType primitive)
    {
        mPrimitiveType = GL4Mapping::get(primitive);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
    {
        if (mCurrentProgram != 0 && mProgramDirty)
        {
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
        }

        const void *offset = reinterpret_cast<const void*>((uintptr_t)(startIndex * mIndexSize));
        glDrawElementsBaseVertex(mPrimitiveType, indexCount, mIndexType, offset, baseVertex);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::render(indexed)");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::render(uint32_t vertexCount, uint32_t startVertex)
    {
        if (mCurrentProgram != 0 && mProgramDirty)
        {
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
        }

        glDrawArrays(mPrimitiveType, startVertex, vertexCount);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::render(non-indexed)");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::reset()
    {
        mCurrentRenderTarget = nullptr;

        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 删除当前 program，下次渲染重新创建
        GL_SAFE_DELETE_PROGRAM(mCurrentProgram);
        mPendingUBOs.clear();
        mProgramDirty = false;
        mCurrentVAO = 0;

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::reset");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    // Blit operations
    //--------------------------------------------------------------------------

    TResult GL4Context::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        // TODO: implement using glBlitFramebuffer
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
            return T3D_ERR_INVALID_PARAM;

        // 获取源纹理的 FBO（作为 READ framebuffer）
        Texture2D *tex2D = static_cast<Texture2D*>(src);
        GL4PixelBuffer2D *glSrcPB = static_cast<GL4PixelBuffer2D*>(
            tex2D->getPixelBuffer()->getRHIResource().get());
        if (glSrcPB == nullptr || glSrcPB->GLFBO == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "blit: source texture has no FBO");
            return T3D_ERR_INVALID_PARAM;
        }

        // 确定目标 FBO
        GLuint dstFBO = 0;
        GLsizei dstWidth = 0, dstHeight = 0;

        if (dst->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            dstFBO = 0; // 默认帧缓冲
            dstWidth = static_cast<GLsizei>(dst->getRenderWindow()->getDescriptor().Width);
            dstHeight = static_cast<GLsizei>(dst->getRenderWindow()->getDescriptor().Height);
        }
        else if (dst->getNumOfRenderTextures() > 0)
        {
            GL4PixelBuffer2D *glDstPB = static_cast<GL4PixelBuffer2D*>(
                dst->getRenderTexture()->getPixelBuffer()->getRHIResource().get());
            if (glDstPB != nullptr)
            {
                dstFBO = glDstPB->GLFBO;
                dstWidth = static_cast<GLsizei>(dst->getRenderTexture()->getWidth());
                dstHeight = static_cast<GLsizei>(dst->getRenderTexture()->getHeight());
            }
        }

        // srcOffset 和 size 是像素坐标
        GLint srcX0 = static_cast<GLint>(srcOffset.x());
        GLint srcY0 = static_cast<GLint>(srcOffset.y());
        GLint srcX1 = srcX0 + static_cast<GLint>(size.x());
        GLint srcY1 = srcY0 + static_cast<GLint>(size.y());

        GLint dstX0 = static_cast<GLint>(dstOffset.x());
        GLint dstY0 = static_cast<GLint>(dstOffset.y());
        GLint dstX1 = dstX0 + static_cast<GLint>(size.x());
        GLint dstY1 = dstY0 + static_cast<GLint>(size.y());

        glBindFramebuffer(GL_READ_FRAMEBUFFER, glSrcPB->GLFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFBO);

        glBlitFramebuffer(
            srcX0, srcY0, srcX1, srcY1,
            dstX0, dstY0, dstX1, dstY1,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // 恢复：将当前 render target 的 FBO 重新绑定
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::blit(Texture->RenderTarget)");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)
    {
        // TODO: implement using glCopyBufferSubData
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)
    {
        TResult ret = T3D_OK;

        do
        {
            GLenum target = 0;
            GLuint glBuf = 0;

            switch (renderBuffer->getRHIResource()->getResourceType())
            {
            case RHIResource::ResourceType::kVertexBuffer:
                target = GL_ARRAY_BUFFER;
                glBuf = static_cast<GL4VertexBuffer*>(renderBuffer->getRHIResource().get())->GLBuffer;
                break;
            case RHIResource::ResourceType::kIndexBuffer:
                target = GL_ELEMENT_ARRAY_BUFFER;
                glBuf = static_cast<GL4IndexBuffer*>(renderBuffer->getRHIResource().get())->GLBuffer;
                break;
            case RHIResource::ResourceType::kConstantBuffer:
                target = GL_UNIFORM_BUFFER;
                glBuf = static_cast<GL4ConstantBuffer*>(renderBuffer->getRHIResource().get())->GLBuffer;
                break;
            case RHIResource::ResourceType::kPixelBuffer2D:
                {
                    GL4PixelBuffer2D *glPB = static_cast<GL4PixelBuffer2D*>(renderBuffer->getRHIResource().get());
                    glBindTexture(GL_TEXTURE_2D, glPB->GLTexture);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        0, 0, // 需要实际的宽高，这里简化处理
                        GL_RGBA, GL_UNSIGNED_BYTE, buffer.Data);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::writeBuffer(texture)");
                    return T3D_OK;
                }
            default:
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Unsupported resource type for writeBuffer");
                return T3D_ERR_GL4_INVALID_USAGE;
            }

            glBindBuffer(target, glBuf);

            if (discardWholeBuffer)
            {
                // 用 glBufferData 整体替换（类似 MAP_WRITE_DISCARD）
                glBufferData(target, (GLsizeiptr)buffer.DataSize, buffer.Data, GL_DYNAMIC_DRAW);
            }
            else
            {
                // 用 glBufferSubData 更新部分
                glBufferSubData(target, 0, (GLsizeiptr)buffer.DataSize, buffer.Data);
            }

            glBindBuffer(target, 0);

            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::writeBuffer");
        } while (false);

        return ret;
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

        GLint texUnit = 0;
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
                    glUniform1i(loc, texUnit);
                    T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "setupSamplerBindings: sampler='%s' loc=%d texUnit=%d",
                        name, loc, texUnit);
                    texUnit++;
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::stageConstantBuffers(const ConstantBuffers &buffers)
    {
        for (uint32_t i = 0; i < buffers.size(); ++i)
        {
            GL4ConstantBuffer *glCB = static_cast<GL4ConstantBuffer*>(buffers[i]->getRHIResource().get());
            const String &name = buffers[i]->getName();
            mPendingUBOs[name] = glCB->GLBuffer;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::bindPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "bindPixelBuffers: startSlot=%d bufferCount=%d",
            startSlot, (int)buffers.size());

        for (uint32_t i = 0; i < buffers.size(); ++i)
        {
            if (buffers[i] == nullptr)
            {
                T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "  pixelBuffer[%d]: NULL (skipped)", i);
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
                texHandle = static_cast<GL4PixelBuffer2D*>(buffers[i]->getRHIResource().get())->GLTexture;
                texTarget = GL_TEXTURE_2D;
                break;
            case RHIResource::ResourceType::kPixelBuffer3D:
                texHandle = static_cast<GL4PixelBuffer3D*>(buffers[i]->getRHIResource().get())->GLTexture;
                texTarget = GL_TEXTURE_3D;
                break;
            case RHIResource::ResourceType::kPixelBufferCubemap:
                texHandle = static_cast<GL4PixelBuffer2D*>(buffers[i]->getRHIResource().get())->GLTexture;
                texTarget = GL_TEXTURE_CUBE_MAP;
                break;
            default:
                break;
            }

            GLuint slot = startSlot + i;
            T3D_LOG_DEBUG(LOG_TAG_GL4RENDERER, "  pixelBuffer[%d]: texHandle=%u texTarget=0x%X -> GL_TEXTURE%d",
                i, texHandle, texTarget, slot);
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(texTarget, texHandle);
        }

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::bindPixelBuffers");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::bindSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        for (uint32_t i = 0; i < samplers.size(); ++i)
        {
            if (samplers[i] != nullptr)
            {
                GL4SamplerState *glSampler = static_cast<GL4SamplerState*>(samplers[i]->getRHIState().get());
                glBindSampler(startSlot + i, glSampler->GLSampler);
            }
        }

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::bindSamplers");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
