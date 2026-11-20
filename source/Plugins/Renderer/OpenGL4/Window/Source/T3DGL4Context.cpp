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
        // 下面释放成员引用会触发资源析构，必须先有 current context，
        // 否则 releaseGLObjects 会回头经 GL4Renderer 取正在析构的本对象
        makeDummyCurrent();

        mCurrentRenderTarget = nullptr;
        mPendingVertexDecl = nullptr;
        mCurrentVSVariant = nullptr;
        mCurrentPSVariant = nullptr;
        mCurrentHSVariant = nullptr;
        mCurrentDSVariant = nullptr;
        mCurrentGSVariant = nullptr;
        mCurrentCSVariant = nullptr;

        for (auto &pair : mGraphicsProgramCache)
        {
            GL_SAFE_DELETE_PROGRAM(pair.second);
        }
        mGraphicsProgramCache.clear();
        for (auto &pair : mComputeProgramCache)
        {
            GL_SAFE_DELETE_PROGRAM(pair.second);
        }
        mComputeProgramCache.clear();
        mProgramUBONames.clear();
        mCurrentProgram = 0;
        mCurrentComputeProgram = 0;
        GL_SAFE_DELETE_FBO(mScratchReadFBO);
        GL_SAFE_DELETE_FBO(mScratchDrawFBO);
        GL_SAFE_DELETE_FBO(mDepthOnlyFBO);
        GL_SAFE_DELETE_FBO(mWindowProxyFBO);
        GL_SAFE_DELETE_TEXTURE(mWindowProxyColor);
        GL_SAFE_DELETE_FBO(mWindowProxyResolveFBO);
        GL_SAFE_DELETE_TEXTURE(mWindowProxyResolveTex);
        mWindowProxyOwner = nullptr;
        GL_SAFE_DELETE_VAO(mCurrentVAO);

        for (auto &pair : mPendingReadbacks)
        {
            GL_SAFE_DELETE_BUFFER(pair.second.Staging);
        }
        mPendingReadbacks.clear();
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::init()
    {
        return GL4ContextBase::init();
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::swapBackBuffer(GL4RenderWindow *renderWindow)
    {
        auto lambda = [this](GL4RenderWindowPtr renderWindow)
        {
            TResult ret = T3D_OK;

            do
            {
                // 中间 FBO 保持生效：present 之后的 draw 仍按外部深度画，下一次 present 再拷回
                if (mWindowProxyOwner == renderWindow.get())
                {
                    copyWindowProxy(true);
                }

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

        auto lambda = [this](const GL4RenderWindowPtr &rw, uint32_t w, uint32_t h)
        {
            TResult ret = T3D_OK;

            do
            {
                // 改尺寸后 backbuffer 内容本就失效，中间 FBO 直接按新尺寸重建颜色附件
                if (mWindowProxyOwner == rw.get())
                {
                    ret = ensureWindowProxyColor(w, h, mWindowProxySamples);
                }

                glViewport(0, 0, (GLsizei)w, (GLsizei)h);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::resizeRenderWindow");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, GL4RenderWindowPtr(rw), w, h);
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
        if (renderTarget == nullptr)
        {
            return resetRenderTarget();
        }

        const bool isWindow = (renderTarget->getType() == RenderTarget::Type::E_RT_WINDOW);
        const bool useWindowProxy = isWindow && renderTarget->getDepthStencil() != nullptr;

        TResult ret = T3D_OK;

        if (useWindowProxy)
        {
            ret = checkWindowDepthStencil(renderTarget);
            if (T3D_FAILED(ret))
            {
                return ret;
            }
        }

        uint32_t windowWidth = 0, windowHeight = 0;
        if (isWindow)
        {
            windowWidth = renderTarget->getRenderWindow()->getDescriptor().Width;
            windowHeight = renderTarget->getRenderWindow()->getDescriptor().Height;
        }

        auto lambda = [this](RenderTargetPtr renderTarget, uint32_t windowWidth, uint32_t windowHeight)
        {
            TResult ret = T3D_OK;

            do
            {
                // 入口处清空残留 GL error
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget: ENTRY (stale error from outside)");

                const bool isWindow = (renderTarget->getType() == RenderTarget::Type::E_RT_WINDOW);
                if (!isWindow || renderTarget->getDepthStencil() == nullptr)
                {
                    releaseWindowProxy();
                }

                switch (renderTarget->getType())
                {
                case RenderTarget::Type::E_RT_WINDOW:
                    {
                        if (renderTarget->getDepthStencil() == nullptr)
                        {
                            glBindFramebuffer(GL_FRAMEBUFFER, 0);
                            break;
                        }

                        GL4RenderWindow *glWindow = static_cast<GL4RenderWindow *>(
                            renderTarget->getRenderWindow()->getRHIRenderWindow());
                        GL4PixelBuffer2D *glDS = static_cast<GL4PixelBuffer2D *>(
                            renderTarget->getDepthStencil()->getPixelBuffer()->getRHIResource().get());
                        if (glDS == nullptr || glDS->GLTexture == 0)
                        {
                            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget : depth stencil of window target has no GL texture !");
                            releaseWindowProxy();
                            glBindFramebuffer(GL_FRAMEBUFFER, 0);
                            ret = T3D_ERR_INVALID_POINTER;
                            break;
                        }

                        ret = bindWindowProxy(glWindow, windowWidth, windowHeight, glDS);
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
                                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                            GL_TEXTURE_2D, 0, 0);
                                        GLenum dsTexTarget = (glDS->GLMSAACount > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
                                        GLenum dsAttachment = glDS->GLHasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
                                        glFramebufferTexture2D(GL_FRAMEBUFFER, dsAttachment,
                                            dsTexTarget, glDS->GLTexture, 0);
                                        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget: glFramebufferTexture2D(depth texture)");
                                    }
                                }
                                else if (glPB->GLDepthRBO != 0)
                                {
                                    // 若上次曾挂载过外部深度纹理，切回无外部深度时恢复自带 RBO
                                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                        GL_TEXTURE_2D, 0, 0);
                                    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                        GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glPB->GLDepthRBO);
                                }

                                const uint32_t numRT = renderTarget->getNumOfRenderTextures();
                                const uint32_t activeRTs = std::min<uint32_t>(numRT, 8);
                                GLenum drawBuffers[8] = {};
                                for (uint32_t i = 0; i < activeRTs; ++i)
                                {
                                    drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
                                    if (i > 0)
                                    {
                                        GL4PixelBuffer2D *pb = static_cast<GL4PixelBuffer2D*>(
                                            renderTarget->getRenderTexture(i)->getPixelBuffer()->getRHIResource().get());
                                        if (pb != nullptr)
                                        {
                                            GLenum rtTarget = (pb->GLMSAACount > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
                                            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                                rtTarget, pb->GLTexture, 0);
                                        }
                                    }
                                }

                                for (uint32_t i = activeRTs; i < glPB->GLAttachedColorCount && i < 8; ++i)
                                {
                                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                        GL_TEXTURE_2D, 0, 0);
                                }
                                glPB->GLAttachedColorCount = activeRTs;

                                glDrawBuffers(static_cast<GLsizei>(activeRTs), drawBuffers);
                            }
                        }
                        else if (renderTarget->getDepthStencil() != nullptr)
                        {
                            GL4PixelBuffer2D *glDS = static_cast<GL4PixelBuffer2D*>(
                                renderTarget->getDepthStencil()->getPixelBuffer()->getRHIResource().get());
                            if (glDS != nullptr)
                            {
                                if (mDepthOnlyFBO == 0)
                                {
                                    glGenFramebuffers(1, &mDepthOnlyFBO);
                                }

                                glBindFramebuffer(GL_FRAMEBUFFER, mDepthOnlyFBO);
                                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                    GL_TEXTURE_2D, 0, 0);
                                GLenum dsTexTarget = (glDS->GLMSAACount > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
                                GLenum dsAttachment = glDS->GLHasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
                                glFramebufferTexture2D(GL_FRAMEBUFFER, dsAttachment,
                                    dsTexTarget, glDS->GLTexture, 0);
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

        ret = ENQUEUE_UNIQUE_COMMAND(lambda, RenderTargetPtr(renderTarget), windowWidth, windowHeight);

        if (T3D_SUCCEEDED(ret))
        {
            mCurrentRenderTarget = renderTarget;
            // 中间 FBO 与纹理目标一样按上原点布局，投影同样要翻转 Y，外部深度才能与纹理目标共用
            mRenderingToFBO = !isWindow || useWindowProxy;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::checkWindowDepthStencil(RenderTarget *renderTarget) const
    {
        GL4RenderWindow *glWindow = static_cast<GL4RenderWindow *>(
            renderTarget->getRenderWindow()->getRHIRenderWindow());
        RenderTexturePtr depthStencil = renderTarget->getDepthStencil();
        PixelBuffer2D *pixelBuffer = static_cast<PixelBuffer2D *>(depthStencil->getPixelBuffer());
        if (glWindow == nullptr || pixelBuffer == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget : window target or its depth stencil has no resource !");
            return T3D_ERR_INVALID_POINTER;
        }

        const auto &dsDesc = pixelBuffer->getDescriptor();
        const auto &wndDesc = renderTarget->getRenderWindow()->getDescriptor();

        if (!GL4Mapping::isDepthFormat(dsDesc.format))
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "setRenderTarget : depth stencil of window target is not a depth format !");
            return T3D_ERR_INVALID_PARAM;
        }

        // 与 D3D11 一样，深度的采样数必须与窗口一致
        const uint32_t dsSamples = std::max<uint32_t>(dsDesc.sampleDesc.Count, 1);
        if (dsSamples != glWindow->mMSAACount)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER,
                "setRenderTarget : depth stencil sample count %u does not match window %u !",
                dsSamples, glWindow->mMSAACount);
            return T3D_ERR_INVALID_PARAM;
        }

        const uint32_t wndWidth = static_cast<uint32_t>(wndDesc.Width);
        const uint32_t wndHeight = static_cast<uint32_t>(wndDesc.Height);
        if (dsDesc.width < wndWidth || dsDesc.height < wndHeight)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER,
                "setRenderTarget : depth stencil [%u x %u] is smaller than window [%u x %u] !",
                dsDesc.width, dsDesc.height, wndWidth, wndHeight);
            return T3D_ERR_INVALID_PARAM;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::ensureWindowProxyColor(uint32_t width, uint32_t height, uint32_t samples)
    {
        if (mWindowProxyFBO == 0)
        {
            glGenFramebuffers(1, &mWindowProxyFBO);
        }

        if (mWindowProxyColor == 0 || mWindowProxyWidth != width || mWindowProxyHeight != height
            || mWindowProxySamples != samples)
        {
            const GLsizei w = static_cast<GLsizei>(width);
            const GLsizei h = static_cast<GLsizei>(height);

            GL_SAFE_DELETE_TEXTURE(mWindowProxyColor);
            GL_SAFE_DELETE_TEXTURE(mWindowProxyResolveTex);
            GL_SAFE_DELETE_FBO(mWindowProxyResolveFBO);

            const GLenum colorTarget = (samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
            glGenTextures(1, &mWindowProxyColor);
            glBindTexture(colorTarget, mWindowProxyColor);
            if (samples > 1)
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, static_cast<GLsizei>(samples), GL_RGBA8, w, h, GL_TRUE);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
            }
            glBindTexture(colorTarget, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, mWindowProxyFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTarget, mWindowProxyColor, 0);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glReadBuffer(GL_COLOR_ATTACHMENT0);

            if (samples > 1)
            {
                glGenTextures(1, &mWindowProxyResolveTex);
                glBindTexture(GL_TEXTURE_2D, mWindowProxyResolveTex);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
                glBindTexture(GL_TEXTURE_2D, 0);

                glGenFramebuffers(1, &mWindowProxyResolveFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, mWindowProxyResolveFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mWindowProxyResolveTex, 0);
                glDrawBuffer(GL_COLOR_ATTACHMENT0);
                glReadBuffer(GL_COLOR_ATTACHMENT0);
            }

            mWindowProxyWidth = width;
            mWindowProxyHeight = height;
            mWindowProxySamples = samples;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, mWindowProxyFBO);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::ensureWindowProxyColor");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::bindWindowProxy(GL4RenderWindow *window, uint32_t width, uint32_t height, GL4PixelBuffer2D *depth)
    {
        if (mWindowProxyOwner != window)
        {
            releaseWindowProxy();
        }

        TResult ret = ensureWindowProxyColor(width, height, std::max<uint32_t>(window->mMSAACount, 1));
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        const GLenum dsTarget = (depth->GLMSAACount > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        const GLenum dsAttachment = depth->GLHasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
        glFramebufferTexture2D(GL_FRAMEBUFFER, dsAttachment, dsTarget, depth->GLTexture, 0);

        const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Window proxy FBO is not complete ! status=0x%04X", status);
            releaseWindowProxy();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return T3D_ERR_GL4_CREATE_FBO;
        }

        // 同一窗口连续绑定时中间 FBO 里已经是最新内容，不再拷入
        if (mWindowProxyOwner != window)
        {
            mWindowProxyOwner = window;
            copyWindowProxy(false);
        }

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::bindWindowProxy");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void GL4Context::copyWindowProxy(bool toWindow)
    {
        // glBlitFramebuffer 受 scissor 约束，整张拷贝前要关掉
        const GLboolean scissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
        if (scissorEnabled)
        {
            glDisable(GL_SCISSOR_TEST);
        }

        GLint prevReadFBO = 0, prevDrawFBO = 0;
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevReadFBO);
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevDrawFBO);

        const GLint w = static_cast<GLint>(mWindowProxyWidth);
        const GLint h = static_cast<GLint>(mWindowProxyHeight);

        auto blit = [w, h](GLuint readFBO, GLuint drawFBO, bool flipY)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
            glReadBuffer(readFBO == 0 ? GL_BACK : GL_COLOR_ATTACHMENT0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFBO);
            glDrawBuffer(drawFBO == 0 ? GL_BACK : GL_COLOR_ATTACHMENT0);
            glBlitFramebuffer(0, 0, w, h, 0, flipY ? h : 0, w, flipY ? 0 : h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        };

        // 中间 FBO 是上原点布局，backbuffer 是下原点布局。
        // MSAA 读源只能同矩形 resolve，翻转放在单采样中转纹理那一步；单采样源写 MSAA 目标时每个样本取同一值
        const GLuint from = toWindow ? mWindowProxyFBO : 0;
        const GLuint to = toWindow ? 0 : mWindowProxyFBO;
        if (mWindowProxySamples > 1)
        {
            blit(from, mWindowProxyResolveFBO, false);
            blit(mWindowProxyResolveFBO, to, true);
        }
        else
        {
            blit(from, to, true);
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, static_cast<GLuint>(prevReadFBO));
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(prevDrawFBO));

        if (scissorEnabled)
        {
            glEnable(GL_SCISSOR_TEST);
        }

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::copyWindowProxy");
    }

    //--------------------------------------------------------------------------

    void GL4Context::releaseWindowProxy()
    {
        if (mWindowProxyOwner == nullptr)
        {
            return;
        }

        copyWindowProxy(true);
        mWindowProxyOwner = nullptr;

        // 摘掉外部深度，避免中间 FBO 一直引用已不再使用的深度纹理
        GLint prevFBO = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mWindowProxyFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(prevFBO));
    }

    //--------------------------------------------------------------------------

    void GL4Context::redirectToWindowProxy(BlitEndpoint &ep) const
    {
        if (!ep.isWindow || ep.window == nullptr || ep.window != mWindowProxyOwner)
        {
            return;
        }

        ep.fbo = mWindowProxyFBO;
        ep.texture = mWindowProxyColor;
        ep.target = (mWindowProxySamples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        ep.internalFormat = GL_RGBA8;
        ep.resolveFbo = mWindowProxyResolveFBO;
        ep.resolveTex = mWindowProxyResolveTex;
        ep.width = mWindowProxyWidth;
        ep.height = mWindowProxyHeight;
        ep.sampleCount = mWindowProxySamples;
        ep.isWindow = false;
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
                releaseWindowProxy();
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
        pb->GLHasStencil = false;
        pb->GLAttachedColorCount = 1;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::buildRenderTextureResources(PixelBuffer2D *buffer, GL4PixelBuffer2D *glPixelBuffer)
    {
        TResult ret = T3D_OK;

        do
        {
            const auto &desc = buffer->getDescriptor();
            bool isColorRT = !GL4Mapping::isDepthFormat(desc.format);

            uint32_t msaaCount = desc.sampleDesc.Count;
            if (msaaCount < 1) msaaCount = 1;
            glPixelBuffer->GLMSAACount = msaaCount;

            const uint32_t width = desc.width;
            const uint32_t height = desc.height;
            const GLenum internalFmt = GL4Mapping::getInternalFormat(desc.format);
            const GLenum pixelFmt = GL4Mapping::get(desc.format);
            const GLenum pixelType = GL4Mapping::getPixelType(desc.format);
            const bool hasStencil = GL4Mapping::hasStencil(desc.format);
            glPixelBuffer->GLInternalFormat = internalFmt;
            glPixelBuffer->GLTarget = (msaaCount > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
            glPixelBuffer->GLHasStencil = hasStencil;
            glPixelBuffer->GLAttachedColorCount = 1;

            bool wantsUAV = (buffer->getGPUAccess() & kGPUUnorderedAccess) != 0;
            if (wantsUAV && (!isColorRT || msaaCount > 1))
            {
                T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "%s render texture cannot have "
                    "unordered access, ignore kGPUUnorderedAccess !", isColorRT ? "MSAA" : "Depth");
                wantsUAV = false;
            }

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
                    if (wantsUAV)
                    {
                        // 与 createPixelBuffer2D 的 UAV 分支一致，用不可变存储供 glBindImageTexture 使用
                        glTexStorage2D(GL_TEXTURE_2D, 1, internalFmt, width, height);
                    }
                    else
                    {
                        glTexImage2D(GL_TEXTURE_2D, 0, internalFmt,
                            width, height, 0, pixelFmt, pixelType, nullptr);
                    }
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
                    // 同颜色 RT：sampler object 带 MIPMAP 类过滤时，不封顶会因 mip 链不完整恒采到 0
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
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
            const bool renderingToFBO = mRenderingToFBO;
            auto lambda = [this](Viewport vp, bool renderingToFBO)
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
                    GLint y = renderingToFBO
                        ? static_cast<GLint>(vp.Top * h)
                        : static_cast<GLint>((1 - vp.Top - vp.Height) * h);
                    GLsizei gw = static_cast<GLsizei>(vp.Width * w);
                    GLsizei gh = static_cast<GLsizei>(vp.Height * h);
                    glViewport(x, y, gw, gh);
                    glDepthRange(static_cast<GLdouble>(vp.MinDepth), static_cast<GLdouble>(vp.MaxDepth));
                    GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setViewport(fallback)");
                } while (false);
                return ret;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda, viewport, renderingToFBO);
        }

        // Viewport::Top 是左上原点的比例。画到 FBO 时投影已翻转 Y，行号与之一致；
        // 画到窗口（默认 FBO，左下原点）时要换算成 GL 的下边界，与 setScissorRect 保持一致。
        GLint x = static_cast<GLint>(viewport.Left * width);
        GLint y = mRenderingToFBO
            ? static_cast<GLint>(viewport.Top * height)
            : static_cast<GLint>((1 - viewport.Top - viewport.Height) * height);
        GLsizei w = static_cast<GLsizei>(viewport.Width * width);
        GLsizei h = static_cast<GLsizei>(viewport.Height * height);
        const GLdouble minDepth = static_cast<GLdouble>(viewport.MinDepth);
        const GLdouble maxDepth = static_cast<GLdouble>(viewport.MaxDepth);

        auto lambda = [this](GLint x, GLint y, GLsizei w, GLsizei h, GLdouble minDepth, GLdouble maxDepth)
        {
            TResult ret = T3D_OK;

            do
            {
                glViewport(x, y, w, h);
                glDepthRange(minDepth, maxDepth);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setViewport");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, x, y, w, h, minDepth, maxDepth);
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
            // 画到窗口时投影未翻转，左上原点要换成 GL 左下原点。
            // 画到 FBO 时投影已经翻过 Y，逻辑矩形与帧缓冲行号一致，不能再翻。
            const GLint glY = mRenderingToFBO
                ? static_cast<GLint>(y)
                : fbHeight - (y + static_cast<int32_t>(height));
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
        const bool renderingToFBO = mRenderingToFBO;
        auto lambda = [this](int32_t x, int32_t y, uint32_t width, uint32_t height, bool renderingToFBO)
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
            const GLint glY = renderingToFBO
                ? static_cast<GLint>(y)
                : fbH - (y + static_cast<int32_t>(height));
            glScissor(static_cast<GLint>(x), glY,
                static_cast<GLsizei>(width), static_cast<GLsizei>(height));
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setScissorRect(fallback)");
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, x, y, width, height, renderingToFBO);
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

    TResult GL4Context::clearStencil(uint32_t stencil)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        auto lambda = [this](uint32_t stencil)
        {
            TResult ret = T3D_OK;

            do
            {
                ClearMaskGuard guard(GL_STENCIL_BUFFER_BIT);
                glClearStencil((GLint)stencil);
                glClear(GL_STENCIL_BUFFER_BIT);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::clearStencil");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, stencil);
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

        // data 是纯 CPU 数据，set*State 在主线程读取它，必须在主线程写入；
        // 若入队到 RHI 线程，同帧先创建后使用会读到默认零值。
        if (glState != nullptr)
        {
            glState->data = d;
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
        d.stencilRef = static_cast<uint8_t>(desc.StencilRef);
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

        if (glState != nullptr)
        {
            glState->data = d;
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

        if (glState != nullptr)
        {
            glState->data = d;
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
            desc.BorderColor.red(), desc.BorderColor.green(),
            desc.BorderColor.blue(), desc.BorderColor.alpha()
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
                    glStencilFuncSeparate(GL_FRONT, d.frontStencilFunc, d.stencilRef, d.stencilReadMask);
                    glStencilFuncSeparate(GL_BACK, d.backStencilFunc, d.stencilRef, d.stencilReadMask);
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

                // DepthClipEnable 为 true 表示做近/远平面裁剪，GL_DEPTH_CLAMP 打开则恰好关掉裁剪
                if (d.depthClipEnabled)
                    glDisable(GL_DEPTH_CLAMP);
                else
                    glEnable(GL_DEPTH_CLAMP);

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

                if (d.multisampleEnabled)
                    glEnable(GL_MULTISAMPLE);
                else
                    glDisable(GL_MULTISAMPLE);

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
        if (decl == nullptr || decl->getRHIResource() == nullptr)
        {
            mPendingVertexDecl = nullptr;
            auto lambda = [this]()
            {
                mCurrentVAO = 0;
                glBindVertexArray(0);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setVertexDeclaration(null)");
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

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
        VertexDeclarationPtr pendingDecl = mPendingVertexDecl;

        auto lambda = [this](uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets, const VertexDeclarationPtr &pendingDecl)
        {
            TResult ret = T3D_OK;

            do
            {
                for (uint32_t i = 0; i < buffers.size(); ++i)
                {
                    if (buffers[i] == nullptr || buffers[i]->getRHIResource() == nullptr)
                    {
                        continue;
                    }

                    GL4VertexBuffer *glVB = static_cast<GL4VertexBuffer*>(buffers[i]->getRHIResource().get());
                    glBindBuffer(GL_ARRAY_BUFFER, glVB->GLBuffer);

                    if (pendingDecl != nullptr)
                    {
                        const GLsizei stride = (i < strides.size()) ? static_cast<GLsizei>(strides[i]) : 0;
                        const uintptr_t baseOffset = (i < offsets.size()) ? static_cast<uintptr_t>(offsets[i]) : 0;

                        for (uint32_t j = 0; j < pendingDecl->getAttributeCount(); ++j)
                        {
                            const VertexAttribute &attrib = pendingDecl->getAttributes()[j];
                            if (attrib.getSlot() != startSlot + i)
                                continue;

                            GLint size = GL4Mapping::getVertexAttribSize(attrib.getType());
                            GLenum type = GL4Mapping::getVertexAttribType(attrib.getType());
                            GLboolean normalized = GL4Mapping::getVertexAttribNormalized(attrib.getType());
                            const void *pointer = reinterpret_cast<const void*>(baseOffset + static_cast<uintptr_t>(attrib.getOffset()));

                            glEnableVertexAttribArray(j);
                            if (GL4Mapping::isIntegerAttrib(attrib.getType()))
                            {
                                glVertexAttribIPointer(j, size, type, stride, pointer);
                            }
                            else
                            {
                                glVertexAttribPointer(j, size, type, normalized, stride, pointer);
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
        if (buffer == nullptr || buffer->getRHIResource() == nullptr)
        {
            auto lambda = [this]()
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setIndexBuffer(null)");
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

        GL4IndexBufferPtr glIB = static_cast<GL4IndexBuffer*>(buffer->getRHIResource().get());
        const GLenum indexType = GL4Mapping::get(buffer->getIndexType());
        const uint32_t indexSize = (buffer->getIndexType() == IndexType::E_IT_16BITS) ? 2 : 4;

        auto lambda = [this](const GL4IndexBufferPtr &glIB, GLenum indexType, uint32_t indexSize)
        {
            TResult ret = T3D_OK;

            do
            {
                // mIndexType/mIndexSize 必须在 RHI 线程随命令流顺序更新，
                // 否则主线程提前改写会与正在执行的上一帧 draw 发生数据竞态（如 16-bit 场景与 32-bit UI 混用）。
                mIndexType = indexType;
                mIndexSize = indexSize;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIB->GLBuffer);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setIndexBuffer");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glIB, indexType, indexSize);
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
                const uint32_t mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                const uint32_t arraySize = std::max<uint32_t>(1, desc.arraySize);
                const bool isArray = (arraySize > 1);
                const GLenum target = isArray ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
                const uint32_t bpp = GL4Mapping::getBytesPerPixel(desc.format);
                const GLenum internalFmt = GL4Mapping::getInternalFormat(desc.format);
                const GLenum pixelFmt = GL4Mapping::get(desc.format);
                const GLenum pixelType = GL4Mapping::getPixelType(desc.format);
                glBuffer->GLInternalFormat = internalFmt;
                glBuffer->GLTarget = target;

                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(target, glBuffer->GLTexture);

                GLint prevUnpackAlign = 4;
                glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpackAlign);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

                const uint8_t *cursor = buffer->getBuffer().Data;
                const size_t totalSize = buffer->getBuffer().DataSize;
                size_t offset = 0;

                if ((buffer->getGPUAccess() & kGPUUnorderedAccess) != 0)
                {
                    if (isArray)
                    {
                        glTexStorage2D(GL_TEXTURE_1D_ARRAY, mipLevels, internalFmt, desc.width, arraySize);
                    }
                    else
                    {
                        glTexStorage1D(GL_TEXTURE_1D, mipLevels, internalFmt, desc.width);
                    }

                    if (cursor != nullptr && bpp > 0)
                    {
                        for (uint32_t slice = 0; slice < arraySize; ++slice)
                        {
                            for (uint32_t level = 0; level < mipLevels; ++level)
                            {
                                const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                                const size_t levelSize = static_cast<size_t>(mipW) * bpp;
                                if (offset + levelSize > totalSize)
                                {
                                    break;
                                }
                                if (isArray)
                                {
                                    glTexSubImage2D(GL_TEXTURE_1D_ARRAY, level, 0, slice,
                                        mipW, 1, pixelFmt, pixelType, cursor + offset);
                                }
                                else
                                {
                                    glTexSubImage1D(GL_TEXTURE_1D, level, 0, mipW,
                                        pixelFmt, pixelType, cursor + offset);
                                }
                                offset += levelSize;
                            }
                        }
                    }
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
                        mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                }
                else
                {
                    uint32_t maxLevelUploaded = 0;
                    if (isArray)
                    {
                        for (uint32_t level = 0; level < mipLevels; ++level)
                        {
                            const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                            glTexImage2D(GL_TEXTURE_1D_ARRAY, level, internalFmt,
                                mipW, arraySize, 0, pixelFmt, pixelType, nullptr);
                        }
                        if (cursor != nullptr && bpp > 0)
                        {
                            for (uint32_t slice = 0; slice < arraySize; ++slice)
                            {
                                for (uint32_t level = 0; level < mipLevels; ++level)
                                {
                                    const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                                    const size_t levelSize = static_cast<size_t>(mipW) * bpp;
                                    if (offset + levelSize > totalSize)
                                    {
                                        break;
                                    }
                                    glTexSubImage2D(GL_TEXTURE_1D_ARRAY, level, 0, slice,
                                        mipW, 1, pixelFmt, pixelType, cursor + offset);
                                    offset += levelSize;
                                    maxLevelUploaded = std::max(maxLevelUploaded, level);
                                }
                            }
                        }
                        else
                        {
                            maxLevelUploaded = mipLevels - 1;
                        }
                    }
                    else if (cursor != nullptr && bpp > 0)
                    {
                        uint32_t uploadedLevels = 0;
                        for (uint32_t level = 0; level < mipLevels; ++level)
                        {
                            const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                            const size_t levelSize = static_cast<size_t>(mipW) * bpp;
                            if (offset + levelSize > totalSize)
                            {
                                break;
                            }
                            glTexImage1D(GL_TEXTURE_1D, level, internalFmt,
                                mipW, 0, pixelFmt, pixelType, cursor + offset);
                            offset += levelSize;
                            ++uploadedLevels;
                        }
                        maxLevelUploaded = (uploadedLevels > 0) ? (uploadedLevels - 1) : 0;
                    }
                    else
                    {
                        for (uint32_t level = 0; level < mipLevels; ++level)
                        {
                            const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                            glTexImage1D(GL_TEXTURE_1D, level, internalFmt,
                                mipW, 0, pixelFmt, pixelType, nullptr);
                        }
                        maxLevelUploaded = mipLevels - 1;
                    }

                    glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
                    glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(maxLevelUploaded));
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
                        maxLevelUploaded > 0 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                }

                glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpackAlign);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(target, 0);

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
        const auto &srcDesc = buffer->getDescriptor();
        if (srcDesc.sampleDesc.Count > 1)
        {
            if (srcDesc.mipmaps > 1)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "createPixelBuffer2D : MSAA texture cannot have mipmaps [%u] !", srcDesc.mipmaps);
                return nullptr;
            }
            if (buffer->getBuffer().Data != nullptr && buffer->getBuffer().DataSize > 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "createPixelBuffer2D : MSAA texture cannot have initial data !");
                return nullptr;
            }
            if ((buffer->getGPUAccess() & kGPUUnorderedAccess) != 0)
            {
                T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "createPixelBuffer2D : MSAA texture cannot have "
                    "unordered access, ignore kGPUUnorderedAccess !");
            }
        }

        GL4PixelBuffer2DPtr glBuffer = GL4PixelBuffer2D::create();

        auto lambda = [this](const GL4PixelBuffer2DPtr &glBuffer, const PixelBuffer2DPtr &buffer)
        {
            TResult ret = T3D_OK;

            do
            {
                const auto &desc = buffer->getDescriptor();
                const uint32_t mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                const uint32_t arraySize = std::max<uint32_t>(1, desc.arraySize);
                const uint32_t sampleCount = std::max<uint32_t>(1, desc.sampleDesc.Count);
                const bool isArray = (arraySize > 1);
                const GLenum target = isArray ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
                const uint32_t bpp = GL4Mapping::getBytesPerPixel(desc.format);
                const GLenum internalFmt = GL4Mapping::getInternalFormat(desc.format);
                const GLenum pixelFmt = GL4Mapping::get(desc.format);
                const GLenum pixelType = GL4Mapping::getPixelType(desc.format);
                glBuffer->GLInternalFormat = internalFmt;
                glBuffer->GLTarget = target;
                glBuffer->GLHasStencil = GL4Mapping::hasStencil(desc.format);

                if (sampleCount > 1)
                {
                    const GLenum msTarget = isArray ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;

                    // GL_SAMPLES 按降序返回该格式支持的采样数，第一个就是上限
                    GLint maxSamples = 0;
                    glGetInternalformativ(msTarget, internalFmt, GL_SAMPLES, 1, &maxSamples);
                    if (static_cast<GLint>(sampleCount) > maxSamples)
                    {
                        T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "createPixelBuffer2D : format [%d] supports at most %d samples, "
                            "requested %u !", (int32_t)desc.format, maxSamples, sampleCount);
                        ret = T3D_ERR_INVALID_PARAM;
                        break;
                    }

                    glBuffer->GLTarget = msTarget;
                    glBuffer->GLMSAACount = sampleCount;

                    glGenTextures(1, &glBuffer->GLTexture);
                    glBindTexture(msTarget, glBuffer->GLTexture);
                    if (isArray)
                    {
                        glTexStorage3DMultisample(msTarget, static_cast<GLsizei>(sampleCount), internalFmt,
                            desc.width, desc.height, arraySize, GL_TRUE);
                    }
                    else
                    {
                        glTexStorage2DMultisample(msTarget, static_cast<GLsizei>(sampleCount), internalFmt,
                            desc.width, desc.height, GL_TRUE);
                    }
                    glBindTexture(msTarget, 0);

                    GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::createPixelBuffer2D(MSAA)");
                    break;
                }

                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(target, glBuffer->GLTexture);

                GLint prevUnpackAlign = 4;
                glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpackAlign);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

                const uint8_t *cursor = buffer->getBuffer().Data;
                const size_t totalSize = buffer->getBuffer().DataSize;
                size_t offset = 0;

                if ((buffer->getGPUAccess() & kGPUUnorderedAccess) != 0)
                {
                    if (isArray)
                    {
                        glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, internalFmt, desc.width, desc.height, arraySize);
                    }
                    else
                    {
                        glTexStorage2D(GL_TEXTURE_2D, mipLevels, internalFmt, desc.width, desc.height);
                    }

                    if (cursor != nullptr && bpp > 0)
                    {
                        for (uint32_t slice = 0; slice < arraySize; ++slice)
                        {
                            for (uint32_t level = 0; level < mipLevels; ++level)
                            {
                                const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                                const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                                const size_t levelSize = static_cast<size_t>(mipW) * mipH * bpp;
                                if (offset + levelSize > totalSize)
                                {
                                    break;
                                }
                                if (isArray)
                                {
                                    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, 0, 0, slice,
                                        mipW, mipH, 1, pixelFmt, pixelType, cursor + offset);
                                }
                                else
                                {
                                    glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0,
                                        mipW, mipH, pixelFmt, pixelType, cursor + offset);
                                }
                                offset += levelSize;
                            }
                        }
                    }
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
                        mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                }
                else if (isArray)
                {
                    for (uint32_t level = 0; level < mipLevels; ++level)
                    {
                        const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                        const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                        glTexImage3D(GL_TEXTURE_2D_ARRAY, level, internalFmt,
                            mipW, mipH, arraySize, 0, pixelFmt, pixelType, nullptr);
                    }

                    uint32_t maxLevelUploaded = 0;
                    if (cursor != nullptr && bpp > 0)
                    {
                        for (uint32_t slice = 0; slice < arraySize; ++slice)
                        {
                            for (uint32_t level = 0; level < mipLevels; ++level)
                            {
                                const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                                const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                                const size_t levelSize = static_cast<size_t>(mipW) * mipH * bpp;
                                if (offset + levelSize > totalSize)
                                {
                                    break;
                                }
                                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, 0, 0, slice,
                                    mipW, mipH, 1, pixelFmt, pixelType, cursor + offset);
                                offset += levelSize;
                                maxLevelUploaded = std::max(maxLevelUploaded, level);
                            }
                        }
                    }
                    else
                    {
                        maxLevelUploaded = mipLevels - 1;
                    }

                    glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
                    glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(maxLevelUploaded));
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
                        maxLevelUploaded > 0 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                }
                else if (cursor != nullptr && bpp > 0)
                {
                    uint32_t uploadedLevels = 0;
                    for (uint32_t level = 0; level < mipLevels; ++level)
                    {
                        const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                        const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                        const size_t levelSize = static_cast<size_t>(mipW) * mipH * bpp;
                        if (offset + levelSize > totalSize)
                        {
                            break;
                        }
                        glTexImage2D(GL_TEXTURE_2D, level, internalFmt,
                            mipW, mipH, 0, pixelFmt, pixelType,
                            cursor + offset);
                        offset += levelSize;
                        ++uploadedLevels;
                    }
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,
                        uploadedLevels > 0 ? static_cast<GLint>(uploadedLevels - 1) : 0);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        uploadedLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                }
                else
                {
                    for (uint32_t level = 0; level < mipLevels; ++level)
                    {
                        const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                        const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                        glTexImage2D(GL_TEXTURE_2D, level, internalFmt,
                            mipW, mipH, 0, pixelFmt, pixelType, nullptr);
                    }
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(mipLevels - 1));
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                }

                glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpackAlign);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(target, 0);

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
                const uint32_t mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                const uint32_t bpp = GL4Mapping::getBytesPerPixel(desc.format);
                const GLenum internalFmt = GL4Mapping::getInternalFormat(desc.format);
                const GLenum pixelFmt = GL4Mapping::get(desc.format);
                const GLenum pixelType = GL4Mapping::getPixelType(desc.format);
                glBuffer->GLInternalFormat = internalFmt;

                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(GL_TEXTURE_3D, glBuffer->GLTexture);

                GLint prevUnpackAlign = 4;
                glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpackAlign);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

                const uint8_t *cursor = buffer->getBuffer().Data;
                const size_t totalSize = buffer->getBuffer().DataSize;
                size_t offset = 0;

                if ((buffer->getGPUAccess() & kGPUUnorderedAccess) != 0)
                {
                    glTexStorage3D(GL_TEXTURE_3D, mipLevels, internalFmt, desc.width, desc.height, desc.depth);
                    if (cursor != nullptr && bpp > 0)
                    {
                        for (uint32_t level = 0; level < mipLevels; ++level)
                        {
                            const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                            const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                            const uint32_t mipD = std::max<uint32_t>(1, desc.depth >> level);
                            const size_t levelSize = static_cast<size_t>(mipW) * mipH * mipD * bpp;
                            if (offset + levelSize > totalSize)
                            {
                                break;
                            }
                            glTexSubImage3D(GL_TEXTURE_3D, level, 0, 0, 0,
                                mipW, mipH, mipD, pixelFmt, pixelType, cursor + offset);
                            offset += levelSize;
                        }
                    }
                    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,
                        mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                }
                else if (cursor != nullptr && bpp > 0)
                {
                    uint32_t uploadedLevels = 0;
                    for (uint32_t level = 0; level < mipLevels; ++level)
                    {
                        const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                        const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                        const uint32_t mipD = std::max<uint32_t>(1, desc.depth >> level);
                        const size_t levelSize = static_cast<size_t>(mipW) * mipH * mipD * bpp;
                        if (offset + levelSize > totalSize)
                        {
                            break;
                        }
                        glTexImage3D(GL_TEXTURE_3D, level, internalFmt,
                            mipW, mipH, mipD, 0,
                            pixelFmt, pixelType, cursor + offset);
                        offset += levelSize;
                        ++uploadedLevels;
                    }
                    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
                    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL,
                        uploadedLevels > 0 ? static_cast<GLint>(uploadedLevels - 1) : 0);
                    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,
                        uploadedLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                }
                else
                {
                    for (uint32_t level = 0; level < mipLevels; ++level)
                    {
                        const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                        const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                        const uint32_t mipD = std::max<uint32_t>(1, desc.depth >> level);
                        glTexImage3D(GL_TEXTURE_3D, level, internalFmt,
                            mipW, mipH, mipD, 0,
                            pixelFmt, pixelType, nullptr);
                    }
                    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
                    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(mipLevels - 1));
                    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,
                        mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                }

                glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpackAlign);
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

                const uint32_t mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                const uint32_t faceCount = std::max<uint32_t>(PixelBufferCubemap::FACE_COUNT, desc.arraySize);
                const uint32_t cubeCount = std::max<uint32_t>(1, faceCount / PixelBufferCubemap::FACE_COUNT);
                const bool isArray = (cubeCount > 1);
                const GLenum target = isArray ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;

                const size_t bpp = GL4Mapping::getBytesPerPixel(desc.format);
                const GLenum internalFmt = GL4Mapping::getInternalFormat(desc.format);
                const GLenum pixelFmt = GL4Mapping::get(desc.format);
                const GLenum pixelType = GL4Mapping::getPixelType(desc.format);
                glBuffer->GLInternalFormat = internalFmt;
                glBuffer->GLTarget = target;

                size_t singleFaceFullMipBytes = 0;
                for (uint32_t level = 0; level < mipLevels; ++level)
                {
                    const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                    const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                    singleFaceFullMipBytes += static_cast<size_t>(mipW) * mipH * bpp;
                }
                const size_t fullMipBytes = singleFaceFullMipBytes * faceCount;
                const size_t mip0FaceBytes = static_cast<size_t>(desc.width) * desc.height * bpp;
                const uint8_t *data = static_cast<const uint8_t*>(src.Data);
                const bool hasFullMipData = (data != nullptr && bpp > 0 && mipLevels > 1 && src.DataSize >= fullMipBytes);

                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(target, glBuffer->GLTexture);

                GLint prevUnpackAlign = 4;
                glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpackAlign);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

                if ((buffer->getGPUAccess() & kGPUUnorderedAccess) != 0)
                {
                    if (isArray)
                    {
                        glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, mipLevels, internalFmt, desc.width, desc.height, faceCount);
                    }
                    else
                    {
                        glTexStorage2D(GL_TEXTURE_CUBE_MAP, mipLevels, internalFmt, desc.width, desc.height);
                    }

                    if (data != nullptr && bpp > 0)
                    {
                        if (hasFullMipData)
                        {
                            size_t offset = 0;
                            for (uint32_t face = 0; face < faceCount; ++face)
                            {
                                for (uint32_t level = 0; level < mipLevels; ++level)
                                {
                                    const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                                    const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                                    const size_t levelSize = static_cast<size_t>(mipW) * mipH * bpp;
                                    if (isArray)
                                    {
                                        glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, level, 0, 0, face,
                                            mipW, mipH, 1, pixelFmt, pixelType, data + offset);
                                    }
                                    else
                                    {
                                        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, 0, 0,
                                            mipW, mipH, pixelFmt, pixelType, data + offset);
                                    }
                                    offset += levelSize;
                                }
                            }
                        }
                        else if (src.DataSize >= mip0FaceBytes * faceCount)
                        {
                            for (uint32_t face = 0; face < faceCount; ++face)
                            {
                                const uint8_t *facePtr = data + face * mip0FaceBytes;
                                if (isArray)
                                {
                                    glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, face,
                                        desc.width, desc.height, 1, pixelFmt, pixelType, facePtr);
                                }
                                else
                                {
                                    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, 0, 0,
                                        desc.width, desc.height, pixelFmt, pixelType, facePtr);
                                }
                            }
                            if (mipLevels > 1)
                            {
                                glGenerateMipmap(target);
                            }
                        }
                    }

                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
                        mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                }
                else if (isArray)
                {
                    for (uint32_t level = 0; level < mipLevels; ++level)
                    {
                        const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                        const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                        glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, level, internalFmt,
                            mipW, mipH, faceCount, 0, pixelFmt, pixelType, nullptr);
                    }

                    if (hasFullMipData)
                    {
                        size_t offset = 0;
                        for (uint32_t face = 0; face < faceCount; ++face)
                        {
                            for (uint32_t level = 0; level < mipLevels; ++level)
                            {
                                const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                                const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                                const size_t levelSize = static_cast<size_t>(mipW) * mipH * bpp;
                                glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, level, 0, 0, face,
                                    mipW, mipH, 1, pixelFmt, pixelType, data + offset);
                                offset += levelSize;
                            }
                        }
                        glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
                        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(mipLevels - 1));
                        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    }
                    else if (data != nullptr && bpp > 0 && src.DataSize >= mip0FaceBytes * faceCount)
                    {
                        for (uint32_t face = 0; face < faceCount; ++face)
                        {
                            glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, face,
                                desc.width, desc.height, 1, pixelFmt, pixelType, data + face * mip0FaceBytes);
                        }
                        if (desc.mipmaps == 0 || mipLevels > 1)
                        {
                            glGenerateMipmap(target);
                            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        }
                        else
                        {
                            glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
                            glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);
                            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        }
                    }
                    else
                    {
                        glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
                        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(mipLevels - 1));
                        glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
                            mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                    }
                }
                else
                {
                    if (hasFullMipData)
                    {
                        size_t offset = 0;
                        for (uint32_t face = 0; face < PixelBufferCubemap::FACE_COUNT; ++face)
                        {
                            for (uint32_t level = 0; level < mipLevels; ++level)
                            {
                                const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                                const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                                const size_t levelSize = static_cast<size_t>(mipW) * mipH * bpp;
                                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level,
                                    internalFmt, mipW, mipH, 0, pixelFmt, pixelType, data + offset);
                                offset += levelSize;
                            }
                        }
                        glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
                        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(mipLevels - 1));
                        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    }
                    else
                    {
                        for (uint32_t level = 0; level < mipLevels; ++level)
                        {
                            const uint32_t mipW = std::max<uint32_t>(1, desc.width >> level);
                            const uint32_t mipH = std::max<uint32_t>(1, desc.height >> level);
                            for (uint32_t face = 0; face < PixelBufferCubemap::FACE_COUNT; ++face)
                            {
                                const uint8_t *facePtr = (level == 0 && data != nullptr && src.DataSize >= mip0FaceBytes * PixelBufferCubemap::FACE_COUNT)
                                    ? (data + face * mip0FaceBytes)
                                    : nullptr;
                                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level,
                                    internalFmt, mipW, mipH, 0, pixelFmt, pixelType, facePtr);
                            }
                        }

                        if (data != nullptr && (desc.mipmaps == 0 || mipLevels > 1))
                        {
                            glGenerateMipmap(target);
                            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        }
                        else
                        {
                            glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
                            glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(mipLevels - 1));
                            glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
                                mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                        }
                    }
                }

                glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpackAlign);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glBindTexture(target, 0);

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

    // GL 对象的名字是在 RHI 线程执行创建命令时才写进 RHI 资源的。命令是按入队顺序
    // 执行的，所以在 lambda 里取句柄一定拿得到；但在主线程入队之前取，就可能读到还
    // 没被写过的 0——资源创建和使用落在同一批命令里时必然如此。句柄必须在 lambda
    // 内部解析。
    static GLuint getGLShaderHandle(ShaderVariant *variant)
    {
        if (variant == nullptr)
        {
            return 0;
        }

        GL4Shader *glShader = static_cast<GL4Shader*>(variant->getRHIShader());
        return (glShader != nullptr) ? glShader->GLShaderHandle : 0;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setVertexShader(ShaderVariant *shader)
    {
        return attachGraphicsShader(shader, mCurrentVSVariant, GL_VERTEX_SHADER);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(kTexStageVS, startSlot, buffers);
    }

    TResult GL4Context::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(kTexStageVS, startSlot, buffers);
    }

    TResult GL4Context::setVSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(kTexStageVS, startSlot, samplers);
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
        return attachGraphicsShader(shader, mCurrentPSVariant, GL_FRAGMENT_SHADER);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(kTexStagePS, startSlot, buffers);
    }

    TResult GL4Context::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(kTexStagePS, startSlot, buffers);
    }

    TResult GL4Context::setPSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(kTexStagePS, startSlot, samplers);
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
        return attachGraphicsShader(shader, mCurrentHSVariant, GL_TESS_CONTROL_SHADER);
    }

    TResult GL4Context::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(kTexStageHS, startSlot, buffers);
    }

    TResult GL4Context::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(kTexStageHS, startSlot, buffers);
    }

    TResult GL4Context::setHSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(kTexStageHS, startSlot, samplers);
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
        return attachGraphicsShader(shader, mCurrentDSVariant, GL_TESS_EVALUATION_SHADER);
    }

    TResult GL4Context::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(kTexStageDS, startSlot, buffers);
    }

    TResult GL4Context::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(kTexStageDS, startSlot, buffers);
    }

    TResult GL4Context::setDSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(kTexStageDS, startSlot, samplers);
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
        return attachGraphicsShader(shader, mCurrentGSVariant, GL_GEOMETRY_SHADER);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(kTexStageGS, startSlot, buffers);
    }

    TResult GL4Context::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(kTexStageGS, startSlot, buffers);
    }

    TResult GL4Context::setGSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(kTexStageGS, startSlot, samplers);
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
                mCurrentComputeShader = 0;
                mCurrentComputeProgram = 0;
                mComputeProgramDirty = false;
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

        auto lambda = [this](const ShaderVariantPtr &variant)
        {
            mCurrentCSVariant = variant;

            const GLuint shaderHandle = getGLShaderHandle(variant.get());
            if (shaderHandle != mCurrentComputeShader || mCurrentComputeProgram == 0)
            {
                mCurrentComputeShader = shaderHandle;
                mComputeProgramDirty = true;
            }
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, ShaderVariantPtr(shader));
    }

    TResult GL4Context::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(kTexStageCS, startSlot, buffers);
    }

    TResult GL4Context::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(kTexStageCS, startSlot, buffers);
    }

    TResult GL4Context::setCSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(kTexStageCS, startSlot, samplers);
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

    uint32_t GL4Context::getGraphicsStageIndex(GLenum stageType)
    {
        switch (stageType)
        {
        case GL_VERTEX_SHADER:          return kStageVS;
        case GL_TESS_CONTROL_SHADER:    return kStageHS;
        case GL_TESS_EVALUATION_SHADER: return kStageDS;
        case GL_GEOMETRY_SHADER:        return kStageGS;
        case GL_FRAGMENT_SHADER:        return kStagePS;
        default:                        return kStageCount;
        }
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::linkProgram(GLuint program, const char *tag, bool isCompute)
    {
        glLinkProgram(program);

        GLint linked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked)
        {
            GLint logLen = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen > 0)
            {
                TArray<char> log(logLen + 1, 0);
                glGetProgramInfoLog(program, logLen, nullptr, log.data());
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "%s program link error: %s", tag, log.data());
            }
            return T3D_ERR_GL4_LINK_PROGRAM;
        }

        // uniform block 绑定和 sampler uniform 都存在 program 对象里，链接后设一次即可
        GLint numBlocks = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
        TArray<String> &names = mProgramUBONames[program];
        names.clear();
        names.reserve(numBlocks);
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
            names.push_back(cbufferName);

            glUniformBlockBinding(program, i, static_cast<GLuint>(i));
        }

        glUseProgram(program);
        setupSamplerBindings(program, isCompute);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::linkProgram");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::ensureProgramLinked()
    {
        if (mProgramDirty)
        {
            mProgramDirty = false;
            mProgramLinkFailed = false;
            mCurrentProgram = 0;

            bool hasShader = false;
            for (GLuint handle : mStageShaders)
            {
                hasShader = hasShader || (handle != 0);
            }

            if (hasShader)
            {
                auto it = mGraphicsProgramCache.find(mStageShaders);
                if (it != mGraphicsProgramCache.end())
                {
                    mCurrentProgram = it->second;
                    mProgramLinkFailed = (it->second == 0);
                }
                else
                {
                    GLuint program = glCreateProgram();
                    for (GLuint handle : mStageShaders)
                    {
                        if (handle != 0)
                        {
                            glAttachShader(program, handle);
                        }
                    }

                    if (T3D_FAILED(linkProgram(program, "Graphics", false)))
                    {
                        mProgramUBONames.erase(program);
                        GL_SAFE_DELETE_PROGRAM(program);
                        mProgramLinkFailed = true;
                    }
                    mGraphicsProgramCache[mStageShaders] = program;
                    mCurrentProgram = program;
                }
            }
        }

        if (mProgramLinkFailed)
            return T3D_ERR_GL4_LINK_PROGRAM;

        if (mCurrentProgram == 0)
            return T3D_OK;

        // 即使 program 没变也要走一遍：中间若夹了 dispatch，compute program 会把
        // 同一批 UNIFORM_BUFFER binding point 抢走，不重绑就会读到 compute 的 CB。
        glUseProgram(mCurrentProgram);
        bindPendingUniformBlocks(mCurrentProgram, false);

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::ensureProgramLinked");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::ensureComputeProgramLinked()
    {
        if (mComputeProgramDirty)
        {
            mComputeProgramDirty = false;
            mCurrentComputeProgram = 0;

            if (mCurrentComputeShader != 0)
            {
                auto it = mComputeProgramCache.find(mCurrentComputeShader);
                if (it != mComputeProgramCache.end())
                {
                    mCurrentComputeProgram = it->second;
                }
                else
                {
                    GLuint program = glCreateProgram();
                    glAttachShader(program, mCurrentComputeShader);
                    if (T3D_FAILED(linkProgram(program, "Compute", true)))
                    {
                        mProgramUBONames.erase(program);
                        GL_SAFE_DELETE_PROGRAM(program);
                    }
                    mComputeProgramCache[mCurrentComputeShader] = program;
                    mCurrentComputeProgram = program;
                }
            }
        }

        if (mCurrentComputeProgram == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "ensureComputeProgramLinked: no valid compute program bound !");
            return T3D_ERR_GL4_LINK_PROGRAM;
        }

        glUseProgram(mCurrentComputeProgram);
        bindPendingUniformBlocks(mCurrentComputeProgram, true);

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
        const bool isTyped = (desc.kind == StructuredBufferKind::kTyped);
        const GLenum typedFormat = isTyped ? GL4Mapping::getInternalFormat(desc.format) : GL_RGBA8;

        glBuffer->Kind = desc.kind;
        glBuffer->GLInternalFormat = typedFormat;
        glBuffer->ElementCount = desc.elementCount;
        glBuffer->ElementSize = desc.elementSize;
        glBuffer->HasCounter = hasCounter;

        auto lambda = [this](const GL4StructuredBufferPtr &glBuffer, const StructuredBufferPtr &buffer,
            size_t byteWidth, GLenum usage, bool hasCounter, bool isTyped, GLenum typedFormat)
        {
            TResult ret = T3D_OK;

            do
            {
                const GLenum target = isTyped ? GL_TEXTURE_BUFFER : GL_SHADER_STORAGE_BUFFER;
                glGenBuffers(1, &glBuffer->GLBuffer);
                glBindBuffer(target, glBuffer->GLBuffer);
                glBufferData(target, (GLsizeiptr)byteWidth,
                    buffer->getBuffer().Data, usage);
                glBindBuffer(target, 0);

                if (isTyped)
                {
                    glGenTextures(1, &glBuffer->GLTextureBuffer);
                    glBindTexture(GL_TEXTURE_BUFFER, glBuffer->GLTextureBuffer);
                    glTexBuffer(GL_TEXTURE_BUFFER, typedFormat, glBuffer->GLBuffer);
                    glBindTexture(GL_TEXTURE_BUFFER, 0);
                }

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

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, StructuredBufferPtr(buffer), byteWidth, usage, hasCounter, isTyped, typedFormat);
        if (T3D_FAILED(ret))
        {
            return nullptr;
        }

        return glBuffer;
    }

    TResult GL4Context::setVSStructuredBuffers(uint32_t startSlot, const ShaderResourceBuffers &buffers)
    {
        return bindStructuredBuffers(kTexStageVS, startSlot, buffers);
    }

    TResult GL4Context::setPSStructuredBuffers(uint32_t startSlot, const ShaderResourceBuffers &buffers)
    {
        return bindStructuredBuffers(kTexStagePS, startSlot, buffers);
    }

    TResult GL4Context::setCSStructuredBuffers(uint32_t startSlot, const ShaderResourceBuffers &buffers)
    {
        return bindStructuredBuffers(kTexStageCS, startSlot, buffers);
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

        // 槽位范围按 SSBO 上限（maxUnorderedAccessSlots）校验，image 与计数器只对实际用到它们的槽校验
        const uint32_t maxSlots = mCapabilities.maxUnorderedAccessSlots;
        const uint32_t maxImageUnits = mCapabilities.maxImageUnorderedAccessSlots;
        const uint32_t maxCounterBindings = mCapabilities.maxCounterUnorderedAccessSlots;

        if ((uint64_t)startSlot + buffers.size() > maxSlots)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "setCSUnorderedAccessBuffers : slot range [%u, %llu) "
                "exceeds the %u available UAV slots !", startSlot,
                (unsigned long long)((uint64_t)startSlot + buffers.size()), maxSlots);
            return T3D_ERR_INVALID_PARAM;
        }

        for (uint32_t i = 0; i < buffers.size(); ++i)
        {
            const uint32_t slot = startSlot + i;
            RenderBuffer *rb = buffers[i].get();
            if (rb == nullptr)
            {
                continue;
            }

            if ((rb->getGPUAccess() & kGPUUnorderedAccess) == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "setCSUnorderedAccessBuffers : resource at slot [%u] "
                    "was not created with kGPUUnorderedAccess !", slot);
                return T3D_ERR_INVALID_PARAM;
            }

            bool usesImage = false;
            bool usesCounter = false;
            switch (rb->getType())
            {
            case RenderResource::Type::kPixelBuffer1D:
            case RenderResource::Type::kPixelBuffer2D:
            case RenderResource::Type::kPixelBuffer3D:
            case RenderResource::Type::kPixelBufferCubemap:
                usesImage = true;
                break;
            case RenderResource::Type::kStructuredBuffer:
                {
                    const StructuredBuffer *sb = static_cast<const StructuredBuffer*>(rb);
                    usesImage = (sb->getDescriptor().kind == StructuredBufferKind::kTyped);
                    usesCounter = sb->hasUAVCounter();
                }
                break;
            default:
                break;
            }

            if (usesImage && slot >= maxImageUnits)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "setCSUnorderedAccessBuffers : image resource at slot [%u] "
                    "exceeds the %u available image units !", slot, maxImageUnits);
                return T3D_ERR_INVALID_PARAM;
            }

            if (usesCounter && slot >= maxCounterBindings)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "setCSUnorderedAccessBuffers : counter buffer at slot [%u] "
                    "exceeds the %u available atomic counter bindings !", slot, maxCounterBindings);
                return T3D_ERR_INVALID_PARAM;
            }
        }

        auto lambda = [this, maxImageUnits, maxCounterBindings](uint32_t startSlot, const UnorderedAccessBuffers &buffers, const UAVInitialCounts &initialCounts)
        {
            TResult ret = T3D_OK;

            auto unbindImage = [this, maxImageUnits](uint32_t slot)
            {
                if (slot < maxImageUnits)
                {
                    glBindImageTexture(slot, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
                }
                mBoundCSImages.erase(slot);
            };

            auto unbindCounter = [maxCounterBindings](uint32_t slot)
            {
                if (slot < maxCounterBindings)
                {
                    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, slot, 0);
                }
            };

            do
            {
                for (uint32_t i = 0; i < buffers.size(); ++i)
                {
                    const uint32_t slot = startSlot + i;
                    RenderBuffer *rb = buffers[i].get();

                    if (rb == nullptr || rb->getRHIResource() == nullptr)
                    {
                        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, 0);
                        unbindCounter(slot);
                        unbindImage(slot);
                        continue;
                    }

                    GLuint imgTex = 0;
                    GLenum imgFmt = GL_RGBA8;
                    GLboolean imgLayered = GL_FALSE;
                    if (getGLImageInfo(rb, imgTex, imgFmt, imgLayered))
                    {
                        if (imgTex == 0)
                        {
                            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER,
                                "setCSUnorderedAccessBuffers : texture at slot [%u] is not valid for image load/store (MSAA not supported) !",
                                slot);
                            ret = T3D_ERR_INVALID_PARAM;
                            break;
                        }

                        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, 0);
                        unbindCounter(slot);
                        glBindImageTexture(slot, imgTex, 0, imgLayered, 0, GL_READ_WRITE, imgFmt);
                        mBoundCSImages[slot] = imgTex;
                        continue;
                    }

                    GLuint ssbo = getGLBufferHandle(rb);
                    GLuint counter = 0;
                    bool hasCounter = false;

                    if (rb->getRHIResource()->getResourceType() == RHIResource::ResourceType::kStructuredBuffer)
                    {
                        GL4StructuredBuffer *glSB = static_cast<GL4StructuredBuffer*>(rb->getRHIResource().get());
                        counter = glSB->GLCounterBuffer;
                        hasCounter = glSB->HasCounter;
                    }

                    unbindImage(slot);
                    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, ssbo);

                    if (hasCounter && counter != 0)
                    {
                        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, slot, counter);
                        if (!initialCounts.empty() && initialCounts[i] != kKeepUAVCounter)
                        {
                            uint32_t count = initialCounts[i];
                            glNamedBufferSubData(counter, 0, sizeof(uint32_t), &count);
                        }
                    }
                    else
                    {
                        unbindCounter(slot);
                    }
                }

                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::setCSUnorderedAccessBuffers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, buffers, initialCounts);
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

        auto lambda = [this](const RenderBufferPtr &argsBuffer, GLintptr argsOffset) -> TResult
        {
            TResult ret = ensureComputeProgramLinked();
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            const GLuint glBuf = getGLBufferHandle(argsBuffer.get());
            if (glBuf == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "dispatchIndirect : underlying GL buffer is not ready !");
                return T3D_ERR_INVALID_POINTER;
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

        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderBufferPtr(argsBuffer), (GLintptr)argsOffset);
    }

    TResult GL4Context::uavBarrier(const UnorderedAccessBuffers &buffers)
    {
        if (!mCapabilities.supportsUnorderedAccess)
        {
            T3D_RHI_UNSUPPORTED(supportsUnorderedAccess);
        }

        auto lambda = [this](const UnorderedAccessBuffers &buffers)
        {
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT
                | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
                | GL_TEXTURE_FETCH_BARRIER_BIT
                | GL_TEXTURE_UPDATE_BARRIER_BIT
                | GL_FRAMEBUFFER_BARRIER_BIT
                | GL_ATOMIC_COUNTER_BARRIER_BIT
                | GL_COMMAND_BARRIER_BIT
                | GL_BUFFER_UPDATE_BARRIER_BIT
                | GL_PIXEL_BUFFER_BARRIER_BIT);

            if (buffers.empty())
            {
                for (const auto &kv : mBoundCSImages)
                {
                    glBindImageTexture(kv.first, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
                }
                mBoundCSImages.clear();
            }
            else
            {
                for (const auto &buf : buffers)
                {
                    GLuint imgTex = 0;
                    GLenum imgFmt = GL_RGBA8;
                    GLboolean imgLayered = GL_FALSE;
                    if (!getGLImageInfo(buf.get(), imgTex, imgFmt, imgLayered) || imgTex == 0)
                    {
                        continue;
                    }

                    for (auto it = mBoundCSImages.begin(); it != mBoundCSImages.end(); )
                    {
                        if (it->second == imgTex)
                        {
                            glBindImageTexture(it->first, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
                            it = mBoundCSImages.erase(it);
                        }
                        else
                        {
                            ++it;
                        }
                    }
                }
            }

            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::uavBarrier");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, buffers);
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

        auto lambda = [this](const RenderBufferPtr &dstBuffer, GLintptr dstOffset, const RenderBufferPtr &srcBuffer) -> TResult
        {
            const GLuint dstBuf = getGLBufferHandle(dstBuffer.get());
            GL4StructuredBuffer *glSrc = static_cast<GL4StructuredBuffer*>(srcBuffer->getRHIResource().get());
            const GLuint srcCounter = (glSrc != nullptr) ? glSrc->GLCounterBuffer : 0;

            if (dstBuf == 0 || srcCounter == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyStructureCount : underlying GL objects are not ready !");
                return T3D_ERR_INVALID_POINTER;
            }

            glCopyNamedBufferSubData(srcCounter, dstBuf, 0, dstOffset, sizeof(uint32_t));
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::copyStructureCount");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderBufferPtr(dstBuffer), (GLintptr)dstOffset, RenderBufferPtr(srcBuffer));
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

        auto lambda = [this](const RenderBufferPtr &argsBuffer, GLintptr argsOffset) -> TResult
        {
            TResult ret = ensureProgramLinked();
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            const GLuint glBuf = getGLBufferHandle(argsBuffer.get());
            if (glBuf == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "renderIndexedIndirect : underlying GL buffer is not ready !");
                return T3D_ERR_INVALID_POINTER;
            }

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, glBuf);
            glDrawElementsIndirect(mPrimitiveType, mIndexType, reinterpret_cast<const void *>(argsOffset));
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::renderIndexedIndirect");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderBufferPtr(argsBuffer), (GLintptr)argsOffset);
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

        auto lambda = [this](const RenderBufferPtr &argsBuffer, GLintptr argsOffset) -> TResult
        {
            TResult ret = ensureProgramLinked();
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            const GLuint glBuf = getGLBufferHandle(argsBuffer.get());
            if (glBuf == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "renderIndirect : underlying GL buffer is not ready !");
                return T3D_ERR_INVALID_POINTER;
            }

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, glBuf);
            glDrawArraysIndirect(mPrimitiveType, reinterpret_cast<const void *>(argsOffset));
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::renderIndirect");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderBufferPtr(argsBuffer), (GLintptr)argsOffset);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::reset()
    {
        mCurrentRenderTarget = nullptr;
        mPendingVertexDecl = nullptr;
        mRenderingToFBO = false;
        mProjectionFlipped = false;

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

                for (const auto &kv : mBoundCSImages)
                {
                    glBindImageTexture(kv.first, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
                }
                mBoundCSImages.clear();

                // program 归缓存所有，这里只解除当前引用
                mCurrentComputeProgram = 0;
                mCurrentComputeShader = 0;
                mComputeProgramDirty = false;

                glUseProgram(0);
                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                unbindTextureUnits();
                releaseWindowProxy();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                mCurrentProgram = 0;
                mStageShaders.fill(0);
                for (auto &slots : mPendingUBOs)
                {
                    slots.clear();
                }
                mPendingUBORevision = 0;
                mUBOBoundProgram = 0;
                mUBOBoundRevision = 0;
                mProgramDirty = false;
                mProgramLinkFailed = false;
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

        // 上面那次解析只做参数校验：GL 句柄由 RHI 线程创建资源时才写入，主线程读到的可能还是 0，
        // 所以在 RHI 线程里重新解析一次再 blit
        auto lambda = [this](const RenderTargetPtr &src, const RenderTargetPtr &dst, Vector3 srcOffset, Vector3 size, Vector3 dstOffset)
        {
            BlitEndpoint srcEp {}, dstEp {};
            TResult ret = resolveBlitEndpoint(src.get(), true, srcEp);
            if (T3D_SUCCEEDED(ret))
            {
                ret = resolveBlitEndpoint(dst.get(), false, dstEp);
            }
            if (T3D_FAILED(ret))
            {
                return ret;
            }
            return doBlit(srcEp, dstEp, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderTargetPtr(src), RenderTargetPtr(dst), srcOffset, size, dstOffset);
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

        // 上面那次解析只做参数校验：GL 句柄由 RHI 线程创建资源时才写入，主线程读到的可能还是 0，
        // 所以在 RHI 线程里重新解析一次再 blit
        auto lambda = [this](const TexturePtr &src, const RenderTargetPtr &dst, Vector3 srcOffset, Vector3 size, Vector3 dstOffset)
        {
            BlitEndpoint srcEp {}, dstEp {};
            TResult ret = resolveBlitEndpoint(src.get(), true, srcEp);
            if (T3D_SUCCEEDED(ret))
            {
                ret = resolveBlitEndpoint(dst.get(), false, dstEp);
            }
            if (T3D_FAILED(ret))
            {
                return ret;
            }
            return doBlit(srcEp, dstEp, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, TexturePtr(src), RenderTargetPtr(dst), srcOffset, size, dstOffset);
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

        // 上面那次解析只做参数校验：GL 句柄由 RHI 线程创建资源时才写入，主线程读到的可能还是 0，
        // 所以在 RHI 线程里重新解析一次再 blit
        auto lambda = [this](const RenderTargetPtr &src, const TexturePtr &dst, Vector3 srcOffset, Vector3 size, Vector3 dstOffset)
        {
            BlitEndpoint srcEp {}, dstEp {};
            TResult ret = resolveBlitEndpoint(src.get(), true, srcEp);
            if (T3D_SUCCEEDED(ret))
            {
                ret = resolveBlitEndpoint(dst.get(), false, dstEp);
            }
            if (T3D_FAILED(ret))
            {
                return ret;
            }
            return doBlit(srcEp, dstEp, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderTargetPtr(src), TexturePtr(dst), srcOffset, size, dstOffset);
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

        // 上面那次解析只做参数校验：GL 句柄由 RHI 线程创建资源时才写入，主线程读到的可能还是 0，
        // 所以在 RHI 线程里重新解析一次再 blit
        auto lambda = [this](const TexturePtr &src, const TexturePtr &dst, Vector3 srcOffset, Vector3 size, Vector3 dstOffset)
        {
            BlitEndpoint srcEp {}, dstEp {};
            TResult ret = resolveBlitEndpoint(src.get(), true, srcEp);
            if (T3D_SUCCEEDED(ret))
            {
                ret = resolveBlitEndpoint(dst.get(), false, dstEp);
            }
            if (T3D_FAILED(ret))
            {
                return ret;
            }
            return doBlit(srcEp, dstEp, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, TexturePtr(src), TexturePtr(dst), srcOffset, size, dstOffset);
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

        auto lambda = [this](const RenderBufferPtr &src, const RenderBufferPtr &dst, GLintptr srcOffset, GLintptr dstOffset, GLsizeiptr copySize) -> TResult
        {
            const GLuint srcBuf = getGLBufferHandle(src.get());
            const GLuint dstBuf = getGLBufferHandle(dst.get());
            if (srcBuf == 0 || dstBuf == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "copyBuffer : underlying GL buffers are not ready !");
                return T3D_ERR_INVALID_POINTER;
            }

            glCopyNamedBufferSubData(srcBuf, dstBuf, srcOffset, dstOffset, copySize);
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::copyBuffer");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderBufferPtr(src), RenderBufferPtr(dst),
            (GLintptr)srcOffset, (GLintptr)dstOffset, (GLsizeiptr)copySize);
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

            // 必须是 glCreateBuffers 而不是 glGenBuffers：后者只预留名字，缓冲对象
            // 要等第一次 glBindBuffer 才真正存在，而 DSA 的 glNamedBufferData 要求
            // 对象已存在，拿 glGenBuffers 的名字去调会得到 GL_INVALID_OPERATION，
            // 于是 staging 建不出来，整条回读链路静默失败。
            glCreateBuffers(1, &request->Staging);
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

            if (src->getRHIResource() == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : texture has no RHI resource !");
                request->CopyResult = T3D_ERR_INVALID_POINTER;
                return request->CopyResult;
            }

            const RHIResource::ResourceType resType = src->getRHIResource()->getResourceType();
            uint32_t dimension = 2;
            uint32_t width = 1;
            uint32_t height = 1;
            uint32_t depth = 1;
            uint32_t mipLevels = 1;
            uint32_t arraySize = 1;
            PixelFormat format = PixelFormat::E_PF_UNKNOWN;
            GLuint tex = 0;

            switch (resType)
            {
            case RHIResource::ResourceType::kPixelBuffer1D:
                {
                    PixelBuffer1D *pb = static_cast<PixelBuffer1D*>(src.get());
                    const auto &desc = pb->getDescriptor();
                    dimension = 1;
                    width = desc.width;
                    mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                    arraySize = std::max<uint32_t>(1, desc.arraySize);
                    format = desc.format;
                    tex = static_cast<GL4PixelBuffer1D*>(src->getRHIResource().get())->GLTexture;
                }
                break;
            case RHIResource::ResourceType::kPixelBuffer2D:
                {
                    PixelBuffer2D *pb = static_cast<PixelBuffer2D*>(src.get());
                    const auto &desc = pb->getDescriptor();
                    dimension = 2;
                    width = desc.width;
                    height = desc.height;
                    mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                    arraySize = std::max<uint32_t>(1, desc.arraySize);
                    format = desc.format;

                    GL4PixelBuffer2D *glPB = static_cast<GL4PixelBuffer2D*>(src->getRHIResource().get());
                    tex = glPB->GLTexture;
                    if (glPB->GLMSAACount > 1)
                    {
                        if (glPB->GLResolveTex != 0 && glPB->GLFBO != 0 && glPB->GLResolveFBO != 0)
                        {
                            // map 在 onRender 中间调用，之后还有 draw，必须恢复当前渲染目标
                            GLint prevFBO = 0;
                            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
                            glBindFramebuffer(GL_READ_FRAMEBUFFER, glPB->GLFBO);
                            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, glPB->GLResolveFBO);
                            glBlitFramebuffer(0, 0, (GLint)desc.width, (GLint)desc.height,
                                0, 0, (GLint)desc.width, (GLint)desc.height,
                                GL_COLOR_BUFFER_BIT, GL_NEAREST);
                            glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(prevFBO));
                            tex = glPB->GLResolveTex;
                        }
                        else
                        {
                            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : MSAA texture without resolve FBO is not supported for readback !");
                            request->CopyResult = T3D_ERR_GL4_UNSUPPORTED_OPERATION;
                            return request->CopyResult;
                        }
                    }
                }
                break;
            case RHIResource::ResourceType::kPixelBuffer3D:
                {
                    PixelBuffer3D *pb = static_cast<PixelBuffer3D*>(src.get());
                    const auto &desc = pb->getDescriptor();
                    dimension = 3;
                    width = desc.width;
                    height = desc.height;
                    depth = desc.depth;
                    mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                    arraySize = 1;
                    format = desc.format;
                    tex = static_cast<GL4PixelBuffer3D*>(src->getRHIResource().get())->GLTexture;
                }
                break;
            case RHIResource::ResourceType::kPixelBufferCubemap:
                {
                    PixelBufferCubemap *pb = static_cast<PixelBufferCubemap*>(src.get());
                    const auto &desc = pb->getDescriptor();
                    dimension = 2;
                    width = desc.width;
                    height = desc.height;
                    mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                    arraySize = std::max<uint32_t>(PixelBufferCubemap::FACE_COUNT, desc.arraySize);
                    format = desc.format;
                    tex = static_cast<GL4PixelBufferCubemap*>(src->getRHIResource().get())->GLTexture;
                }
                break;
            default:
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : unsupported texture resource type [%d] !", (int32_t)resType);
                request->CopyResult = T3D_ERR_GL4_UNSUPPORTED_OPERATION;
                return request->CopyResult;
            }

            if (tex == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : underlying GL texture is not ready !");
                request->CopyResult = T3D_ERR_INVALID_POINTER;
                return request->CopyResult;
            }

            const uint32_t bpp = GL4Mapping::getBytesPerPixel(format);
            if (bpp == 0 || GL4Mapping::isDepthFormat(format))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : format [%d] is not supported for readback (compressed and depth/stencil formats are out of scope) !", (int32_t)format);
                request->CopyResult = T3D_ERR_GL4_UNSUPPORTED_OPERATION;
                return request->CopyResult;
            }

            if (request->Region.mipLevel >= mipLevels || request->Region.arraySlice >= arraySize)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : out of range ! mip [%u / %u] slice [%u / %u]",
                    request->Region.mipLevel, mipLevels, request->Region.arraySlice, arraySize);
                request->CopyResult = T3D_ERR_INVALID_PARAM;
                return request->CopyResult;
            }

            const uint32_t mipWidth = std::max<uint32_t>(1, width >> request->Region.mipLevel);
            const uint32_t mipHeight = (dimension >= 2) ? std::max<uint32_t>(1, height >> request->Region.mipLevel) : 1;
            const uint32_t mipDepth = (dimension == 3) ? std::max<uint32_t>(1, depth >> request->Region.mipLevel) : 1;

            const uint32_t offsetX = static_cast<uint32_t>(request->Region.offset.x());
            const uint32_t offsetY = (dimension >= 2) ? static_cast<uint32_t>(request->Region.offset.y()) : 0;
            const uint32_t offsetZ = (dimension == 3) ? static_cast<uint32_t>(request->Region.offset.z()) : 0;

            uint32_t copyWidth = static_cast<uint32_t>(request->Region.size.x());
            uint32_t copyHeight = (dimension >= 2) ? static_cast<uint32_t>(request->Region.size.y()) : 1;
            uint32_t copyDepth = (dimension == 3) ? static_cast<uint32_t>(request->Region.size.z()) : 1;

            if (copyWidth == 0)
            {
                copyWidth = mipWidth - std::min(offsetX, mipWidth);
            }
            if (copyHeight == 0)
            {
                copyHeight = (dimension >= 2) ? (mipHeight - std::min(offsetY, mipHeight)) : 1;
            }
            if (copyDepth == 0)
            {
                copyDepth = (dimension == 3) ? (mipDepth - std::min(offsetZ, mipDepth)) : 1;
            }

            if (copyWidth == 0 || copyHeight == 0 || copyDepth == 0
                || offsetX + copyWidth > mipWidth
                || (dimension >= 2 && offsetY + copyHeight > mipHeight)
                || (dimension == 3 && offsetZ + copyDepth > mipDepth))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "map : region out of range !");
                request->CopyResult = T3D_ERR_INVALID_PARAM;
                return request->CopyResult;
            }

            request->CopyWidth = copyWidth;
            request->CopyHeight = copyHeight;
            request->CopyDepth = copyDepth;
            request->TightRowPitch = copyWidth * bpp;
            request->TightSlicePitch = request->TightRowPitch * copyHeight;
            request->TotalBytes = static_cast<size_t>(request->TightSlicePitch) * copyDepth;

            GLint glYOffset = static_cast<GLint>(offsetY);
            GLint glZOffset = 0;
            if (dimension == 1)
            {
                glYOffset = (arraySize > 1) ? static_cast<GLint>(request->Region.arraySlice) : 0;
                glZOffset = 0;
            }
            else if (dimension == 2)
            {
                glZOffset = static_cast<GLint>(request->Region.arraySlice);
            }
            else if (dimension == 3)
            {
                glZOffset = static_cast<GLint>(offsetZ);
            }

            GLint prevPackAlign = 4;
            glGetIntegerv(GL_PACK_ALIGNMENT, &prevPackAlign);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);

            // 同上：glNamedBufferData 要求缓冲对象已存在，只能用 glCreateBuffers
            glCreateBuffers(1, &request->Staging);
            glNamedBufferData(request->Staging, (GLsizeiptr)request->TotalBytes, nullptr, GL_STREAM_READ);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, request->Staging);
            glGetTextureSubImage(tex, (GLint)request->Region.mipLevel,
                (GLint)offsetX, glYOffset, glZOffset,
                (GLsizei)copyWidth, (GLsizei)copyHeight, (GLsizei)copyDepth,
                GL4Mapping::get(format), GL4Mapping::getPixelType(format),
                (GLsizei)request->TotalBytes, nullptr);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            glPixelStorei(GL_PACK_ALIGNMENT, prevPackAlign);

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
        if (renderBuffer == nullptr || renderBuffer->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "writeBuffer : renderBuffer or its RHI resource is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        if (buffer.Data == nullptr || buffer.DataSize == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "writeBuffer : source buffer data is null or empty !");
            return T3D_ERR_INVALID_PARAM;
        }

        if (renderBuffer->getUsage() == Usage::kImmutable)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "writeBuffer : cannot write to an immutable resource !");
            return T3D_ERR_GL4_INVALID_USAGE;
        }

        auto rhiRes = renderBuffer->getRHIResource();
        const RHIResource::ResourceType resType = rhiRes->getResourceType();
        bool isTexture = false;

        switch (resType)
        {
        case RHIResource::ResourceType::kVertexBuffer:
        case RHIResource::ResourceType::kIndexBuffer:
        case RHIResource::ResourceType::kConstantBuffer:
        case RHIResource::ResourceType::kStructuredBuffer:
            if (!discardWholeBuffer && buffer.DataSize > renderBuffer->getGPUSizeInBytes())
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "writeBuffer : data size [%zu] exceeds buffer size [%zu] !",
                    buffer.DataSize, renderBuffer->getGPUSizeInBytes());
                return T3D_ERR_OUT_OF_BOUND;
            }
            break;
        case RHIResource::ResourceType::kPixelBuffer1D:
        case RHIResource::ResourceType::kPixelBuffer2D:
        case RHIResource::ResourceType::kPixelBuffer3D:
        case RHIResource::ResourceType::kPixelBufferCubemap:
            isTexture = true;
            break;
        default:
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "Unsupported resource type for writeBuffer");
            return T3D_ERR_GL4_INVALID_USAGE;
        }

        if (isTexture)
        {
            uint32_t dimension = 2;
            uint32_t width = 1;
            uint32_t height = 1;
            uint32_t depth = 1;
            uint32_t mipLevels = 1;
            uint32_t arraySize = 1;
            PixelFormat format = PixelFormat::E_PF_UNKNOWN;

            switch (resType)
            {
            case RHIResource::ResourceType::kPixelBuffer1D:
                {
                    const auto &desc = static_cast<PixelBuffer1D*>(renderBuffer)->getDescriptor();
                    dimension = 1;
                    width = desc.width;
                    mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                    arraySize = std::max<uint32_t>(1, desc.arraySize);
                    format = desc.format;
                }
                break;
            case RHIResource::ResourceType::kPixelBuffer2D:
                {
                    const auto &desc = static_cast<PixelBuffer2D*>(renderBuffer)->getDescriptor();
                    GL4PixelBuffer2D *glTex = static_cast<GL4PixelBuffer2D*>(rhiRes.get());
                    if (glTex->GLMSAACount > 1)
                    {
                        T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "writeBuffer : cannot write to an MSAA texture !");
                        return T3D_ERR_GL4_UNSUPPORTED_OPERATION;
                    }
                    dimension = 2;
                    width = desc.width;
                    height = desc.height;
                    mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                    arraySize = std::max<uint32_t>(1, desc.arraySize);
                    format = desc.format;
                }
                break;
            case RHIResource::ResourceType::kPixelBuffer3D:
                {
                    const auto &desc = static_cast<PixelBuffer3D*>(renderBuffer)->getDescriptor();
                    dimension = 3;
                    width = desc.width;
                    height = desc.height;
                    depth = desc.depth;
                    mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                    arraySize = 1;
                    format = desc.format;
                }
                break;
            case RHIResource::ResourceType::kPixelBufferCubemap:
                {
                    const auto &desc = static_cast<PixelBufferCubemap*>(renderBuffer)->getDescriptor();
                    dimension = 4; // cubemap
                    width = desc.width;
                    height = desc.height;
                    mipLevels = std::max<uint32_t>(1, desc.mipmaps);
                    arraySize = std::max<uint32_t>(PixelBufferCubemap::FACE_COUNT, desc.arraySize);
                    format = desc.format;
                }
                break;
            default:
                break;
            }

            const uint32_t bpp = GL4Mapping::getBytesPerPixel(format);
            const size_t sliceMip0Bytes = static_cast<size_t>(width) * height * depth * bpp;
            const size_t minRequiredBytes = sliceMip0Bytes * arraySize;
            if (bpp == 0 || width == 0 || height == 0 || depth == 0 || buffer.DataSize < minRequiredBytes)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER,
                    "writeBuffer(texture) : invalid format or insufficient data size=%zu expected>=%zu bpp=%u %ux%ux%u slices=%u",
                    buffer.DataSize, minRequiredBytes, bpp, width, height, depth, arraySize);
                return T3D_ERR_INVALID_PARAM;
            }

            const GLenum pixelFmt = GL4Mapping::get(format);
            const GLenum pixelType = GL4Mapping::getPixelType(format);

            Buffer ownedBuffer;
            ownedBuffer.DataSize = buffer.DataSize;
            ownedBuffer.Data = T3D_POD_NEW_ARRAY(uint8_t, buffer.DataSize);
            memcpy(ownedBuffer.Data, buffer.Data, buffer.DataSize);

            auto lambda = [this](const RHIResourcePtr &rhiRes, Buffer ownedBuffer,
                uint32_t dimension, uint32_t width, uint32_t height, uint32_t depth,
                uint32_t mipLevels, uint32_t arraySize, uint32_t bpp,
                GLenum pixelFmt, GLenum pixelType)
            {
                TResult ret = T3D_OK;

                GLint prevUnpackAlign = 4;
                glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpackAlign);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

                GLuint glTex = 0;
                GLenum glTarget = GL_TEXTURE_2D;

                switch (rhiRes->getResourceType())
                {
                case RHIResource::ResourceType::kPixelBuffer1D:
                    {
                        GL4PixelBuffer1D *pb1 = static_cast<GL4PixelBuffer1D*>(rhiRes.get());
                        glTex = pb1->GLTexture;
                        glTarget = pb1->GLTarget;
                    }
                    break;
                case RHIResource::ResourceType::kPixelBuffer2D:
                    {
                        GL4PixelBuffer2D *pb2 = static_cast<GL4PixelBuffer2D*>(rhiRes.get());
                        glTex = pb2->GLTexture;
                        glTarget = pb2->GLTarget;
                    }
                    break;
                case RHIResource::ResourceType::kPixelBuffer3D:
                    {
                        GL4PixelBuffer3D *pb3 = static_cast<GL4PixelBuffer3D*>(rhiRes.get());
                        glTex = pb3->GLTexture;
                        glTarget = GL_TEXTURE_3D;
                    }
                    break;
                case RHIResource::ResourceType::kPixelBufferCubemap:
                    {
                        GL4PixelBufferCubemap *pbCube = static_cast<GL4PixelBufferCubemap*>(rhiRes.get());
                        glTex = pbCube->GLTexture;
                        glTarget = pbCube->GLTarget;
                    }
                    break;
                default:
                    break;
                }

                if (glTex != 0)
                {
                    size_t singleSliceFullChain = 0;
                    for (uint32_t level = 0; level < mipLevels; ++level)
                    {
                        const uint32_t mipW = std::max<uint32_t>(1, width >> level);
                        const uint32_t mipH = (dimension >= 2) ? std::max<uint32_t>(1, height >> level) : 1;
                        const uint32_t mipD = (dimension == 3) ? std::max<uint32_t>(1, depth >> level) : 1;
                        singleSliceFullChain += static_cast<size_t>(mipW) * mipH * mipD * bpp;
                    }
                    const bool uploadFullChain = (mipLevels > 1 && ownedBuffer.DataSize >= singleSliceFullChain * arraySize);
                    const uint32_t uploadMips = uploadFullChain ? mipLevels : 1;

                    // 用 DSA 上传，不经过 bind 点，避免把当前纹理单元上已绑定的纹理换成 0
                    size_t offset = 0;

                    for (uint32_t slice = 0; slice < arraySize; ++slice)
                    {
                        for (uint32_t level = 0; level < uploadMips; ++level)
                        {
                            const uint32_t mipW = std::max<uint32_t>(1, width >> level);
                            const uint32_t mipH = (dimension >= 2) ? std::max<uint32_t>(1, height >> level) : 1;
                            const uint32_t mipD = (dimension == 3) ? std::max<uint32_t>(1, depth >> level) : 1;
                            const size_t levelSize = static_cast<size_t>(mipW) * mipH * mipD * bpp;
                            if (offset + levelSize > ownedBuffer.DataSize)
                            {
                                break;
                            }

                            const uint8_t *srcPtr = ownedBuffer.Data + offset;
                            if (glTarget == GL_TEXTURE_1D)
                            {
                                glTextureSubImage1D(glTex, level, 0, mipW, pixelFmt, pixelType, srcPtr);
                            }
                            else if (glTarget == GL_TEXTURE_1D_ARRAY)
                            {
                                glTextureSubImage2D(glTex, level, 0, slice, mipW, 1, pixelFmt, pixelType, srcPtr);
                            }
                            else if (glTarget == GL_TEXTURE_2D)
                            {
                                glTextureSubImage2D(glTex, level, 0, 0, mipW, mipH, pixelFmt, pixelType, srcPtr);
                            }
                            else if (glTarget == GL_TEXTURE_2D_ARRAY || glTarget == GL_TEXTURE_CUBE_MAP_ARRAY
                                || glTarget == GL_TEXTURE_CUBE_MAP)
                            {
                                // DSA 下 cubemap 的 6 个面按 layer 访问，zoffset 即 face 下标
                                glTextureSubImage3D(glTex, level, 0, 0, slice, mipW, mipH, 1, pixelFmt, pixelType, srcPtr);
                            }
                            else if (glTarget == GL_TEXTURE_3D)
                            {
                                glTextureSubImage3D(glTex, level, 0, 0, 0, mipW, mipH, mipD, pixelFmt, pixelType, srcPtr);
                            }

                            offset += levelSize;
                        }
                    }

                    GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::writeBuffer(texture)");
                }

                glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpackAlign);
                T3D_POD_SAFE_DELETE_ARRAY(ownedBuffer.Data);
                return ret;
            };

            return ENQUEUE_UNIQUE_COMMAND(lambda, rhiRes, ownedBuffer,
                dimension, width, height, depth, mipLevels, arraySize, bpp, pixelFmt, pixelType);
        }

        // Deep copy Buffer.Data to avoid dangling pointer in multi-threaded mode.
        // The lambda executes on the RHI thread after the caller's stack frame is gone.
        Buffer ownedBuffer;
        ownedBuffer.DataSize = buffer.DataSize;
        ownedBuffer.Data = T3D_POD_NEW_ARRAY(uint8_t, buffer.DataSize);
        memcpy(ownedBuffer.Data, buffer.Data, buffer.DataSize);

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
                    break;
                }

                // Use DSA to avoid unbinding the current VAO's EBO
                if (discardWholeBuffer)
                {
                    glNamedBufferData(glBuf, (GLsizeiptr)ownedBuffer.DataSize, ownedBuffer.Data, GL_DYNAMIC_DRAW);
                    if (rhiResPtr->getResourceType() == RHIResource::ResourceType::kStructuredBuffer)
                    {
                        GL4StructuredBuffer *glSB = static_cast<GL4StructuredBuffer*>(rhiResPtr.get());
                        if (glSB->Kind == StructuredBufferKind::kTyped && glSB->GLTextureBuffer != 0)
                        {
                            glTextureBuffer(glSB->GLTextureBuffer, glSB->GLInternalFormat, glSB->GLBuffer);
                        }
                    }
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

    void GL4Context::bindPendingUniformBlocks(GLuint program, bool isCompute)
    {
        // program 和 UBO 集合都没变就没什么可做的。每次 draw / dispatch 都重跑一遍
        // glGetActiveUniformBlockName 既费时又会刷屏，而 binding point 上的绑定还在。
        if (program == mUBOBoundProgram && mPendingUBORevision == mUBOBoundRevision)
        {
            return;
        }

        mUBOBoundProgram = program;
        mUBOBoundRevision = mPendingUBORevision;

        auto itNames = mProgramUBONames.find(program);
        if (itNames == mProgramUBONames.end())
        {
            return;
        }

        // compute program 只看 CS 设置的 CB，graphics program 只看当前挂了 shader 的图形阶段。
        // 同名 cbuffer 在一个 program 里只有一个 block，多个阶段都设置了时以 PS 为准（与 sampler 一致）。
        struct StageVariant
        {
            TextureUnitStage    stage;
            ShaderVariant       *variant;
        };
        const StageVariant graphicsStages[] = {
            { kTexStagePS, mCurrentPSVariant.get() },
            { kTexStageVS, mCurrentVSVariant.get() },
            { kTexStageGS, mCurrentGSVariant.get() },
            { kTexStageHS, mCurrentHSVariant.get() },
            { kTexStageDS, mCurrentDSVariant.get() },
        };
        const StageVariant computeStages[] = {
            { kTexStageCS, mCurrentCSVariant.get() },
        };
        const StageVariant *stages = isCompute ? computeStages : graphicsStages;
        const size_t stageCount = isCompute ? std::size(computeStages) : std::size(graphicsStages);

        // block i 在 linkProgram 里已经固定绑到 binding point i
        const TArray<String> &names = itNames->second;
        for (size_t i = 0; i < names.size(); ++i)
        {
            const String &cbufferName = names[i];
            const PendingUBO *found = nullptr;
            uint32_t foundStage = 0;
            uint32_t foundSlot = 0;
            for (size_t s = 0; s < stageCount; ++s)
            {
                if (stages[s].variant == nullptr)
                {
                    continue;
                }

                const uint32_t stage = static_cast<uint32_t>(stages[s].stage);
                for (const auto &item : mPendingUBOs[stage])
                {
                    if (item.second.name != cbufferName)
                    {
                        continue;
                    }

                    if (found == nullptr)
                    {
                        found = &item.second;
                        foundStage = stage;
                        foundSlot = item.first;
                    }
                    else if (item.second.ubo != found->ubo)
                    {
                        T3D_LOG_WARNING(LOG_TAG_GL4RENDERER,
                            "cbuffer '%s' is set to different buffers by stage %u slot %u and stage %u slot %u, "
                            "GL links it as one block; use the former",
                            cbufferName.c_str(), foundStage, foundSlot, stage, item.first);
                    }
                    break;
                }
            }

            if (found != nullptr)
            {
                glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(i), found->ubo);
            }
            else
            {
                // 与 D3D11 槽位为空时一致，不留上一个 program 的 UBO
                glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(i), 0);
                T3D_LOG_WARNING(LOG_TAG_GL4RENDERER,
                    "No pending UBO for cbuffer '%s'", cbufferName.c_str());
            }
        }

        GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::bindPendingUniformBlocks");
    }

    //--------------------------------------------------------------------------

    void GL4Context::setupSamplerBindings(GLuint program, bool isCompute)
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

        struct StageVariant
        {
            TextureUnitStage    stage;
            ShaderVariant       *variant;
        };

        // compute program 只含 CS；graphics program 不含 CS，不能去查当前 CS 的反射。
        // PS 排第一：同名 sampler 被多个阶段使用时，GL 链接后只有一个 uniform，
        // 只能指向一个纹理单元，此时以 PS 为准。
        const StageVariant graphicsStages[] = {
            { kTexStagePS, mCurrentPSVariant.get() },
            { kTexStageVS, mCurrentVSVariant.get() },
            { kTexStageGS, mCurrentGSVariant.get() },
            { kTexStageHS, mCurrentHSVariant.get() },
            { kTexStageDS, mCurrentDSVariant.get() },
        };
        const StageVariant computeStages[] = {
            { kTexStageCS, mCurrentCSVariant.get() },
        };
        const StageVariant *stages = isCompute ? computeStages : graphicsStages;
        const size_t stageCount = isCompute ? std::size(computeStages) : std::size(graphicsStages);

        // 按名称在各阶段的反射结果里查阶段内 slot，返回第一个命中的阶段，未命中返回 false。
        // GL 的 sampler uniform 值是 texture unit 号，语义上对应 texBinding；
        // GL4 反射把 texBinding / samplerBinding 赋成同一个序号，两者等价。
        auto findSlot = [stages, stageCount](const String &texName, bool isSamplerBuf,
            TextureUnitStage &outStage, uint32_t &outSlot, uint32_t &outHitCount) -> bool
        {
            outHitCount = 0;
            for (size_t s = 0; s < stageCount; ++s)
            {
                ShaderVariant *variant = stages[s].variant;
                if (variant == nullptr)
                {
                    continue;
                }

                bool found = false;
                uint32_t slot = 0;
                if (isSamplerBuf)
                {
                    const auto &resParams = variant->getShaderResourceParams();
                    const auto itr = resParams.find(texName);
                    if (itr != resParams.end())
                    {
                        found = true;
                        slot = itr->second->getBinding();
                    }
                }
                else
                {
                    const auto &params = variant->getShaderSamplerParams();
                    const auto itr = params.find(texName);
                    if (itr != params.end())
                    {
                        found = true;
                        slot = itr->second->getTexBinding();
                    }
                }

                if (found)
                {
                    if (outHitCount == 0)
                    {
                        outStage = stages[s].stage;
                        outSlot = slot;
                    }
                    ++outHitCount;
                }
            }
            return outHitCount > 0;
        };

        GLint fallbackTexUnit = 0;
        for (GLint i = 0; i < numUniforms; ++i)
        {
            char name[256] = {};
            GLsizei nameLen = 0;
            GLint uniformSize = 0;
            GLenum uniformType = 0;
            glGetActiveUniform(program, i, sizeof(name), &nameLen, &uniformSize, &uniformType, name);

            TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;
            bool isSampler = GL4ContextBase::mapSamplerGLType(uniformType, texType);
            bool isSamplerBuf = !isSampler && GL4ContextBase::isSamplerBufferGLType(uniformType);

            if (isSampler || isSamplerBuf)
            {
                GLint loc = glGetUniformLocation(program, name);
                if (loc >= 0)
                {
                    String texName = parseSpirvName(String(name));
                    TextureUnitStage stage = kTexStagePS;
                    uint32_t slot = 0;
                    uint32_t hitCount = 0;

                    GLint texUnit;
                    if (findSlot(texName, isSamplerBuf, stage, slot, hitCount))
                    {
                        if (slot >= mTexUnitCount[stage])
                        {
                            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER,
                                "setupSamplerBindings: sampler='%s' slot %u exceeds %u texture units of stage %u",
                                name, slot, mTexUnitCount[stage], static_cast<uint32_t>(stage));
                            fallbackTexUnit++;
                            continue;
                        }

                        if (hitCount > 1)
                        {
                            T3D_LOG_WARNING(LOG_TAG_GL4RENDERER,
                                "setupSamplerBindings: sampler='%s' is used by %u stages, GL links it as one uniform; "
                                "it reads the texture bound to stage %u slot %u",
                                name, hitCount, static_cast<uint32_t>(stage), slot);
                        }

                        texUnit = static_cast<GLint>(mTexUnitBase[stage] + slot);
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

    TResult GL4Context::stageConstantBuffers(TextureUnitStage stage, uint32_t startSlot, const ConstantBuffers &buffers)
    {
        if (static_cast<uint64_t>(startSlot) + buffers.size() > std::numeric_limits<uint32_t>::max())
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "stageConstantBuffers : slot range overflows, startSlot=%u, count=%u !",
                startSlot, static_cast<uint32_t>(buffers.size()));
            return T3D_ERR_INVALID_PARAM;
        }

        auto lambda = [this](TextureUnitStage stage, uint32_t startSlot, const ConstantBuffers &buffers)
        {
            TMap<uint32_t, PendingUBO> &slots = mPendingUBOs[stage];
            bool changed = false;

            for (uint32_t i = 0; i < buffers.size(); ++i)
            {
                const uint32_t slot = startSlot + i;

                if (buffers[i] == nullptr || buffers[i]->getRHIResource() == nullptr)
                {
                    changed |= (slots.erase(slot) > 0);
                    continue;
                }

                const String &name = buffers[i]->getName();
                const GLuint ubo = static_cast<GL4ConstantBuffer*>(
                    buffers[i]->getRHIResource().get())->GLBuffer;

                // GL 按 cbuffer 名绑定，同一阶段的同名 CB 只保留最后设置的槽位
                for (auto itr = slots.begin(); itr != slots.end(); )
                {
                    if (itr->first != slot && itr->second.name == name)
                    {
                        itr = slots.erase(itr);
                        changed = true;
                    }
                    else
                    {
                        ++itr;
                    }
                }

                auto itr = slots.find(slot);
                if (itr == slots.end() || itr->second.name != name || itr->second.ubo != ubo)
                {
                    slots[slot] = PendingUBO{name, ubo};
                    changed = true;
                }
            }

            if (changed)
            {
                ++mPendingUBORevision;
            }

            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, stage, startSlot, buffers);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::checkTextureSlots(TextureUnitStage stage, uint32_t startSlot, size_t count, const char *func) const
    {
        if (static_cast<size_t>(startSlot) + count > mTexUnitCount[stage])
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "%s : slots [%u, %u) exceed %u texture units of stage %u !",
                func, startSlot, static_cast<uint32_t>(startSlot + count), mTexUnitCount[stage],
                static_cast<uint32_t>(stage));
            return T3D_ERR_INVALID_PARAM;
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    GLuint GL4Context::toTextureUnit(TextureUnitStage stage, uint32_t slot)
    {
        mTexUnitUsed[stage] = std::max(mTexUnitUsed[stage], slot + 1);
        return mTexUnitBase[stage] + slot;
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::bindPixelBuffers(TextureUnitStage stage, uint32_t startSlot, const PixelBuffers &buffers)
    {
        TResult ret = checkTextureSlots(stage, startSlot, buffers.size(), "bindPixelBuffers");
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        auto lambda = [this](TextureUnitStage stage, uint32_t startSlot, const PixelBuffers &buffers)
        {
            TResult ret = T3D_OK;

            do
            {
                for (uint32_t i = 0; i < buffers.size(); ++i)
                {
                    GLuint texHandle = 0;
                    GLenum texTarget = GL_TEXTURE_2D;

                    if (buffers[i] != nullptr && buffers[i]->getRHIResource() != nullptr)
                    {
                        switch (buffers[i]->getRHIResource()->getResourceType())
                        {
                        case RHIResource::ResourceType::kPixelBuffer1D:
                            {
                                GL4PixelBuffer1D *glPB = static_cast<GL4PixelBuffer1D*>(buffers[i]->getRHIResource().get());
                                texHandle = glPB->GLTexture;
                                texTarget = glPB->GLTarget;
                            }
                            break;
                        case RHIResource::ResourceType::kPixelBuffer2D:
                            {
                                GL4PixelBuffer2D *glPB = static_cast<GL4PixelBuffer2D*>(buffers[i]->getRHIResource().get());
                                if (glPB->GLMSAACount > 1 && glPB->GLResolveTex != 0)
                                {
                                    texHandle = glPB->GLResolveTex;
                                    texTarget = GL_TEXTURE_2D;
                                }
                                else
                                {
                                    texHandle = glPB->GLTexture;
                                    texTarget = glPB->GLTarget;
                                }
                            }
                            break;
                        case RHIResource::ResourceType::kPixelBuffer3D:
                            texHandle = static_cast<GL4PixelBuffer3D*>(buffers[i]->getRHIResource().get())->GLTexture;
                            texTarget = GL_TEXTURE_3D;
                            break;
                        case RHIResource::ResourceType::kPixelBufferCubemap:
                            {
                                GL4PixelBufferCubemap *glPB = static_cast<GL4PixelBufferCubemap*>(buffers[i]->getRHIResource().get());
                                texHandle = glPB->GLTexture;
                                texTarget = glPB->GLTarget;
                            }
                            break;
                        default:
                            break;
                        }
                    }

                    glActiveTexture(GL_TEXTURE0 + toTextureUnit(stage, startSlot + i));
                    glBindTexture(texTarget, texHandle);
                }
                glActiveTexture(GL_TEXTURE0);
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::bindPixelBuffers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, stage, startSlot, buffers);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::bindSamplers(TextureUnitStage stage, uint32_t startSlot, const Samplers &samplers)
    {
        TResult ret = checkTextureSlots(stage, startSlot, samplers.size(), "bindSamplers");
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        auto lambda = [this](TextureUnitStage stage, uint32_t startSlot, const Samplers &samplers)
        {
            TResult ret = T3D_OK;

            do
            {
                for (uint32_t i = 0; i < samplers.size(); ++i)
                {
                    GLuint handle = 0;
                    if (samplers[i] != nullptr && samplers[i]->getRHIState() != nullptr)
                    {
                        handle = static_cast<GL4SamplerState*>(samplers[i]->getRHIState().get())->GLSampler;
                    }

                    // handle==0 也要绑：sampler object 是全局状态，不显式解绑就会
                    // 把上一 pass 的采样参数（比如 comparison 模式）留给下一 pass。
                    glBindSampler(toTextureUnit(stage, startSlot + i), handle);
                }
                GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::bindSamplers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, stage, startSlot, samplers);
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::bindStructuredBuffers(TextureUnitStage stage, uint32_t startSlot, const ShaderResourceBuffers &buffers)
    {
        if (!mCapabilities.supportsStructuredBuffer)
        {
            T3D_RHI_UNSUPPORTED(supportsStructuredBuffer);
        }

        auto lambda = [this](TextureUnitStage stage, uint32_t startSlot, const ShaderResourceBuffers &buffers)
        {
            for (uint32_t i = 0; i < buffers.size(); ++i)
            {
                const uint32_t slot = startSlot + i;
                RenderBuffer *rb = buffers[i].get();
                if (rb != nullptr && rb->getRHIResource() != nullptr
                    && rb->getRHIResource()->getResourceType() == RHIResource::ResourceType::kStructuredBuffer)
                {
                    GL4StructuredBuffer *glSB = static_cast<GL4StructuredBuffer*>(rb->getRHIResource().get());
                    if (glSB->Kind == StructuredBufferKind::kTyped && glSB->GLTextureBuffer != 0)
                    {
                        // samplerBuffer 占的是纹理单元，与该阶段的纹理共用分区
                        if (T3D_FAILED(checkTextureSlots(stage, slot, 1, "bindStructuredBuffers")))
                        {
                            continue;
                        }
                        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, 0);
                        glActiveTexture(GL_TEXTURE0 + toTextureUnit(stage, slot));
                        glBindTexture(GL_TEXTURE_BUFFER, glSB->GLTextureBuffer);
                        continue;
                    }
                }

                const GLuint handle = getGLBufferHandle(rb);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, handle);
            }
            glActiveTexture(GL_TEXTURE0);
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::bindStructuredBuffers");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, stage, startSlot, buffers);
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

    TResult GL4Context::attachGraphicsShader(ShaderVariant *shader, ShaderVariantPtr &currentVariant, GLenum stageType)
    {
        ShaderVariantPtr *slot = &currentVariant;
        const uint32_t stage = getGraphicsStageIndex(stageType);

        // 只记录各阶段的 shader 句柄，program 推迟到 draw 时按组合查缓存，
        // 这样调用方可以按任意顺序设置各阶段（比如先 PS 后 VS）
        auto lambda = [this, stage](const ShaderVariantPtr &variant, ShaderVariantPtr *slot)
        {
            *slot = variant;

            const GLuint shaderHandle = getGLShaderHandle(variant.get());
            if (stage < kStageCount && mStageShaders[stage] != shaderHandle)
            {
                mStageShaders[stage] = shaderHandle;
                mProgramDirty = true;
            }
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, ShaderVariantPtr(shader), slot);
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

    bool GL4Context::getGLImageInfo(RenderBuffer *buffer, GLuint &outTex, GLenum &outFormat, GLboolean &outLayered) const
    {
        outTex = 0;
        outFormat = GL_RGBA8;
        outLayered = GL_FALSE;

        if (buffer == nullptr || buffer->getRHIResource() == nullptr)
        {
            return false;
        }

        switch (buffer->getRHIResource()->getResourceType())
        {
        case RHIResource::ResourceType::kPixelBuffer1D:
            {
                GL4PixelBuffer1D *pb = static_cast<GL4PixelBuffer1D*>(buffer->getRHIResource().get());
                outTex = pb->GLTexture;
                outFormat = pb->GLInternalFormat;
                outLayered = (pb->GLTarget == GL_TEXTURE_1D_ARRAY) ? GL_TRUE : GL_FALSE;
                return true;
            }
        case RHIResource::ResourceType::kPixelBuffer2D:
            {
                GL4PixelBuffer2D *pb = static_cast<GL4PixelBuffer2D*>(buffer->getRHIResource().get());
                if (pb->GLMSAACount > 1)
                {
                    outTex = 0;
                }
                else
                {
                    outTex = pb->GLTexture;
                }
                outFormat = pb->GLInternalFormat;
                outLayered = (pb->GLTarget == GL_TEXTURE_2D_ARRAY) ? GL_TRUE : GL_FALSE;
                return true;
            }
        case RHIResource::ResourceType::kPixelBuffer3D:
            {
                GL4PixelBuffer3D *pb = static_cast<GL4PixelBuffer3D*>(buffer->getRHIResource().get());
                outTex = pb->GLTexture;
                outFormat = pb->GLInternalFormat;
                outLayered = GL_TRUE;
                return true;
            }
        case RHIResource::ResourceType::kPixelBufferCubemap:
            {
                GL4PixelBufferCubemap *pb = static_cast<GL4PixelBufferCubemap*>(buffer->getRHIResource().get());
                outTex = pb->GLTexture;
                outFormat = pb->GLInternalFormat;
                outLayered = GL_TRUE;
                return true;
            }
        case RHIResource::ResourceType::kStructuredBuffer:
            {
                GL4StructuredBuffer *sb = static_cast<GL4StructuredBuffer*>(buffer->getRHIResource().get());
                if (sb->Kind == StructuredBufferKind::kTyped && sb->GLTextureBuffer != 0)
                {
                    outTex = sb->GLTextureBuffer;
                    outFormat = sb->GLInternalFormat;
                    outLayered = GL_FALSE;
                    return true;
                }
                return false;
            }
        default:
            break;
        }

        return false;
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

        PixelBuffer *pixelBuffer = tex->getPixelBuffer();
        if (!asSource && pixelBuffer->getUsage() == Usage::kImmutable)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "blit : destination texture [%s] is immutable !", tex->getName().c_str());
            return T3D_ERR_GL4_INVALID_USAGE;
        }

        const TEXTURE_TYPE type = tex->getTextureType();
        RHIResource *rhiRes = pixelBuffer->getRHIResource().get();

        switch (type)
        {
        case TEXTURE_TYPE::TT_1D:
            {
                PixelBuffer1D *pb1 = static_cast<PixelBuffer1D*>(pixelBuffer);
                GL4PixelBuffer1D *glPB = static_cast<GL4PixelBuffer1D*>(rhiRes);
                const auto &desc = pb1->getDescriptor();
                out.texture = glPB->GLTexture;
                out.target = glPB->GLTarget;
                out.internalFormat = glPB->GLInternalFormat;
                out.width = desc.width;
                out.height = 1;
                out.depth = 1;
                out.arraySize = std::max<uint32_t>(1, desc.arraySize);
                out.sampleCount = 1;
                out.isWindow = false;
                out.isDepth = GL4Mapping::isDepthFormat(desc.format);
                out.hasStencil = GL4Mapping::hasStencil(desc.format);
                out.needsScratchFbo = true;
            }
            break;
        case TEXTURE_TYPE::TT_2D:
        case TEXTURE_TYPE::TT_2D_ARRAY:
        case TEXTURE_TYPE::TT_RENDER_TEXTURE:
            {
                PixelBuffer2D *pb2 = static_cast<PixelBuffer2D*>(pixelBuffer);
                GL4PixelBuffer2D *glPB = static_cast<GL4PixelBuffer2D*>(rhiRes);
                const auto &desc = pb2->getDescriptor();
                out.texture = glPB->GLTexture;
                out.target = glPB->GLTarget;
                out.internalFormat = glPB->GLInternalFormat;
                out.fbo = glPB->GLFBO;
                out.resolveFbo = glPB->GLResolveFBO;
                out.resolveTex = glPB->GLResolveTex;
                out.width = desc.width;
                out.height = desc.height;
                out.depth = 1;
                out.arraySize = std::max<uint32_t>(1, desc.arraySize);
                out.sampleCount = glPB->GLMSAACount;
                out.isWindow = false;
                out.isDepth = GL4Mapping::isDepthFormat(desc.format);
                out.hasStencil = GL4Mapping::hasStencil(desc.format);
                out.needsScratchFbo = (glPB->GLFBO == 0);
            }
            break;
        case TEXTURE_TYPE::TT_3D:
            {
                PixelBuffer3D *pb3 = static_cast<PixelBuffer3D*>(pixelBuffer);
                GL4PixelBuffer3D *glPB = static_cast<GL4PixelBuffer3D*>(rhiRes);
                const auto &desc = pb3->getDescriptor();
                out.texture = glPB->GLTexture;
                out.target = GL_TEXTURE_3D;
                out.internalFormat = glPB->GLInternalFormat;
                out.width = desc.width;
                out.height = desc.height;
                out.depth = std::max<uint32_t>(1, desc.depth);
                out.arraySize = 1;
                out.sampleCount = 1;
                out.isWindow = false;
                out.isDepth = GL4Mapping::isDepthFormat(desc.format);
                out.hasStencil = GL4Mapping::hasStencil(desc.format);
                out.needsScratchFbo = true;
            }
            break;
        case TEXTURE_TYPE::TT_CUBE:
        case TEXTURE_TYPE::TT_CUBE_ARRAY:
            {
                PixelBufferCubemap *pbCube = static_cast<PixelBufferCubemap*>(pixelBuffer);
                GL4PixelBufferCubemap *glPB = static_cast<GL4PixelBufferCubemap*>(rhiRes);
                const auto &desc = pbCube->getDescriptor();
                out.texture = glPB->GLTexture;
                out.target = glPB->GLTarget;
                out.internalFormat = glPB->GLInternalFormat;
                out.width = desc.width;
                out.height = desc.height;
                out.depth = 1;
                out.arraySize = std::max<uint32_t>(PixelBufferCubemap::FACE_COUNT, desc.arraySize);
                out.sampleCount = 1;
                out.isWindow = false;
                out.isDepth = GL4Mapping::isDepthFormat(desc.format);
                out.hasStencil = GL4Mapping::hasStencil(desc.format);
                out.needsScratchFbo = true;
            }
            break;
        default:
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "blit : unsupported texture type [%d] !", (int32_t)type);
            return T3D_ERR_GL4_UNSUPPORTED_OPERATION;
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
            out.target = GL_TEXTURE_2D;
            out.internalFormat = GL_RGBA8;
            out.width = rt->getRenderWindow()->getDescriptor().Width;
            out.height = rt->getRenderWindow()->getDescriptor().Height;
            out.depth = 1;
            out.arraySize = 1;
            out.sampleCount = 1;
            out.isWindow = true;
            out.isDepth = false;
            out.hasStencil = false;
            out.needsScratchFbo = false;
            out.window = static_cast<GL4RenderWindow *>(rt->getRenderWindow()->getRHIRenderWindow());
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
        for (uint32_t stage = 0; stage < kTexStageCount; ++stage)
        {
            const GLuint base = mTexUnitBase[stage];
            for (uint32_t slot = 0; slot < mTexUnitUsed[stage]; ++slot)
            {
                const GLuint unit = base + slot;
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(GL_TEXTURE_1D, 0);
                glBindTexture(GL_TEXTURE_1D_ARRAY, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 0);
                glBindTexture(GL_TEXTURE_3D, 0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);
                glBindTexture(GL_TEXTURE_BUFFER, 0);
                glBindSampler(unit, 0);
            }
            mTexUnitUsed[stage] = 0;
        }
        glActiveTexture(GL_TEXTURE0);
    }

    //--------------------------------------------------------------------------

    void GL4Context::attachScratchFBO(GLuint fbo, const BlitEndpoint &ep, GLint layer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

        const GLenum attachment = ep.isDepth
            ? (ep.hasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT)
            : GL_COLOR_ATTACHMENT0;

        switch (ep.target)
        {
        case GL_TEXTURE_1D:
            glFramebufferTexture1D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_1D, ep.texture, 0);
            break;
        case GL_TEXTURE_2D:
        case GL_TEXTURE_2D_MULTISAMPLE:
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, ep.target, ep.texture, 0);
            break;
        case GL_TEXTURE_CUBE_MAP:
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, ep.texture, 0);
            break;
        case GL_TEXTURE_1D_ARRAY:
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        case GL_TEXTURE_3D:
        case GL_TEXTURE_CUBE_MAP_ARRAY:
            glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, ep.texture, 0, layer);
            break;
        default:
            break;
        }

        if (ep.isDepth)
        {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        else
        {
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
        }
    }

    //--------------------------------------------------------------------------

    TResult GL4Context::doBlit(const BlitEndpoint &srcIn, const BlitEndpoint &dstIn,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
    {
        TResult ret = T3D_OK;

        BlitEndpoint src = srcIn;
        BlitEndpoint dst = dstIn;
        redirectToWindowProxy(src);
        redirectToWindowProxy(dst);

        // 池化临时 RT 上一帧可能还绑在某个 texture unit 上，这里先摘干净，
        // 避免同一张纹理既挂在 unit 上又当 FBO 附件写。
        unbindTextureUnits();

        if (src.isDepth != dst.isDepth)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "blit : cannot blit between color and depth/stencil endpoints !");
            return T3D_ERR_GL4_INCOMPATIBLE_COPY;
        }

        if (!src.isWindow && !dst.isWindow && src.texture == dst.texture && srcOffset == dstOffset)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "blit : source and destination are the same subresource !");
            return T3D_ERR_GL4_INCOMPATIBLE_COPY;
        }

        const bool isFullCopy = (size == Vector3::ZERO);
        const uint32_t srcOffX = static_cast<uint32_t>(srcOffset.x());
        const uint32_t srcOffY = static_cast<uint32_t>(srcOffset.y());
        const uint32_t srcOffZ = static_cast<uint32_t>(srcOffset.z());
        const uint32_t dstOffX = static_cast<uint32_t>(dstOffset.x());
        const uint32_t dstOffY = static_cast<uint32_t>(dstOffset.y());
        const uint32_t dstOffZ = static_cast<uint32_t>(dstOffset.z());

        const uint32_t srcSlices = (src.depth > 1) ? src.depth : src.arraySize;
        const uint32_t dstSlices = (dst.depth > 1) ? dst.depth : dst.arraySize;

        const uint32_t copySrcW = isFullCopy
            ? (src.width - std::min(srcOffX, src.width))
            : static_cast<uint32_t>(size.x());
        const uint32_t copySrcH = isFullCopy
            ? (src.height - std::min(srcOffY, src.height))
            : ((src.height == 1 && size.y() == 0) ? 1u : static_cast<uint32_t>(size.y()));
        const uint32_t copySrcD = isFullCopy
            ? (srcSlices - std::min(srcOffZ, srcSlices))
            : std::max<uint32_t>(1, static_cast<uint32_t>(size.z()));

        const uint32_t copyDstW = isFullCopy
            ? ((src.width == dst.width && src.height == dst.height)
                ? copySrcW
                : (dst.width - std::min(dstOffX, dst.width)))
            : copySrcW;
        const uint32_t copyDstH = isFullCopy
            ? ((src.width == dst.width && src.height == dst.height)
                ? copySrcH
                : (dst.height - std::min(dstOffY, dst.height)))
            : copySrcH;

        if (copySrcW == 0 || copySrcH == 0 || copySrcD == 0 || copyDstW == 0 || copyDstH == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "blit : copy size is zero !");
            return T3D_ERR_INVALID_PARAM;
        }

        if (srcOffZ + copySrcD > srcSlices || dstOffZ + copySrcD > dstSlices)
        {
            // 当目标是非数组单层时，如果只传输 1 层（常见于 3D/Cubemap 某层 -> RT/Window），允许 dstOffZ == 0
            if (!(copySrcD == 1 && dstOffZ == 0 && dstSlices >= 1))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4RENDERER, "blit : slice/depth out of range !");
                return T3D_ERR_INVALID_PARAM;
            }
        }

        // 同格式、非 MSAA、非窗口、无需缩放时，优先走 glCopyImageSubData（支持 1D/2D/3D/Cube/Array 及深度）
        if (!src.isWindow && !dst.isWindow
            && src.texture != 0 && dst.texture != 0
            && src.sampleCount == 1 && dst.sampleCount == 1
            && src.internalFormat == dst.internalFormat
            && copySrcW == copyDstW && copySrcH == copyDstH)
        {
            const GLint glSrcY = (src.target == GL_TEXTURE_1D_ARRAY) ? static_cast<GLint>(srcOffZ) : static_cast<GLint>(srcOffY);
            const GLint glSrcZ = (src.target == GL_TEXTURE_1D_ARRAY) ? 0 : static_cast<GLint>(srcOffZ);
            const GLint glDstY = (dst.target == GL_TEXTURE_1D_ARRAY) ? static_cast<GLint>(dstOffZ) : static_cast<GLint>(dstOffY);
            const GLint glDstZ = (dst.target == GL_TEXTURE_1D_ARRAY) ? 0 : static_cast<GLint>(dstOffZ);
            const GLsizei glSrcH = (src.target == GL_TEXTURE_1D_ARRAY) ? static_cast<GLsizei>(copySrcD) : static_cast<GLsizei>(copySrcH);
            const GLsizei glSrcD = (src.target == GL_TEXTURE_1D_ARRAY) ? 1 : static_cast<GLsizei>(copySrcD);

            glCopyImageSubData(
                src.texture, src.target, 0, static_cast<GLint>(srcOffX), glSrcY, glSrcZ,
                dst.texture, dst.target, 0, static_cast<GLint>(dstOffX), glDstY, glDstZ,
                static_cast<GLsizei>(copySrcW), glSrcH, glSrcD);
            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::doBlit(glCopyImageSubData)");
            return T3D_OK;
        }

        GLint prevFBO = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

        do
        {
            GLuint readFBO = src.fbo;
            GLuint drawFBO = dst.fbo;

            if (src.needsScratchFbo)
            {
                if (mScratchReadFBO == 0)
                {
                    glGenFramebuffers(1, &mScratchReadFBO);
                }
                readFBO = mScratchReadFBO;
            }

            if (dst.needsScratchFbo)
            {
                if (mScratchDrawFBO == 0)
                {
                    glGenFramebuffers(1, &mScratchDrawFBO);
                }
                drawFBO = mScratchDrawFBO;
            }

            GLint srcX0 = static_cast<GLint>(srcOffX);
            GLint srcY0 = static_cast<GLint>(srcOffY);
            GLint srcX1 = srcX0 + static_cast<GLint>(copySrcW);
            GLint srcY1 = srcY0 + static_cast<GLint>(copySrcH);

            GLint dstX0 = static_cast<GLint>(dstOffX);
            GLint dstY0 = static_cast<GLint>(dstOffY);
            GLint dstX1 = dstX0 + static_cast<GLint>(copyDstW);
            GLint dstY1 = dstY0 + static_cast<GLint>(copyDstH);

            // 引擎纹理/FBO 统一采用上原点布局（row 0 为画面顶部，与 D3D11 一致），
            // 而默认窗口 backbuffer（FBO 0）为 OpenGL 原生下原点布局（row H-1 为顶部）。
            // 跨 Window <-> Texture/FBO 传输时必须做 Y 翻转；不能依赖易变的
            // mProjectionFlipped（reset() 或后处理 drawFullscreen() 会清零它，且存在
            // 多线程 RHI 队列竞态）。
            if (!src.isWindow && dst.isWindow)
            {
                GLint tmp = srcY0;
                srcY0 = srcY1;
                srcY1 = tmp;

                dstY0 = static_cast<GLint>(dst.height) - static_cast<GLint>(dstOffY + copyDstH);
                dstY1 = static_cast<GLint>(dst.height) - static_cast<GLint>(dstOffY);
            }
            else if (src.isWindow && !dst.isWindow)
            {
                srcY0 = static_cast<GLint>(src.height) - static_cast<GLint>(srcOffY);
                srcY1 = static_cast<GLint>(src.height) - static_cast<GLint>(srcOffY + copySrcH);
            }

            GLbitfield mask = GL_COLOR_BUFFER_BIT;
            if (src.isDepth)
            {
                mask = GL_DEPTH_BUFFER_BIT;
                if (src.hasStencil && dst.hasStencil)
                {
                    mask |= GL_STENCIL_BUFFER_BIT;
                }
            }

            const bool isScaled = (copySrcW != copyDstW || copySrcH != copyDstH);

            // MSAA 源一般直接 resolve 到目标；但 GL 要求此时源/目标矩形完全一致、格式相同，
            // 缩放或与窗口之间的 Y 翻转都会 GL_INVALID_OPERATION。这种情况先整张 resolve
            // 到 GLResolveFBO，再把它当单采样源走普通 blit。
            uint32_t srcSampleCount = src.sampleCount;
            if (src.sampleCount > 1 && !src.isDepth && src.fbo != 0 && src.resolveFbo != 0
                && (isScaled || dst.isWindow || src.internalFormat != dst.internalFormat))
            {
                glBindFramebuffer(GL_READ_FRAMEBUFFER, src.fbo);
                glReadBuffer(GL_COLOR_ATTACHMENT0);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, src.resolveFbo);
                glDrawBuffer(GL_COLOR_ATTACHMENT0);
                glBlitFramebuffer(0, 0, static_cast<GLint>(src.width), static_cast<GLint>(src.height),
                    0, 0, static_cast<GLint>(src.width), static_cast<GLint>(src.height),
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
                readFBO = src.resolveFbo;
                srcSampleCount = 1;
            }

            const GLenum filter = (!src.isDepth && srcSampleCount == 1 && dst.sampleCount == 1 && isScaled)
                ? GL_LINEAR
                : GL_NEAREST;

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

            for (uint32_t z = 0; z < copySrcD; ++z)
            {
                if (src.needsScratchFbo)
                {
                    attachScratchFBO(mScratchReadFBO, src, static_cast<GLint>(srcOffZ + z));
                }

                if (dst.needsScratchFbo)
                {
                    attachScratchFBO(mScratchDrawFBO, dst, static_cast<GLint>(dstOffZ + z));
                }

                bindRead(readFBO, src.isWindow, src.isDepth);
                bindDraw(drawFBO, dst.isWindow, dst.isDepth);
                glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1,
                    dstX0, dstY0, dstX1, dstY1,
                    mask, filter);
            }

            GL_CHECK_ERROR(LOG_TAG_GL4RENDERER, "GL4Context::doBlit");
        } while (false);

        glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
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

        releaseGLObjects({{GLObjectType::kBuffer, request.Staging}});
        request.Staging = 0;
        mPendingReadbacks.erase(itr);

        if (T3D_FAILED(ret))
        {
            dst.release();
        }

        return ret;
    }

    //--------------------------------------------------------------------------
}
