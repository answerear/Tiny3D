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

#ifndef __T3D_GL4_RENDER_WINDOW_H__
#define __T3D_GL4_RENDER_WINDOW_H__


#include "T3DGL4Prerequisites.h"


namespace Tiny3D
{
    class GL4RenderWindow : public RHIRenderWindow
    {
    public:
        static GL4RenderWindowPtr create(RenderWindow *renderWindow);

        ~GL4RenderWindow() override;

        bool init(RenderWindow *renderWindow);

        TResult swapBuffers() override;

        TResult resize(uint32_t w, uint32_t h) override;

        void *getNativeObject() const override;

        /// OpenGL 上下文 (WGL: HGLRC, GLX: GLXContext)
#if defined(T3D_OS_WINDOWS)
        HGLRC   GLContext {nullptr};
        HDC     GLDeviceContext {nullptr};
#elif defined(T3D_OS_LINUX)
        GLXContext  GLContext {nullptr};
        Display     *GLDisplay {nullptr};
        ::Window    GLWindow {0};
#endif
        /// 默认 FBO (通常为 0)
        GLuint  GLDefaultFBO {0};
        /// 深度缓冲 RBO
        GLuint  GLDepthRBO {0};

        uint32_t mWidth {0};
        uint32_t mHeight {0};
        /// 实际获得的 MSAA 采样数
        uint32_t mMSAACount {1};

    protected:
        GL4RenderWindow();
    };
}


#endif  /*__T3D_GL4_RENDER_WINDOW_H__*/
