
#include <windows.h>
#include <Tiny3D.h>


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    Tiny3D::ObjectPoolManager *objPoolMgr = new Tiny3D::ObjectPoolManager();
    Tiny3D::AutoreleasePool *pool = new Tiny3D::AutoreleasePool();
    objPoolMgr->addObjectPool(pool);

    Tiny3D::Entrance *entrance = new Tiny3D::Entrance("../../../../media/config/Tiny3D.cfg");

    Tiny3D::RendererList renderers;
    Tiny3D::Entrance::getInstance().enumerateAvailableRenderers(renderers);

    Tiny3D::RendererListItr itr = renderers.begin();
    while (itr != renderers.end())
    {
        if ((*itr)->getName() == Tiny3D::Renderer::DIRECT3D9)
        {
            Tiny3D::Entrance::getInstance().setActiveRenderer(*itr);
            break;
        }
        ++itr;
    }

    Tiny3D::RenderWindow *renderWindow = nullptr;
    Tiny3D::Entrance::getInstance().initialize(true, renderWindow);
    Tiny3D::Entrance::getInstance().run();

    objPoolMgr->removeObjectPool(pool);
    delete objPoolMgr;

    return 0;
}