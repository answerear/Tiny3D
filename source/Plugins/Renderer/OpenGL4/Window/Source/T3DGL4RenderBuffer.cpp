/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#include "T3DGL4RenderBuffer.h"
#include "T3DGL4Renderer.h"
#include "T3DGL4ContextBase.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    namespace
    {
        bool hasCurrentGLContext()
        {
#if defined(T3D_OS_WINDOWS)
            return wglGetCurrentContext() != nullptr;
#elif defined(T3D_OS_LINUX)
            return glXGetCurrentContext() != nullptr;
#else
            return true;
#endif
        }

        void deleteGLObjects(const TArray<GLObjectName> &objects)
        {
            for (const GLObjectName &obj : objects)
            {
                GLuint name = obj.name;
                switch (obj.type)
                {
                case GLObjectType::kBuffer:         glDeleteBuffers(1, &name);          break;
                case GLObjectType::kTexture:        glDeleteTextures(1, &name);         break;
                case GLObjectType::kVertexArray:    glDeleteVertexArrays(1, &name);     break;
                case GLObjectType::kSampler:        glDeleteSamplers(1, &name);         break;
                case GLObjectType::kFramebuffer:    glDeleteFramebuffers(1, &name);     break;
                case GLObjectType::kRenderbuffer:   glDeleteRenderbuffers(1, &name);    break;
                case GLObjectType::kShader:         glDeleteShader(name);               break;
                case GLObjectType::kProgram:        glDeleteProgram(name);              break;
                }
            }
        }
    }

    void releaseGLObjects(std::initializer_list<GLObjectName> objects)
    {
        TArray<GLObjectName> names;
        names.reserve(objects.size());
        for (const GLObjectName &obj : objects)
        {
            if (obj.name != 0)
            {
                names.push_back(obj);
            }
        }

        if (names.empty())
        {
            return;
        }

        if (hasCurrentGLContext())
        {
            deleteGLObjects(names);
            return;
        }

        RHIThread *rhiThread = RHIThread::getInstancePtr();
        if (rhiThread != nullptr && rhiThread->isRunning())
        {
            auto lambda = [](const TArray<GLObjectName> &names)
            {
                deleteGLObjects(names);
                return T3D_OK;
            };

            rhiThread->enqueue_unique_command(lambda, std::move(names));
            return;
        }

        // RHI 线程已停止时 context 还留在已退出的 RHI 线程上，主线程只能借 dummy context
        GL4Renderer *renderer = GL4Renderer::getInstancePtr();
        if (renderer != nullptr)
        {
            RHIContextPtr ctx = renderer->getContext();
            if (ctx != nullptr)
            {
                static_cast<GL4ContextBase *>(ctx.get())->makeDummyCurrent();
            }
        }

        if (hasCurrentGLContext())
        {
            deleteGLObjects(names);
        }
        else
        {
            T3D_LOG_WARNING(LOG_TAG_GL4RENDERER, "releaseGLObjects : no GL context available, %u GL objects leaked !",
                (uint32_t)names.size());
        }
    }

    //--------------------------------------------------------------------------

    GL4VertexDeclarationPtr GL4VertexDeclaration::create()
    {
        return T3D_NEW GL4VertexDeclaration();
    }

    GL4VertexDeclaration::~GL4VertexDeclaration()
    {
        releaseGLObjects({{GLObjectType::kVertexArray, GLVAO}});
    }

    void *GL4VertexDeclaration::getNativeObject() const
    {
        return (void *)(uintptr_t)GLVAO;
    }

    //--------------------------------------------------------------------------

    GL4VertexBufferPtr GL4VertexBuffer::create()
    {
        return T3D_NEW GL4VertexBuffer();
    }

    GL4VertexBuffer::~GL4VertexBuffer()
    {
        releaseGLObjects({{GLObjectType::kBuffer, GLBuffer}});
    }

    void *GL4VertexBuffer::getNativeObject() const
    {
        return (void *)(uintptr_t)GLBuffer;
    }

    //--------------------------------------------------------------------------

    GL4IndexBufferPtr GL4IndexBuffer::create()
    {
        return T3D_NEW GL4IndexBuffer();
    }

    GL4IndexBuffer::~GL4IndexBuffer()
    {
        releaseGLObjects({{GLObjectType::kBuffer, GLBuffer}});
    }

    void *GL4IndexBuffer::getNativeObject() const
    {
        return (void *)(uintptr_t)GLBuffer;
    }

    //--------------------------------------------------------------------------

    GL4ConstantBufferPtr GL4ConstantBuffer::create()
    {
        return T3D_NEW GL4ConstantBuffer();
    }

    GL4ConstantBuffer::~GL4ConstantBuffer()
    {
        releaseGLObjects({{GLObjectType::kBuffer, GLBuffer}});
    }

    void *GL4ConstantBuffer::getNativeObject() const
    {
        return (void *)(uintptr_t)GLBuffer;
    }

    //--------------------------------------------------------------------------

    GL4PixelBuffer1DPtr GL4PixelBuffer1D::create()
    {
        return T3D_NEW GL4PixelBuffer1D();
    }

    GL4PixelBuffer1D::~GL4PixelBuffer1D()
    {
        releaseGLObjects({{GLObjectType::kTexture, GLTexture}});
    }

    void *GL4PixelBuffer1D::getNativeObject() const
    {
        return (void *)(uintptr_t)GLTexture;
    }

    //--------------------------------------------------------------------------

    GL4PixelBuffer2DPtr GL4PixelBuffer2D::create()
    {
        return T3D_NEW GL4PixelBuffer2D();
    }

    GL4PixelBuffer2D::~GL4PixelBuffer2D()
    {
        releaseGLObjects({
            {GLObjectType::kFramebuffer, GLResolveFBO},
            {GLObjectType::kTexture, GLResolveTex},
            {GLObjectType::kRenderbuffer, GLDepthRBO},
            {GLObjectType::kFramebuffer, GLFBO},
            {GLObjectType::kTexture, GLTexture},
        });
    }

    void *GL4PixelBuffer2D::getNativeObject() const
    {
        return (void *)(uintptr_t)GLTexture;
    }

    //--------------------------------------------------------------------------

    GL4PixelBuffer3DPtr GL4PixelBuffer3D::create()
    {
        return T3D_NEW GL4PixelBuffer3D();
    }

    GL4PixelBuffer3D::~GL4PixelBuffer3D()
    {
        releaseGLObjects({{GLObjectType::kTexture, GLTexture}});
    }

    void *GL4PixelBuffer3D::getNativeObject() const
    {
        return (void *)(uintptr_t)GLTexture;
    }

    //--------------------------------------------------------------------------

    GL4PixelBufferCubemapPtr GL4PixelBufferCubemap::create()
    {
        return T3D_NEW GL4PixelBufferCubemap();
    }

    GL4PixelBufferCubemap::~GL4PixelBufferCubemap()
    {
        releaseGLObjects({{GLObjectType::kTexture, GLTexture}});
    }

    void *GL4PixelBufferCubemap::getNativeObject() const
    {
        return (void *)(uintptr_t)GLTexture;
    }

    //--------------------------------------------------------------------------

    GL4StructuredBufferPtr GL4StructuredBuffer::create()
    {
        return T3D_NEW GL4StructuredBuffer();
    }

    GL4StructuredBuffer::~GL4StructuredBuffer()
    {
        releaseGLObjects({
            {GLObjectType::kTexture, GLTextureBuffer},
            {GLObjectType::kBuffer, GLCounterBuffer},
            {GLObjectType::kBuffer, GLBuffer},
        });
    }

    void *GL4StructuredBuffer::getNativeObject() const
    {
        return (void *)(uintptr_t)GLBuffer;
    }

    //--------------------------------------------------------------------------
}
