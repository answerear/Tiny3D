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

#include "T3DGL3PWindowEventHandler.h"


namespace Tiny3D
{
    GL3PWindowEventHandler *GL3PWindowEventHandler::mWinEventHandler = nullptr;

    GL3PWindowEventHandler::GL3PWindowEventHandler()
        : mWindow(nullptr)
    {
        mWinEventHandler = this;
    }

    GL3PWindowEventHandler::~GL3PWindowEventHandler()
    {
        mWinEventHandler = nullptr;
    }

    void GL3PWindowEventHandler::pollEvents()
    {
        if (mWindow != nullptr)
        {
            glfwPollEvents();
        }
    }

    void GL3PWindowEventHandler::setGLFWwindow(GLFWwindow *window)
    {
        if (window != nullptr && mWindow != window)
        {
            glfwSetWindowCloseCallback(window, GL3PWindowEventHandler::windowClosedCallback);
            glfwSetWindowSizeCallback(window, GL3PWindowEventHandler::windowSizeChangedCallback);
        }

        mWindow = window;
    }

    void GL3PWindowEventHandler::windowClosedCallback(GLFWwindow *window)
    {
        mWinEventHandler->handleWindowClosed(window);
    }

    void GL3PWindowEventHandler::handleWindowClosed(GLFWwindow *window)
    {
        RenderWindow *renderWindow = (RenderWindow *)glfwGetWindowUserPointer(window);

        WindowEventListenersItr index,
            start = mListeners.lower_bound(renderWindow),
            end = mListeners.upper_bound(renderWindow);

        bool close = true;
        for (index = start; index != end; ++index)
        {
            if (!(index->second)->windowClosing(renderWindow))
                close = false;
        }
        if (!close)
            return;

        for (index = mListeners.lower_bound(renderWindow); index != end; ++index)
            (index->second)->windowClosed(renderWindow);
        renderWindow->destroy();
    }

    void GL3PWindowEventHandler::windowSizeChangedCallback(GLFWwindow *window, int32_t width, int32_t height)
    {
        mWinEventHandler->handleWindowSizeChanged(window, width, height);
    }

    void GL3PWindowEventHandler::handleWindowSizeChanged(GLFWwindow *window, int32_t width, int32_t height)
    {
        RenderWindow *renderWindow = (RenderWindow *)glfwGetWindowUserPointer(window);

        WindowEventListenersItr index,
            start = mListeners.lower_bound(renderWindow),
            end = mListeners.upper_bound(renderWindow);

        renderWindow->windowMovedOrResized();
        for (index = start; index != end; ++index)
            (index->second)->windowResized(renderWindow);
    }
}
