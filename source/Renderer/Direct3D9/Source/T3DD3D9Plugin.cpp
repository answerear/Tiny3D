

#include "T3DD3D9Plugin.h"
#include "Misc/T3DEntrance.h"


namespace Tiny3D
{
    D3D9Plugin::D3D9Plugin()
        : mName("Direct3D9 Renderer Plugin")
    {

    }

    D3D9Plugin::~D3D9Plugin()
    {

    }

    const String &D3D9Plugin::getName() const
    {
        return mName;
    }

    bool D3D9Plugin::install()
    {
        return true;
    }

    bool D3D9Plugin::startup()
    {
        return true;
    }

    void D3D9Plugin::shutdown()
    {
        
    }

    void D3D9Plugin::uninstall()
    {

    }
}
