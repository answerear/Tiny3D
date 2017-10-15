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

#include "T3DGL3PPlugin.h"
#include "T3DGL3PRenderer.h"
#include "T3DGL3PWindowEventHandler.h"

namespace Tiny3D
{
    GL3PPlugin::GL3PPlugin()
        : mName("OpenGL 3+ Renderer Plugin")
        , mRenderer(nullptr)
    {

    }

    GL3PPlugin::~GL3PPlugin()
    {

    }

    const String &GL3PPlugin::getName() const
    {
        return mName;
    }

    bool GL3PPlugin::install()
    {
        // #if defined (T3D_EXPORT_API)
        //         HINSTANCE hInstance = GetModuleHandle("T3DD3D9Renderer.dll");
        // #else
        //         HINSTANCE hInstance = GetModuleHandle(NULL);
        // #endif
//         HINSTANCE hInstance;
//         static const TCHAR staticVar;
//         GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
//             | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
//             &staticVar, &hInstance);

        if (!glfwInit())
        {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        mRenderer = new GL3PRenderer();
        Entrance::getInstance().addRenderer(mRenderer);

        mWinEventHandler = new GL3PWindowEventHandler();
        Entrance::getInstance().setWindowEventHandler(mWinEventHandler);

        return true;
    }

    bool GL3PPlugin::startup()
    {
        return true;
    }

    void GL3PPlugin::shutdown()
    {
    }

    void GL3PPlugin::uninstall()
    {
        Entrance::getInstance().removeRenderer(mRenderer);
        T3D_SAFE_RELEASE(mRenderer);

        Entrance::getInstance().setWindowEventHandler(nullptr);
        T3D_SAFE_DELETE(mWinEventHandler);

        glfwTerminate();
    }
}
