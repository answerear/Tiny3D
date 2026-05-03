/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGL4ConsolePlugin.h"


Tiny3D::GL4ConsolePlugin *gPlugin = nullptr;

extern "C"
{
    T3D_GL4CONSOLE_API TResult dllStartPlugin()
    {
        gPlugin = T3D_NEW Tiny3D::GL4ConsolePlugin();
        return Tiny3D::Agent::getInstance().installPlugin(gPlugin);
    }

    T3D_GL4CONSOLE_API TResult dllStopPlugin()
    {
        TResult ret = Tiny3D::Agent::getInstance().uninstallPlugin(gPlugin);

        if (ret == Tiny3D::T3D_OK)
        {
            T3D_SAFE_DELETE(gPlugin);
        }

        return ret;
    }
}
