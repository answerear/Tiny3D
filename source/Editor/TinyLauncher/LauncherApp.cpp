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

#include "LauncherApp.h"
#include "ImGuiImpl.h"
#include "MainWindow.h"
#include "T3DEditorInfoDX11.h"


Tiny3D::Launcher::LauncherApp *app = nullptr;

namespace Tiny3D
{
    NS_BEGIN(Launcher)

    LauncherApp theApp;

    #define IMGUI_DX11_PLUGIN   "ImGuiDX11"

    //--------------------------------------------------------------------------
    
    LauncherApp::LauncherApp()
    { 
        app = this;
    }

    //--------------------------------------------------------------------------
    
    LauncherApp::~LauncherApp()
    { 
        app = nullptr;
    }

    //--------------------------------------------------------------------------

    void LauncherApp::setEditorRenderer(ImGuiImpl *impl)
    {
        mImGuiImpl = impl;
    }
    
    //--------------------------------------------------------------------------

    void LauncherApp::exitApp()
    {
        mExitApp = true;
    }

    //--------------------------------------------------------------------------
    
    TResult LauncherApp::go(int32_t argc, char *argv[])
    {
        TResult ret;

        Agent *engine = new Agent();

        do
        {
            Settings settings;
            settings.renderSettings.resizable = true;
            settings.renderSettings.title = "Launcher";
            settings.pluginSettings.pluginPath = ".";
            settings.pluginSettings.plugins.emplace_back("FileSystemArchive");
            settings.pluginSettings.plugins.emplace_back("D3D11Renderer");
            
            // 初始化引擎，只有初始化后才能使用
            ret = engine->init(argc, argv, true, true, settings);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Init engine failed ! ERROR [%d]", ret);
                break;
            }

            // 加载语言文件
            LanguageManagerPtr langMgr = LanguageManager::create();
            // String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Launcher" + Dir::getNativeSeparator() + "Language" + Dir::getNativeSeparator() + "lang-en-us.txt";
            String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Launcher" + Dir::getNativeSeparator() + "Language" + Dir::getNativeSeparator() + "lang-zh-hans.txt";
            ret = langMgr->init(path);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Init language file failed ! ERROR [%d]", ret);
                break;
            }

            // 创建 imgui 环境
            ret = createImGuiEnv(engine);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Create ImGui environment failed ! ERROR [%d]", ret);
                break;
            }

            // 主窗口
            mMainWindow = new MainWindow();
            ret = mMainWindow->create("Project Manager Window", nullptr);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Create project manager window failed ! ERROR [%d]", ret);
                break;
            }

            // 构建引擎运行数据，并运行引擎
            EditorRunningData runningData;
            runningData.pollEvents = std::bind(&LauncherApp::enginePollEvents, this);
            runningData.update = std::bind(&LauncherApp::engineUpdate, this);
            runningData.preRender = std::bind(&LauncherApp::enginePreRender, this);
            runningData.postRender = std::bind(&LauncherApp::enginePostRender, this);
            engine->runForEditor(runningData);

            if (mMainWindow != nullptr)
            {
                mMainWindow->destroy();
            }
            
            ImWidget::GC();

            langMgr = nullptr;
            
            // 删除清理 imgui 环境，此后无法再使用 imgui
            destroyImGuiEnv(engine);
        } while (false);
        
        delete engine;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult LauncherApp::createImGuiEnv(Agent *engine)
    {
        TResult ret = T3D_OK;

        do
        {
            ret = engine->loadPlugin(IMGUI_DX11_PLUGIN);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Load ImGuiDX11 plugin failed ! ERROR [%d]", ret);
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
            mImGuiImpl->init(&info);
            mSDLWindow = info.sdlWindow;
#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_LINUX)
#endif

            String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Launcher" + Dir::getNativeSeparator() + "fonts" + Dir::getNativeSeparator() + "arial unicode ms.ttf";
            io.Fonts->AddFontFromFileTTF(path.c_str(), 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void LauncherApp::destroyImGuiEnv(Agent *engine)
    {
        engine->unloadPlugin(IMGUI_DX11_PLUGIN);
        ImGui::DestroyContext();
    }

    //--------------------------------------------------------------------------
    
    bool LauncherApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
    {
        return true;
    }

    //--------------------------------------------------------------------------
    
    void LauncherApp::applicationDidEnterBackground()
    {
        T3D_AGENT.appDidEnterBackground();
    }

    //--------------------------------------------------------------------------
    
    void LauncherApp::applicationWillEnterForeground()
    {
        T3D_AGENT.appWillEnterForeground();
    }

    //--------------------------------------------------------------------------
    
    void LauncherApp::applicationWillTerminate()
    {

    }

    //--------------------------------------------------------------------------
    
    void LauncherApp::applicationLowMemory()
    {

    }

    //--------------------------------------------------------------------------

    bool LauncherApp::enginePollEvents()
    {
        bool done = false;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            mImGuiImpl->pollEvents(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(mSDLWindow))
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(mSDLWindow))
            {
                // Release all outstanding references to the swap chain's buffers before resizing.
                // CleanupRenderTarget();
                // g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
                // CreateRenderTarget();
                T3D_AGENT.getDefaultRenderWindow()->resize(event.window.data1, event.window.data2);

#if defined(T3D_OS_WINDOWS)
                EditorInfoDX11 info;
                T3D_AGENT.getEditorInfo(&info);
                mImGuiImpl->refreshInfo(&info);
#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_LINUX)
#endif
            }
        }
        
        return !done && !mExitApp;
    }

    //--------------------------------------------------------------------------

    void LauncherApp::engineUpdate()
    {
        mImGuiImpl->update();
        ImGui::NewFrame();

        ImWidget::beginUpdate();
        
        if (mMainWindow != nullptr)
        {
            mMainWindow->update();
        }

        ImWidget::endUpdate();
    };

    //--------------------------------------------------------------------------

    void LauncherApp::enginePreRender()
    {
        ImGui::Render();
        mImGuiImpl->preRender();
    }

    //--------------------------------------------------------------------------

    void LauncherApp::enginePostRender()
    {
        mImGuiImpl->postRender();

        // Update and Render additional Platform Windows
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    //--------------------------------------------------------------------------
    
    NS_END
}



