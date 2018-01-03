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

#include "Kernel/T3DEngine.h"
#include "Application/T3DAppEventListener.h"


namespace Tiny3D
{
    Engine::Engine()
        : mSystem(nullptr)
        , mAppEventListener(nullptr)
        , mWindow(nullptr)
        , mShutdown(false)
        , mWindowCreated(true)
    {

    }

    Engine::~Engine()
    {
        if (mWindowCreated)
        {
            T3D_SAFE_DELETE(mWindow);
        }

        T3D_SAFE_DELETE(mSystem);
    }

    void Engine::setAppEventListener(AppEventListener *listener)
    {
        mAppEventListener = listener;
    }

    bool Engine::startup(Window *window /* = nullptr */)
    {
        bool ret = false;

        mSystem = new System();

        if (window != nullptr)
        {
            mWindow = new Window(false);
            if (ret = mWindow->create("Demo_Hello", 100, 100, 800, 600, false, 3, NULL, NULL, NULL))
            {
                mWindowCreated = true;
            }
        }
        else
        {
            mWindow = window;
            mWindowCreated = false;
        }

        return ret;
    }

    bool Engine::run()
    {
        if (mAppEventListener != nullptr)
        {
            mAppEventListener->applicationDidFinishLaunching();
        }
        
        if (mWindow != nullptr)
        {
            mWindow->pollEvents();
        }

        return true;
    }
}
