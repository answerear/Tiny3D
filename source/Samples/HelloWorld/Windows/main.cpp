
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

    Tiny3D::RenderWindowCreateParam param;
    param._windowLeft = 100;
    param._windowTop = 100;
    param._windowWidth = 960;
    param._windowHeight = 640;
    param._fullscreen = false;
    param._colorDepth = 32;
    param._windowTitle = "HelloWorldApp Demo";
    Tiny3D::RenderWindowCreateParamEx paramEx;
    Tiny3D::Entrance::getInstance().createRenderWindow(param, paramEx);

    Tiny3D::Entrance::getInstance().run();

    return 0;
}