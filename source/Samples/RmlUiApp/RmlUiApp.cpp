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

#include "RmlUiApp.h"

#include "UI/T3DUISystem.h"

#include <cstdlib>


using namespace Tiny3D;


RmlUiApp theApp;


RmlUiApp::RmlUiApp()
{
}

RmlUiApp::~RmlUiApp()
{
}

TResult RmlUiApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    (void)argc;
    (void)argv;

    UISystem *uiSystem = T3D_AGENT.getUISystem();
    if (uiSystem == nullptr)
    {
        T3D_LOG_ERROR("RmlUiApp", "UISystem is not registered.");
        return T3D_ERR_FAIL;
    }

    T3D_LOG_INFO("RmlUiApp", "UISystem [%s] is ready.", uiSystem->getName().c_str());
    return T3D_OK;
}

bool RmlUiApp::pollEvents()
{
    // Phase 0 验收要观察退出顺序。设置 T3D_RMLUI_SMOKE=1 时启动后立即退出。
    const char *smoke = std::getenv("T3D_RMLUI_SMOKE");
    if (smoke != nullptr && smoke[0] == '1')
    {
        return false;
    }
    return SampleWindowApp::pollEvents();
}
