/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "SampleApp.h"


using namespace Tiny3D;

SampleApp *app = nullptr;


SampleWindowApp::SampleWindowApp()
{
    app = this;
}

SampleWindowApp::~SampleWindowApp()
{
    app = nullptr;
}

TResult SampleWindowApp::go(int32_t argc, char *argv[])
{
    TResult ret;

    Tiny3D::Agent *theEngine = new Tiny3D::Agent();

    ret = theEngine->init(argc, argv, true, true);
    if (ret == T3D_OK)
        theEngine->run();

    delete theEngine;

    return ret;
}

bool SampleWindowApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    return true;
}

void SampleWindowApp::applicationDidEnterBackground()
{
    T3D_AGENT.appDidEnterBackground();
}

void SampleWindowApp::applicationWillEnterForeground()
{
    T3D_AGENT.appWillEnterForeground();
}

void SampleWindowApp::applicationWillTerminate()
{

}

void SampleWindowApp::applicationLowMemory()
{

}

//------------------------------------------------------------------------------

SampleConsoleApp::SampleConsoleApp()
{
    app = this;
}

SampleConsoleApp::~SampleConsoleApp()
{
    app = nullptr;
}

TResult SampleConsoleApp::go(int32_t argc, char *argv[])
{
    TResult ret;

    Tiny3D::Agent *theEngine = new Tiny3D::Agent();

    Settings settings;
    settings.pluginSettings.pluginPath = ".";
    settings.pluginSettings.plugins.push_back("FileSystemArchive");
    settings.pluginSettings.plugins.push_back("NullRenderer");
    settings.renderSettings.renderer = "NullRenderer";
    ret = theEngine->init(argc, argv, true, false, settings);
    if (ret == T3D_OK)
        theEngine->run();

    delete theEngine;

    return ret;
}

bool SampleConsoleApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    return true;
}

void SampleConsoleApp::applicationDidEnterBackground()
{
    T3D_AGENT.appDidEnterBackground();
}

void SampleConsoleApp::applicationWillEnterForeground()
{
    T3D_AGENT.appWillEnterForeground();
}

void SampleConsoleApp::applicationWillTerminate()
{

}

void SampleConsoleApp::applicationLowMemory()
{

}
