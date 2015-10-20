

#include "T3DD3D9Prerequisites.h"
#include "T3DD3D9Plugin.h"
#include "Misc/T3DEntrance.h"


namespace Tiny3D
{
    D3D9Plugin *gPlugin = nullptr;

    extern "C"
    {
        void T3D_D3D9RENDERER_API dllStartPlugin()
        {
            gPlugin = new D3D9Plugin();

        }

        void T3D_D3D9RENDERER_API dllStopPlugin()
        {
            delete gPlugin;
        }
    }

}