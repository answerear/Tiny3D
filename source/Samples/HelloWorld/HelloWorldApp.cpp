

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
    Entrance *entrance = new Entrance();

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
