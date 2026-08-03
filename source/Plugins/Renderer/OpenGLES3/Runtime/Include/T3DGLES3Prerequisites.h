/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GLES3_PREREQUISITES_H__
#define __T3D_GLES3_PREREQUISITES_H__


#include "T3DGLES3PrerequisitesBase.h"


#if defined(GLES3RENDERER_EXPORT)
    #define T3D_GLES3RENDERER_API   T3D_EXPORT_API
#else
    #define T3D_GLES3RENDERER_API   T3D_IMPORT_API
#endif


namespace Tiny3D
{
    #define GL_SAFE_DELETE_BUFFER(p) \
        if ((p) != 0) { glDeleteBuffers(1, &(p)); (p) = 0; }

    #define GL_SAFE_DELETE_TEXTURE(p) \
        if ((p) != 0) { glDeleteTextures(1, &(p)); (p) = 0; }

    #define GL_SAFE_DELETE_VAO(p) \
        if ((p) != 0) { glDeleteVertexArrays(1, &(p)); (p) = 0; }

    #define GL_SAFE_DELETE_SAMPLER(p) \
        if ((p) != 0) { glDeleteSamplers(1, &(p)); (p) = 0; }

    #define GL_SAFE_DELETE_FBO(p) \
        if ((p) != 0) { glDeleteFramebuffers(1, &(p)); (p) = 0; }

    #define GL_SAFE_DELETE_RBO(p) \
        if ((p) != 0) { glDeleteRenderbuffers(1, &(p)); (p) = 0; }

    class GLES3Renderer;
    class GLES3Context;
    class GLES3RenderWindow;
    class GLES3BlendState;
    class GLES3DepthStencilState;
    class GLES3RasterizerState;
    class GLES3SamplerState;
    class GLES3VertexDeclaration;
    class GLES3VertexBuffer;
    class GLES3IndexBuffer;
    class GLES3ConstantBuffer;
    class GLES3PixelBuffer1D;
    class GLES3PixelBuffer2D;
    class GLES3PixelBuffer3D;
    class GLES3PixelBufferCubemap;
    class GLES3Shader;
    class GLES3VertexShader;
    class GLES3PixelShader;
    class GLES3HullShader;
    class GLES3DomainShader;
    class GLES3GeometryShader;
    class GLES3ComputeShader;

    T3D_DECLARE_SMART_PTR(GLES3Renderer);
    T3D_DECLARE_SMART_PTR(GLES3Context);
    T3D_DECLARE_SMART_PTR(GLES3RenderWindow);
    T3D_DECLARE_SMART_PTR(GLES3BlendState);
    T3D_DECLARE_SMART_PTR(GLES3DepthStencilState);
    T3D_DECLARE_SMART_PTR(GLES3RasterizerState);
    T3D_DECLARE_SMART_PTR(GLES3SamplerState);
    T3D_DECLARE_SMART_PTR(GLES3VertexDeclaration);
    T3D_DECLARE_SMART_PTR(GLES3VertexBuffer);
    T3D_DECLARE_SMART_PTR(GLES3IndexBuffer);
    T3D_DECLARE_SMART_PTR(GLES3ConstantBuffer);
    T3D_DECLARE_SMART_PTR(GLES3PixelBuffer1D);
    T3D_DECLARE_SMART_PTR(GLES3PixelBuffer2D);
    T3D_DECLARE_SMART_PTR(GLES3PixelBuffer3D);
    T3D_DECLARE_SMART_PTR(GLES3PixelBufferCubemap);
    T3D_DECLARE_SMART_PTR(GLES3Shader);
    T3D_DECLARE_SMART_PTR(GLES3VertexShader);
    T3D_DECLARE_SMART_PTR(GLES3PixelShader);
    T3D_DECLARE_SMART_PTR(GLES3HullShader);
    T3D_DECLARE_SMART_PTR(GLES3DomainShader);
    T3D_DECLARE_SMART_PTR(GLES3GeometryShader);
    T3D_DECLARE_SMART_PTR(GLES3ComputeShader);
}


#endif  /*__T3D_GLES3_PREREQUISITES_H__*/
