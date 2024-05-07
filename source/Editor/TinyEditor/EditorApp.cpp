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
#include "EditorImGuiImpl.h"
#include "T3DEditorInfoDX11.h"

Tiny3D::EditorApp *app = nullptr;

namespace Tiny3D
{
    EditorApp theApp;

    //--------------------------------------------------------------------------
    
    EditorApp::EditorApp()
    {
        app = this;
    }

    //--------------------------------------------------------------------------
    
    EditorApp::~EditorApp()
    {
        app = nullptr;
    }

    //--------------------------------------------------------------------------
    
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
            mEditorImGuiImpl->init(&info);
#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_LINUX)
#endif

            EditorRunningData runningData;
            runningData.pollEvents = [this]()
            {
                return mEditorImGuiImpl->pollEvents();
            };
            runningData.update = [this]()
            {
                mEditorImGuiImpl->update();
                ImGui::NewFrame();

                ImGuiIO& io = ImGui::GetIO();
                
                // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
                if (show_demo_window)
                    ImGui::ShowDemoWindow(&show_demo_window);

                // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
                {
                    static float f = 0.0f;
                    static int counter = 0;

                    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

                    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
                    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
                    // ImGui::Checkbox("Another Window", &show_another_window);

                    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                    // ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

                    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                        counter++;
                    ImGui::SameLine();
                    ImGui::Text("counter = %d", counter);

                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                    ImGui::End();
                }
            };
            runningData.preRender = [this]()
            {
                ImGui::Render();
                mEditorImGuiImpl->preRender();
            };
            runningData.postRender = [this]()
            {
                mEditorImGuiImpl->postRender();

                // Update and Render additional Platform Windows
                ImGuiIO& io = ImGui::GetIO();
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                }
            };

            // buildScene();
            
            engine->runForEditor(runningData);
        } while (false);

        delete engine;

        ImGui::DestroyContext();

        return ret;
    }

    //--------------------------------------------------------------------------
    
    bool EditorApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
    {
        return true;
    }

    //--------------------------------------------------------------------------
    
    void EditorApp::applicationDidEnterBackground()
    {
        T3D_AGENT.appDidEnterBackground();
    }

    //--------------------------------------------------------------------------
    
    void EditorApp::applicationWillEnterForeground()
    {
        T3D_AGENT.appWillEnterForeground();
    }

    //--------------------------------------------------------------------------
    
    void EditorApp::applicationWillTerminate()
    {

    }

    //--------------------------------------------------------------------------
    
    void EditorApp::applicationLowMemory()
    {

    }

    //--------------------------------------------------------------------------
    
    void EditorApp::buildScene()
    {
        // create scene
        ScenePtr scene = T3D_SCENE_MGR.createScene("TestScene");
        T3D_SCENE_MGR.setCurrentScene(scene);
    
        // root game object
        GameObjectPtr go = GameObject::create("TestScene");
        scene->addRootGameObject(go);
        Transform3DPtr root = go->addComponent<Transform3D>();

        RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
        RenderTargetPtr rt = RenderTarget::create(rw);
    
        CameraPtr camera;

        // center camera
        go = GameObject::create("CenterCamera");
        Transform3DPtr center = go->addComponent<Transform3D>();
        root->addChild(center);
        camera = go->addComponent<Camera>();
        camera->setOrder(1);
        Viewport vpCenter {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
        camera->setViewport(vpCenter);
        camera->setClearColor(ColorRGB::GRAY);
        camera->setRenderTarget(rt);
        scene->addCamera(camera);
    }

    //--------------------------------------------------------------------------
}



