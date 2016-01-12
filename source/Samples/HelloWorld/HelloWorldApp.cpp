

#include "HelloWorldApp.h"
#include <Tiny3D.h>


using namespace Tiny3D;

HelloWorldApp::HelloWorldApp()
{

}

HelloWorldApp::~HelloWorldApp()
{

}

int32_t HelloWorldApp::go()
{
    Entrance *entrance = new Entrance("../../../../media/config/Tiny3D.cfg");

    entrance->setApplicationListener(this);

    RenderWindow *renderWindow = nullptr;
    Entrance::getInstance().initialize(true, renderWindow);
    mRenderWindow = renderWindow;
    Entrance::getInstance().run();

    delete entrance;

    return 0;
}

bool HelloWorldApp::applicationDidFinishLaunching()
{
    SGNodePtr root = T3D_SCENE_MGR.getRoot();
    SGTransformNodePtr node = SGTransformNode::create();
    root->addChild((SGNodePtr)node);

    SGCameraPtr camera = SGCamera::create();
    node->addChild((SGNodePtr)camera);

    node = SGTransformNode::create();
    root->addChild((SGNodePtr)node);

    SGMeshPtr mesh = SGMesh::create();
    node->addChild((SGNodePtr)mesh);
    mesh->loadBox();

    mRenderWindow->addViewport(camera, 0, 0.0, 0.0, 640.0, 480.0);

    return true;
}

void HelloWorldApp::applicationDidEnterBackground()
{

}

void HelloWorldApp::applicationWillEnterForeground()
{

}

void HelloWorldApp::applicationWillTerminate()
{

}
