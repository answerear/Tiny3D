

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
    Entrance::getInstance().run();

    delete entrance;

    return 0;
}

bool HelloWorldApp::applicationDidFinishLaunching()
{
    SGTransformNode *node = new SGTransformNode();
    T3D_SCENE_MGR.getRoot()->addChild(node);

    SGCamera *camera = new SGCamera();
    node->addChild(camera);

    node = new SGTransformNode();
    T3D_SCENE_MGR.getRoot()->addChild(node);

    SGBox *box = new SGBox();
    node->addChild(box);

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
