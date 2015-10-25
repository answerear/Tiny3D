
#include <windows.h>
#include <Tiny3D.h>


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    Tiny3D::Entrance *entrance = new Tiny3D::Entrance("../../media/config/Tiny3D.cfg");

    Tiny3D::RendererList renderers;
    Tiny3D::Entrance::getInstance().enumerateAvailableRenderers(renderers);

    Tiny3D::RendererListItr itr = renderers.begin();
    while (itr != renderers.end())
    {
        if ((*itr)->getName() == "T3DD3D9Renderer")
        {
            Tiny3D::Entrance::getInstance().setActiveRenderer(*itr);
            break;
        }
        ++itr;
    }

    Tiny3D::Entrance::getInstance().run();

    return 0;
}