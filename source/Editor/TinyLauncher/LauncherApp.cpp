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
#include "ProjectManager.h"
#include "AppSettings.h"
#include "NetworkManager.h"
#include "LauncherWidgetID.h"
#include "LauncherEventDefine.h"


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
        if (mNetworkMgr != nullptr)
        {
            mNetworkMgr->shutdown();
        }
        
        T3D_SAFE_DELETE(mNetworkMgr);
        T3D_SAFE_DELETE(mProjectMgr);
        mLangMgr = nullptr;
        T3D_SAFE_DELETE(mEngine);
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

        return ret;
    }
    
    //--------------------------------------------------------------------------

    bool LauncherApp::run()
    {
        EditorRunningData runningData;
        runningData.pollEvents = std::bind(&LauncherApp::enginePollEvents, this);
        runningData.update = std::bind(&LauncherApp::engineUpdate, this);
        runningData.preRender = std::bind(&LauncherApp::enginePreRender, this);
        runningData.postRender = std::bind(&LauncherApp::enginePostRender, this);
        return mEngine->runForEditor(runningData);
    }
    
    //--------------------------------------------------------------------------

    TResult LauncherApp::startup(int32_t argc, char *argv[])
    {
        TResult ret = T3D_OK;

        do
        {
            // 先检查是否有同名进程，如果已经启动过 launcher，则不再启动新实例
            const String &procName = Process::getCurrentProcessName();
            ulong_t pid = Process::getCurrentProcessID();
            ulong_t runningProcID = 0;
            bool isRunning = false;
            Process::traverseAllProcesses(
                [&runningProcID, &isRunning, &procName, pid](ulong_t id, const String &name)
                {
                    isRunning = (name == procName && pid != id);
                    if (isRunning)
                    {
                        runningProcID = id;
                    }
                    return isRunning;
                });
            if (isRunning)
            {
                // 如果已经存在的进程，直接拉起
                Process::wakeupProcess(runningProcID);
                printf("Exists the same name process !");
                ret = T3D_ERR_FAIL;
                break;
            }

            // 设置缓存文件路径
            Dir::setCachePathInfo("Tiny3D", "TinyLauncher");

            // 创建引擎
            ret = createEngine(argc, argv);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 读取 app 设置
            readAppSettings();

            // 创建语言管理器并加载语言文件
            ret = createLanguageMgr();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 创建工程管理器
            mProjectMgr = new ProjectManager();

            // 创建网络管理器
            mNetworkMgr = new NetworkManager();
            ret = mNetworkMgr->startup();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Startup network failed ! ERROR [%d]", ret);
                break;
            }

            // 创建 imgui 环境
            ret = createImGuiEnv();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Create ImGui environment failed ! ERROR [%d]", ret);
                break;
            }

            // 主窗口
            mMainWindow = new MainWindow();
            ret = mMainWindow->create(ID_MAIN_WINDOW, "Project Manager Window", nullptr);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Create project manager window failed ! ERROR [%d]", ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult LauncherApp::createEngine(int32_t argc, char *argv[])
    {
        TResult ret = T3D_OK;
        
        do
        {
            mEngine = new Agent();

            Settings settings;
            settings.renderSettings.resizable = true;
            settings.renderSettings.title = "Launcher";
            settings.pluginSettings.pluginPath = ".";
            settings.pluginSettings.plugins.emplace_back("FileSystemArchiveEditor");
            settings.pluginSettings.plugins.emplace_back("D3D11RendererEditor");
            settings.pluginSettings.plugins.emplace_back("FreeImageCodecEditor");
            
            // 初始化引擎，只有初始化后才能使用
            ret = mEngine->init(argc, argv, true, true, settings, kEvtMax);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Init engine failed ! ERROR [%d]", ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void LauncherApp::readAppSettings()
    {
        do
        {
            String path = getSettingsPath();
            if (Dir::exists(path))
            {
                FileDataStream fs;
                if (!fs.open(path.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
                {
                    T3D_LOG_WARNING(LOG_TAG_LAUNCHER, "Read setting file [%s] failed !", path.c_str());
                    break;
                }

                TResult ret = T3D_SERIALIZER_MGR.deserialize(fs, mAppSettings);
                if (T3D_FAILED(ret))
                {
                    fs.close();
                    T3D_LOG_WARNING(LOG_TAG_LAUNCHER, "Deserialize settings object failed !");
                    mAppSettings = AppSettings();
                    break;
                }
                
                fs.close();
            }
        } while (false);
    }

    //--------------------------------------------------------------------------

    void LauncherApp::writeAppSettings()
    {
        String path = getSettingsPath();
        FileDataStream fs;
        if (fs.open(path.c_str(), FileDataStream::EOpenMode::E_MODE_TRUNCATE|FileDataStream::EOpenMode::E_MODE_WRITE_ONLY))
        {
            T3D_SERIALIZER_MGR.serialize(fs, mAppSettings);
            fs.close();
        }
    }

    //--------------------------------------------------------------------------

    TResult LauncherApp::createLanguageMgr()
    {
        TResult ret = T3D_OK;

        do
        {
            mLangMgr = LanguageManager::create();
            String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Launcher" + Dir::getNativeSeparator() + "Language" + Dir::getNativeSeparator() + mAppSettings.languageFileName;
            ret = mLangMgr->init(path);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Init language file failed ! ERROR [%d]", ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult LauncherApp::createImGuiEnv()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = mEngine->loadPlugin(IMGUI_DX11_PLUGIN);
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
            mEngine->getEditorInfo(&info);
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

    void LauncherApp::destroyImGuiEnv()
    {
        mEngine->unloadPlugin(IMGUI_DX11_PLUGIN);
        ImGui::DestroyContext();
    }

    //--------------------------------------------------------------------------

    void LauncherApp::shutdown()
    {
        // 删除主窗口
        if (mMainWindow != nullptr)
        {
            mMainWindow->destroy();
        }

        // 回收所有 widgets
        ImWidget::GC();

        // 删除清理 imgui 环境，此后无法再使用 imgui
        destroyImGuiEnv();

        // 写 app 设置
        writeAppSettings();

        if (mNetworkMgr != nullptr)
        {
            mNetworkMgr->shutdown();
        }
        
        T3D_SAFE_DELETE(mNetworkMgr);
        T3D_SAFE_DELETE(mProjectMgr);
        mLangMgr = nullptr;
        T3D_SAFE_DELETE(mEngine);
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
        mNetworkMgr->poll();
        
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



