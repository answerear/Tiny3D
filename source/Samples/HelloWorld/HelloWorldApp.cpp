

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
    SGNodePtr node = SGTransformNode::create();
    root->addChild(node);

    SGNodePtr camera = SGCamera::create();
    node->addChild(camera);

    node = SGTransformNode::create();
    root->addChild(node);

    SGNodePtr box = SGBox::create();
    node->addChild(box);

    SGNode *c = camera;
    SGCamera *cam = (SGCamera *)c;
    mRenderWindow->addViewport(cam, 0, 0.0, 0.0, 640.0, 480.0);

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
