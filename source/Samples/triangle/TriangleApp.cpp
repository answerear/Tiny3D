

#include "TriangleApp.h"
#include <Tiny3D.h>


using namespace Tiny3D;


TriangleApp::TriangleApp()
{

}

TriangleApp::~TriangleApp()
{

}

int32_t TriangleApp::go()
{
    Entrance *entrance = new Entrance();

    entrance->setApplicationListener(this);

    RenderWindow *renderWindow = nullptr;
    Entrance::getInstance().initialize(true, renderWindow);
    mRenderWindow = renderWindow;
    Entrance::getInstance().run();

    delete entrance;

    return 0;
}

bool TriangleApp::applicationDidFinishLaunching()
{
    Renderer *renderer = T3D_ENTRANCE.getActiveRenderer();
    renderer->setLightEnabled(false);
    renderer->setRenderMode(Renderer::E_RM_SOLID);
    renderer->setAmbientLight(Color4::WHITE);

    SGNodePtr root = T3D_SCENE_MGR.getRoot();
    

    return true;
}

void TriangleApp::applicationDidEnterBackground()
{

}

void TriangleApp::applicationWillEnterForeground()
{

}

void TriangleApp::applicationWillTerminate()
{

}
