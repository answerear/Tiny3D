﻿/*******************************************************************************
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

#ifndef __T3D_SDL_MOBILE_WINDOW_H__
#define __T3D_SDL_MOBILE_WINDOW_H__


#include "Adapter/T3DWindowInterface.h"
#include <SDL.h>

namespace Tiny3D
{
    class IWindow;

    class SDLMobileWindow : public IWindow
    {
        T3D_DISABLE_COPY(SDLMobileWindow);

    public:
        SDLMobileWindow();

        virtual ~SDLMobileWindow();

        virtual bool create(const char *title, int32_t x, int32_t y, 
            int32_t w, int32_t h, uint32_t flags) override;

        virtual void destroy() override;

        virtual void *getNativeWinObject() override;

    protected:
        SDL_Window  *mSDLWindow;
    };
}


#endif  /*__T3D_SDL_MOBILE_WINDOW_H__*/