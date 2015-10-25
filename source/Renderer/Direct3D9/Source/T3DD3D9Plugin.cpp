

#include "T3DD3D9Plugin.h"
#include "Misc/T3DEntrance.h"
#include "T3DD3d9Renderer.h"


namespace Tiny3D
{
    D3D9Plugin::D3D9Plugin()
        : mName("Direct3D9 Renderer Plugin")
        , mRenderer(nullptr)
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
#if defined (T3D_EXPORT_API)
        HINSTANCE hInstance = GetModuleHandle("T3DD3D9Renderer.dll");
#else
        HINSTANCE hInstance = GetModuleHandle(NULL);
#endif
        mRenderer = new D3D9Renderer(hInstance);

        Entrance::getInstance().addRenderer(mRenderer);

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
        if (mRenderer != nullptr)
        {
            delete mRenderer;
            mRenderer = nullptr;
        }
    }
}
