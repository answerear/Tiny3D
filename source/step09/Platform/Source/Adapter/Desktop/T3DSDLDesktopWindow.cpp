﻿/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
 * Copyright (C) 2015-2019  Answer Wong
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

#include "Adapter/Desktop/T3DSDLDesktopWindow.h"
#include "T3DPlatformErrorDef.h"

#if defined (T3D_OS_WINDOWS)
#include <SDL_syswm.h>
#endif

namespace Tiny3D
{
    SDLDesktopWindow::SDLDesktopWindow()
        : mSDLWindow(nullptr)
        , mSDLIconSurface(nullptr)
    {

    }

    SDLDesktopWindow::~SDLDesktopWindow()
    {

    }

    TResult SDLDesktopWindow::create(const char *title, int32_t x, int32_t y,
        int32_t w, int32_t h, uint32_t flags)
    {
        TResult ret = T3D_ERR_FAIL;

        do 
        {
            if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO)
            {
                
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

    void SDLDesktopWindow::destroy()
    {
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

    bool SDLDesktopWindow::getSystemInfo(SysWMInfo &info)
    {
        SDL_SysWMinfo sdlInfo;

        bool ret = SDL_GetWindowWMInfo(mSDLWindow, &sdlInfo);

        if (ret)
        {
#if defined (T3D_OS_WINDOWS)
            info.hWnd = sdlInfo.info.win.window;
            info.hDC = sdlInfo.info.win.hdc;
            info.hInstance = sdlInfo.info.win.hinstance;
#elif defiend (T3D_OS_LINUX)
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
#elif defiend (T3D_OS_ANDROID)
            info.window = sdlInfo.info.android.window;
            info.surface = sdlInfo.info.android.surface;
#endif
        }

        return ret;
    }

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
}
