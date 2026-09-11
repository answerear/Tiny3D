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

        GL_SAFE_DELETE_PROGRAM(mCurrentComputeProgram);
        GL_SAFE_DELETE_PROGRAM(mCurrentProgram);
        GL_SAFE_DELETE_FBO(mScratchReadFBO);
        GL_SAFE_DELETE_FBO(mScratchDrawFBO);
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

            fillCapabilities();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void GLES3Context::fillCapabilities()
    {
        const bool has31 = (mGLESMajor > 3) || (mGLESMajor == 3 && mGLESMinor >= 1);
        const bool has32 = (mGLESMajor > 3) || (mGLESMajor == 3 && mGLESMinor >= 2);

        mCapabilities.supportsInstancing = true;
        mCapabilities.supportsBaseInstance = false;
        mCapabilities.supportsCompute = has31;
        mCapabilities.supportsUnorderedAccess = has31;
        mCapabilities.supportsStructuredBuffer = has31;
        mCapabilities.supportsIndirectDraw = has31;
        mCapabilities.supportsIndirectDispatch = has31;
        mCapabilities.supportsAppendConsumeBuffer = has31;
        mCapabilities.supportsReadback = true;

        mSupportsIndexedBlend = has32;

        if (has31 && eglGetCurrentContext() != EGL_NO_CONTEXT)
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

            GLint vsStorage = 0;
            glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &vsStorage);
            mMaxVertexShaderStorageBlocks = (uint32_t)vsStorage;
        }

        while (glGetError() != GL_NO_ERROR) {}
    }

    //--------------------------------------------------------------------------

    void GLES3Context::confirmDeviceVersion()
    {
        GLint major = 0;
        GLint minor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        if (major > 0)
        {
            mGLESMajor = major;
            mGLESMinor = minor;
        }

        GLint units = 0;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &units);
        if (units > 0)
        {
            mMaxTextureImageUnits = static_cast<uint32_t>(units);
        }

        while (glGetError() != GL_NO_ERROR) {}

        T3D_LOG_INFO(LOG_TAG_GLES3RENDERER, "Confirmed GLES %d.%d, texture units=%u",
            mGLESMajor, mGLESMinor, mMaxTextureImageUnits);

        fillCapabilities();
    }

    //--------------------------------------------------------------------------

    bool GLES3Context::supportsTessellation() const
    {
        return (mGLESMinor >= 2) || GLES3Mapping::isTessellationSupported();
    }

    //--------------------------------------------------------------------------

    bool GLES3Context::supportsIndexedBlend() const
    {
        return mSupportsIndexedBlend;
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
                        confirmDeviceVersion();
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

        auto lambda = [this](const PixelBuffer2DPtr &buffer, const GLES3PixelBuffer2DPtr &glPixelBuffer)
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

    void GLES3Context::releaseRenderTextureResources(GLES3PixelBuffer2D *pb)
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

    TResult GLES3Context::buildRenderTextureResources(PixelBuffer2D *buffer, GLES3PixelBuffer2D *glPixelBuffer)
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
            const GLenum internalFmt = GLES3Mapping::getInternalFormat(desc.format);
            const GLenum pixelType = GLES3Mapping::getPixelType(desc.format);
            const bool hasStencil = (desc.format == PixelFormat::E_PF_D24_UNORM_S8_UINT
                || desc.format == PixelFormat::E_PF_D32_FLOAT_S8X24_UINT);

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
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

                GLenum none = GL_NONE;
                glDrawBuffers(1, &none);
                glReadBuffer(GL_NONE);

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

            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::buildRenderTextureResources");
        } while (false);

        return ret;
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
        mRenderingToFBO = false;

        auto lambda = [this]()
        {
            TResult ret = T3D_OK;

            do
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                mRenderingToFBO = false;
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::resizeRenderTexture(RenderTexture *rt, uint32_t width, uint32_t height)
    {
        if (rt == nullptr || width == 0 || height == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "resizeRenderTexture : invalid render texture or size [%u x %u] !", width, height);
            return T3D_ERR_INVALID_PARAM;
        }

        PixelBuffer2D *pixelBuffer = static_cast<PixelBuffer2D *>(rt->getPixelBuffer());
        if (pixelBuffer == nullptr || pixelBuffer->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "resizeRenderTexture : render texture [%s] has no RHI resource !", rt->getName().c_str());
            return T3D_ERR_INVALID_POINTER;
        }

        if (pixelBuffer->getDescriptor().width != width || pixelBuffer->getDescriptor().height != height)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "resizeRenderTexture : descriptor [%u x %u] does not match requested size [%u x %u] !",
                pixelBuffer->getDescriptor().width, pixelBuffer->getDescriptor().height, width, height);
            return T3D_ERR_INVALID_PARAM;
        }

        GLES3PixelBuffer2D *glPixelBuffer = static_cast<GLES3PixelBuffer2D *>(pixelBuffer->getRHIResource().get());
        RenderTargetPtr currentRT = mCurrentRenderTarget;
        bool needRebind = false;
        if (currentRT != nullptr && currentRT->getType() == RenderTarget::Type::E_RT_TEXTURE)
        {
            const uint32_t numRT = currentRT->getNumOfRenderTextures();
            for (uint32_t i = 0; i < numRT; ++i)
            {
                if (currentRT->getRenderTexture(i).get() == rt)
                {
                    needRebind = true;
                    break;
                }
            }
            if (!needRebind && currentRT->getDepthStencil().get() == rt)
            {
                needRebind = true;
            }
        }

        auto lambda = [this](const PixelBuffer2DPtr &pixelBuffer, const GLES3PixelBuffer2DPtr &glPixelBuffer)
        {
            releaseRenderTextureResources(glPixelBuffer.get());
            return buildRenderTextureResources(pixelBuffer.get(), glPixelBuffer.get());
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, PixelBuffer2DPtr(pixelBuffer), GLES3PixelBuffer2DPtr(glPixelBuffer));
        if (T3D_SUCCEEDED(ret) && needRebind)
        {
            return setRenderTarget(currentRT.get());
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::resizeRenderTarget(RenderTarget *rt, uint32_t width, uint32_t height)
    {
        if (rt == nullptr || width == 0 || height == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "resizeRenderTarget : invalid render target or size [%u x %u] !", width, height);
            return T3D_ERR_INVALID_PARAM;
        }

        if (rt->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            GLES3RenderWindow *glWindow = static_cast<GLES3RenderWindow *>(rt->getRenderWindow()->getRHIRenderWindow());
            if (glWindow == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "resizeRenderTarget : render window has no RHI resource !");
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

    GLES3Context::ClearMaskGuard::ClearMaskGuard(GLbitfield mask)
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

    GLES3Context::ClearMaskGuard::~ClearMaskGuard()
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

    TResult GLES3Context::clearColor(const ColorRGB &color)
    {
        auto lambda = [this](ColorRGB color)
        {
            TResult ret = T3D_OK;

            do
            {
                ClearMaskGuard guard(GL_COLOR_BUFFER_BIT);
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
                ClearMaskGuard guard(GL_DEPTH_BUFFER_BIT);
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
                ClearMaskGuard guard(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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

        GLES3BlendStateData d {};
        d.independentBlend = desc.IndependentBlendEnable;
        d.alphaToCoverage = desc.AlphaToCoverageEnable;

        const uint32_t rtCount = d.independentBlend ? BlendDesc::kMaxRenderTarget : 1;
        for (uint32_t i = 0; i < rtCount; ++i)
        {
            const auto &src = desc.RenderTargetStates[i];
            auto &dst = d.targets[i];
            dst.enabled = src.BlendEnable;
            dst.srcRGB = GLES3Mapping::get(src.SrcBlend);
            dst.dstRGB = GLES3Mapping::get(src.DestBlend);
            dst.opRGB = GLES3Mapping::get(src.BlendOp);
            dst.srcAlpha = GLES3Mapping::get(src.SrcBlendAlpha);
            dst.dstAlpha = GLES3Mapping::get(src.DstBlendAlpha);
            dst.opAlpha = GLES3Mapping::get(src.BlendOpAlpha);
            dst.colorMask[0] = (src.ColorMask & kWriteMaskRed) ? GL_TRUE : GL_FALSE;
            dst.colorMask[1] = (src.ColorMask & kWriteMaskGreen) ? GL_TRUE : GL_FALSE;
            dst.colorMask[2] = (src.ColorMask & kWriteMaskBlue) ? GL_TRUE : GL_FALSE;
            dst.colorMask[3] = (src.ColorMask & kWriteMaskAlpha) ? GL_TRUE : GL_FALSE;
        }

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
        d.stencilRef = static_cast<uint8_t>(desc.StencilRef);
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
        const bool applyAniso = (maxAniso > 1.0f && GLES3Mapping::isAnisotropicSupported());
        const GLfloat anisoLimit = GLES3Mapping::getMaxAnisotropy();
        const bool applyBorder = GLES3Mapping::isBorderClampSupported()
            && (desc.AddressU == TextureAddressMode::kBorder
                || desc.AddressV == TextureAddressMode::kBorder
                || desc.AddressW == TextureAddressMode::kBorder);
        GLfloat borderColor[4] = {
            desc.BorderColor.red(), desc.BorderColor.green(),
            desc.BorderColor.blue(), desc.BorderColor.alpha()
        };

        auto lambda = [this](const GLES3SamplerStatePtr &glState,
            GLenum wrapS, GLenum wrapT, GLenum wrapR,
            GLenum minFilter, GLenum magFilter, GLfloat maxAniso,
            GLfloat minLOD, GLfloat maxLOD,
            bool isComparison, GLenum compareFunc,
            bool applyAniso, GLfloat anisoLimit)
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

                if (applyAniso)
                {
                    glSamplerParameterf(glState->GLSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                        std::min(maxAniso, anisoLimit));
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
            minLOD, maxLOD, isComparison, compareFunc, applyAniso, anisoLimit);

        if (applyBorder)
        {
#ifndef GL_TEXTURE_BORDER_COLOR_EXT
#define GL_TEXTURE_BORDER_COLOR_EXT 0x1004
#endif
            auto lambdaBorder = [this](const GLES3SamplerStatePtr &glState,
                GLfloat b0, GLfloat b1, GLfloat b2, GLfloat b3)
            {
                GLfloat bc[4] = { b0, b1, b2, b3 };
                glSamplerParameterfv(glState->GLSampler, GL_TEXTURE_BORDER_COLOR_EXT, bc);
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createSamplerState(border)");
                return T3D_OK;
            };

            ENQUEUE_UNIQUE_COMMAND(lambdaBorder, glState,
                borderColor[0], borderColor[1], borderColor[2], borderColor[3]);
        }

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
                if (d.alphaToCoverage)
                {
                    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
                }
                else
                {
                    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
                }

                if (d.independentBlend && supportsIndexedBlend())
                {
                    for (uint32_t i = 0; i < BlendDesc::kMaxRenderTarget; ++i)
                    {
                        const auto &rt = d.targets[i];
                        if (rt.enabled)
                        {
                            glEnablei(GL_BLEND, i);
                            glBlendFuncSeparatei(i, rt.srcRGB, rt.dstRGB, rt.srcAlpha, rt.dstAlpha);
                            glBlendEquationSeparatei(i, rt.opRGB, rt.opAlpha);
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
                    if (d.independentBlend)
                    {
                        static bool sIndexedBlendWarned = false;
                        if (!sIndexedBlendWarned)
                        {
                            T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER,
                                "Independent blend requires GLES 3.2, falling back to RT0");
                            sIndexedBlendWarned = true;
                        }
                    }

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
                    glStencilFuncSeparate(GL_FRONT, d.frontStencilFunc, d.stencilRef, d.stencilReadMask);
                    glStencilFuncSeparate(GL_BACK, d.backStencilFunc, d.stencilRef, d.stencilReadMask);
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
        return attachGraphicsShader(shader, mCurrentVSVariant);
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
        return attachGraphicsShader(shader, mCurrentPSVariant);
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
    // Hull Shader (GLES 3.2 / GL_EXT_tessellation_shader)
    //--------------------------------------------------------------------------

    RHIShaderPtr GLES3Context::createHullShader(ShaderVariant *shader)
    {
        if (!supportsTessellation())
        {
            static bool sWarned = false;
            if (!sWarned)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "Hull shader requires GLES 3.2 or GL_EXT_tessellation_shader");
                sWarned = true;
            }
            return nullptr;
        }

        GLES3HullShaderPtr glShader = GLES3HullShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);
        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GLES3HullShaderPtr &glShader, String shaderSource)
        {
            return compileGLSLShader(GL_TESS_CONTROL_SHADER, shaderSource, glShader->GLShaderHandle, "Hull");
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    TResult GLES3Context::setHullShader(ShaderVariant *shader)
    {
        if (!supportsTessellation())
        {
            static bool sWarned = false;
            if (!sWarned)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "setHullShader is not supported on this device");
                sWarned = true;
            }
            return T3D_ERR_NOT_IMPLEMENT;
        }
        return attachGraphicsShader(shader, mCurrentHSVariant);
    }

    TResult GLES3Context::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        if (!supportsTessellation())
        {
            static bool sWarned = false;
            if (!sWarned)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "setHSConstantBuffers is not supported on this device");
                sWarned = true;
            }
            return T3D_ERR_NOT_IMPLEMENT;
        }
        return stageConstantBuffers(buffers);
    }

    TResult GLES3Context::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        if (!supportsTessellation())
        {
            static bool sWarned = false;
            if (!sWarned)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "setHSPixelBuffers is not supported on this device");
                sWarned = true;
            }
            return T3D_ERR_NOT_IMPLEMENT;
        }
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GLES3Context::setHSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        if (!supportsTessellation())
        {
            static bool sWarned = false;
            if (!sWarned)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "setHSSamplers is not supported on this device");
                sWarned = true;
            }
            return T3D_ERR_NOT_IMPLEMENT;
        }
        return bindSamplers(startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    // Domain Shader (GLES 3.2 / GL_EXT_tessellation_shader)
    //--------------------------------------------------------------------------

    RHIShaderPtr GLES3Context::createDomainShader(ShaderVariant *shader)
    {
        if (!supportsTessellation())
        {
            static bool sWarned = false;
            if (!sWarned)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "Domain shader requires GLES 3.2 or GL_EXT_tessellation_shader");
                sWarned = true;
            }
            return nullptr;
        }

        GLES3DomainShaderPtr glShader = GLES3DomainShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);
        String shaderSource(bytecode, bytecodeLength);

        auto lambda = [this](const GLES3DomainShaderPtr &glShader, String shaderSource)
        {
            return compileGLSLShader(GL_TESS_EVALUATION_SHADER, shaderSource, glShader->GLShaderHandle, "Domain");
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, glShader, shaderSource);
        if (T3D_FAILED(ret)) { return nullptr; }
        return glShader;
    }

    TResult GLES3Context::setDomainShader(ShaderVariant *shader)
    {
        if (!supportsTessellation())
        {
            static bool sWarned = false;
            if (!sWarned)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "setDomainShader is not supported on this device");
                sWarned = true;
            }
            return T3D_ERR_NOT_IMPLEMENT;
        }
        return attachGraphicsShader(shader, mCurrentDSVariant);
    }

    TResult GLES3Context::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        if (!supportsTessellation())
        {
            static bool sWarned = false;
            if (!sWarned)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "setDSConstantBuffers is not supported on this device");
                sWarned = true;
            }
            return T3D_ERR_NOT_IMPLEMENT;
        }
        return stageConstantBuffers(buffers);
    }

    TResult GLES3Context::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        if (!supportsTessellation())
        {
            static bool sWarned = false;
            if (!sWarned)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "setDSPixelBuffers is not supported on this device");
                sWarned = true;
            }
            return T3D_ERR_NOT_IMPLEMENT;
        }
        return bindPixelBuffers(startSlot, buffers);
    }

    TResult GLES3Context::setDSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        if (!supportsTessellation())
        {
            static bool sWarned = false;
            if (!sWarned)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "setDSSamplers is not supported on this device");
                sWarned = true;
            }
            return T3D_ERR_NOT_IMPLEMENT;
        }
        return bindSamplers(startSlot, samplers);
    }

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
        {
            static bool sWarned = false;
            if (!sWarned)
            {
                T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER, "Geometry shader requires GLES 3.2, current: 3.%d", mGLESMinor);
                sWarned = true;
            }
            return T3D_ERR_NOT_IMPLEMENT;
        }

        return attachGraphicsShader(shader, mCurrentGSVariant);
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
        if (!mCapabilities.supportsCompute)
        {
            T3D_RHI_UNSUPPORTED_PTR(supportsCompute);
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
        if (!mCapabilities.supportsCompute)
        {
            T3D_RHI_UNSUPPORTED(supportsCompute);
        }

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

        GLES3Shader *glShader = static_cast<GLES3Shader*>(shader->getRHIShader());
        if (glShader == nullptr || glShader->GLShaderHandle == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "setComputeShader : shader has no RHI object !");
            return T3D_ERR_INVALID_POINTER;
        }

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
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setComputeShader");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, shaderHandle, shader);
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

    TResult GLES3Context::ensureProgramLinked()
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
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Program link error: %s", log.data());
            }
            return T3D_ERR_GLES3_LINK_PROGRAM;
        }

        glUseProgram(mCurrentProgram);
        bindPendingUniformBlocks(mCurrentProgram);
        setupSamplerBindings(mCurrentProgram);
        mProgramDirty = false;

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
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

                GLint boundEBO = 0;
                glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundEBO);
                if (boundEBO == 0)
                {
                    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::render(indexed): No EBO bound! Skipping draw to avoid crash.");
                    ret = T3D_ERR_GLES3_INVALID_USAGE;
                    break;
                }

                if (baseVertex != 0 && (mGLESMinor >= 2 || GLES3Mapping::isBaseVertexExtSupported()))
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
                ret = ensureProgramLinked();
                if (T3D_FAILED(ret))
                    break;

                glDrawArrays(mPrimitiveType, startVertex, vertexCount);

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::render(non-indexed)");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, vertexCount, startVertex);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::renderIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
        uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)
    {
        if (startInstance != 0)
        {
            // GLES3 无 base instance 支持，只能由上层改用偏移后的实例缓冲
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER,
                "GLES3 does not support non-zero startInstance (%u) !", startInstance);
            return T3D_ERR_NOT_IMPLEMENT;
        }

        auto lambda = [this](uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex)
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
                    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::renderIndexedInstanced(): No EBO bound! Skipping draw to avoid crash.");
                    ret = T3D_ERR_GLES3_INVALID_USAGE;
                    break;
                }

                if (baseVertex != 0 && (mGLESMinor >= 2 || GLES3Mapping::isBaseVertexExtSupported()))
                {
                    glDrawElementsInstancedBaseVertex(mPrimitiveType, indexCount, mIndexType,
                        offset, instanceCount, baseVertex);
                }
                else
                {
                    if (baseVertex != 0)
                    {
                        T3D_LOG_WARNING(LOG_TAG_GLES3RENDERER,
                            "baseVertex (%d) requires GLES 3.2, ignored !", baseVertex);
                    }
                    glDrawElementsInstanced(mPrimitiveType, indexCount, mIndexType, offset, instanceCount);
                }

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::renderIndexedInstanced");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, indexCount, instanceCount, startIndex, baseVertex);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::renderInstanced(uint32_t vertexCount, uint32_t instanceCount,
        uint32_t startVertex, uint32_t startInstance)
    {
        if (startInstance != 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER,
                "GLES3 does not support non-zero startInstance (%u) !", startInstance);
            return T3D_ERR_NOT_IMPLEMENT;
        }

        auto lambda = [this](uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex)
        {
            TResult ret = T3D_OK;

            do
            {
                ret = ensureProgramLinked();
                if (T3D_FAILED(ret))
                    break;

                glDrawArraysInstanced(mPrimitiveType, startVertex, vertexCount, instanceCount);

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::renderInstanced");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, vertexCount, instanceCount, startVertex);
    }

    //--------------------------------------------------------------------------

    RHIStructuredBufferPtr GLES3Context::createStructuredBuffer(StructuredBuffer *buffer)
    {
        if (!mCapabilities.supportsStructuredBuffer)
        {
            T3D_RHI_UNSUPPORTED_PTR(supportsStructuredBuffer);
        }

        if (buffer == nullptr)
        {
            return nullptr;
        }

        GLES3StructuredBufferPtr glBuffer = GLES3StructuredBuffer::create();
        const StructuredBufferDesc &desc = buffer->getDescriptor();
        const size_t byteWidth = buffer->getGPUSizeInBytes();
        const GLenum usage = GLES3Mapping::getBufferUsage(buffer->getUsage());
        const bool hasCounter = desc.hasCounter || desc.isAppendConsume;

        glBuffer->ElementCount = desc.elementCount;
        glBuffer->ElementSize = desc.elementSize;
        glBuffer->HasCounter = hasCounter;

        auto lambda = [this](const GLES3StructuredBufferPtr &glBuffer, const StructuredBufferPtr &buffer,
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

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::createStructuredBuffer");
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

    TResult GLES3Context::setVSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers)
    {
        if (!mCapabilities.supportsStructuredBuffer)
        {
            T3D_RHI_UNSUPPORTED(supportsStructuredBuffer);
        }

        if (mMaxVertexShaderStorageBlocks == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER,
                "setVSStructuredBuffers : GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS is 0 on this device");
            return T3D_ERR_GLES3_UNSUPPORTED_OPERATION;
        }

        return bindStructuredBuffers(startSlot, buffers);
    }

    TResult GLES3Context::setPSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers)
    {
        return bindStructuredBuffers(startSlot, buffers);
    }

    TResult GLES3Context::setCSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers)
    {
        return bindStructuredBuffers(startSlot, buffers);
    }

    TResult GLES3Context::setCSUnorderedAccessBuffers(uint32_t startSlot, const UnorderedAccessBuffers &buffers, const UAVInitialCounts &initialCounts)
    {
        if (!mCapabilities.supportsUnorderedAccess)
        {
            T3D_RHI_UNSUPPORTED(supportsUnorderedAccess);
        }

        if (!initialCounts.empty() && initialCounts.size() != buffers.size())
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "setCSUnorderedAccessBuffers : initial count array "
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
                GLES3StructuredBuffer *glSB = static_cast<GLES3StructuredBuffer*>(rb->getRHIResource().get());
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
                            glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, bindings[i].counter);
                            glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(uint32_t), &count);
                            glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
                        }
                    }
                    else
                    {
                        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, slot, 0);
                    }
                }

                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::setCSUnorderedAccessBuffers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, bindings);
    }

    TResult GLES3Context::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        if (!mCapabilities.supportsCompute)
        {
            T3D_RHI_UNSUPPORTED(supportsCompute);
        }

        if (groupCountX == 0 || groupCountY == 0 || groupCountZ == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "dispatch : group count [%u, %u, %u] must not contain zero !",
                groupCountX, groupCountY, groupCountZ);
            return T3D_ERR_INVALID_PARAM;
        }

        const uint32_t *maxGroups = mCapabilities.maxDispatchGroupCount;
        if (groupCountX > maxGroups[0] || groupCountY > maxGroups[1] || groupCountZ > maxGroups[2])
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "dispatch : group count [%u, %u, %u] exceeds device "
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

            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::dispatch");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, groupCountX, groupCountY, groupCountZ);
    }

    TResult GLES3Context::dispatchIndirect(RenderBuffer *argsBuffer, size_t argsOffset)
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

            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::dispatchIndirect");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glBuf, (GLintptr)argsOffset);
    }

    TResult GLES3Context::uavBarrier(const UnorderedAccessBuffers &buffers)
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
            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::uavBarrier");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    TResult GLES3Context::copyStructureCount(RenderBuffer *dstBuffer, size_t dstOffset, RenderBuffer *srcBuffer)
    {
        if (!mCapabilities.supportsAppendConsumeBuffer)
        {
            T3D_RHI_UNSUPPORTED(supportsAppendConsumeBuffer);
        }

        if (dstBuffer == nullptr || srcBuffer == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "copyStructureCount : null buffer !");
            return T3D_ERR_INVALID_POINTER;
        }

        if ((dstOffset % 4) != 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "copyStructureCount : destination offset [%zu] must be a multiple of 4 !", dstOffset);
            return T3D_ERR_INVALID_PARAM;
        }

        if (dstOffset + sizeof(uint32_t) > dstBuffer->getGPUSizeInBytes())
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "copyStructureCount : offset [%zu] + 4 exceeds destination buffer size [%zu] !",
                dstOffset, dstBuffer->getGPUSizeInBytes());
            return T3D_ERR_OUT_OF_BOUND;
        }

        if (srcBuffer->getType() != RenderResource::Type::kStructuredBuffer
            || !static_cast<StructuredBuffer*>(srcBuffer)->hasUAVCounter())
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "copyStructureCount : source must be a structured buffer with a hidden counter !");
            return T3D_ERR_INVALID_PARAM;
        }

        GLuint dstBuf = getGLBufferHandle(dstBuffer);
        GLES3StructuredBuffer *glSrc = static_cast<GLES3StructuredBuffer*>(srcBuffer->getRHIResource().get());
        GLuint srcCounter = (glSrc != nullptr) ? glSrc->GLCounterBuffer : 0;
        if (dstBuf == 0 || srcCounter == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "copyStructureCount : underlying GL objects are not ready !");
            return T3D_ERR_INVALID_POINTER;
        }

        auto lambda = [this](GLuint dstBuf, GLintptr dstOffset, GLuint srcCounter)
        {
            glBindBuffer(GL_COPY_READ_BUFFER, srcCounter);
            glBindBuffer(GL_COPY_WRITE_BUFFER, dstBuf);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, dstOffset, sizeof(uint32_t));
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::copyStructureCount");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, dstBuf, (GLintptr)dstOffset, srcCounter);
    }

    TResult GLES3Context::renderIndexedIndirect(RenderBuffer *argsBuffer, size_t argsOffset)
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
            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::renderIndexedIndirect");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glBuf, (GLintptr)argsOffset);
    }

    TResult GLES3Context::renderIndirect(RenderBuffer *argsBuffer, size_t argsOffset)
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
            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::renderIndirect");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, glBuf, (GLintptr)argsOffset);
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
                mRenderingToFBO = false;
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
                GL_SAFE_DELETE_FBO(mScratchReadFBO);
                GL_SAFE_DELETE_FBO(mScratchDrawFBO);
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

    TResult GLES3Context::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
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

    TResult GLES3Context::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
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

    TResult GLES3Context::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
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

    TResult GLES3Context::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "copyBuffer : null buffer !");
            return T3D_ERR_INVALID_POINTER;
        }

        if (src == dst)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "copyBuffer : source and destination must be different !");
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
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "copyBuffer : only linear buffers are supported, use blit for textures !");
            return T3D_ERR_GLES3_UNSUPPORTED_OPERATION;
        }

        if (dst->getUsage() == Usage::kImmutable)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "copyBuffer : destination is immutable !");
            return T3D_ERR_GLES3_INVALID_USAGE;
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
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "copyBuffer : out of range ! src [%zu + %zu / %zu] dst [%zu + %zu / %zu]",
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
            glBindBuffer(GL_COPY_READ_BUFFER, srcBuf);
            glBindBuffer(GL_COPY_WRITE_BUFFER, dstBuf);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, dstOffset, copySize);
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::copyBuffer");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, srcBuf, dstBuf, (GLintptr)srcOffset, (GLintptr)dstOffset, (GLsizeiptr)copySize);
    }

    //--------------------------------------------------------------------------

    ReadbackHandle GLES3Context::map(RenderBuffer *src, size_t offset, size_t size)
    {
        if (!mCapabilities.supportsReadback)
        {
            T3D_RHI_UNSUPPORTED_VALUE(supportsReadback, ReadbackHandle::invalid());
        }

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
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "map : failed to retrieve underlying GL buffer !");
                request->CopyResult = T3D_ERR_INVALID_POINTER;
                return request->CopyResult;
            }

            const size_t srcSize = src->getGPUSizeInBytes();
            const size_t offset = std::min(request->BufferOffset, srcSize);
            const size_t copySize = (request->BufferSize == 0) ? (srcSize - offset) : request->BufferSize;

            if (copySize == 0 || offset + copySize > srcSize)
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "map : buffer out of range ! [%zu + %zu / %zu]", offset, copySize, srcSize);
                request->CopyResult = T3D_ERR_INVALID_PARAM;
                return request->CopyResult;
            }

            glGenBuffers(1, &request->Staging);
            glBindBuffer(GL_COPY_WRITE_BUFFER, request->Staging);
            glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)copySize, nullptr, GL_STREAM_READ);
            glBindBuffer(GL_COPY_READ_BUFFER, srcBuf);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, (GLintptr)offset, 0, (GLsizeiptr)copySize);
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

            request->TotalBytes = copySize;
            request->TightRowPitch = static_cast<uint32_t>(copySize);
            request->TightSlicePitch = static_cast<uint32_t>(copySize);
            request->CopyWidth = static_cast<uint32_t>(copySize);
            request->CopyResult = T3D_OK;
            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::map(buffer)");
            return T3D_OK;
        };

        ENQUEUE_UNIQUE_COMMAND(lambda, request, RenderBufferPtr(src));
        return handle;
    }

    //--------------------------------------------------------------------------

    ReadbackHandle GLES3Context::map(RenderBuffer *src, const ReadbackRegion &region)
    {
        if (!mCapabilities.supportsReadback)
        {
            T3D_RHI_UNSUPPORTED_VALUE(supportsReadback, ReadbackHandle::invalid());
        }

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
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "map : only 2D textures are supported for texture readback !");
                request->CopyResult = T3D_ERR_GLES3_UNSUPPORTED_OPERATION;
                return request->CopyResult;
            }

            PixelBuffer2D *pb = static_cast<PixelBuffer2D*>(src.get());
            const auto &desc = pb->getDescriptor();
            if (desc.format >= PixelFormat::E_PF_D24_UNORM_S8_UINT
                && desc.format <= PixelFormat::E_PF_D16_UNORM)
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "map : depth / stencil readback is not supported on GLES !");
                request->CopyResult = T3D_ERR_GLES3_UNSUPPORTED_OPERATION;
                return request->CopyResult;
            }

            uint32_t bpp = GLES3Mapping::getBytesPerPixel(desc.format);
            GLenum readFmt = GLES3Mapping::get(desc.format);
            GLenum readType = GLES3Mapping::getPixelType(desc.format);
            if (bpp == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "map : format is not supported for readback !");
                request->CopyResult = T3D_ERR_GLES3_UNSUPPORTED_OPERATION;
                return request->CopyResult;
            }

            GLES3PixelBuffer2D *glPB = static_cast<GLES3PixelBuffer2D*>(src->getRHIResource().get());
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

            if (mScratchReadFBO == 0)
            {
                glGenFramebuffers(1, &mScratchReadFBO);
            }

            glBindFramebuffer(GL_READ_FRAMEBUFFER, mScratchReadFBO);
            glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, (GLint)request->Region.mipLevel);
            if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "map : scratch FBO is incomplete for texture readback !");
                request->CopyResult = T3D_ERR_GLES3_CREATE_FBO;
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                return request->CopyResult;
            }
            glReadBuffer(GL_COLOR_ATTACHMENT0);

            GLint implFormat = 0;
            GLint implType = 0;
            glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &implFormat);
            glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &implType);
            if ((readFmt != static_cast<GLenum>(implFormat) || readType != static_cast<GLenum>(implType))
                && !(readFmt == GL_RGBA && readType == GL_UNSIGNED_BYTE))
            {
                readFmt = static_cast<GLenum>(implFormat);
                readType = static_cast<GLenum>(implType);
                if (readFmt == GL_RGBA && readType == GL_UNSIGNED_BYTE)
                {
                    bpp = 4;
                }
                else if (readFmt == GL_RGB && readType == GL_UNSIGNED_BYTE)
                {
                    bpp = 3;
                }
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
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "map : region out of range !");
                request->CopyResult = T3D_ERR_INVALID_PARAM;
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                return request->CopyResult;
            }

            request->CopyWidth = copyWidth;
            request->CopyHeight = copyHeight;
            request->CopyDepth = 1;
            request->TightRowPitch = copyWidth * bpp;
            request->TightSlicePitch = request->TightRowPitch * copyHeight;
            request->TotalBytes = request->TightSlicePitch;

            glGenBuffers(1, &request->Staging);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, request->Staging);
            glBufferData(GL_PIXEL_PACK_BUFFER, (GLsizeiptr)request->TotalBytes, nullptr, GL_STREAM_READ);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glReadPixels((GLint)offsetX, (GLint)offsetY, (GLsizei)copyWidth, (GLsizei)copyHeight,
                readFmt, readType, nullptr);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            request->CopyResult = T3D_OK;
            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::map(texture)");
            return T3D_OK;
        };

        ENQUEUE_UNIQUE_COMMAND(lambda, request, RenderBufferPtr(src));
        return handle;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::unmap(ReadbackHandle handle, Buffer &dst)
    {
        if (!mCapabilities.supportsReadback)
        {
            T3D_RHI_UNSUPPORTED(supportsReadback);
        }

        return finishReadback(handle, dst);
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
        case RHIResource::ResourceType::kStructuredBuffer:
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
            PixelBuffer2D *pb = static_cast<PixelBuffer2D*>(renderBuffer);
            const auto &desc = pb->getDescriptor();
            const GLenum pixelFmt  = GLES3Mapping::get(desc.format);
            const GLenum pixelType = GLES3Mapping::getPixelType(desc.format);
            const GLsizei texW = static_cast<GLsizei>(desc.width);
            const GLsizei texH = static_cast<GLsizei>(desc.height);
            const uint32_t bpp = GLES3Mapping::getBytesPerPixel(desc.format);
            const size_t expected = static_cast<size_t>(texW) * static_cast<size_t>(texH) * bpp;
            if (bpp == 0 || ownedBuffer.DataSize < expected)
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER,
                    "writeBuffer(texture) : size mismatch, data=%zu expected>=%zu bpp=%u %ux%u",
                    ownedBuffer.DataSize, expected, bpp, desc.width, desc.height);
                T3D_POD_SAFE_DELETE_ARRAY(ownedBuffer.Data);
                return T3D_ERR_INVALID_PARAM;
            }

            GLES3PixelBuffer2DPtr glTex = static_cast<GLES3PixelBuffer2D*>(rhiRes.get());

            auto lambda = [this](const GLES3PixelBuffer2DPtr &glTex, Buffer ownedBuffer,
                GLsizei texW, GLsizei texH, GLenum pixelFmt, GLenum pixelType)
            {
                TResult ret = T3D_OK;

                do
                {
                    glBindTexture(GL_TEXTURE_2D, glTex->GLTexture);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texW, texH, pixelFmt, pixelType, ownedBuffer.Data);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::writeBuffer(texture)");
                } while (false);

                T3D_POD_SAFE_DELETE_ARRAY(ownedBuffer.Data);
                return ret;
            };

            return ENQUEUE_UNIQUE_COMMAND(lambda, glTex, ownedBuffer, texW, texH, pixelFmt, pixelType);
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
                case RHIResource::ResourceType::kStructuredBuffer:
                    glBuf = static_cast<GLES3StructuredBuffer*>(rhiResPtr.get())->GLBuffer;
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
            if (mCurrentGSVariant != nullptr)
            {
                const auto &params = mCurrentGSVariant->getShaderSamplerParams();
                const auto itr = params.find(texName);
                if (itr != params.end())
                {
                    return static_cast<int32_t>(itr->second->getTexBinding());
                }
            }
            if (mCurrentHSVariant != nullptr)
            {
                const auto &params = mCurrentHSVariant->getShaderSamplerParams();
                const auto itr = params.find(texName);
                if (itr != params.end())
                {
                    return static_cast<int32_t>(itr->second->getTexBinding());
                }
            }
            if (mCurrentDSVariant != nullptr)
            {
                const auto &params = mCurrentDSVariant->getShaderSamplerParams();
                const auto itr = params.find(texName);
                if (itr != params.end())
                {
                    return static_cast<int32_t>(itr->second->getTexBinding());
                }
            }
            if (mCurrentCSVariant != nullptr)
            {
                const auto &params = mCurrentCSVariant->getShaderSamplerParams();
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
                {
                    GLES3PixelBuffer2D *glPB = static_cast<GLES3PixelBuffer2D*>(buffers[i]->getRHIResource().get());
                    if (glPB->GLMSAACount > 1 && glPB->GLResolveTex != 0)
                    {
                        texHandle = glPB->GLResolveTex;
                        texTarget = GL_TEXTURE_2D;
                    }
                    else if (glPB->GLMSAACount > 1)
                    {
                        if (mGLESMinor < 1)
                        {
                            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER,
                                "bindPixelBuffers : MSAA texture without resolve requires GLES 3.1");
                            bindings.push_back({0, GL_TEXTURE_2D});
                            continue;
                        }
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
                    glActiveTexture(GL_TEXTURE0 + startSlot + i);
                    glBindTexture(bindings[i].target, bindings[i].handle);
                }
                glActiveTexture(GL_TEXTURE0);
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
            if (mCurrentGSVariant != nullptr)
            {
                int32_t unit = search(mCurrentGSVariant->getShaderSamplerParams());
                if (unit >= 0)
                {
                    return static_cast<uint32_t>(unit);
                }
            }
            if (mCurrentHSVariant != nullptr)
            {
                int32_t unit = search(mCurrentHSVariant->getShaderSamplerParams());
                if (unit >= 0)
                {
                    return static_cast<uint32_t>(unit);
                }
            }
            if (mCurrentDSVariant != nullptr)
            {
                int32_t unit = search(mCurrentDSVariant->getShaderSamplerParams());
                if (unit >= 0)
                {
                    return static_cast<uint32_t>(unit);
                }
            }
            if (mCurrentCSVariant != nullptr)
            {
                int32_t unit = search(mCurrentCSVariant->getShaderSamplerParams());
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
                    glBindSampler(samplerUnits[i], samplerHandles[i]);
                }
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::bindSamplers");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, samplerHandles, samplerUnits);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::bindStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers)
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
                handle = static_cast<GLES3StructuredBuffer*>(buffers[i]->getRHIResource().get())->GLBuffer;
            }
            handles.push_back(handle);
        }

        auto lambda = [this](uint32_t startSlot, TArray<GLuint> handles)
        {
            for (uint32_t i = 0; i < handles.size(); ++i)
            {
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, startSlot + i, handles[i]);
            }
            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::bindStructuredBuffers");
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, handles);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::ensureComputeProgramLinked()
    {
        if (mCurrentComputeProgram == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "ensureComputeProgramLinked: no compute program bound !");
            return T3D_ERR_GLES3_LINK_PROGRAM;
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
                    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Compute program link error: %s", log.data());
                }
                return T3D_ERR_GLES3_LINK_PROGRAM;
            }

            mComputeProgramDirty = false;
        }

        glUseProgram(mCurrentComputeProgram);
        bindPendingUniformBlocks(mCurrentComputeProgram);
        setupSamplerBindings(mCurrentComputeProgram);

        GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::ensureComputeProgramLinked");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::compileGLSLShader(GLenum shaderType, const String &source, GLuint &outHandle, const char *stageName)
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
                    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "%s shader compile error: %s", stageName, log.data());
                }
                GL_SAFE_DELETE_SHADER(outHandle);
                ret = T3D_ERR_GLES3_COMPILE_SHADER;
                break;
            }

            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::compileGLSLShader");
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void GLES3Context::detachShaderStage(GLuint program, GLuint shader)
    {
        // 程序对象是跨材质复用的，同一个阶段挂上第二个 shader 会被 GL 以
        // GL_INVALID_OPERATION 拒掉，链接时用的还是上一个，画面自然不对。
        // 切材质（比如从场景着色器切到后处理着色器）每帧都会走到这里
        GLint stage = 0;
        glGetShaderiv(shader, GL_SHADER_TYPE, &stage);

        GLint count = 0;
        glGetProgramiv(program, GL_ATTACHED_SHADERS, &count);
        if (count <= 0)
        {
            return;
        }

        TArray<GLuint> attached(count, 0);
        glGetAttachedShaders(program, count, nullptr, attached.data());

        for (GLuint attachedShader : attached)
        {
            if (attachedShader == 0)
            {
                continue;
            }

            GLint attachedStage = 0;
            glGetShaderiv(attachedShader, GL_SHADER_TYPE, &attachedStage);
            if (attachedStage == stage)
            {
                glDetachShader(program, attachedShader);
            }
        }
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::attachGraphicsShader(ShaderVariant *shader, ShaderVariant *&currentVariant)
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

        GLES3Shader *glShader = static_cast<GLES3Shader*>(shader->getRHIShader());
        if (glShader == nullptr || glShader->GLShaderHandle == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "attachGraphicsShader : shader has no RHI object !");
            return T3D_ERR_INVALID_POINTER;
        }

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

                detachShaderStage(mCurrentProgram, shaderHandle);

                glAttachShader(mCurrentProgram, shaderHandle);
                mProgramDirty = true;
                GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::attachGraphicsShader");
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, shaderHandle, shader, slot);
    }

    //--------------------------------------------------------------------------

    GLuint GLES3Context::getGLBufferHandle(RenderBuffer *buffer) const
    {
        if (buffer == nullptr || buffer->getRHIResource() == nullptr)
        {
            return 0;
        }

        switch (buffer->getRHIResource()->getResourceType())
        {
        case RHIResource::ResourceType::kVertexBuffer:
            return static_cast<GLES3VertexBuffer*>(buffer->getRHIResource().get())->GLBuffer;
        case RHIResource::ResourceType::kIndexBuffer:
            return static_cast<GLES3IndexBuffer*>(buffer->getRHIResource().get())->GLBuffer;
        case RHIResource::ResourceType::kConstantBuffer:
            return static_cast<GLES3ConstantBuffer*>(buffer->getRHIResource().get())->GLBuffer;
        case RHIResource::ResourceType::kStructuredBuffer:
            return static_cast<GLES3StructuredBuffer*>(buffer->getRHIResource().get())->GLBuffer;
        default:
            break;
        }

        return 0;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::validateIndirectArgs(RenderBuffer *argsBuffer, size_t argsOffset, size_t argsSize)
    {
        if (argsBuffer == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Indirect args buffer is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        if ((argsBuffer->getGPUAccess() & kGPUIndirectArgs) == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Indirect args buffer was not created with kGPUIndirectArgs !");
            return T3D_ERR_INVALID_PARAM;
        }

        if ((argsOffset % 4) != 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Indirect args offset [%zu] must be a multiple of 4 !", argsOffset);
            return T3D_ERR_INVALID_PARAM;
        }

        if (argsOffset + argsSize > argsBuffer->getGPUSizeInBytes())
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Indirect args offset [%zu] + size [%zu] exceeds buffer size [%zu] !",
                argsOffset, argsSize, argsBuffer->getGPUSizeInBytes());
            return T3D_ERR_OUT_OF_BOUND;
        }

        if (argsBuffer->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Indirect args buffer has no RHI resource, is it loaded ?");
            return T3D_ERR_INVALID_POINTER;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::resolveBlitEndpoint(Texture *tex, bool asSource, BlitEndpoint &out)
    {
        if (tex == nullptr || tex->getPixelBuffer() == nullptr
            || tex->getPixelBuffer()->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "blit : texture has no RHI resource !");
            return T3D_ERR_INVALID_POINTER;
        }

        const TEXTURE_TYPE type = tex->getTextureType();
        if (type != TEXTURE_TYPE::TT_2D && type != TEXTURE_TYPE::TT_RENDER_TEXTURE)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "blit : only 2D / render textures are supported !");
            return T3D_ERR_GLES3_UNSUPPORTED_OPERATION;
        }

        Texture2D *tex2D = static_cast<Texture2D*>(tex);
        PixelBuffer2D *pb = static_cast<PixelBuffer2D*>(tex2D->getPixelBuffer());
        GLES3PixelBuffer2D *glPB = static_cast<GLES3PixelBuffer2D*>(pb->getRHIResource().get());

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
        (void)asSource;

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::resolveBlitEndpoint(RenderTarget *rt, bool asSource, BlitEndpoint &out)
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

        T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "blit : render target has no color or depth attachment !");
        return T3D_ERR_INVALID_PARAM;
    }

    //--------------------------------------------------------------------------

    void GLES3Context::unbindTextureUnits()
    {
        const int unitCount = static_cast<int>(mMaxTextureImageUnits);
        for (int i = 0; i < unitCount; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindTexture(GL_TEXTURE_3D, 0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            if (mGLESMinor >= 1)
            {
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            }
            glBindSampler(i, 0);
        }
        glActiveTexture(GL_TEXTURE0);
    }

    //--------------------------------------------------------------------------

    TResult GLES3Context::doBlit(const BlitEndpoint &src, const BlitEndpoint &dst,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
    {
        TResult ret = T3D_OK;

        do
        {
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
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "blit : scratch read FBO is incomplete !");
                    ret = T3D_ERR_GLES3_CREATE_FBO;
                    break;
                }
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
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "blit : scratch draw FBO is incomplete !");
                    ret = T3D_ERR_GLES3_CREATE_FBO;
                    break;
                }
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
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "blit : copy size is zero !");
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            if (src.sampleCount > 1)
            {
                if (src.resolveFbo == 0 || src.resolveTex == 0)
                {
                    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "blit : MSAA source has no resolve FBO !");
                    ret = T3D_ERR_GLES3_UNSUPPORTED_OPERATION;
                    break;
                }

                glBindFramebuffer(GL_READ_FRAMEBUFFER, src.fbo);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, src.resolveFbo);
                glBlitFramebuffer(0, 0, (GLint)src.width, (GLint)src.height,
                    0, 0, (GLint)src.width, (GLint)src.height,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
                readFBO = src.resolveFbo;
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

            glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
            if (!src.isDepth)
            {
                glReadBuffer((src.isWindow || readFBO == 0) ? GL_BACK : GL_COLOR_ATTACHMENT0);
            }

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFBO);
            if (!dst.isDepth)
            {
                const GLenum drawBuf = (dst.isWindow || drawFBO == 0) ? GL_BACK : GL_COLOR_ATTACHMENT0;
                glDrawBuffers(1, &drawBuf);
            }

            glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, GL_NEAREST);

            if (readFBO != 0)
            {
                GLenum discards[] = { static_cast<GLenum>(
                    src.isDepth ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0) };
                glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 1, discards);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
            GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::doBlit");
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    ReadbackHandle GLES3Context::allocReadbackRequest(RenderBuffer *src, bool isTexture, ReadbackRequest *&outRequest)
    {
        outRequest = nullptr;

        if (src == nullptr || src->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "readback : source buffer is null or has no RHI resource !");
            return ReadbackHandle::invalid();
        }

        if ((src->getCPUAccessMode() & kCPURead) != kCPURead)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "readback : source was not created with kCPURead, "
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
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "map : resource type [%d] is not a texture, use the buffer overload !", (int32_t)type);
            return ReadbackHandle::invalid();
        }

        if (!isTexture && !isLinearResource)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "map : resource type [%d] is not a linear buffer, use the texture overload !", (int32_t)type);
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

    TResult GLES3Context::finishReadback(ReadbackHandle handle, Buffer &dst)
    {
        auto itr = mPendingReadbacks.find(handle.index);
        if (!handle.isValid() || itr == mPendingReadbacks.end()
            || itr->second.Handle.generation != handle.generation)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "unmap : invalid or already consumed readback handle !");
            return T3D_ERR_INVALID_PARAM;
        }

        T3D_AGENT.syncRHIThread();

        ReadbackRequest &request = itr->second;
        TResult ret = request.CopyResult;

        if (T3D_OK == ret && !request.CopyRecorded)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "unmap : copy command has not been executed. "
                "map must be called inside onRender, unmap inside onPostRender !");
            ret = T3D_ERR_FAIL;
        }

        if (T3D_OK == ret)
        {
            auto lambda = [this](ReadbackRequest *request, Buffer *dst) -> TResult
            {
                glBindBuffer(GL_COPY_READ_BUFFER, request->Staging);
                void *mapped = glMapBufferRange(GL_COPY_READ_BUFFER, 0,
                    (GLsizeiptr)request->TotalBytes, GL_MAP_READ_BIT);
                if (mapped == nullptr)
                {
                    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "unmap : failed to map staging buffer !");
                    glBindBuffer(GL_COPY_READ_BUFFER, 0);
                    request->CopyResult = T3D_ERR_GLES3_MAP_BUFFER;
                    return request->CopyResult;
                }

                memcpy(dst->Data, mapped, request->TotalBytes);
                glUnmapBuffer(GL_COPY_READ_BUFFER);
                glBindBuffer(GL_COPY_READ_BUFFER, 0);
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
