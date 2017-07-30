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

#ifndef __T3D_WINDOW_EVENT_HANDLER__
#define __T3D_WINDOW_EVENT_HANDLER__


#include "Misc/T3DObject.h"
#include "T3DTypedef.h"


#if defined (T3D_OS_WINDOWS)
#include <windows.h>
#endif


namespace Tiny3D
{
    class T3D_ENGINE_API WindowEventHandler 
        : public Singleton<WindowEventHandler>
    {
        friend class Entrance;

    public:
        virtual ~WindowEventHandler();

        void handleWindowMessage();

        void addWindowEventListener(RenderWindowPtr window, WindowEventListener *listener);
        void removeWindowEventListener(RenderWindowPtr window, WindowEventListener *listener);

        void addRenderWindow(RenderWindowPtr window);
        void removeRenderWindow(RenderWindowPtr window);

#if defined (T3D_OS_WINDOWS)
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    protected:
        LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif defined (T3D_OS_ANDROID)
#elif defined (T3D_OS_IOS)
#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_LINUX)
#endif

    protected:
        WindowEventHandler();

        typedef std::multimap<RenderWindowPtr, WindowEventListener*>    WindowEventListeners;
        typedef WindowEventListeners::iterator                          WindowEventListenersItr;
        typedef WindowEventListeners::const_iterator                    WindowEventListenersConstItr;
        typedef WindowEventListeners::value_type                        WindowEventListenersValue;

        WindowEventListeners    mListeners;
        RenderWindowList        mWindows;
    };
}


#endif  /*__T3D_WINDOW_EVENT_HANDLER__*/
