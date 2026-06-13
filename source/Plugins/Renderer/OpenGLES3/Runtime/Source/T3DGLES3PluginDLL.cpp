/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGLES3Plugin.h"


Tiny3D::GLES3Plugin *gPlugin = nullptr;

extern "C"
{
    T3D_GLES3RENDERER_API TResult dllStartPlugin()
    {
        gPlugin = T3D_NEW Tiny3D::GLES3Plugin();
        return Tiny3D::Agent::getInstance().installPlugin(gPlugin);
    }

    T3D_GLES3RENDERER_API TResult dllStopPlugin()
    {
        TResult ret = Tiny3D::Agent::getInstance().uninstallPlugin(gPlugin);

        if (ret == Tiny3D::T3D_OK)
        {
            T3D_SAFE_DELETE(gPlugin);
        }

        return ret;
    }
}
