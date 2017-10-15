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

#ifndef __T3D_GL3P_WINDOW_EVENT_HANDLER__
#define __T3D_GL3P_WINDOW_EVENT_HANDLER__


#include "T3DGL3PPrerequisites.h"


namespace Tiny3D
{
    class GL3PWindowEventHandler : public WindowEventHandler
    {
    public:
        GL3PWindowEventHandler();
        virtual ~GL3PWindowEventHandler();

        virtual void pollEvents() override;

        void setGLFWwindow(GLFWwindow *window);

    protected:
        static void windowClosedCallback(GLFWwindow *window);
        void handleWindowClosed(GLFWwindow *window);

        static void windowSizeChangedCallback(GLFWwindow *window, int32_t width, int32_t height);
        void handleWindowSizeChanged(GLFWwindow *window, int32_t width, int32_t height);

    protected:
        static GL3PWindowEventHandler   *mWinEventHandler;

        GLFWwindow  *mWindow;
    };
}


#endif  /*__T3D_GL3P_WINDOW_EVENT_HANDLER__*/
