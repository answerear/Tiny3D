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

#include "Adapter/T3DSDLWindow.h"
#include <SDL_syswm.h>

namespace Tiny3D
{
    SDLWindow::SDLWindow()
        : mWindow(nullptr)
    {

    }

    SDLWindow::~SDLWindow()
    {

    }

    bool SDLWindow::create(const char *title, int32_t x, int32_t y,
        int32_t w, int32_t h, uint32_t flags)
    {
        bool ret = false;

        ret = (SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO);

        if (ret)
        {
            mWindow = SDL_CreateWindow(title, x, y, w, h, flags);
            if (mWindow == nullptr)
            {
                ret = false;
            }
            else
            {

            }
        }

        return ret;
    }

    void SDLWindow::destroy()
    {
        if (mWindow != nullptr)
        {
            SDL_DestroyWindow(mWindow);
            mWindow = nullptr;
        }
    }

    void *SDLWindow::getNativeWinObject()
    {
        SDL_SysWMinfo info;
        bool ret = (SDL_GetWindowWMInfo(mWindow, &info) == SDL_TRUE);
        return info.info.win.window;
    }
}
