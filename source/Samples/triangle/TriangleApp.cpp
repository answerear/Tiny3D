

#include "TriangleApp.h"

using namespace Tiny3D;


TriangleApp::TriangleApp()
{

}

TriangleApp::~TriangleApp()
{

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

