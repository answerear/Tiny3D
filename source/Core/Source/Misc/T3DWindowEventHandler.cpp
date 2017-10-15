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

#include "Misc/T3DWindowEventHandler.h"
#include "Render/T3DRenderWindow.h"
#include "Listener/T3DWindowEventListener.h"


namespace Tiny3D
{
    WindowEventHandler::WindowEventHandler()
    {

    }

    WindowEventHandler::~WindowEventHandler()
    {

    }

    void WindowEventHandler::addWindowEventListener(RenderWindowPtr window, WindowEventListener *listener)
    {
        mListeners.insert(WindowEventListenersValue(window, listener));
    }

    void WindowEventHandler::removeWindowEventListener(RenderWindowPtr window, WindowEventListener *listener)
    {
        WindowEventListenersItr itr = mListeners.begin();

        while (itr != mListeners.end())
        {
            if (itr->first == window && itr->second == listener)
            {
                mListeners.erase(itr);
                break;
            }
        }
    }

    void WindowEventHandler::addRenderWindow(RenderWindowPtr window)
    {
        mWindows.push_back(window);
    }

    void WindowEventHandler::removeRenderWindow(RenderWindowPtr window)
    {
        RenderWindowListItr itr = std::find(mWindows.begin(), mWindows.end(), window);

        if (itr != mWindows.end())
        {
            mWindows.erase(itr);
        }
    }
}
