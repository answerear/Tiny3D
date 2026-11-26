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


#ifndef __T3D_GL4_PREREQUISITES_H__
#define __T3D_GL4_PREREQUISITES_H__


#include "T3DGL4PrerequisitesBase.h"


#if defined GL4RENDERER_EXPORT
    #define T3D_GL4RENDERER_API     T3D_EXPORT_API
#else
    #define T3D_GL4RENDERER_API     T3D_IMPORT_API
#endif


namespace Tiny3D
{
    //-------------------------------------------------------------------------
    // GL 对象安全释放宏
    //-------------------------------------------------------------------------

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

    //-------------------------------------------------------------------------
    // 跨线程释放 GL 对象
    //-------------------------------------------------------------------------

    enum class GLObjectType : uint8_t
    {
        kBuffer = 0,
        kTexture,
        kVertexArray,
        kSampler,
        kFramebuffer,
        kRenderbuffer,
        kShader,
        kProgram,
    };

    struct GLObjectName
    {
        GLObjectType    type;
        GLuint          name;
    };

    /**
     * \brief 释放一组 GL 对象，名字为 0 的项会被忽略
     * \remarks 开启 RHI 线程后 GL context 归 RHI 线程所有，主线程上没有 current context，
     *  而 RHI 资源的最后一个引用往往在主线程释放。当前线程有 context 时立即删除；
     *  否则作为一条命令投递到 RHI 线程，排在所有已入队命令之后执行；
     *  RHI 线程已停止时借 dummy context 在当前线程删除。
     *  RHI 资源的析构函数必须走这里，不能直接调用 GL_SAFE_DELETE_*。
     */
    void releaseGLObjects(std::initializer_list<GLObjectName> objects);

    /// 同上，供个数在运行时才确定的对象集合（如子资源纹理视图）使用
    void releaseGLObjects(const TArray<GLObjectName> &objects);

    //-------------------------------------------------------------------------
    // 前置声明
    //-------------------------------------------------------------------------

    class GL4Renderer;
    class GL4Context;
    class GL4RenderWindow;
    class GL4BlendState;
    class GL4DepthStencilState;
    class GL4RasterizerState;
    class GL4SamplerState;
    class GL4VertexDeclaration;
    class GL4VertexBuffer;
    class GL4IndexBuffer;
    class GL4ConstantBuffer;
    class GL4PixelBuffer1D;
    class GL4PixelBuffer2D;
    class GL4PixelBuffer3D;
    class GL4PixelBufferCubemap;
    class GL4StructuredBuffer;
    class GL4Shader;
    class GL4VertexShader;
    class GL4PixelShader;
    class GL4HullShader;
    class GL4DomainShader;
    class GL4GeometryShader;
    class GL4ComputeShader;

    //-------------------------------------------------------------------------
    // 智能指针声明
    //-------------------------------------------------------------------------

    T3D_DECLARE_SMART_PTR(GL4Renderer);
    T3D_DECLARE_SMART_PTR(GL4Context);
    T3D_DECLARE_SMART_PTR(GL4RenderWindow);
    T3D_DECLARE_SMART_PTR(GL4BlendState);
    T3D_DECLARE_SMART_PTR(GL4DepthStencilState);
    T3D_DECLARE_SMART_PTR(GL4RasterizerState);
    T3D_DECLARE_SMART_PTR(GL4SamplerState);
    T3D_DECLARE_SMART_PTR(GL4VertexDeclaration);
    T3D_DECLARE_SMART_PTR(GL4VertexBuffer);
    T3D_DECLARE_SMART_PTR(GL4IndexBuffer);
    T3D_DECLARE_SMART_PTR(GL4ConstantBuffer);
    T3D_DECLARE_SMART_PTR(GL4PixelBuffer1D);
    T3D_DECLARE_SMART_PTR(GL4PixelBuffer2D);
    T3D_DECLARE_SMART_PTR(GL4PixelBuffer3D);
    T3D_DECLARE_SMART_PTR(GL4PixelBufferCubemap);
    T3D_DECLARE_SMART_PTR(GL4StructuredBuffer);
    T3D_DECLARE_SMART_PTR(GL4Shader);
    T3D_DECLARE_SMART_PTR(GL4VertexShader);
    T3D_DECLARE_SMART_PTR(GL4PixelShader);
    T3D_DECLARE_SMART_PTR(GL4HullShader);
    T3D_DECLARE_SMART_PTR(GL4DomainShader);
    T3D_DECLARE_SMART_PTR(GL4GeometryShader);
    T3D_DECLARE_SMART_PTR(GL4ComputeShader);
}


#endif  /*__T3D_GL4_PREREQUISITES_H__*/
