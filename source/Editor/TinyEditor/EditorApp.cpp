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
#include "ImGuiImpl.h"
#include "MainWindow.h"
#include "T3DEditorInfoDX11.h"


Tiny3D::Editor::EditorApp *app = nullptr;

namespace Tiny3D
{
    NS_BEGIN(Editor)

    EditorApp theApp;

    #define IMGUI_DX11_PLUGIN   "ImGuiDX11"

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

    void EditorApp::setEditorRenderer(ImGuiImpl *impl)
    {
        mImGuiImpl = impl;
    }
    
    //--------------------------------------------------------------------------

    void EditorApp::exitApp()
    {
        
    }

    //--------------------------------------------------------------------------
    
    TResult EditorApp::go(int32_t argc, char *argv[])
    {
        TResult ret;

        Agent *engine = new Agent();

        do
        {
            Dir::setCachePathInfo("Tiny3D", "TinyEditor");
            
            Settings settings;
            settings.renderSettings.resizable = true;
            settings.renderSettings.title = "Tiny3D Editor";
            settings.pluginSettings.pluginPath = ".";
            settings.pluginSettings.plugins.emplace_back("FileSystemArchive");
            settings.pluginSettings.plugins.emplace_back("D3D11Renderer");
            
            // 初始化引擎，只有初始化后才能使用
            ret = engine->init(argc, argv, true, true, settings);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Init engine failed ! ERROR [%d]", ret);
                break;
            }

            // T3D_LOG_INFO(LOG_TAG_EDITOR, "argv[0] = %s, argv[1] = %s, argv[2] = %s", argv[0], argv[1], argv[2]);

            // 加载语言文件
            LanguageManagerPtr langMgr = LanguageManager::create();
            // String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Editor" + Dir::getNativeSeparator() + "Language" + Dir::getNativeSeparator() + "lang-en-us.txt";
            String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Editor" + Dir::getNativeSeparator() + "Language" + Dir::getNativeSeparator() + "lang-zh-hans.txt";
            ret = langMgr->init(path);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Init language file failed ! ERROR [%d]", ret);
                break;
            }
            
            // 创建 imgui 环境
            ret = createImGuiEnv(engine);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create ImGui environment failed ! ERROR [%d]", ret);
                break;
            }

            // 主窗口
            mMainWindow = new MainWindow();
            ret = mMainWindow->create("Main Window", nullptr);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create main window failed ! ERROR [%d]", ret);
                break;
            }
            
            // 构建编辑器场景
            // buildScene();

            // 构建引擎运行数据，并运行引擎
            EditorRunningData runningData;
            runningData.pollEvents = std::bind(&EditorApp::enginePollEvents, this);
            runningData.update = std::bind(&EditorApp::engineUpdate, this);
            runningData.preRender = std::bind(&EditorApp::enginePreRender, this);
            runningData.postRender = std::bind(&EditorApp::enginePostRender, this);
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

    TResult EditorApp::createImGuiEnv(Agent *engine)
    {
        TResult ret = T3D_OK;

        do
        {
            ret = engine->loadPlugin(IMGUI_DX11_PLUGIN);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Load ImGuiDX11 plugin failed ! ERROR [%d]", ret);
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

            String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Editor" + Dir::getNativeSeparator() + "fonts" + Dir::getNativeSeparator() + "arial unicode ms.ttf";
            io.Fonts->AddFontFromFileTTF(path.c_str(), 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void EditorApp::destroyImGuiEnv(Agent *engine)
    {
        engine->unloadPlugin(IMGUI_DX11_PLUGIN);
        ImGui::DestroyContext();
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

#if 0
        RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
        RenderTargetPtr rt = RenderTarget::create(rw);
#else
        RenderWindow *rw = T3D_AGENT.getDefaultRenderWindow();
        RenderTexturePtr renderTex = T3D_TEXTURE_MGR.createRenderTexture("RT_Scene", 640, 480, PixelFormat::E_PF_R8G8B8A8);
        RenderTargetPtr rt = RenderTarget::create(renderTex);
#endif
    
        CameraPtr camera;

        // center camera
        go = GameObject::create("CenterCamera");
        Transform3DPtr center = go->addComponent<Transform3D>();
        root->addChild(center);
        camera = go->addComponent<Camera>();
        camera->setOrder(1);
        Viewport vpCenter {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
        camera->setViewport(vpCenter);
        camera->setClearColor(ColorRGB::BLUE);
        camera->setRenderTarget(rt);
        scene->addCamera(camera);

        mSceneRT = renderTex->getPixelBuffer()->getRHIResource()->getNativeObject();
    }

    //--------------------------------------------------------------------------

    bool EditorApp::enginePollEvents()
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
        return !done;
    }

    //--------------------------------------------------------------------------

    void EditorApp::engineUpdate()
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

    void EditorApp::enginePreRender()
    {
        ImGui::Render();
        mImGuiImpl->preRender();
    }

    //--------------------------------------------------------------------------

    void EditorApp::enginePostRender()
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



