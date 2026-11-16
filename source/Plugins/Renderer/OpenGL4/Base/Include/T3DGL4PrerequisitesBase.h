/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GL4_PREREQUISITES_BASE_H__
#define __T3D_GL4_PREREQUISITES_BASE_H__


#include <Tiny3D.h>
#include <glad/glad.h>

#if defined(T3D_OS_WINDOWS)
    #include <Windows.h>

    //-------------------------------------------------------------------------
    // WGL_ARB_multisample / WGL_ARB_pixel_format constants
    //-------------------------------------------------------------------------
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

    //-------------------------------------------------------------------------
    // WGL_ARB_create_context / WGL_ARB_create_context_profile constants
    //-------------------------------------------------------------------------
    #ifndef WGL_CONTEXT_MAJOR_VERSION_ARB
        #define WGL_CONTEXT_MAJOR_VERSION_ARB   0x2091
    #endif
    #ifndef WGL_CONTEXT_MINOR_VERSION_ARB
        #define WGL_CONTEXT_MINOR_VERSION_ARB   0x2092
    #endif
    #ifndef WGL_CONTEXT_FLAGS_ARB
        #define WGL_CONTEXT_FLAGS_ARB           0x2094
    #endif
    #ifndef WGL_CONTEXT_PROFILE_MASK_ARB
        #define WGL_CONTEXT_PROFILE_MASK_ARB    0x9126
    #endif
    #ifndef WGL_CONTEXT_CORE_PROFILE_BIT_ARB
        #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
    #endif
    #ifndef WGL_CONTEXT_DEBUG_BIT_ARB
        #define WGL_CONTEXT_DEBUG_BIT_ARB       0x00000001
    #endif

    // WGL extension function pointer types
    typedef BOOL (WINAPI *PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
    typedef const char *(WINAPI *PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC hdc);
    typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int *attribList);
#elif defined(T3D_OS_LINUX)
    #include <GL/glx.h>
    #include <GL/glxext.h>
    #include <X11/Xlib.h>
#endif

#include <algorithm>
#undef min


namespace Tiny3D
{
    #define GL_SAFE_DELETE_SHADER(p) \
        if ((p) != 0) { glDeleteShader(p); (p) = 0; }

    #define GL_SAFE_DELETE_PROGRAM(p) \
        if ((p) != 0) { glDeleteProgram(p); (p) = 0; }

#if defined(T3D_DEBUG)
    #define GL_CHECK_ERROR(tag, msg) \
        { \
            GLenum _glErr; \
            int _glErrCount = 0; \
            while ((_glErr = glGetError()) != GL_NO_ERROR && _glErrCount < 16) \
            { \
                T3D_LOG_ERROR(tag, "%s : GL Error 0x%04X", msg, _glErr); \
                ++_glErrCount; \
            } \
            if (_glErrCount >= 16) \
            { \
                T3D_LOG_ERROR(tag, "%s : Too many GL errors (no GL context on this thread?)", msg); \
            } \
        }
#else
    #define GL_CHECK_ERROR(tag, msg) (void)0
#endif

    #define LOG_TAG_GL4RENDERER     "GL4RENDERER"

    class GL4ContextBase;
    T3D_DECLARE_SMART_PTR(GL4ContextBase);
}


#endif  /*__T3D_GL4_PREREQUISITES_BASE_H__*/
