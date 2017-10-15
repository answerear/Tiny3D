/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * You may use this sample code for anything you like, it is not covered by the
 * same license as the rest of the engine.
*******************************************************************************/

#include "SampleApp.h"


using namespace Tiny3D;

SampleApp *app = nullptr;


SampleApp::SampleApp()
{
    app = this;
}

SampleApp::~SampleApp()
{
    app = nullptr;
}

int32_t SampleApp::go(const String &appPath)
{
    Entrance *entrance = new Entrance(appPath, true, "../media/config/Tiny3D.cfg");

    entrance->setApplicationListener(this);

    RenderWindow *renderWindow = nullptr;
    Entrance::getInstance().initialize(true, renderWindow);
    mRenderWindow = renderWindow;
    Entrance::getInstance().getWindowEventHandler()->addWindowEventListener(mRenderWindow, this);
    Entrance::getInstance().run();
    Entrance::getInstance().getWindowEventHandler()->removeWindowEventListener(mRenderWindow, this);
    delete entrance;

    return 0;
}

bool SampleApp::applicationDidFinishLaunching()
{
    return true;
}

void SampleApp::applicationDidEnterBackground()
{

}

void SampleApp::applicationWillEnterForeground()
{

}

void SampleApp::applicationWillTerminate()
{

}
