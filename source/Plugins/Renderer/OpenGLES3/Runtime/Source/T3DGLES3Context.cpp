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


#include "T3DGLES3Context.h"
#include "T3DGLES3RenderWindow.h"
#include "T3DGLES3Error.h"
#include "T3DGLES3RenderBuffer.h"
#include "T3DGLES3Mapping.h"
#include "T3DGLES3RenderState.h"
#include "T3DGLES3Shader.h"
#include "T3DGLES3Renderer.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>

#include <algorithm>
#include <cstring>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GLES3ContextPtr GLES3Context::create()
    {
        GLES3ContextPtr ctx = T3D_NEW GLES3Context();
        if (ctx && T3D_FAILED(ctx->init()))
        {
            ctx = nullptr;
        }
        return ctx;
    }

    //--------------------------------------------------------------------------

    GLES3Context::GLES3Context()
    {
    }

    //--------------------------------------------------------------------------

    GLES3Context::~GLES3Context()
    {
        mCurrentRenderTarget = nullptr;

        GL_SAFE_DELETE_PROGRAM(mCurrentProgram);
        GL_SAFE_DELETE_VAO(mCurrentVAO);

        if (mMainSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(mEGLDisplay, mMainSurface);
            mMainSurface = EGL_NO_SURFACE;
        }
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::init()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = initEGLContext();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Failed to init EGL context !");
                break;
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::swapBackBuffer(GLES3RenderWindow *renderWindow)
    {
        auto lambda = [this](GLES3RenderWindowPtr renderWindow)
        {
            TResult ret = T3D_OK;

            do
            {
                EGLSurface surface = renderWindow->getEGLSurface();
                if (surface == EGL_NO_SURFACE)
                {
                    ret = T3D_ERR_GLES3_PRESENT;
                    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "swapBackBuffer: EGL surface is invalid !");
                    break;
                }

                if (!eglSwapBuffers(mEGLDisplay, surface))
                {
                    ret = T3D_ERR_GLES3_PRESENT;
                    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "eglSwapBuffers failed ! EGL error=0x%04X", eglGetError());
                    break;
                }
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, GLES3RenderWindowPtr(renderWindow));
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::resizeRenderWindow(GLES3RenderWindow *rw, uint32_t w, uint32_t h)
    {
        rw->mWidth = w;
        rw->mHeight = h;

        auto lambda = [this](uint32_t w, uint32_t h)
        {
            TResult ret = T3D_OK;

            do
            {
                glViewport(0, 0, (GLsizei)w, (GLsizei)h);
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::resizeRenderWindow");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, w, h);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)
    {
        // GLES3 has no glClipControl — depth range stays [-1,1].
        // No Z remapping needed. When rendering to FBO, flip Y to match D3D orientation.
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

    const Matrix4& GLES3Context::getDepthRemapMatrix() const
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

    RHIRenderTargetPtr GLES3Context::createRenderWindow(RenderWindow *renderWindow)
    {
        GLES3RenderWindowPtr glRenderWindow = GLES3RenderWindow::create(renderWindow);

        if (glRenderWindow != nullptr)
        {
            mMainSurface = glRenderWindow->getEGLSurface();

            if (T3D_RHI_THREAD.isRunning())
            {
                eglMakeCurrent(mEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                T3D_RHI_THREAD.setThreadInitCallback([this, glRenderWindow]()
                {
                    EGLSurface surface = glRenderWindow->getEGLSurface();
                    if (!eglMakeCurrent(mEGLDisplay, surface, surface, mEGLContext))
                    {
                        T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "RHI thread: eglMakeCurrent failed !");
                    }
                    else
                    {
                        T3D_LOG_INFO(LOG_TAG_GLES3RENDERER, "RHI thread: EGL context acquired successfully.");
                    }
                });
            }
        }

        return glRenderWindow;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr GLES3Context::createRenderTexture(PixelBuffer2D *buffer)
    {
        GLES3PixelBuffer2DPtr glPixelBuffer = GLES3PixelBuffer2D::create();

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

        GLenum internalFmt = GLES3Mapping::getInternalFormat(desc.format);
        GLenum pixelFmt = GLES3Mapping::get(desc.format);
        GLenum pixelType = GLES3Mapping::getPixelType(desc.format);

        bool hasStencil = (desc.format == PixelFormat::E_PF_D24_UNORM_S8_UINT
            || desc.format == PixelFormat::E_PF_D32_FLOAT_S8X24_UINT);

        auto lambda = [this](const GLES3PixelBuffer2DPtr &glPixelBuffer,
            bool isColorRT, uint32_t msaaCount, uint32_t width, uint32_t height,
            GLenum internalFmt, GLenum pixelFmt, GLenum pixelType, bool hasStencil)
        {
            TResult ret = T3D_OK;

            do
            {
                if (isColorRT)
                {
                    if (msaaCount > 1)
                    {
                        glGenTextures(1, &glPixelBuffer->GLTexture);
                        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, glPixelBuffer->GLTexture);
                        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaCount,
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
                            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "MSAA color render texture FBO is not complete !");
                            ret = T3D_ERR_GLES3_CREATE_FBO;
                        }

                        glBindFramebuffer(GL_FRAMEBUFFER, 0);

                        glGenTextures(1, &glPixelBuffer->GLResolveTex);
                        glBindTexture(GL_TEXTURE_2D, glPixelBuffer->GLResolveTex);
                        glTexStorage2D(GL_TEXTURE_2D, 1, internalFmt, width, height);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glBindTexture(GL_TEXTURE_2D, 0);

                        glGenFramebuffers(1, &glPixelBuffer->GLResolveFBO);
                        glBindFramebuffer(GL_FRAMEBUFFER, glPixelBuffer->GLResolveFBO);
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D, glPixelBuffer->GLResolveTex, 0);

                        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                        {
                            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "MSAA resolve FBO is not complete !");
                            ret = T3D_ERR_GLES3_CREATE_FBO;
                        }

                        glBindFramebuffer(GL_FRAMEBUFFER, 0);

                        T3D_LOG_INFO(LOG_TAG_GLES3RENDERER, "Created MSAA render texture: %ux%u, %dx MSAA", width, height, msaaCount);
                    }
                    else
                    {
                        glGenTextures(1, &glPixelBuffer->GLTexture);
                        glBindTexture(GL_TEXTURE_2D, glPixelBuffer->GLTexture);
                        glTexStorage2D(GL_TEXTURE_2D, 1, internalFmt, width, height);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glBindTexture(GL_TEXTURE_2D, 0);

                        glGenFramebuffers(1, &glPixelBuffer->GLFBO);
                        glBindFramebuffer(GL_FRAMEBUFFER, glPixelBuffer->GLFBO);
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D, glPixelBuffer->GLTexture, 0);

                        glGenRenderbuffers(1, &glPixelBuffer->GLDepthRBO);
                        glBindRenderbuffer(GL_RENDERBUFFER, glPixelBuffer->GLDepthRBO);
                        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
                        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, glPixelBuffer->GLDepthRBO);

                        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                        {
                            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Color render texture FBO is not complete !");
                            ret = T3D_ERR_GLES3_CREATE_FBO;
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
                        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaCount,
                            internalFmt, width, height, GL_TRUE);
                        GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "Depth: glTexStorage2DMultisample");
                    }
                    else
                    {
                        glTexStorage2D(GL_TEXTURE_2D, 1, internalFmt, width, height);
                        GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "Depth: glTexStorage2D");
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
                    GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "Depth: glFramebufferTexture2D");

                    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
                    {
                        T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER,
                            "Depth render texture FBO is not complete ! status=0x%04X, "
                            "size=%ux%u, internalFmt=0x%04X, texFormat=0x%04X, pixelType=0x%04X, "
                            "attachment=0x%04X, texTarget=0x%04X, msaa=%u, tex=%u, fbo=%u",
                            fboStatus, width, height, internalFmt, texFormat, pixelType,
                            attachment, texTarget, msaaCount,
                            glPixelBuffer->GLTexture, glPixelBuffer->GLFBO);
                        ret = T3D_ERR_GLES3_CREATE_FBO;
                    }

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                }

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createRenderTexture");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glPixelBuffer,
            isColorRT, msaaCount, desc.width, desc.height,
            internalFmt, pixelFmt, pixelType, hasStencil);

        if (T3D_FAILED(ret))
        {
            glPixelBuffer = nullptr;
        }

        return glPixelBuffer;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setRenderTarget(RenderTarget *renderTarget)
    {
        TResult ret = T3D_OK;

        auto lambda = [this](RenderTargetPtr renderTarget)
        {
            TResult ret = T3D_OK;

            do
            {
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "setRenderTarget: ENTRY (stale error from outside)");

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
                            GLES3PixelBuffer2D *glPB = static_cast<GLES3PixelBuffer2D*>(
                                renderTarget->getRenderTexture()->getPixelBuffer()->getRHIResource().get());
                            if (glPB != nullptr && glPB->GLFBO != 0)
                            {
                                glBindFramebuffer(GL_FRAMEBUFFER, glPB->GLFBO);
                                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "setRenderTarget: glBindFramebuffer(color FBO)");

                                if (renderTarget->getDepthStencil() != nullptr)
                                {
                                    if (glPB->GLDepthRBO != 0)
                                    {
                                        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                            GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
                                        GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "setRenderTarget: glFramebufferRenderbuffer(detach RBO)");
                                    }

                                    GLES3PixelBuffer2D *glDS = static_cast<GLES3PixelBuffer2D*>(
                                        renderTarget->getDepthStencil()->getPixelBuffer()->getRHIResource().get());
                                    if (glDS != nullptr)
                                    {
                                        GLenum dsTexTarget = (glDS->GLMSAACount > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
                                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                            dsTexTarget, glDS->GLTexture, 0);
                                        GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "setRenderTarget: glFramebufferTexture2D(depth texture)");
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
                                            GLES3PixelBuffer2D *pb = static_cast<GLES3PixelBuffer2D*>(
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
                            GLES3PixelBuffer2D *glDS = static_cast<GLES3PixelBuffer2D*>(
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

                                GLenum none = GL_NONE;
                                glDrawBuffers(1, &none);
                                glReadBuffer(GL_NONE);
                                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "setRenderTarget: depth-only FBO");
                            }
                        }
                    }
                    break;
                default:
                    T3D_ASSERT(false);
                    break;
                }

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setRenderTarget");
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

    TResult GLES3Context::resetRenderTarget()
    {
        mCurrentRenderTarget = nullptr;

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

    TResult GLES3Context::setViewport(const Viewport &viewport)
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
                    T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "GLES3Context::setViewport: no color texture and depth stencil texture !");
                    return T3D_OK;
                }
            }
        }
        else
        {
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
                        T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER,
                            "GLES3Context::setViewport: no render target and GL viewport is zero");
                        break;
                    }
                    GLint x = static_cast<GLint>(vp.Left * w);
                    GLint y = static_cast<GLint>(vp.Top * h);
                    GLsizei gw = static_cast<GLsizei>(vp.Width * w);
                    GLsizei gh = static_cast<GLsizei>(vp.Height * h);
                    glViewport(x, y, gw, gh);
                    GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setViewport(fallback)");
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
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setViewport");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, x, y, w, h);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height)
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
            const GLint glY = fbHeight - (y + static_cast<int32_t>(height));
            auto lambda = [this](GLint x, GLint y, GLsizei w, GLsizei h)
            {
                glScissor(x, y, w, h);
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setScissorRect");
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda,
                static_cast<GLint>(x), glY,
                static_cast<GLsizei>(width), static_cast<GLsizei>(height));
        }

        auto lambda = [this](int32_t x, int32_t y, uint32_t width, uint32_t height)
        {
            GLint currentVP[4];
            glGetIntegerv(GL_VIEWPORT, currentVP);
            const int32_t fbH = currentVP[3];
            if (fbH <= 0)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER,
                    "GLES3Context::setScissorRect: no render target and GL viewport height is zero");
                return T3D_OK;
            }
            const GLint glY = fbH - (y + static_cast<int32_t>(height));
            glScissor(static_cast<GLint>(x), glY,
                static_cast<GLsizei>(width), static_cast<GLsizei>(height));
            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setScissorRect(fallback)");
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, x, y, width, height);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::clearColor(const ColorRGB &color)
    {
        auto lambda = [this](ColorRGB color)
        {
            TResult ret = T3D_OK;

            do
            {
                glClearColor(color.red(), color.green(), color.blue(), 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::clearColor");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, color);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::clearDepth(Real depth)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        auto lambda = [this](Real depth)
        {
            TResult ret = T3D_OK;

            do
            {
                glClearDepthf((GLfloat)depth);
                glClear(GL_DEPTH_BUFFER_BIT);
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::clearDepth");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, depth);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::clearDepthStencil(Real depth, uint32_t stencil)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        auto lambda = [this](Real depth, uint32_t stencil)
        {
            TResult ret = T3D_OK;

            do
            {
                glClearDepthf((GLfloat)depth);
                glClearStencil((GLint)stencil);
                glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::clearDepthStencil");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, depth, stencil);
    }

    //--------------------------------------------------------------------------

    RHIBlendStatePtr GLES3Context::createBlendState(BlendState *state)
    {
        GLES3BlendStatePtr glState = GLES3BlendState::create();

        const BlendDesc &desc = state->getStateDesc();
        const auto &rt0 = desc.RenderTargetStates[0];

        GLES3BlendStateData d {};
        d.enabled = rt0.BlendEnable;
        d.srcRGB = GLES3Mapping::get(rt0.SrcBlend);
        d.dstRGB = GLES3Mapping::get(rt0.DestBlend);
        d.opRGB = GLES3Mapping::get(rt0.BlendOp);
        d.srcAlpha = GLES3Mapping::get(rt0.SrcBlendAlpha);
        d.dstAlpha = GLES3Mapping::get(rt0.DstBlendAlpha);
        d.opAlpha = GLES3Mapping::get(rt0.BlendOpAlpha);
        d.colorMask[0] = (rt0.ColorMask & kWriteMaskRed) ? GL_TRUE : GL_FALSE;
        d.colorMask[1] = (rt0.ColorMask & kWriteMaskGreen) ? GL_TRUE : GL_FALSE;
        d.colorMask[2] = (rt0.ColorMask & kWriteMaskBlue) ? GL_TRUE : GL_FALSE;
        d.colorMask[3] = (rt0.ColorMask & kWriteMaskAlpha) ? GL_TRUE : GL_FALSE;

        auto lambda = [this](const GLES3BlendStatePtr &glState, GLES3BlendStateData d)
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

    RHIDepthStencilStatePtr GLES3Context::createDepthStencilState(DepthStencilState *state)
    {
        GLES3DepthStencilStatePtr glState = GLES3DepthStencilState::create();

        const DepthStencilDesc &desc = state->getStateDesc();

        GLES3DepthStencilStateData d {};
        d.depthTestEnabled = desc.DepthTestEnable;
        d.depthWriteEnabled = desc.DepthWriteEnable;
        d.depthFunc = GLES3Mapping::get(desc.DepthFunc);
        d.stencilEnabled = desc.StencilEnable;
        d.stencilReadMask = desc.StencilReadMask;
        d.stencilWriteMask = desc.StencilWriteMask;
        d.frontStencilFail = GLES3Mapping::get(desc.FrontFace.StencilFailOp);
        d.frontDepthFail = GLES3Mapping::get(desc.FrontFace.StencilDepthFailOp);
        d.frontStencilPass = GLES3Mapping::get(desc.FrontFace.StencilPassOp);
        d.frontStencilFunc = GLES3Mapping::get(desc.FrontFace.StencilFunc);
        d.backStencilFail = GLES3Mapping::get(desc.BackFace.StencilFailOp);
        d.backDepthFail = GLES3Mapping::get(desc.BackFace.StencilDepthFailOp);
        d.backStencilPass = GLES3Mapping::get(desc.BackFace.StencilPassOp);
        d.backStencilFunc = GLES3Mapping::get(desc.BackFace.StencilFunc);

        auto lambda = [this](const GLES3DepthStencilStatePtr &glState, GLES3DepthStencilStateData d)
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

    RHIRasterizerStatePtr GLES3Context::createRasterizerState(RasterizerState *state)
    {
        GLES3RasterizerStatePtr glState = GLES3RasterizerState::create();

        const RasterizerDesc &desc = state->getStateDesc();

        GLES3RasterizerStateData d {};
        d.cullMode = GLES3Mapping::get(desc.CullMode);
        d.cullEnabled = (desc.CullMode != CullingMode::kNone);
        d.frontCCW = desc.FrontAnticlockwise;
        d.scissorEnabled = desc.ScissorEnable;
        d.depthBias = static_cast<GLfloat>(desc.DepthBias);
        d.slopeScaledDepthBias = static_cast<GLfloat>(desc.SlopeScaledDepthBias);
        d.multisampleEnabled = desc.MultisampleEnable;

        auto lambda = [this](const GLES3RasterizerStatePtr &glState, GLES3RasterizerStateData d)
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

    RHISamplerStatePtr GLES3Context::createSamplerState(SamplerState *state)
    {
        GLES3SamplerStatePtr glState = GLES3SamplerState::create();

        const SamplerDesc &desc = state->getStateDesc();

        GLenum wrapS = GLES3Mapping::get(desc.AddressU);
        GLenum wrapT = GLES3Mapping::get(desc.AddressV);
        GLenum wrapR = GLES3Mapping::get(desc.AddressW);
        GLenum minFilter = GLES3Mapping::getMinFilter(desc.MinFilter, desc.MipFilter);
        GLenum magFilter = GLES3Mapping::getMagFilter(desc.MagFilter);
        GLfloat maxAniso = static_cast<GLfloat>(desc.MaxAnisotropy);
        GLfloat minLOD = desc.MinLOD;
        GLfloat maxLOD = desc.MaxLOD;
        bool isComparison = desc.IsComparison;
        GLenum compareFunc = GLES3Mapping::get(desc.CompareFunc);

        auto lambda = [this](const GLES3SamplerStatePtr &glState,
            GLenum wrapS, GLenum wrapT, GLenum wrapR,
            GLenum minFilter, GLenum magFilter, GLfloat maxAniso,
            GLfloat minLOD, GLfloat maxLOD,
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

                if (maxAniso > 1.0f)
                {
                    glSamplerParameterf(glState->GLSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
                }

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

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createSamplerState");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glState,
            wrapS, wrapT, wrapR, minFilter, magFilter, maxAniso,
            minLOD, maxLOD, isComparison, compareFunc);

        if (T3D_FAILED(ret))
        {
            glState = nullptr;
        }

        return glState;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setBlendState(BlendState *state)
    {
        GLES3BlendState *glState = static_cast<GLES3BlendState*>(state->getRHIState().get());
        const auto &d = glState->data;

        auto lambda = [this](GLES3BlendStateData d)
        {
            TResult ret = T3D_OK;

            do
            {
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

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setBlendState");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, d);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setDepthStencilState(DepthStencilState *state)
    {
        GLES3DepthStencilState *glState = static_cast<GLES3DepthStencilState*>(state->getRHIState().get());
        const auto &d = glState->data;

        auto lambda = [this](GLES3DepthStencilStateData d)
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

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setDepthStencilState");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, d);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setRasterizerState(RasterizerState *state)
    {
        GLES3RasterizerState *glState = static_cast<GLES3RasterizerState*>(state->getRHIState().get());
        const auto &d = glState->data;
        bool projFlipped = mProjectionFlipped;

        auto lambda = [this](GLES3RasterizerStateData d, bool projFlipped)
        {
            TResult ret = T3D_OK;

            do
            {
                // GLES3 has no glPolygonMode — always fill mode.
                // GLES3 has no GL_DEPTH_CLAMP.

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

                if (d.depthBias != 0.0f || d.slopeScaledDepthBias != 0.0f)
                {
                    glEnable(GL_POLYGON_OFFSET_FILL);
                    glPolygonOffset(d.slopeScaledDepthBias, d.depthBias);
                }
                else
                {
                    glDisable(GL_POLYGON_OFFSET_FILL);
                }

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setRasterizerState");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, d, projFlipped);
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr GLES3Context::createVertexDeclaration(VertexDeclaration *decl)
    {
        GLES3VertexDeclarationPtr glDecl = GLES3VertexDeclaration::create();

        auto lambda = [this](const GLES3VertexDeclarationPtr &glDecl)
        {
            TResult ret = T3D_OK;

            do
            {
                glGenVertexArrays(1, &glDecl->GLVAO);
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createVertexDeclaration");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glDecl);
        if (T3D_FAILED(ret)) { glDecl = nullptr; }
        return glDecl;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setVertexDeclaration(VertexDeclaration *decl)
    {
        GLES3VertexDeclarationPtr glDecl = static_cast<GLES3VertexDeclaration*>(decl->getRHIResource().get());
        mPendingVertexDecl = decl;

        auto lambda = [this](const GLES3VertexDeclarationPtr &glDecl)
        {
            TResult ret = T3D_OK;

            do
            {
                GLuint vao = glDecl->GLVAO;
                mCurrentVAO = vao;
                glBindVertexArray(vao);
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setVertexDeclaration");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glDecl);
    }

    //--------------------------------------------------------------------------

    RHIVertexBufferPtr GLES3Context::createVertexBuffer(VertexBuffer *buffer)
    {
        GLES3VertexBufferPtr glBuffer = GLES3VertexBuffer::create();

        GLenum usage = GLES3Mapping::getBufferUsage(buffer->getUsage());

        auto lambda = [this](const GLES3VertexBufferPtr &glBuffer, const VertexBufferPtr &buffer, GLenum usage)
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
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createVertexBuffer");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, VertexBufferPtr(buffer), usage);
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)
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
                    GLES3VertexBuffer *glVB = static_cast<GLES3VertexBuffer*>(buffers[i]->getRHIResource().get());
                    glBindBuffer(GL_ARRAY_BUFFER, glVB->GLBuffer);

                    if (pendingDecl != nullptr)
                    {
                        for (uint32_t j = 0; j < pendingDecl->getAttributeCount(); ++j)
                        {
                            const VertexAttribute &attrib = pendingDecl->getAttributes()[j];
                            if (attrib.getSlot() != startSlot + i)
                                continue;

                            GLint size = GLES3Mapping::getVertexAttribSize(attrib.getType());
                            GLenum type = GLES3Mapping::getVertexAttribType(attrib.getType());
                            GLboolean normalized = GLES3Mapping::getVertexAttribNormalized(attrib.getType());

                            glEnableVertexAttribArray(j);
                            if (GLES3Mapping::isIntegerAttrib(attrib.getType()))
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
                        }
                    }
                }

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setVertexBuffers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, buffers, strides, offsets, pendingDecl);
    }

    //--------------------------------------------------------------------------

    RHIIndexBufferPtr GLES3Context::createIndexBuffer(IndexBuffer *buffer)
    {
        GLES3IndexBufferPtr glBuffer = GLES3IndexBuffer::create();

        GLenum usage = GLES3Mapping::getBufferUsage(buffer->getUsage());

        auto lambda = [this](const GLES3IndexBufferPtr &glBuffer, const IndexBufferPtr &buffer, GLenum usage)
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
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createIndexBuffer");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, IndexBufferPtr(buffer), usage);
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setIndexBuffer(IndexBuffer *buffer)
    {
        GLES3IndexBufferPtr glIB = static_cast<GLES3IndexBuffer*>(buffer->getRHIResource().get());

        mIndexType = GLES3Mapping::get(buffer->getIndexType());
        mIndexSize = (buffer->getIndexType() == IndexType::E_IT_16BITS) ? 2 : 4;

        auto lambda = [this](const GLES3IndexBufferPtr &glIB)
        {
            TResult ret = T3D_OK;

            do
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIB->GLBuffer);
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setIndexBuffer");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glIB);
    }

    //--------------------------------------------------------------------------

    RHIConstantBufferPtr GLES3Context::createConstantBuffer(ConstantBuffer *buffer)
    {
        GLES3ConstantBufferPtr glBuffer = GLES3ConstantBuffer::create();

        GLenum usage = GLES3Mapping::getBufferUsage(buffer->getUsage());

        auto lambda = [this](const GLES3ConstantBufferPtr &glBuffer, const ConstantBufferPtr &buffer, GLenum usage)
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
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createConstantBuffer");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, ConstantBufferPtr(buffer), usage);
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer1DPtr GLES3Context::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        GLES3PixelBuffer1DPtr glBuffer = GLES3PixelBuffer1D::create();

        auto lambda = [this](const GLES3PixelBuffer1DPtr &glBuffer, const PixelBuffer1DPtr &buffer)
        {
            TResult ret = T3D_OK;

            do
            {
                const auto &desc = buffer->getDescriptor();
                const uint8_t *uploadData = buffer->getBuffer().Data;
                uint8_t *convertedData = nullptr;

                if (GLES3Mapping::isBGRAFormat(desc.format)
                    && uploadData != nullptr)
                {
                    size_t dataSize = buffer->getBuffer().DataSize;
                    convertedData = new uint8_t[dataSize];
                    memcpy(convertedData, uploadData, dataSize);
                    uint32_t bpp = (desc.format == PixelFormat::E_PF_B8G8R8) ? 3 : 4;
                    for (size_t i = 0; i + 2 < dataSize; i += bpp)
                        std::swap(convertedData[i], convertedData[i + 2]);
                    uploadData = convertedData;
                }

                // GLES3 has no GL_TEXTURE_1D — simulate with height=1 2D texture
                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(GL_TEXTURE_2D, glBuffer->GLTexture);
                glTexImage2D(GL_TEXTURE_2D, 0,
                    GLES3Mapping::getInternalFormat(desc.format),
                    desc.width, 1, 0,
                    GLES3Mapping::get(desc.format),
                    GLES3Mapping::getPixelType(desc.format),
                    uploadData);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glBindTexture(GL_TEXTURE_2D, 0);

                delete[] convertedData;

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createPixelBuffer1D");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, PixelBuffer1DPtr(buffer));
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr GLES3Context::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        GLES3PixelBuffer2DPtr glBuffer = GLES3PixelBuffer2D::create();

        auto lambda = [this](const GLES3PixelBuffer2DPtr &glBuffer, const PixelBuffer2DPtr &buffer)
        {
            TResult ret = T3D_OK;

            do
            {
                const auto &desc = buffer->getDescriptor();
                const uint8_t *uploadData = buffer->getBuffer().Data;
                uint8_t *convertedData = nullptr;

                if (GLES3Mapping::isBGRAFormat(desc.format)
                    && uploadData != nullptr)
                {
                    size_t dataSize = buffer->getBuffer().DataSize;
                    convertedData = new uint8_t[dataSize];
                    memcpy(convertedData, uploadData, dataSize);
                    uint32_t bpp = (desc.format == PixelFormat::E_PF_B8G8R8) ? 3 : 4;
                    for (size_t i = 0; i + 2 < dataSize; i += bpp)
                        std::swap(convertedData[i], convertedData[i + 2]);
                    uploadData = convertedData;
                }

                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(GL_TEXTURE_2D, glBuffer->GLTexture);
                glTexImage2D(GL_TEXTURE_2D, 0,
                    GLES3Mapping::getInternalFormat(desc.format),
                    desc.width, desc.height, 0,
                    GLES3Mapping::get(desc.format),
                    GLES3Mapping::getPixelType(desc.format),
                    uploadData);

                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);

                delete[] convertedData;

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createPixelBuffer2D");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, PixelBuffer2DPtr(buffer));
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer3DPtr GLES3Context::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        GLES3PixelBuffer3DPtr glBuffer = GLES3PixelBuffer3D::create();

        auto lambda = [this](const GLES3PixelBuffer3DPtr &glBuffer, const PixelBuffer3DPtr &buffer)
        {
            TResult ret = T3D_OK;

            do
            {
                const auto &desc = buffer->getDescriptor();

                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(GL_TEXTURE_3D, glBuffer->GLTexture);
                glTexImage3D(GL_TEXTURE_3D, 0,
                    GLES3Mapping::getInternalFormat(desc.format),
                    desc.width, desc.height, desc.depth, 0,
                    GLES3Mapping::get(desc.format),
                    GLES3Mapping::getPixelType(desc.format),
                    buffer->getBuffer().Data);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_3D, 0);

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createPixelBuffer3D");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glBuffer, PixelBuffer3DPtr(buffer));
        if (T3D_FAILED(ret)) { glBuffer = nullptr; }
        return glBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBufferCubemapPtr GLES3Context::createPixelBufferCubemap(PixelBufferCubemap *buffer)
    {
        GLES3PixelBufferCubemapPtr glBuffer = GLES3PixelBufferCubemap::create();

        auto lambda = [this](const GLES3PixelBufferCubemapPtr &glBuffer, const PixelBufferCubemapPtr &buffer)
        {
            TResult ret = T3D_OK;

            do
            {
                const auto &desc = buffer->getDescriptor();
                const uint8_t *uploadData = buffer->getBuffer().Data;
                uint8_t *convertedData = nullptr;

                if (GLES3Mapping::isBGRAFormat(desc.format)
                    && uploadData != nullptr)
                {
                    size_t dataSize = buffer->getBuffer().DataSize;
                    convertedData = new uint8_t[dataSize];
                    memcpy(convertedData, uploadData, dataSize);
                    uint32_t bpp = (desc.format == PixelFormat::E_PF_B8G8R8) ? 3 : 4;
                    for (size_t i = 0; i + 2 < dataSize; i += bpp)
                        std::swap(convertedData[i], convertedData[i + 2]);
                    uploadData = convertedData;
                }

                const size_t bpp = Image::getBPP(desc.format) / 8;
                const size_t faceSize = (size_t)desc.width * desc.height * bpp;

                glGenTextures(1, &glBuffer->GLTexture);
                glBindTexture(GL_TEXTURE_CUBE_MAP, glBuffer->GLTexture);

                for (uint32_t face = 0; face < PixelBufferCubemap::FACE_COUNT; ++face)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
                        GLES3Mapping::getInternalFormat(desc.format),
                        desc.width, desc.height, 0,
                        GLES3Mapping::get(desc.format),
                        GLES3Mapping::getPixelType(desc.format),
                        uploadData != nullptr ? uploadData + face * faceSize : nullptr);
                }

                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

                delete[] convertedData;

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createPixelBufferCubemap");
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

    RHIShaderPtr GLES3Context::createVertexShader(ShaderVariant *shader)
    {
        GLES3VertexShaderPtr glShader = GLES3VertexShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GLES3VertexShaderPtr &glShader, String shaderSource)
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
                        T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Vertex shader compile error: %s", log.data());
                    }
                    GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
                    ret = T3D_ERR_GLES3_COMPILE_SHADER;
                    break;
                }

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createVertexShader");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setVertexShader(ShaderVariant *shader)
    {
        GLES3Shader *glShader = static_cast<GLES3Shader*>(shader->getRHIShader());
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
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setVertexShader");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, shaderHandle, shader);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(buffers);
    }

    TResult GLES3Context::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GLES3Context::setVSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    // Pixel Shader
    //--------------------------------------------------------------------------

    RHIShaderPtr GLES3Context::createPixelShader(ShaderVariant *shader)
    {
        GLES3PixelShaderPtr glShader = GLES3PixelShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GLES3PixelShaderPtr &glShader, String shaderSource)
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
                        T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Pixel shader compile error: %s", log.data());
                    }
                    GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
                    ret = T3D_ERR_GLES3_COMPILE_SHADER;
                    break;
                }

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createPixelShader");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setPixelShader(ShaderVariant *shader)
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

        GLES3Shader *glShader = static_cast<GLES3Shader*>(shader->getRHIShader());
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
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setPixelShader");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, shaderHandle, shader);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(buffers);
    }

    TResult GLES3Context::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GLES3Context::setPSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    // Hull Shader (not supported in GLES3)
    //--------------------------------------------------------------------------

    RHIShaderPtr GLES3Context::createHullShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    TResult GLES3Context::setHullShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GLES3Context::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GLES3Context::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GLES3Context::setHSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }

    //--------------------------------------------------------------------------
    // Domain Shader (not supported in GLES3)
    //--------------------------------------------------------------------------

    RHIShaderPtr GLES3Context::createDomainShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    TResult GLES3Context::setDomainShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GLES3Context::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GLES3Context::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GLES3Context::setDSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }

    //--------------------------------------------------------------------------
    // Geometry Shader (requires GLES 3.2)
    //--------------------------------------------------------------------------

    RHIShaderPtr GLES3Context::createGeometryShader(ShaderVariant *shader)
    {
        if (mGLESMinor < 2)
        {
            T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "Geometry shader requires GLES 3.2, current: 3.%d", mGLESMinor);
            return nullptr;
        }

        GLES3GeometryShaderPtr glShader = GLES3GeometryShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GLES3GeometryShaderPtr &glShader, String shaderSource)
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
                        T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Geometry shader compile error: %s", log.data());
                    }
                    GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
                    ret = T3D_ERR_GLES3_COMPILE_SHADER;
                    break;
                }

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createGeometryShader");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setGeometryShader(ShaderVariant *shader)
    {
        if (mGLESMinor < 2)
            return T3D_OK;

        if (shader == nullptr)
            return T3D_OK;

        GLES3Shader *glShader = static_cast<GLES3Shader*>(shader->getRHIShader());
        GLuint shaderHandle = glShader->GLShaderHandle;

        auto lambda = [this](GLuint shaderHandle)
        {
            TResult ret = T3D_OK;

            do
            {
                if (mCurrentProgram == 0)
                {
                    mCurrentProgram = glCreateProgram();
                }
                glAttachShader(mCurrentProgram, shaderHandle);
                mProgramDirty = true;
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setGeometryShader");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, shaderHandle);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(buffers);
    }

    TResult GLES3Context::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GLES3Context::setGSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    // Compute Shader (requires GLES 3.1)
    //--------------------------------------------------------------------------

    RHIShaderPtr GLES3Context::createComputeShader(ShaderVariant *shader)
    {
        if (mGLESMinor < 1)
        {
            T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "Compute shader requires GLES 3.1, current: 3.%d", mGLESMinor);
            return nullptr;
        }

        GLES3ComputeShaderPtr glShader = GLES3ComputeShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GLES3ComputeShaderPtr &glShader, String shaderSource)
        {
            TResult ret = T3D_OK;

            do
            {
                const char *src = shaderSource.c_str();
                GLint len = static_cast<GLint>(shaderSource.size());

                glShader->GLShaderHandle = glCreateShader(GL_COMPUTE_SHADER);
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
                        T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Compute shader compile error: %s", log.data());
                    }
                    GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
                    ret = T3D_ERR_GLES3_COMPILE_SHADER;
                    break;
                }

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createComputeShader");
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setComputeShader(ShaderVariant *shader)
    {
        if (mGLESMinor < 1)
            return T3D_OK;

        if (shader == nullptr)
            return T3D_OK;

        GLES3Shader *glShader = static_cast<GLES3Shader*>(shader->getRHIShader());
        GLuint shaderHandle = glShader->GLShaderHandle;

        auto lambda = [this](GLuint shaderHandle)
        {
            TResult ret = T3D_OK;

            do
            {
                if (mCurrentProgram == 0)
                {
                    mCurrentProgram = glCreateProgram();
                }
                glAttachShader(mCurrentProgram, shaderHandle);
                mProgramDirty = true;
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setComputeShader");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, shaderHandle);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return stageConstantBuffers(buffers);
    }

    TResult GLES3Context::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GLES3Context::setCSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return bindSamplers(startSlot, samplers);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::compileShader(ShaderVariant *shader)
    {
        return GLES3ContextBase::compileShader(shader);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
    {
        return GLES3ContextBase::reflectShaderAllBindings(shader, constantParams, samplerParams);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)
    {
        return GLES3ContextBase::reflectSamplerBindings(shader, samplerParams);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::setPrimitiveType(PrimitiveType primitive)
    {
        GLenum glPrimitive = GLES3Mapping::get(primitive);

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

    TResult GLES3Context::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
    {
        auto lambda = [this](uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
        {
            TResult ret = T3D_OK;

            do
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
                            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Program link error: %s", log.data());
                        }
                        ret = T3D_ERR_GLES3_LINK_PROGRAM;
                        break;
                    }

                    glUseProgram(mCurrentProgram);
                    bindPendingUniformBlocks(mCurrentProgram);
                    setupSamplerBindings(mCurrentProgram);
                    mProgramDirty = false;
                }

                const void *offset = reinterpret_cast<const void*>((uintptr_t)(startIndex * mIndexSize));

                GLint boundEBO = 0;
                glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundEBO);
                if (boundEBO == 0)
                {
                    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::render(indexed): No EBO bound! Skipping draw to avoid crash.");
                    ret = T3D_ERR_GLES3_INVALID_USAGE;
                    break;
                }

                if (baseVertex != 0 && mGLESMinor >= 2)
                {
                    glDrawElementsBaseVertex(mPrimitiveType, indexCount, mIndexType, offset, baseVertex);
                }
                else
                {
                    glDrawElements(mPrimitiveType, indexCount, mIndexType, offset);
                }

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::render(indexed)");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, indexCount, startIndex, baseVertex);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::render(uint32_t vertexCount, uint32_t startVertex)
    {
        auto lambda = [this](uint32_t vertexCount, uint32_t startVertex)
        {
            TResult ret = T3D_OK;

            do
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
                            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Program link error: %s", log.data());
                        }
                        ret = T3D_ERR_GLES3_LINK_PROGRAM;
                        break;
                    }

                    glUseProgram(mCurrentProgram);
                    bindPendingUniformBlocks(mCurrentProgram);
                    setupSamplerBindings(mCurrentProgram);
                    mProgramDirty = false;
                }

                glDrawArrays(mPrimitiveType, startVertex, vertexCount);

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::render(non-indexed)");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, vertexCount, startVertex);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::reset()
    {
        mCurrentRenderTarget = nullptr;

        auto lambda = [this]()
        {
            TResult ret = T3D_OK;

            do
            {
                mCurrentVSVariant = nullptr;
                mCurrentPSVariant = nullptr;

                glUseProgram(0);
                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                GL_SAFE_DELETE_PROGRAM(mCurrentProgram);
                mPendingUBOs.clear();
                mProgramDirty = false;
                mCurrentVAO = 0;

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::reset");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    //--------------------------------------------------------------------------
    // Blit operations
    //--------------------------------------------------------------------------

    TResult GLES3Context::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
            return T3D_ERR_INVALID_PARAM;

        Texture2D *tex2D = static_cast<Texture2D*>(src);
        GLES3PixelBuffer2D *glSrcPB = static_cast<GLES3PixelBuffer2D*>(
            tex2D->getPixelBuffer()->getRHIResource().get());
        if (glSrcPB == nullptr || glSrcPB->GLFBO == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "blit: source texture has no FBO");
            return T3D_ERR_INVALID_PARAM;
        }

        GLuint dstFBO = 0;
        GLsizei dstWidth = 0, dstHeight = 0;

        if (dst->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            dstFBO = 0;
            dstWidth = static_cast<GLsizei>(dst->getRenderWindow()->getDescriptor().Width);
            dstHeight = static_cast<GLsizei>(dst->getRenderWindow()->getDescriptor().Height);
        }
        else if (dst->getNumOfRenderTextures() > 0)
        {
            GLES3PixelBuffer2D *glDstPB = static_cast<GLES3PixelBuffer2D*>(
                dst->getRenderTexture()->getPixelBuffer()->getRHIResource().get());
            if (glDstPB != nullptr)
            {
                dstFBO = glDstPB->GLFBO;
                dstWidth = static_cast<GLsizei>(dst->getRenderTexture()->getWidth());
                dstHeight = static_cast<GLsizei>(dst->getRenderTexture()->getHeight());
            }
        }

        GLint srcX0 = static_cast<GLint>(srcOffset.x());
        GLint srcY0 = static_cast<GLint>(srcOffset.y());
        GLint srcX1 = srcX0 + static_cast<GLint>(size.x());
        GLint srcY1 = srcY0 + static_cast<GLint>(size.y());

        bool flipY = mProjectionFlipped && (dst->getType() == RenderTarget::Type::E_RT_WINDOW);
        if (flipY)
        {
            GLint tmp = srcY0;
            srcY0 = srcY1;
            srcY1 = tmp;
        }

        GLint dstX0 = static_cast<GLint>(dstOffset.x());
        GLint dstY0 = static_cast<GLint>(dstOffset.y());
        GLint dstX1 = dstX0 + static_cast<GLint>(size.x());
        GLint dstY1 = dstY0 + static_cast<GLint>(size.y());

        bool needResolve = (glSrcPB->GLMSAACount > 1 && glSrcPB->GLResolveFBO != 0);
        GLuint resolveFBO = glSrcPB->GLResolveFBO;
        GLuint srcFBO = glSrcPB->GLFBO;
        GLint texW = static_cast<GLint>(tex2D->getWidth());
        GLint texH = static_cast<GLint>(tex2D->getHeight());

        auto lambda = [this](GLuint srcFBO, GLuint resolveFBO, GLuint dstFBO,
            bool needResolve, GLint texW, GLint texH,
            GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
            GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1)
        {
            TResult ret = T3D_OK;

            do
            {
                GLuint readFBO = srcFBO;

                if (needResolve)
                {
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO);
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
                    glBlitFramebuffer(0, 0, texW, texH, 0, 0, texW, texH,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
                    GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "blit: MSAA resolve");
                    readFBO = resolveFBO;
                }

                glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFBO);

                glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1,
                    dstX0, dstY0, dstX1, dstY1,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);

                // TBR optimization: invalidate read framebuffer attachments after blit
                GLenum discards[] = { GL_COLOR_ATTACHMENT0 };
                glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 1, discards);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::blit(Texture->RenderTarget)");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, srcFBO, resolveFBO, dstFBO,
            needResolve, texW, texH,
            srcX0, srcY0, srcX1, srcY1,
            dstX0, dstY0, dstX1, dstY1);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)
    {
        if (src == nullptr || dst == nullptr)
            return T3D_ERR_INVALID_PARAM;

        auto srcRHI = src->getRHIResource();
        auto dstRHI = dst->getRHIResource();

        auto lambda = [this](const RHIResourcePtr &srcRHI, const RHIResourcePtr &dstRHI,
            size_t srcOffset, size_t size, size_t dstOffset)
        {
            TResult ret = T3D_OK;

            do
            {
                GLuint srcBuf = 0, dstBuf = 0;

                switch (srcRHI->getResourceType())
                {
                case RHIResource::ResourceType::kVertexBuffer:
                    srcBuf = static_cast<GLES3VertexBuffer*>(srcRHI.get())->GLBuffer;
                    break;
                case RHIResource::ResourceType::kIndexBuffer:
                    srcBuf = static_cast<GLES3IndexBuffer*>(srcRHI.get())->GLBuffer;
                    break;
                case RHIResource::ResourceType::kConstantBuffer:
                    srcBuf = static_cast<GLES3ConstantBuffer*>(srcRHI.get())->GLBuffer;
                    break;
                default:
                    break;
                }

                switch (dstRHI->getResourceType())
                {
                case RHIResource::ResourceType::kVertexBuffer:
                    dstBuf = static_cast<GLES3VertexBuffer*>(dstRHI.get())->GLBuffer;
                    break;
                case RHIResource::ResourceType::kIndexBuffer:
                    dstBuf = static_cast<GLES3IndexBuffer*>(dstRHI.get())->GLBuffer;
                    break;
                case RHIResource::ResourceType::kConstantBuffer:
                    dstBuf = static_cast<GLES3ConstantBuffer*>(dstRHI.get())->GLBuffer;
                    break;
                default:
                    break;
                }

                if (srcBuf == 0 || dstBuf == 0)
                    break;

                glBindBuffer(GL_COPY_READ_BUFFER, srcBuf);
                glBindBuffer(GL_COPY_WRITE_BUFFER, dstBuf);
                glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                    (GLintptr)srcOffset, (GLintptr)dstOffset, (GLsizeiptr)size);
                glBindBuffer(GL_COPY_READ_BUFFER, 0);
                glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::copyBuffer");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, srcRHI, dstRHI, srcOffset, size, dstOffset);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)
    {
        TResult ret = T3D_OK;

        auto rhiRes = renderBuffer->getRHIResource();
        bool isTexture = false;

        switch (rhiRes->getResourceType())
        {
        case RHIResource::ResourceType::kVertexBuffer:
        case RHIResource::ResourceType::kIndexBuffer:
        case RHIResource::ResourceType::kConstantBuffer:
            break;
        case RHIResource::ResourceType::kPixelBuffer2D:
            isTexture = true;
            break;
        default:
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Unsupported resource type for writeBuffer");
            return T3D_ERR_GLES3_INVALID_USAGE;
        }

        Buffer ownedBuffer;
        ownedBuffer.DataSize = buffer.DataSize;
        ownedBuffer.Data = T3D_POD_NEW_ARRAY(uint8_t, buffer.DataSize);
        memcpy(ownedBuffer.Data, buffer.Data, buffer.DataSize);

        if (isTexture)
        {
            GLES3PixelBuffer2DPtr glTex = static_cast<GLES3PixelBuffer2D*>(rhiRes.get());

            auto lambda = [this](const GLES3PixelBuffer2DPtr &glTex, Buffer ownedBuffer)
            {
                TResult ret = T3D_OK;

                do
                {
                    glBindTexture(GL_TEXTURE_2D, glTex->GLTexture);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        0, 0,
                        GL_RGBA, GL_UNSIGNED_BYTE, ownedBuffer.Data);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::writeBuffer(texture)");
                } while (false);

                T3D_POD_SAFE_DELETE_ARRAY(ownedBuffer.Data);
                return ret;
            };

            return ENQUEUE_UNIQUE_COMMAND(lambda, glTex, ownedBuffer);
        }

        // No DSA in GLES3 — use explicit bind/unbind with GL_COPY_WRITE_BUFFER
        // to avoid disturbing VAO state
        auto rhiResPtr = rhiRes;

        auto lambda = [this](const RHIResourcePtr &rhiResPtr, Buffer ownedBuffer, bool discardWholeBuffer)
        {
            TResult ret = T3D_OK;

            do
            {
                GLuint glBuf = 0;

                switch (rhiResPtr->getResourceType())
                {
                case RHIResource::ResourceType::kVertexBuffer:
                    glBuf = static_cast<GLES3VertexBuffer*>(rhiResPtr.get())->GLBuffer;
                    break;
                case RHIResource::ResourceType::kIndexBuffer:
                    glBuf = static_cast<GLES3IndexBuffer*>(rhiResPtr.get())->GLBuffer;
                    break;
                case RHIResource::ResourceType::kConstantBuffer:
                    glBuf = static_cast<GLES3ConstantBuffer*>(rhiResPtr.get())->GLBuffer;
                    break;
                default:
                    break;
                }

                if (glBuf == 0)
                {
                    T3D_POD_SAFE_DELETE_ARRAY(ownedBuffer.Data);
                    break;
                }

                glBindBuffer(GL_COPY_WRITE_BUFFER, glBuf);

                if (discardWholeBuffer)
                {
                    glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)ownedBuffer.DataSize, ownedBuffer.Data, GL_DYNAMIC_DRAW);
                }
                else
                {
                    glBufferSubData(GL_COPY_WRITE_BUFFER, 0, (GLsizeiptr)ownedBuffer.DataSize, ownedBuffer.Data);
                }

                glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::writeBuffer");
            } while (false);

            T3D_POD_SAFE_DELETE_ARRAY(ownedBuffer.Data);
            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, rhiResPtr, ownedBuffer, discardWholeBuffer);
    }

    //--------------------------------------------------------------------------

    void* GLES3Context::getNativeContext() const
    {
        return (void *)mEGLContext;
    }

    //--------------------------------------------------------------------------

    void GLES3Context::restoreNativeContext()
    {
        if (mEGLContext != EGL_NO_CONTEXT && mMainSurface != EGL_NO_SURFACE)
        {
            eglMakeCurrent(mEGLDisplay, mMainSurface, mMainSurface, mEGLContext);
        }
    }

    //--------------------------------------------------------------------------
    // Helper methods for resource binding
    //--------------------------------------------------------------------------

    void GLES3Context::bindPendingUniformBlocks(GLuint program)
    {
        GLint numBlocks = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);

        T3D_LOG_DEBUG(LOG_TAG_GLES3RENDERER, "bindPendingUniformBlocks: program=%u numBlocks=%d pendingUBOs=%d",
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
                T3D_LOG_DEBUG(LOG_TAG_GLES3RENDERER, "  UBO bound: block='%s' -> '%s' bindingPoint=%u glBuffer=%u",
                    blockName, cbufferName.c_str(), bindingPoint, it->second);
            }
            else
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER,
                    "No pending UBO for uniform block '%s' (cbuffer name '%s')",
                    blockName, cbufferName.c_str());
            }
        }

        GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::bindPendingUniformBlocks");
    }

    //--------------------------------------------------------------------------

    void GLES3Context::setupSamplerBindings(GLuint program)
    {
        GLint numUniforms = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

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

        // GLES3 用 SPIRV-Cross 合并采样器(sampler2D)，纹理/采样器对象/uniform 必须位于同一个
        // GL 纹理单元。纹理由 bindPixelBuffers 绑在 texBinding 单元，所以 sampler uniform 也必须
        // 指向 texBinding，而不是 HLSL 的 s# 寄存器(samplerBinding)。
        auto findSlot = [this](const String &texName) -> int32_t
        {
            if (mCurrentPSVariant != nullptr)
            {
                const auto &params = mCurrentPSVariant->getShaderSamplerParams();
                const auto itr = params.find(texName);
                if (itr != params.end())
                {
                    return static_cast<int32_t>(itr->second->getTexBinding());
                }
            }
            if (mCurrentVSVariant != nullptr)
            {
                const auto &params = mCurrentVSVariant->getShaderSamplerParams();
                const auto itr = params.find(texName);
                if (itr != params.end())
                {
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

            bool isSampler = (uniformType == GL_SAMPLER_2D
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
                        texUnit = fallbackTexUnit;
                        T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER,
                            "setupSamplerBindings: could not find slot for sampler='%s' texName='%s', fallback texUnit=%d",
                            name, texName.c_str(), texUnit);
                    }

                    glUniform1i(loc, texUnit);
                    T3D_LOG_DEBUG(LOG_TAG_GLES3RENDERER, "setupSamplerBindings: sampler='%s' texName='%s' loc=%d texUnit=%d",
                        name, texName.c_str(), loc, texUnit);
                    fallbackTexUnit++;
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::stageConstantBuffers(const ConstantBuffers &buffers)
    {
        using UBOBinding = std::pair<String, GLuint>;
        TArray<UBOBinding> uboBindings;
        uboBindings.reserve(buffers.size());

        for (uint32_t i = 0; i < buffers.size(); ++i)
        {
            GLES3ConstantBuffer *glCB = static_cast<GLES3ConstantBuffer*>(buffers[i]->getRHIResource().get());
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

    TResult GLES3Context::bindPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        T3D_LOG_DEBUG(LOG_TAG_GLES3RENDERER, "bindPixelBuffers: startSlot=%d bufferCount=%d",
            startSlot, (int)buffers.size());

        struct TexBinding { GLuint handle; GLenum target; };
        TArray<TexBinding> bindings;
        bindings.reserve(buffers.size());

        for (uint32_t i = 0; i < buffers.size(); ++i)
        {
            if (buffers[i] == nullptr)
            {
                bindings.push_back({0, GL_TEXTURE_2D});
                continue;
            }

            GLuint texHandle = 0;
            GLenum texTarget = GL_TEXTURE_2D;

            switch (buffers[i]->getRHIResource()->getResourceType())
            {
            case RHIResource::ResourceType::kPixelBuffer1D:
                texHandle = static_cast<GLES3PixelBuffer1D*>(buffers[i]->getRHIResource().get())->GLTexture;
                texTarget = GL_TEXTURE_2D;  // simulated as 2D with height=1
                break;
            case RHIResource::ResourceType::kPixelBuffer2D:
                texHandle = static_cast<GLES3PixelBuffer2D*>(buffers[i]->getRHIResource().get())->GLTexture;
                texTarget = GL_TEXTURE_2D;
                break;
            case RHIResource::ResourceType::kPixelBuffer3D:
                texHandle = static_cast<GLES3PixelBuffer3D*>(buffers[i]->getRHIResource().get())->GLTexture;
                texTarget = GL_TEXTURE_3D;
                break;
            case RHIResource::ResourceType::kPixelBufferCubemap:
                texHandle = static_cast<GLES3PixelBufferCubemap*>(buffers[i]->getRHIResource().get())->GLTexture;
                texTarget = GL_TEXTURE_CUBE_MAP;
                break;
            default:
                break;
            }

            bindings.push_back({texHandle, texTarget});
        }

        auto lambda = [this](uint32_t startSlot, TArray<TexBinding> bindings)
        {
            TResult ret = T3D_OK;

            do
            {
                for (uint32_t i = 0; i < bindings.size(); ++i)
                {
                    if (bindings[i].handle == 0) continue;
                    glActiveTexture(GL_TEXTURE0 + startSlot + i);
                    glBindTexture(bindings[i].target, bindings[i].handle);
                }
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::bindPixelBuffers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, bindings);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::bindSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        // GLES3 合并采样器：采样器对象必须与纹理位于同一个 GL 纹理单元(texBinding)，
        // 而不是 HLSL 的 s# 寄存器(samplerBinding)。samplers 数组第 i 项对应
        // samplerBinding = startSlot + i，这里按 samplerBinding 反查对应的 texBinding 作为绑定单元。
        auto remapUnit = [this](uint32_t samplerBinding) -> uint32_t
        {
            auto search = [samplerBinding](const ShaderSamplerParams &params) -> int32_t
            {
                for (const auto &it : params)
                {
                    if (it.second->getSamplerBinding() == samplerBinding)
                    {
                        return static_cast<int32_t>(it.second->getTexBinding());
                    }
                }
                return -1;
            };

            if (mCurrentPSVariant != nullptr)
            {
                int32_t unit = search(mCurrentPSVariant->getShaderSamplerParams());
                if (unit >= 0)
                {
                    return static_cast<uint32_t>(unit);
                }
            }
            if (mCurrentVSVariant != nullptr)
            {
                int32_t unit = search(mCurrentVSVariant->getShaderSamplerParams());
                if (unit >= 0)
                {
                    return static_cast<uint32_t>(unit);
                }
            }
            return samplerBinding;
        };

        TArray<GLuint> samplerHandles;
        TArray<GLuint> samplerUnits;
        samplerHandles.reserve(samplers.size());
        samplerUnits.reserve(samplers.size());

        for (uint32_t i = 0; i < samplers.size(); ++i)
        {
            samplerUnits.push_back(remapUnit(startSlot + i));

            if (samplers[i] != nullptr)
            {
                GLES3SamplerState *glSampler = static_cast<GLES3SamplerState*>(samplers[i]->getRHIState().get());
                samplerHandles.push_back(glSampler->GLSampler);
            }
            else
            {
                samplerHandles.push_back(0);
            }
        }

        auto lambda = [this](TArray<GLuint> samplerHandles, TArray<GLuint> samplerUnits)
        {
            TResult ret = T3D_OK;

            do
            {
                for (uint32_t i = 0; i < samplerHandles.size(); ++i)
                {
                    if (samplerHandles[i] != 0)
                    {
                        glBindSampler(samplerUnits[i], samplerHandles[i]);
                    }
                }
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::bindSamplers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, samplerHandles, samplerUnits);
    }

    //--------------------------------------------------------------------------
}
