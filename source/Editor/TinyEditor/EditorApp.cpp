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
#include "ProjectManager.h"
#include "NetworkManager.h"
#include "EditorScene.h"
#include "EditorWidgetID.h"
#include "EditorEventDefine.h"


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
        // if (mNetworkMgr != nullptr)
        // {
        //     mNetworkMgr->shutdown();
        // }
        //
        // T3D_SAFE_DELETE(mNetworkMgr);
        T3D_SAFE_DELETE(mProjectMgr)
        mLangMgr = nullptr;
        T3D_SAFE_DELETE(mEditorScene)
        T3D_SAFE_DELETE(mEngine)
        
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
        mExitApp = true;
    }

    //--------------------------------------------------------------------------
    
    TResult EditorApp::go(int32_t argc, char *argv[])
    {
        TResult ret;

        do
        {
            // 启动
            ret = startup(argc, argv);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 运行
            run();

            // 关闭
            shutdown();
        } while (false);

        mLangMgr = nullptr;
        T3D_SAFE_DELETE(mEngine);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool EditorApp::run()
    {
        // 构建引擎运行数据，并运行引擎
        EditorRunningData runningData;
        runningData.update = std::bind(&EditorApp::engineUpdate, this);
        runningData.preRender = std::bind(&EditorApp::enginePreRender, this);
        runningData.postRender = std::bind(&EditorApp::enginePostRender, this);
        return mEngine->runForEditor(runningData);
    }

    //--------------------------------------------------------------------------

    TResult EditorApp::startup(int32_t argc, char *argv[])
    {
        TResult ret = T3D_OK;

        do
        {
            Dir::setCachePathInfo("Tiny3D", "TinyEditor");
            
            ret = createEngine(argc, argv);
            if (T3D_FAILED(ret))
            {
                break;
            }

            std::stringstream ss;
            for (int32_t i = 0; i < argc; ++i)
            {
                ss << argv[i] << " ";
            }
            
            T3D_LOG_INFO(LOG_TAG_EDITOR, "Tthe number of arguments : %d, Command line : %s", argc, ss.str().c_str());
            
            if (!parseCommandLine(argc, argv))
            {
                ret = T3D_ERR_FAIL;
                break;
            }

            // T3D_LOG_INFO(LOG_TAG_EDITOR, "Path:%s, Name:%s, Created: %d", mOptions.path.c_str(), mOptions.name.c_str(), mOptions.created);
            
            // 加载语言文件
            ret = createLanguageMgr();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 创建工程管理器
            mProjectMgr = new ProjectManager();

            // 创建网络管理器
            mNetworkMgr = new NetworkManager();
            ret = mNetworkMgr->startup("127.0.0.1", 5327);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Startup network failed ! ERROR [%d]", ret);
                break;
            }

            if (mOptions.created)
            {
                // 新建工程
                ret = mNetworkMgr->createProject(mOptions.path, mOptions.name);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
            else
            {
                // 打开工程
                ret = mNetworkMgr->openProject(mOptions.path, mOptions.name);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
            
            // 创建 imgui 环境
            ret = createImGuiEnv();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create ImGui environment failed ! ERROR [%d]", ret);
                break;
            }

            // TinyImGui 单例初始化
            mTextureMgr = new ImTextureManager();

            // 编辑器场景
            mEditorScene = new EditorScene();
            mEditorScene->build();

            // 主窗口
            mMainWindow = new MainWindow();
            ret = mMainWindow->create(ID_MAIN_WINDOW, "Main Window", nullptr);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create main window failed ! ERROR [%d]", ret);
                break;
            }

            SDL_MaximizeWindow(mSDLWindow);
            
            // 构建编辑器场景
            // buildScene();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult EditorApp::createEngine(int32_t argc, char *argv[])
    {
        TResult ret = T3D_OK;

        do
        {
            mEngine = new Agent();

            Settings settings;
            settings.renderSettings.resizable = true;
            settings.renderSettings.title = "Tiny3D Editor";
            settings.pluginSettings.pluginPath = ".";
            settings.pluginSettings.plugins.emplace_back("FileSystemArchiveEditor");
            settings.pluginSettings.plugins.emplace_back("D3D11RendererEditor");
            settings.pluginSettings.plugins.emplace_back("FreeImageCodecEditor");
            
            // 初始化引擎，只有初始化后才能使用
            ret = mEngine->init(argc, argv, true, true, settings, kEvtMax);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Init engine failed ! ERROR [%d]", ret);
                break;
            }

            T3D_ARCHIVE_MGR.loadArchive(Dir::getAppPath(), "FileSystem", Archive::AccessMode::kRead);

            mEngine->setRunInBackground(true);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult EditorApp::createLanguageMgr()
    {
        TResult ret = T3D_OK;

        do
        {
            mLangMgr = LanguageManager::create();
            // String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Editor" + Dir::getNativeSeparator() + "Language" + Dir::getNativeSeparator() + "lang-en-us.txt";
            String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Editor" + Dir::getNativeSeparator() + "Language" + Dir::getNativeSeparator() + "lang-zh-hans.txt";
            ret = mLangMgr->init(path);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Init language file failed ! ERROR [%d]", ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    extern const char *kTinyEditorInitData;

    //--------------------------------------------------------------------------

    TResult EditorApp::createImGuiEnv()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = mEngine->loadPlugin(IMGUI_DX11_PLUGIN);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Load ImGuiDX11 plugin failed ! ERROR [%d]", ret);
                break;
            }

            IMGUI_CHECKVERSION();
            ImGuiContext *ctx = ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
            //io.ConfigViewportsNoAutoMerge = true;
            //io.ConfigViewportsNoTaskBarIcon = true;

            static String iniPath = Dir::getAppPath() + Dir::getNativeSeparator() + "TinyEditorGUI.ini";
            io.IniFilename = iniPath.c_str();

            // if (!Dir::exists(iniPath.c_str()))
            // {
            //     ImGui::LoadIniSettingsFromMemory(kTinyEditorInitData);
            //     ctx->SettingsLoaded = true;
            // }
            
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

            style.WindowMenuButtonPosition = ImGuiDir_None;
            
#if defined(T3D_OS_WINDOWS)
            EditorInfoDX11 info;
            mEngine->getEditorInfo(&info);
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

    void EditorApp::destroyImGuiEnv()
    {
        mEngine->unloadPlugin(IMGUI_DX11_PLUGIN);
        ImGui::DestroyContext();
    }

    //--------------------------------------------------------------------------

    void EditorApp::shutdown()
    {
        if (mMainWindow != nullptr)
        {
            mMainWindow->destroy();
        }
            
        ImWidget::GC();

        mTextureMgr->unloadAllTextures();

        T3D_SAFE_DELETE(mTextureMgr);

        // 删除清理 imgui 环境，此后无法再使用 imgui
        destroyImGuiEnv();

        T3D_SAFE_DELETE(mEditorScene)

        if (mNetworkMgr != nullptr)
        {
            mNetworkMgr->shutdown();
        }
        
        T3D_SAFE_DELETE(mNetworkMgr)

        if (mProjectMgr != nullptr)
        {
            mProjectMgr->closeProject();
        }
        T3D_SAFE_DELETE(mProjectMgr)
        
        mLangMgr = nullptr;
        T3D_SAFE_DELETE(mEngine)
    }

    //--------------------------------------------------------------------------

    bool EditorApp::parseCommandLine(int32_t argc, char *argv[])
    {
        bool ret = true;

        do
        {
            if (argc != 6)
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "The number of arguments [%d] in command line is invalid !", argc);
                ret = false;
                break;
            }

            int32_t i = 1;

            while (i < argc)
            {
                if (strcmp(argv[i], "-p") == 0)
                {
                    ++i;
                    // path
                    mOptions.path = argv[i];
                }
                else if (strcmp(argv[i], "-n") == 0)
                {
                    // name
                    ++i;
                    mOptions.name = argv[i];
                }
                else if (strcmp(argv[i], "-c") == 0)
                {
                    // create project
                    mOptions.created = true;
                }
                else if (strcmp(argv[i], "-o") == 0)
                {
                    // open project
                    mOptions.created = false;
                }
            
                ++i;
            }
        } while (false);

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

#if 0
        RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
        RenderTargetPtr rt = RenderTarget::create(rw);
#else
        RenderWindow *rw = T3D_AGENT.getDefaultRenderWindow();
        RenderTexturePtr renderTex = T3D_TEXTURE_MGR.createRenderTexture("RT_Scene", 640, 480, PixelFormat::E_PF_B8G8R8A8);
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

    bool EditorApp::processEvents(const AppEvent &event)
    {
        // SDL_Event &event = *static_cast<SDL_Event*>(ev);
        SDL_Event evt;
        T3D_ASSERT(sizeof(event) == sizeof(evt));
        memcpy(&evt, &event, sizeof(AppEvent));
        mImGuiImpl->processEvents(&evt);
        bool done = !WindowApplication::processEvents(event);

        done = !done && !T3D_AGENT.processEvents(event);
        
        if (event.type == APP_WINDOWEVENT)
        {
            switch (event.window.event)
            {
            case APP_WINDOWEVENT_CLOSE:
                {
                    if (event.window.windowID == SDL_GetWindowID(mSDLWindow))
                    {
                        done = true;
                    }
                }
                break;
            case APP_WINDOWEVENT_RESIZED:
                {
                    if (event.window.windowID == SDL_GetWindowID(mSDLWindow))
                    {
#if defined(T3D_OS_WINDOWS)
                        EditorInfoDX11 info;
                        T3D_AGENT.getEditorInfo(&info);
                        mImGuiImpl->refreshInfo(&info);
#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_LINUX)
#endif
                    }
                }
                break;
            }
        }
        
        return !done && !mExitApp;
    }

    //--------------------------------------------------------------------------

    void EditorApp::engineUpdate()
    {
        mNetworkMgr->poll();
        
        mImGuiImpl->update();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
        
        ImWidget::beginUpdate();
        
        if (mMainWindow != nullptr)
        {
            mMainWindow->update();
        }

        ImWidget::endUpdate();

        ImGui::EndFrame();
    }

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



