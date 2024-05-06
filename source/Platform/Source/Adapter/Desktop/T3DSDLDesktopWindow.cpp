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

#include "Adapter/Desktop/T3DSDLDesktopWindow.h"
#include "T3DPlatformErrorDef.h"
#include <SDL_syswm.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    SDLDesktopWindow::SDLDesktopWindow()
        : mSDLWindow(nullptr)
        , mSDLIconSurface(nullptr)
        , mFramebuffer(nullptr)
        , mFramebufferSize(0)
    {

    }

    //--------------------------------------------------------------------------

    SDLDesktopWindow::~SDLDesktopWindow()
    {

    }

    //--------------------------------------------------------------------------

    TResult SDLDesktopWindow::create(const char *title, int32_t x, int32_t y,
        int32_t w, int32_t h, uint32_t flags)
    {
        TResult ret = T3D_ERR_FAIL;

        do 
        {
            if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO)
            {
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            mSDLWindow = SDL_CreateWindow(title, x, y, w, h, flags);
            if (mSDLWindow == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                std::string str = SDL_GetError();
                break;
            }

            ret = T3D_OK;
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult SDLDesktopWindow::createFrom(const void *data)
    {
        TResult ret = T3D_ERR_FAIL;

        do 
        {
            if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO)
            {
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            mSDLWindow = SDL_CreateWindowFrom(data);
            if (mSDLWindow == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                std::string str = SDL_GetError();
                break;
            }

            ret = T3D_OK;
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    void SDLDesktopWindow::destroy()
    {
        SDL_Surface *buffer = SDL_GetWindowSurface(mSDLWindow);

        if (SDL_MUSTLOCK(buffer))
        {
            T3D_SAFE_DELETE_ARRAY(mFramebuffer);
        }

        if (mSDLIconSurface != nullptr)
        {
            SDL_FreeSurface(mSDLIconSurface);
            mSDLIconSurface = nullptr;
        }

        if (mSDLWindow != nullptr)
        {
            SDL_DestroyWindow(mSDLWindow);
            mSDLWindow = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    bool SDLDesktopWindow::getSystemInfo(SysWMInfo &info) const
    {
        SDL_SysWMinfo sdlInfo;

        SDL_VERSION(&sdlInfo.version);

        bool ret = (SDL_GetWindowWMInfo(mSDLWindow, &sdlInfo) == SDL_TRUE);

        if (ret)
        {
#if defined (T3D_OS_WINDOWS)
            info.hWnd = sdlInfo.info.win.window;
            info.hDC = sdlInfo.info.win.hdc;
            info.hInstance = sdlInfo.info.win.hinstance;
#elif defined (T3D_OS_LINUX)
            // Linux X11
            info.display = sdlInfo.info.x11.display;
            info.window = sdlInfo.info.x11.window;
#elif defined (T3D_OS_OSX)
            // Mac OS X cocoa
            info.window = sdlInfo.info.cocoa.window;
#elif defined (T3D_OS_IOS)
            // iOS UIKit
            info.window = sdlInfo.info.uikit.window;
            info.framebuffer = sdlInfo.info.uikit.framebuffer;
            info.colorbuffer = sdlInfo.info.uikit.colorbuffer;
#elif defined (T3D_OS_ANDROID)
            info.window = sdlInfo.info.android.window;
            info.surface = sdlInfo.info.android.surface;
#endif
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void *SDLDesktopWindow::getNativeObject() const
    {
        return mSDLWindow;
    }

    //--------------------------------------------------------------------------

    void SDLDesktopWindow::setWindowIcon(void *pixels, int32_t width,
        int32_t height, int32_t depth, int32_t pitch, uint32_t format)
    {
        if (mSDLIconSurface != nullptr)
        {
            SDL_FreeSurface(mSDLIconSurface);
            mSDLIconSurface = nullptr;
        }

        if (mSDLWindow != nullptr)
        {
            mSDLIconSurface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width,
                height, depth, pitch, format);

            if (mSDLIconSurface != nullptr)
            {
                SDL_SetWindowIcon(mSDLWindow, mSDLIconSurface);
            }
        }
    }

    //--------------------------------------------------------------------------

    void SDLDesktopWindow::getWindowSize(int32_t &width, 
        int32_t &height) const
    {
        width = 0, height = 0;

        if (mSDLWindow != nullptr)
        {
            SDL_GetWindowSize(mSDLWindow, &width, &height);
        }
    }

    //--------------------------------------------------------------------------

    uint32_t SDLDesktopWindow::getColorDepth() const
    {
        uint32_t depth = 0;

        if (mSDLWindow != nullptr)
        {
            uint32_t format = SDL_GetWindowPixelFormat(mSDLWindow);
            uint32_t bpp = SDL_BYTESPERPIXEL(format);
            depth = (bpp << 3);
        }

        return depth;
    }

    //--------------------------------------------------------------------------

    uint8_t *SDLDesktopWindow::getFramebuffer()
    {
        if (mSDLWindow == nullptr)
            return nullptr;

        SDL_Surface *buffer = SDL_GetWindowSurface(mSDLWindow);

        if (SDL_MUSTLOCK(buffer))
        {
            if (mFramebuffer == nullptr)
            {
                // 没有帧缓冲，先创建跟窗口一样大小的帧缓冲
                int32_t w, h;
                SDL_GetWindowSize(mSDLWindow, &w, &h);
                SDL_Surface *frontbuffer = SDL_GetWindowSurface(mSDLWindow);
                mFramebufferSize = frontbuffer->pitch * h;
                mFramebuffer = new uint8_t[mFramebufferSize];
            }
        }
        else
        {
            int32_t w, h;
            SDL_GetWindowSize(mSDLWindow, &w, &h);
            mFramebuffer = (uint8_t*)buffer->pixels;
            mFramebufferSize = buffer->pitch * h;
        }

        return mFramebuffer;
    }

    size_t SDLDesktopWindow::getFramebufferSize() const
    {
        return mFramebufferSize;
    }

    //--------------------------------------------------------------------------

    TResult SDLDesktopWindow::updateWindow()
    {
        TResult ret = T3D_OK;

        SDL_Surface *frontbuffer = SDL_GetWindowSurface(mSDLWindow);

        if (SDL_MUSTLOCK(frontbuffer))
        {
            SDL_LockSurface(frontbuffer);
            memcpy(frontbuffer->pixels, mFramebuffer, mFramebufferSize);
            SDL_UnlockSurface(frontbuffer);
        }

        SDL_UpdateWindowSurface(mSDLWindow);

        return ret;
    }
}
