

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

    node->lookAt(Vector3(2.5, 2.0, 4.0), Vector3::ZERO, Vector3::UNIT_Y);

    SGCameraPtr camera = SGCamera::create();
    node->addChild((SGNodePtr)camera);

    camera->setProjectionType(SGCamera::E_PT_PERSPECTIVE);

    Radian fovY(Math::PI * Real(0.5));
    Real ratio = Real(640) / Real(480);
    camera->setPerspective(fovY, ratio, 1.0, 1000.0);

    node = SGTransformNode::create();
    root->addChild((SGNodePtr)node);

    SGMeshPtr mesh = SGMesh::create();
    node->addChild((SGNodePtr)mesh);
    mesh->loadBox();

    mRenderWindow->addViewport(camera, 0, 0.0, 0.0, 1.0, 1.0);

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
