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

#include "T3DWindow.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Adapter/T3DWindowInterface.h"
#include "T3DPlatform.h"
#include "T3DPlatformErrorDef.h"
#include <stdarg.h>
#include <SDL.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const uint32_t Window::WINDOW_FULLSCREEN = SDL_WINDOW_FULLSCREEN;
    const uint32_t Window::WINDOW_FULLSCREEN_DESKTOP = SDL_WINDOW_FULLSCREEN_DESKTOP;
    const uint32_t Window::WINDOW_OPENGL = SDL_WINDOW_OPENGL;
    const uint32_t Window::WINDOW_SHOWN = SDL_WINDOW_SHOWN;
    const uint32_t Window::WINDOW_HIDDEN = SDL_WINDOW_HIDDEN;
    const uint32_t Window::WINDOW_BORDERLESS = SDL_WINDOW_BORDERLESS;
    const uint32_t Window::WINDOW_RESIZABLE = SDL_WINDOW_RESIZABLE;
    const uint32_t Window::WINDOW_MINIMIZED = SDL_WINDOW_MINIMIZED;
    const uint32_t Window::WINDOW_MAXIMIZED = SDL_WINDOW_MAXIMIZED;
    const uint32_t Window::WINDOW_INPUT_GRABBED = SDL_WINDOW_INPUT_GRABBED;
    const uint32_t Window::WINDOW_INPUT_FOCUS = SDL_WINDOW_INPUT_FOCUS;
    const uint32_t Window::WINDOW_MOUSE_CAPTURE = SDL_WINDOW_MOUSE_CAPTURE;

    //--------------------------------------------------------------------------

    const uint32_t Window::PIXEL_FORMAT_INDEX8 = SDL_PIXELFORMAT_INDEX8;
    const uint32_t Window::PIXEL_FORMAT_RGB444 = SDL_PIXELFORMAT_RGB444;
    const uint32_t Window::PIXEL_FORMAT_RGB555 = SDL_PIXELFORMAT_RGB555;
    const uint32_t Window::PIXEL_FORMAT_BGR555 = SDL_PIXELFORMAT_BGR555;
    const uint32_t Window::PIXEL_FORMAT_ARGB4444 = SDL_PIXELFORMAT_ARGB4444;
    const uint32_t Window::PIXEL_FORMAT_RGBA4444 = SDL_PIXELFORMAT_RGBA4444;
    const uint32_t Window::PIXEL_FORMAT_ABGR4444 = SDL_PIXELFORMAT_ABGR4444;
    const uint32_t Window::PIXEL_FORMAT_BGRA4444 = SDL_PIXELFORMAT_BGRA4444;
    const uint32_t Window::PIXEL_FORMAT_ARGB1555 = SDL_PIXELFORMAT_ARGB1555;
    const uint32_t Window::PIXEL_FORMAT_RGBA5551 = SDL_PIXELFORMAT_RGBA5551;
    const uint32_t Window::PIXEL_FORMAT_ABGR1555 = SDL_PIXELFORMAT_ABGR1555;
    const uint32_t Window::PIXEL_FORMAT_BGRA5551 = SDL_PIXELFORMAT_BGRA5551;
    const uint32_t Window::PIXEL_FORMAT_RGB565 = SDL_PIXELFORMAT_RGB565;
    const uint32_t Window::PIXEL_FORMAT_BGR565 = SDL_PIXELFORMAT_BGR565;
    const uint32_t Window::PIXEL_FORMAT_RGB24 = SDL_PIXELFORMAT_RGB24;
    const uint32_t Window::PIXEL_FORMAT_BGR24 = SDL_PIXELFORMAT_BGR24;
    const uint32_t Window::PIXEL_FORMAT_RGB888 = SDL_PIXELFORMAT_RGB888;
    const uint32_t Window::PIXEL_FORMAT_RGBX8888 = SDL_PIXELFORMAT_RGBX8888;
    const uint32_t Window::PIXEL_FORMAT_BGR888 = SDL_PIXELFORMAT_BGR888;
    const uint32_t Window::PIXEL_FORMAT_BGRX8888 = SDL_PIXELFORMAT_BGRX8888;
    const uint32_t Window::PIXEL_FORMAT_ARGB8888 = SDL_PIXELFORMAT_ARGB8888;
    const uint32_t Window::PIXEL_FORMAT_RGBA8888 = SDL_PIXELFORMAT_RGBA8888;
    const uint32_t Window::PIXEL_FORMAT_ABGR8888 = SDL_PIXELFORMAT_ABGR8888;
    const uint32_t Window::PIXEL_FORMAT_BGRA8888 = SDL_PIXELFORMAT_BGRA8888;

    //--------------------------------------------------------------------------

    Window::Window()
    {
        mWindow = T3D_PLATFORM_FACTORY.createPlatformWindow();
    }

    Window::~Window()
    {
        T3D_SAFE_DELETE(mWindow);
    }

    //--------------------------------------------------------------------------

    TResult Window::create(const char *title, int32_t x, int32_t y,
        int32_t w, int32_t h, uint32_t flags)
    {
        TResult ret = T3D_ERR_FAIL;

        if (mWindow != nullptr)
        {
            ret = mWindow->create(title, x, y, w, h, flags);
        }
        
        return ret;
    }

    TResult Window::createFrom(const void *data)
    {
        TResult ret = T3D_ERR_FAIL;

        if (mWindow != nullptr)
        {
            ret = mWindow->createFrom(data);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void Window::destroy()
    {
        if (mWindow != nullptr)
        {
            mWindow->destroy();
        }
    }

    //--------------------------------------------------------------------------

    void Window::setWindowIcon(const WindowIcon &icon)
    {
        if (mWindow != nullptr)
        {
            mWindow->setWindowIcon(icon.pixels, (int32_t)icon.width, 
                (int32_t)icon.height, (int32_t)icon.depth, (int32_t)icon.pitch, 
                icon.format);
        }
    }

    //--------------------------------------------------------------------------

    bool Window::getSystemInfo(SysWMInfo &info) const
    {
        if (mWindow != nullptr)
        {
            return mWindow->getSystemInfo(info);
        }

        return false;
    }

    //--------------------------------------------------------------------------

    void Window::getWindowSize(int32_t &width, int32_t &height) const
    {
        if (mWindow != nullptr)
        {
            mWindow->getWindowSize(width, height);
        }
    }

    uint32_t Window::getColorDepth() const
    {
        if (mWindow != nullptr)
        {
            return mWindow->getColorDepth();
        }

        return 0;
    }

    //--------------------------------------------------------------------------

    uint8_t *Window::getFramebuffer()
    {
        if (mWindow != nullptr)
        {
            return mWindow->getFramebuffer();
        }

        return nullptr;
    }

    size_t Window::getFramebufferSize() const
    {
        if (mWindow != nullptr)
        {
            return mWindow->getFramebufferSize();
        }

        return 0;
    }

    //--------------------------------------------------------------------------

    TResult Window::updateWindow()
    {
        TResult ret = T3D_ERR_SYS_NOT_INIT;

        if (mWindow != nullptr)
        {
            ret = mWindow->updateWindow();
        }

        return ret;
    }
}
