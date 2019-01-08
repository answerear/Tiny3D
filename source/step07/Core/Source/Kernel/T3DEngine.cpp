/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#include "Kernel/T3DEngine.h"
#include "Resource/T3DArchiveManager.h"
#include "Resource/T3DFileSystemArchive.h"
#include "Resource/T3DZipArchieve.h"
#include "Kernel/T3DConfigFile.h"
#include "DataStruct/T3DString.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(Engine);

    //--------------------------------------------------------------------------

    Engine::Engine()
        : mLogger(nullptr)
        , mEventMgr(nullptr)
        , mWindow(nullptr)
        , mIsRunning(false)
        , mArchiveMgr(nullptr)
    {
    }

    Engine::~Engine()
    {
        T3D_SAFE_DELETE(mWindow);
        T3D_SAFE_DELETE(mEventMgr);

        mLogger->shutdown();
        T3D_SAFE_DELETE(mLogger);
    }

    //--------------------------------------------------------------------------

    TResult Engine::init(const String &appPath, 
        const String &config /* = "Tiny3D.cfg" */)
    {
        TResult ret = T3D_ERR_FAIL;

        do
        {
            // 获取应用程序路径、应用程序名称
            StringUtil::split(appPath, mAppPath, mAppName);

            // 初始化应用程序框架，这个需要放在最前面，否则平台相关接口均不能用
            ret = initApplication();
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            // 初始化日志系统，这个需要放在前面，避免日志无法输出
            ret = initLogSystem();
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            // 初始化事件系统
            ret = initEventSystem();
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            // 初始化档案系统
            ret = initArchives();
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            ret = loadConfig(config);

            // 创建渲染窗口
            ret = createRenderWindow("Demo");
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            mIsRunning = true;
            ret = T3D_ERR_OK;
        } while (0);

        return ret;
    }

    bool Engine::run()
    {
        Application *theApp = Application::getInstancePtr();
        theApp->applicationDidFinishLaunching();

        while (mIsRunning)
        {
            // 轮询系统事件
            mIsRunning = theApp->pollEvents();

            if (!mIsRunning)
                break;

            // 事件系统派发事件
            T3D_EVENT_MGR.dispatchEvent();

            // 渲染一帧
            renderOneFrame();
        }

        theApp->applicationWillTerminate();

        return true;
    }

    void Engine::renderOneFrame()
    {
        {
            T3D_LOG_INFO("Begin Application Stage ......");
            {
                T3D_LOG_INFO("\t#0 Travel each node in scene graph and transform from local space to world space.");
                T3D_LOG_INFO("\t#1 Travel each node in scene graph, then do object frustum culling and put their into rendering queue.");
                T3D_LOG_INFO("\t#2 Travel rendering queue, then setup their rendering status and call rendering API.");
            }
            T3D_LOG_INFO("End Application Stage.");
        }
        {
            T3D_LOG_INFO("Begin Geometry Stage ......");
            {
                T3D_LOG_INFO("\t#0 Vertex Shader.");
                T3D_LOG_INFO("\t#1 Tessellation Shader.");
                T3D_LOG_INFO("\t#2 Geometry Shader.");
                T3D_LOG_INFO("\t#3 Culling.");
                T3D_LOG_INFO("\t#4 Screen Mapping.");
            }
            T3D_LOG_INFO("End Geometry Stage.");
        }
        {
            T3D_LOG_INFO("Begin Rasterization Stage ......");
            {
                T3D_LOG_INFO("\t#0 Triangle setup.");
                T3D_LOG_INFO("\t#1 Triangle Traversal.");
                T3D_LOG_INFO("\t#2 Fragment Shader.");
                T3D_LOG_INFO("\t#3 Fragment primitvie operating");
                {
                    T3D_LOG_INFO("\t\t3.1 Stencil buffer test.");
                    T3D_LOG_INFO("\t\t3.2 Depth buffer test.");
                    T3D_LOG_INFO("\t\t3.3 Alpha blending.");
                    T3D_LOG_INFO("\t\t3.4 Output framebuffer.");
                }
            }
            T3D_LOG_INFO("End Rasterization Stage.");
        }
    }

    //--------------------------------------------------------------------------

    void Engine::appWillEnterForeground()
    {
        T3D_LOG_ENTER_FOREGROUND();
    }

    void Engine::appDidEnterBackground()
    {
        T3D_LOG_ENTER_BACKGROUND();
    }

    //--------------------------------------------------------------------------

    TResult Engine::initApplication()
    {
        TResult ret = T3D_ERR_OK;

        do
        {
            Application *theApp = Application::getInstancePtr();
            if (theApp == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            ret = theApp->init();
            if (ret != T3D_ERR_OK)
            {
                break;
            }
        } while (0);

        return ret;
    }

    TResult Engine::initLogSystem()
    {
        TResult ret = T3D_ERR_FAIL;

        mLogger = new Logger();

        if (mLogger != nullptr)
        {
            ret = mLogger->startup(1000, "Engine", true, true);
        }

        T3D_LOG_INFO("Start Tiny3D ...... version %s",
            T3D_DEVICE_INFO.getSoftwareVersion().c_str());

        T3D_LOG_INFO("System Information : \n%s",
            T3D_DEVICE_INFO.getSystemInfo().c_str());

        return ret;
    }

    TResult Engine::initEventSystem()
    {
        mEventMgr = new EventManager(10);
        return T3D_ERR_OK;
    }

    TResult Engine::createRenderWindow(const String &title)
    {
        TResult ret = T3D_ERR_OK;

        do 
        {
            mWindow = new Window();
            ret = mWindow->create(title.c_str(), 100, 100, 800, 600,
                Window::WINDOW_SHOWN);
            if (ret != T3D_ERR_OK)
            {
                break;
            }
        } while (0);
        
        return ret;
    }

    TResult Engine::initArchives()
    {
        mArchiveMgr = ArchiveManager::create();

        FileSystemArchiveCreator *fsCreator = new FileSystemArchiveCreator();
        mArchiveMgr->addArchiveCreator(fsCreator);

        ZipArchiveCreator *zipCreator = new ZipArchiveCreator();
        mArchiveMgr->addArchiveCreator(zipCreator);

        return T3D_ERR_OK;
    }

    TResult Engine::loadConfig(const String &cfgPath)
    {
        TResult ret = T3D_ERR_OK;

#if defined (T3D_OS_ANDROID)
        // Android，只能读取apk包里面的文件
        String apkPath;
        ArchivePtr archive = mArchiveMgr->loadArchive(apkPath, "Archive");
        ConfigFile cfgFile(cfgPath, archive);
        ret = cfgFile.loadXML(mSettings);
#else
        // 其他不需要从 apk 包里面读取文件的
        String path = mAppPath + cfgPath;
        ConfigFile cfgFile(path);
        ret = cfgFile.loadXML(mSettings);
#endif

        return ret;
    }
}
