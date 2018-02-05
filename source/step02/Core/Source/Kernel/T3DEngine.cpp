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

#include "Kernel/T3DEngine.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(Engine);

    Engine::Engine()
        : mEventMgr(nullptr)
        , mLogger(nullptr)
        , mWindow(nullptr)
        , mIsRunning(false)
    {
        mLogger = new Logger();
        mEventMgr = new EventManager(10);
    }

    Engine::~Engine()
    {
        T3D_SAFE_DELETE(mWindow);
        T3D_SAFE_DELETE(mEventMgr);

        mLogger->shutdown();
        T3D_SAFE_DELETE(mLogger);
    }

    int32_t Engine::init()
    {
        int32_t ret = T3D_ERR_FAIL;

        do
        {
            Application *theApp = Application::getInstancePtr();
            if (theApp == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            ret = theApp->init();
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            if (mLogger != nullptr)
            {
                mLogger->startup(1000, "Engine", true, true);
            }

            T3D_LOG_INFO("Start Tiny3D ...... version %s", T3D_DEVICE_INFO.getSoftwareVersion().c_str());
            T3D_LOG_INFO("System Information : \n%s", T3D_DEVICE_INFO.getSystemInfo().c_str());

            mWindow = new Window();
            ret = mWindow->create("Demo_Hello", 100, 100, 800, 600, Window::WINDOW_SHOWN);
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            mIsRunning = true;
            ret = T3D_ERR_OK;
        } while (0);

        return ret;
    }

    bool Engine::run()
    {
        Application *theApp = Application::getInstancePtr();
        theApp->applicationDidFinishLaunching();

        while (mIsRunning)
        {
            // 轮询系统事件
            mIsRunning = theApp->pollEvents();

            if (!mIsRunning)
                break;

            // 事件系统派发事件
            T3D_EVENT_MGR.dispatchEvent();

            // 渲染一帧
            renderOneFrame();
        }

        theApp->applicationWillTerminate();

        return true;
    }

    void Engine::renderOneFrame()
    {

    }

    void Engine::appWillEnterForeground()
    {
        T3D_LOG_ENTER_FOREGROUND();
    }

    void Engine::appDidEnterBackground()
    {
        T3D_LOG_ENTER_BACKGROUND();
    }
}
