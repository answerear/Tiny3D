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


#include "Adapter/T3DSDLApplication.h"


namespace Tiny3D
{
    SDLApplication::SDLApplication()
    {

    }

    SDLApplication::~SDLApplication()
    {

    }

    bool SDLApplication::init()
    {
        bool ret = false;

        ret = (SDL_Init(0) == 0);

        if (!ret)
        {
            const char *error = SDL_GetError();
        }

        ret = ret && (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) == 0);
        if (!ret)
        {
            const char *error = SDL_GetError();
        }

        return ret;
    }

    bool SDLApplication::pollEvents()
    {
        bool ret = true;
        SDL_Event ev;

        while (SDL_PollEvent(&ev) != 0)
        {
            if (ev.type == SDL_QUIT)
            {
                ret = false;
                break;
            }
        }

        return ret;
    }

    void SDLApplication::release()
    {
        SDL_Quit();
    }

    void *SDLApplication::getNativeAppObject()
    {
        return nullptr;
    }
}

