/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
