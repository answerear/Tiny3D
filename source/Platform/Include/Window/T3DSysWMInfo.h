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

#ifndef __T3D_SYS_WM_INFO_H__
#define __T3D_SYS_WM_INFO_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    struct SysWMInfo
    {
#if defined (T3D_OS_WINDOWS)
        // Windows
        THandle     hWnd;           /**< The window handle */
        THandle     hDC;            /**< The window device context */
        THandle     hInstance;      /**< The window instance */
#elif defined (T3D_OS_LINUX)
        // Linux X11
        void        *display;       /**< The X11 display (Display) */
        ulong_t     window;        /**< The X11 window (Window) */
#elif defined (T3D_OS_OSX)
        // Mac OS X cocoa
        void        *window;        /**< The Cocoa window (NSWindow) */
#elif defined (T3D_OS_IOS)
        // iOS UIKit
        void        *window;        /**< The UIKit window (UIWindow) */
        uint32_t    framebuffer;    /**< The GL view's Framebuffer Object */
        uint32_t    colorbuffer;    /**< the GL view's color Renderbuffer Object */
#elif defined (T3D_OS_ANDROID)
        void        *window;        /**< The Android native window */
        void        *surface;       /**< The Android EGL surface */
#endif
    };
}


#endif  /*__T3D_SYS_WM_INFO_H__*/
