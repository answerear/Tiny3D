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

#include "EditorApp.h"
#include "EditorRenderer.h"
#include "T3DEditorInfoDX11.h"

Tiny3D::EditorApp *app = nullptr;

namespace Tiny3D
{
    EditorApp theApp;
    
    EditorApp::EditorApp()
    {
        app = this;
    }

    EditorApp::~EditorApp()
    {
        app = nullptr;
    }

    TResult EditorApp::go(int32_t argc, char *argv[])
    {
        TResult ret;

        Agent *engine = new Agent();

        do
        {
            ret = engine->init(argc, argv, true, true);
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = engine->loadPlugin("ImGuiDX11");
            if (T3D_FAILED(ret))
            {
                break;
            }

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
            //io.ConfigViewportsNoAutoMerge = true;
            //io.ConfigViewportsNoTaskBarIcon = true;

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            //ImGui::StyleColorsLight();

            // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
            ImGuiStyle& style = ImGui::GetStyle();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
            
#if defined(T3D_OS_WINDOWS)
            EditorInfoDX11 info;
            engine->getEditorInfo(&info);
            mEditorRenderer->init(&info);
#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_LINUX)
#endif
            engine->run();
        } while (false);

        delete engine;

        return ret;
    }

    bool EditorApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
    {
        return true;
    }

    void EditorApp::applicationDidEnterBackground()
    {
        T3D_AGENT.appDidEnterBackground();
    }

    void EditorApp::applicationWillEnterForeground()
    {
        T3D_AGENT.appWillEnterForeground();
    }

    void EditorApp::applicationWillTerminate()
    {

    }

    void EditorApp::applicationLowMemory()
    {

    }
}



