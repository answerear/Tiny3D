/***************************************************************************************************
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
 **************************************************************************************************/

#include "T3DGL3PRenderWindow.h"
#include "T3DGL3PWindowEventHandler.h"

namespace Tiny3D
{
    GL3PRenderWindow::GL3PRenderWindow()
        : mWindow(nullptr)
        , mIsFullScreen(false)
    {
    }

    GL3PRenderWindow::~GL3PRenderWindow()
    {
        destroy();
    }

    bool GL3PRenderWindow::create(const String &name, const RenderWindowCreateParam &rkCreateParams, const RenderWindowCreateParamEx &rkCreateParamEx)
    {
        bool ret = false;

        do 
        {
            GLFWmonitor *primary = glfwGetPrimaryMonitor();
            if (primary == nullptr)
            {
                T3D_LOG_ERROR("Retrieve primary monitor failed !");
                break;
            }

            const GLFWvidmode *videoMode = glfwGetVideoMode(primary);
            if (videoMode == nullptr)
            {
                T3D_LOG_ERROR("Retrieve video mode failed !");
                break;
            }

            glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

            if (rkCreateParams._fullscreen)
            {
                mWindow = glfwCreateWindow(videoMode->width, videoMode->height,
                    rkCreateParams._windowTitle.c_str(), primary, nullptr);
            }
            else
            {
                mWindow = glfwCreateWindow(rkCreateParams._windowWidth, rkCreateParams._windowHeight,
                    rkCreateParams._windowTitle.c_str(), nullptr, nullptr);
            }

            if (mWindow == nullptr)
            {
                T3D_LOG_ERROR("Create window failed !");
                break;
            }

            mIsActive = true;
            mIsFullScreen = rkCreateParams._fullscreen;
            mWidth = rkCreateParams._windowWidth;
            mHeight = rkCreateParams._windowHeight;
            mColorDepth = videoMode->redBits + videoMode->greenBits + videoMode->blueBits;
            mName = name;

            GL3PWindowEventHandler *handler = (GL3PWindowEventHandler *)Entrance::getInstance().getWindowEventHandler();
            handler->setGLFWwindow(mWindow);

            glfwSetWindowUserPointer(mWindow, this);

            glfwMakeContextCurrent(mWindow);

            int32_t w, h;
            glfwGetFramebufferSize(mWindow, &w, &h);

            ret = true;
        } while (0);


        return ret;
    }

    void GL3PRenderWindow::destroy()
    {
        if (mWindow != nullptr)
        {
            glfwDestroyWindow(mWindow);
            mWindow = nullptr;
        }

        Entrance::getInstance().shutdown();
    }

    void GL3PRenderWindow::swapBuffers()
    {
        if (mWindow != nullptr)
        {
            glfwSwapBuffers(mWindow);
        }
    }

    bool GL3PRenderWindow::isFullScreen() const
    {
        return mIsFullScreen;
    }
}