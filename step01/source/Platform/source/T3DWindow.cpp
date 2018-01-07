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


namespace Tiny3D
{
    Window::Window(bool isGLWindow)
    {
        mWindow = T3D_PLATFORM_FACTORY.createPlatformWindow(isGLWindow);
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
        int32_t w, int32_t h, bool isFullscreen, int32_t argc, ...)
    {
        bool ret = false;

        if (mWindow != nullptr)
        {
            va_list args;
            va_start(args, argc);
            ret = mWindow->create(title, x, y, w, h, isFullscreen, argc, args);
            va_end(args);
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

    void Window::render()
    {
        if (mWindow != nullptr)
        {
            mWindow->render();
        }
    }
}
