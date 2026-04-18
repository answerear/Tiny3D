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


#ifndef __T3D_NULLGL4_PREREQUISITES_H__
#define __T3D_NULLGL4_PREREQUISITES_H__


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


#if defined T3D_NULLGL4_EXPORT
    #define T3D_NULLGL4_API        T3D_EXPORT_API
#else
    #define T3D_NULLGL4_API        T3D_IMPORT_API
#endif


namespace Tiny3D
{
    #define GL_SAFE_DELETE_SHADER(p) \
        if ((p) != 0) { glDeleteShader(p); (p) = 0; }

    #define GL_SAFE_DELETE_PROGRAM(p) \
        if ((p) != 0) { glDeleteProgram(p); (p) = 0; }

    #define LOG_TAG_NULLGL4RENDERER        "NullGL4Renderer"

    class NullGL4Renderer;
    class NullGL4Context;
    class NullGL4Window;
    class NullGL4VertexDeclaration;
    class NullGL4VertexBuffer;
    class NullGL4IndexBuffer;
    class NullGL4ConstantBuffer;
    class NullGL4PixelBuffer1D;
    class NullGL4PixelBuffer2D;
    class NullGL4PixelBuffer3D;
    class NullGL4BlendState;
    class NullGL4RasterizerState;
    class NullGL4DepthStencilState;
    class NullGL4SamplerState;
    class NullGL4Shader;
    class NullGL4VertexShader;
    class NullGL4HullShader;
    class NullGL4DomainShader;
    class NullGL4GeometryShader;
    class NullGL4PixelShader;
    class NullGL4ComputeShader;

    T3D_DECLARE_SMART_PTR(NullGL4Renderer);
    T3D_DECLARE_SMART_PTR(NullGL4Context);
    T3D_DECLARE_SMART_PTR(NullGL4Window);
    T3D_DECLARE_SMART_PTR(NullGL4VertexDeclaration);
    T3D_DECLARE_SMART_PTR(NullGL4VertexBuffer);
    T3D_DECLARE_SMART_PTR(NullGL4IndexBuffer);
    T3D_DECLARE_SMART_PTR(NullGL4ConstantBuffer);
    T3D_DECLARE_SMART_PTR(NullGL4PixelBuffer1D);
    T3D_DECLARE_SMART_PTR(NullGL4PixelBuffer2D);
    T3D_DECLARE_SMART_PTR(NullGL4PixelBuffer3D);
    T3D_DECLARE_SMART_PTR(NullGL4BlendState);
    T3D_DECLARE_SMART_PTR(NullGL4RasterizerState);
    T3D_DECLARE_SMART_PTR(NullGL4DepthStencilState);
    T3D_DECLARE_SMART_PTR(NullGL4SamplerState);
    T3D_DECLARE_SMART_PTR(NullGL4Shader);
    T3D_DECLARE_SMART_PTR(NullGL4VertexShader);
    T3D_DECLARE_SMART_PTR(NullGL4HullShader);
    T3D_DECLARE_SMART_PTR(NullGL4DomainShader);
    T3D_DECLARE_SMART_PTR(NullGL4GeometryShader);
    T3D_DECLARE_SMART_PTR(NullGL4PixelShader);
    T3D_DECLARE_SMART_PTR(NullGL4ComputeShader);
}


#endif  /*__T3D_NULLGL4_PREREQUISITES_H__*/
