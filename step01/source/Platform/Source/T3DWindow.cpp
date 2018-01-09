/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "T3DWindow.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Adapter/T3DWindowInterface.h"
#include "T3DSystem.h"
#include <stdarg.h>
#include <SDL.h>


namespace Tiny3D
{
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

    Window::Window()
    {
        mWindow = T3D_PLATFORM_FACTORY.createPlatformWindow();
    }

    Window::~Window()
    {
        T3D_SAFE_DELETE(mWindow);
    }

    void Window::setWindowEventListener(WindowEventListener *listener)
    {
        mEventListener = listener;

        if (mWindow != nullptr)
        {
            mWindow->setWindowEventListener(listener);
        }
    }

    bool Window::create(const char *title, int32_t x, int32_t y,
        int32_t w, int32_t h, uint32_t flags)
    {
        bool ret = false;

        if (mWindow != nullptr)
        {
            ret = mWindow->create(title, x, y, w, h, flags);
        }
        
        return ret;
    }

    void Window::destroy()
    {
        if (mWindow != nullptr)
        {
            mWindow->destroy();
        }
    }
}
