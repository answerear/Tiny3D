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
