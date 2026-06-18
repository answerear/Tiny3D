/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGLES3RenderWindow.h"
#include "T3DGLES3Context.h"
#include "T3DGLES3Error.h"
#include "T3DGLES3Renderer.h"
#include "T3DGLES3Mapping.h"

#if defined(T3D_OS_ANDROID)
#include <android/native_window.h>

// ABI-stable struct layout of ANativeWindow (from AOSP system/window.h).
// This layout hasn't changed since Android 4.0 and all EGL/Vulkan
// implementations depend on it being stable.
struct T3DNativeWindowBase
{
    int magic;
    int version;
    void *reserved[4];
    void (*incRef)(void *base);
    void (*decRef)(void *base);
};

struct T3DNativeWindow
{
    T3DNativeWindowBase common;
    uint32_t flags;
    int minSwapInterval;
    int maxSwapInterval;
    float xdpi;
    float ydpi;
    intptr_t oem[4];
    int (*setSwapInterval)(ANativeWindow *window, int interval);
    int (*dequeueBuffer_DEPRECATED)(ANativeWindow *window, void **buffer);
    int (*lockBuffer_DEPRECATED)(ANativeWindow *window, void *buffer);
    int (*queueBuffer_DEPRECATED)(ANativeWindow *window, void *buffer);
    int (*query)(const ANativeWindow *window, int what, int *value);
    int (*perform)(ANativeWindow *window, int operation, ...);
};

enum
{
    T3D_NATIVE_WINDOW_API_EGL = 1,
    T3D_NATIVE_WINDOW_API_CPU = 2,
    T3D_NATIVE_WINDOW_API_DISCONNECT = 14,
};

static inline int t3dNativeWindowApiDisconnect(ANativeWindow *window, int api)
{
    T3DNativeWindow *win = reinterpret_cast<T3DNativeWindow *>(window);
    if (win && win->perform)
    {
        return win->perform(window, T3D_NATIVE_WINDOW_API_DISCONNECT, api);
    }
    return -1;
}
#endif


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GLES3RenderWindowPtr GLES3RenderWindow::create(RenderWindow *renderWindow)
    {
        GLES3RenderWindowPtr rw = T3D_NEW GLES3RenderWindow();
        if (rw != nullptr && !rw->init(renderWindow))
        {
            rw = nullptr;
        }
        return rw;
    }

    //--------------------------------------------------------------------------

    GLES3RenderWindow::GLES3RenderWindow()
    {
    }

    //--------------------------------------------------------------------------

    GLES3RenderWindow::~GLES3RenderWindow()
    {
        GL_SAFE_DELETE_RBO(GLDepthRBO);

        if (mEGLSurface != EGL_NO_SURFACE)
        {
            GLES3ContextPtr ctx = GLES3_CONTEXT;
            if (ctx != nullptr)
            {
                eglDestroySurface(ctx->getEGLDisplay(), mEGLSurface);
            }
            mEGLSurface = EGL_NO_SURFACE;
        }
    }

    //--------------------------------------------------------------------------

    bool GLES3RenderWindow::init(RenderWindow *renderWindow)
    {
        GLES3ContextPtr ctx = GLES3_CONTEXT;
        if (ctx == nullptr)
            return false;

        SysWMInfo info;
        if (!renderWindow->getSystemInfo(info))
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "getSystemInfo failed!");
            return false;
        }

#if defined(T3D_OS_ANDROID)
        EGLNativeWindowType nativeWindow = (EGLNativeWindowType)info.window;

        if (nativeWindow == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "ANativeWindow is NULL!");
            return false;
        }

        EGLint nativeVisualId = 0;
        eglGetConfigAttrib(ctx->getEGLDisplay(), ctx->getEGLConfig(),
            EGL_NATIVE_VISUAL_ID, &nativeVisualId);

        int32_t winWidth = ANativeWindow_getWidth(nativeWindow);
        int32_t winHeight = ANativeWindow_getHeight(nativeWindow);
        int32_t winFormat = ANativeWindow_getFormat(nativeWindow);

        T3D_LOG_INFO(LOG_TAG_GLES3RENDERER,
            "ANativeWindow=%p, size=%dx%d, format=%d, EGL nativeVisualId=%d",
            nativeWindow, winWidth, winHeight, winFormat, nativeVisualId);

        ANativeWindow_setBuffersGeometry(nativeWindow, 0, 0, nativeVisualId);

        // Disconnect any existing API producer that may have been connected
        // by SDL or HWUI, allowing EGL to connect as the new producer.
        t3dNativeWindowApiDisconnect(nativeWindow, T3D_NATIVE_WINDOW_API_CPU);
        t3dNativeWindowApiDisconnect(nativeWindow, T3D_NATIVE_WINDOW_API_EGL);

        mEGLSurface = eglCreateWindowSurface(
            ctx->getEGLDisplay(), ctx->getEGLConfig(), nativeWindow, nullptr);

        if (mEGLSurface == EGL_NO_SURFACE)
        {
            EGLint err = eglGetError();
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER,
                "eglCreateWindowSurface failed (EGL error 0x%04X)", err);
            return false;
        }
#else
        T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Unsupported platform for GLES3 render window");
        return false;
#endif

        if (!eglMakeCurrent(ctx->getEGLDisplay(), mEGLSurface, mEGLSurface, ctx->getEGLContext()))
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "eglMakeCurrent failed (EGL error 0x%04X)", eglGetError());
            return false;
        }

        ctx->setMainSurface(mEGLSurface);

        EGLint w = 0, h = 0;
        eglQuerySurface(ctx->getEGLDisplay(), mEGLSurface, EGL_WIDTH, &w);
        eglQuerySurface(ctx->getEGLDisplay(), mEGLSurface, EGL_HEIGHT, &h);
        mWidth = static_cast<uint32_t>(w);
        mHeight = static_cast<uint32_t>(h);

        GLDefaultFBO = 0;

        T3D_LOG_INFO(LOG_TAG_GLES3RENDERER, "OpenGL ES Vendor: %s", glGetString(GL_VENDOR));
        T3D_LOG_INFO(LOG_TAG_GLES3RENDERER, "OpenGL ES Renderer: %s", glGetString(GL_RENDERER));
        T3D_LOG_INFO(LOG_TAG_GLES3RENDERER, "OpenGL ES Version: %s", glGetString(GL_VERSION));
        T3D_LOG_INFO(LOG_TAG_GLES3RENDERER, "GLSL ES Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
        T3D_LOG_INFO(LOG_TAG_GLES3RENDERER, "RenderWindow created (%u x %u)", mWidth, mHeight);

        GLES3Mapping::detectExtensions();

        return true;
    }

    //--------------------------------------------------------------------------

    TResult GLES3RenderWindow::swapBuffers()
    {
        GLES3ContextPtr ctx = GLES3_CONTEXT;
        if (ctx != nullptr)
        {
            return ctx->swapBackBuffer(this);
        }
        return T3D_ERR_GLES3_SWAP_BUFFERS;
    }

    //--------------------------------------------------------------------------

    TResult GLES3RenderWindow::resize(uint32_t w, uint32_t h)
    {
        GLES3ContextPtr ctx = GLES3_CONTEXT;
        if (ctx != nullptr)
        {
            return ctx->resizeRenderWindow(this, w, h);
        }
        return T3D_ERR_GLES3_RESIZE_BUFFERS;
    }

    //--------------------------------------------------------------------------

    void *GLES3RenderWindow::getNativeObject() const
    {
        return (void *)(intptr_t)GLDefaultFBO;
    }
}
