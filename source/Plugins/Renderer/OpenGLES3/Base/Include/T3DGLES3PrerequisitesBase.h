/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GLES3_PREREQUISITES_BASE_H__
#define __T3D_GLES3_PREREQUISITES_BASE_H__


#include <Tiny3D.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>

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

    #define LOG_TAG_GLES3RENDERER     "GLES3RENDERER"

    class GLES3ContextBase;
    T3D_DECLARE_SMART_PTR(GLES3ContextBase);
}


#endif  /*__T3D_GLES3_PREREQUISITES_BASE_H__*/
