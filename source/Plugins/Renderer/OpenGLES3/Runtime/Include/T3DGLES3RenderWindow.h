/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GLES3_RENDER_WINDOW_H__
#define __T3D_GLES3_RENDER_WINDOW_H__


#include "T3DGLES3Prerequisites.h"


namespace Tiny3D
{
    class GLES3RenderWindow : public RHIRenderWindow
    {
    public:
        static GLES3RenderWindowPtr create(RenderWindow *renderWindow);

        ~GLES3RenderWindow() override;

        bool init(RenderWindow *renderWindow);

        TResult swapBuffers() override;

        TResult resize(uint32_t w, uint32_t h) override;

        void *getNativeObject() const override;

        EGLSurface getEGLSurface() const { return mEGLSurface; }

        EGLSurface  mEGLSurface {EGL_NO_SURFACE};
        GLuint      GLDefaultFBO {0};
        GLuint      GLDepthRBO {0};

        uint32_t mWidth {0};
        uint32_t mHeight {0};
        uint32_t mMSAACount {1};

    protected:
        GLES3RenderWindow();
    };
}


#endif  /*__T3D_GLES3_RENDER_WINDOW_H__*/
