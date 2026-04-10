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


#include <Tiny3D.h>
#include <glad/glad.h>

#if defined(T3D_OS_WINDOWS)
    #include <Windows.h>
#elif defined(T3D_OS_LINUX)
    #include <GL/glx.h>
    #include <GL/glxext.h>
    #include <X11/Xlib.h>
#endif

#include <algorithm>
#undef min


//-------------------------------------------------------------------------
// WGL_ARB_multisample / WGL_ARB_pixel_format constants
//-------------------------------------------------------------------------
#if defined(T3D_OS_WINDOWS)
    #ifndef WGL_DRAW_TO_WINDOW_ARB
        #define WGL_DRAW_TO_WINDOW_ARB          0x2001
    #endif
    #ifndef WGL_SUPPORT_OPENGL_ARB
        #define WGL_SUPPORT_OPENGL_ARB          0x2010
    #endif
    #ifndef WGL_DOUBLE_BUFFER_ARB
        #define WGL_DOUBLE_BUFFER_ARB           0x2011
    #endif
    #ifndef WGL_PIXEL_TYPE_ARB
        #define WGL_PIXEL_TYPE_ARB              0x2013
    #endif
    #ifndef WGL_TYPE_RGBA_ARB
        #define WGL_TYPE_RGBA_ARB               0x202B
    #endif
    #ifndef WGL_COLOR_BITS_ARB
        #define WGL_COLOR_BITS_ARB              0x2014
    #endif
    #ifndef WGL_DEPTH_BITS_ARB
        #define WGL_DEPTH_BITS_ARB              0x2022
    #endif
    #ifndef WGL_STENCIL_BITS_ARB
        #define WGL_STENCIL_BITS_ARB            0x2023
    #endif
    #ifndef WGL_SAMPLE_BUFFERS_ARB
        #define WGL_SAMPLE_BUFFERS_ARB          0x2041
    #endif
    #ifndef WGL_SAMPLES_ARB
        #define WGL_SAMPLES_ARB                 0x2042
    #endif

    // WGL extension function pointer types
    typedef BOOL (WINAPI *PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
    typedef const char *(WINAPI *PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC hdc);
#endif


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

    #define GL_SAFE_DELETE_PROGRAM(p) \
        if ((p) != 0) { glDeleteProgram(p); (p) = 0; }

    #define GL_SAFE_DELETE_SHADER(p) \
        if ((p) != 0) { glDeleteShader(p); (p) = 0; }

    #define GL_SAFE_DELETE_SAMPLER(p) \
        if ((p) != 0) { glDeleteSamplers(1, &(p)); (p) = 0; }

    #define GL_SAFE_DELETE_FBO(p) \
        if ((p) != 0) { glDeleteFramebuffers(1, &(p)); (p) = 0; }

    #define GL_SAFE_DELETE_RBO(p) \
        if ((p) != 0) { glDeleteRenderbuffers(1, &(p)); (p) = 0; }

    //-------------------------------------------------------------------------
    // GL 错误检查宏
    //-------------------------------------------------------------------------

#if defined(T3D_DEBUG)
    #define GL_CHECK_ERROR(tag, msg) \
        { \
            GLenum _glErr; \
            while ((_glErr = glGetError()) != GL_NO_ERROR) \
            { \
                T3D_LOG_ERROR(tag, "%s : GL Error 0x%04X", msg, _glErr); \
            } \
        }
#else
    #define GL_CHECK_ERROR(tag, msg) (void)0
#endif

    //-------------------------------------------------------------------------
    // 日志标签
    //-------------------------------------------------------------------------

    #define LOG_TAG_GL4RENDERER     "GL4RENDERER"

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
    T3D_DECLARE_SMART_PTR(GL4Shader);
    T3D_DECLARE_SMART_PTR(GL4VertexShader);
    T3D_DECLARE_SMART_PTR(GL4PixelShader);
    T3D_DECLARE_SMART_PTR(GL4HullShader);
    T3D_DECLARE_SMART_PTR(GL4DomainShader);
    T3D_DECLARE_SMART_PTR(GL4GeometryShader);
    T3D_DECLARE_SMART_PTR(GL4ComputeShader);
}


#endif  /*__T3D_GL4_PREREQUISITES_H__*/
