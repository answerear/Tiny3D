/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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

#include "T3DErrorDef.h"
#include "Kernel/T3DEngine.h"
#include "Kernel/T3DPlugin.h"
#include "Kernel/T3DConfigFile.h"

#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveCreator.h"
#include "Resource/T3DArchiveManager.h"
#include "Resource/T3DDylib.h"
#include "Resource/T3DDylibManager.h"

#include "DataStruct/T3DString.h"

#include "Memory/T3DObjectTracer.h"



namespace Tiny3D
{
    typedef TResult (*DLL_START_PLUGIN)(void);
    typedef TResult (*DLL_STOP_PLUGIN)(void);

    //--------------------------------------------------------------------------

    T3D_INIT_SINGLETON(Engine);

    //--------------------------------------------------------------------------

    Engine::Engine()
        : mLogger(nullptr)
        , mEventMgr(nullptr)
        , mObjTracer(nullptr)
        , mWindow(nullptr)
        , mIsRunning(false)
        , mArchiveMgr(nullptr)
    {
    }

    Engine::~Engine()
    {
        unloadPlugins();

        mDylibMgr = nullptr;
        mArchiveMgr = nullptr;

        T3D_SAFE_DELETE(mWindow);
        T3D_SAFE_DELETE(mEventMgr);

        mObjTracer->dumpMemoryInfo();
        T3D_SAFE_DELETE(mObjTracer);

        mLogger->shutdown();
        T3D_SAFE_DELETE(mLogger);
    }

    //--------------------------------------------------------------------------

    TResult Engine::init(const String &appPath, 
        const String &config /* = "Tiny3D.cfg" */)
    {
        TResult ret = T3D_ERR_OK;

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

            // 初始化对象追踪器
            ret = initObjectTracer();
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            // 初始化各种管理器
            ret = initManagers();
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            // 加载配置文件
            ret = loadConfig(config);
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            // 加载配置文件中指定的插件
            ret = loadPlugins();
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            // 创建渲染窗口
            ret = createRenderWindow();
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            mIsRunning = true;
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

    TResult Engine::installPlugin(Plugin *plugin)
    {
        TResult ret = T3D_ERR_OK;

        do 
        {
            if (plugin == nullptr)
            {
                // 空指针
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR("Invalid plugin !!!");
                break;
            }

            auto rval 
                = mPlugins.insert(PluginsValue(plugin->getName(), plugin));
            if (!rval.second)
            {
                ret = T3D_ERR_PLG_DUPLICATED;
                T3D_LOG_ERROR("Duplicated plugin [%s] !", 
                    plugin->getName().c_str());
                break;
            }

            // 安装插件
            ret = plugin->install();
            if (ret != T3D_ERR_OK)
            {
                mPlugins.erase(plugin->getName());
                T3D_LOG_ERROR("Install plugin [%s] failed !",
                    plugin->getName().c_str());
                break;
            }

            // 启动插件
            ret = plugin->startup();
            if (ret != T3D_ERR_OK)
            {
                mPlugins.erase(plugin->getName());
                T3D_LOG_ERROR("Startup plugin [%s] failed !", 
                    plugin->getName().c_str());
                break;
            }
        } while (0);

        return ret;
    }

    TResult Engine::uninstallPlugin(Plugin *plugin)
    {
        TResult ret = T3D_ERR_OK;

        do 
        {
            if (plugin == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR("Invalid plugin !!!");
                break;
            }

            ret = plugin->shutdown();
            if (ret != T3D_ERR_OK)
            {
                T3D_LOG_ERROR("Shutdown plugin [%s] failed !", 
                    plugin->getName().c_str());
                break;
            }

            ret = plugin->uninstall();
            if (ret != T3D_ERR_OK)
            {
                T3D_LOG_ERROR("Uninstall plugin [%s] failed !",
                    plugin->getName().c_str())
                break;
            }

            mPlugins.erase(plugin->getName());
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Engine::loadPlugin(const String &name)
    {
        T3D_LOG_INFO("Load plugin %s ...", name.c_str());

        TResult ret = T3D_ERR_OK;

        do 
        {
            DylibPtr dylib = DylibManager::getInstance().loadDylib(name);

            if (dylib->getType() != Resource::E_TYPE_DYLIB)
            {
                ret = T3D_ERR_PLG_NOT_DYLIB;
                T3D_LOG_ERROR("Load plugin [%s] failed !", name.c_str());
                break;
            }

            DLL_START_PLUGIN pFunc 
                = (DLL_START_PLUGIN)(dylib->getSymbol("dllStartPlugin"));
            if (pFunc == nullptr)
            {
                ret = T3D_ERR_PLG_NO_FUNCTION;
                T3D_LOG_ERROR("Load plugin [%s] get function dllStartPlugin \
                    failed !", name.c_str());
                break;
            }

            ret = pFunc();
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            mDylibs.insert(DylibsValue(dylib->getName(), dylib));
        } while (0);

        return ret;
    }

    TResult Engine::unloadPlugin(const String &name)
    {
        T3D_LOG_INFO("Unload plugin %s ...", name.c_str());

        TResult ret = T3D_ERR_OK;

        do 
        {
            DylibsItr itr = mDylibs.find(name);
            if (itr == mDylibs.end())
            {
                ret = T3D_ERR_PLG_NOT_EXISTS;
                T3D_LOG_ERROR("Unload plugin [%s] , it don't exist !",
                    name.c_str());
                break;
            }

            DylibPtr dylib = itr->second;
            DLL_STOP_PLUGIN pFunc 
                = (DLL_STOP_PLUGIN)(dylib->getSymbol("dllStopPlugin"));
            if (pFunc == nullptr)
            {
                ret = T3D_ERR_PLG_NO_FUNCTION;
                T3D_LOG_ERROR("Unload plugin [%s], get function dllStopPlugin \
                    failed !", name.c_str());
                break;
            }

            ret = pFunc();
            if (ret != T3D_ERR_OK)
            {
                break;
            }

            mDylibs.erase(itr);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Engine::addArchiveCreator(ArchiveCreator *creator)
    {
        TResult ret = T3D_ERR_OK;
        mArchiveMgr->addArchiveCreator(creator);
        return ret;
    }

    TResult Engine::removeArchiveCreator(ArchiveCreator *creator)
    {
        TResult ret = T3D_ERR_OK;
        mArchiveMgr->removeArchiveCreator(creator->getType());
        return ret;
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

    TResult Engine::initObjectTracer()
    {
        mObjTracer = new ObjectTracer();
        return T3D_ERR_OK;
    }

    TResult Engine::initManagers()
    {
        mArchiveMgr = ArchiveManager::create();
        mDylibMgr = DylibManager::create();

        return T3D_ERR_OK;
    }

    TResult Engine::loadConfig(const String &cfgPath)
    {
        TResult ret = T3D_ERR_OK;

#if defined (T3D_OS_ANDROID)
        // Android，只能读取apk包里面的文件
        String apkPath = Dir::getAppPath();
        ArchivePtr archive = mArchiveMgr->loadArchive(apkPath, "Zip");
        ConfigFile cfgFile("assets/" + cfgPath, archive);
        ret = cfgFile.loadXML(mSettings);
#else
        // 其他不需要从 apk 包里面读取文件的
        String path = mAppPath + cfgPath;
        ConfigFile cfgFile(path);
        ret = cfgFile.loadXML(mSettings);
#endif

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Engine::loadPlugins()
    {
        TResult ret = T3D_ERR_OK;

        do 
        {
            Settings pluginSettings = mSettings["Plugins"].mapValue();
            String s("Path");
            Variant key(s);
            Settings::const_iterator itr = pluginSettings.find(key);
            if (itr == pluginSettings.end())
            {
                ret = T3D_ERR_PLG_NO_PATH;
                T3D_LOG_ERROR("Load plguins - the plugin path don't set !");
                break;
            }

            mPluginsPath = itr->second.stringValue();

            key.setString("List");
            itr = pluginSettings.find(key);

            if (itr == pluginSettings.end())
            {
                // 虽然没有获取到任何插件，但是仍然是合法的，正常返回
                ret = T3D_ERR_OK;
                break;
            }

            const VariantArray &plugins = itr->second.arrayValue();
            VariantArrayConstItr i = plugins.begin();

            while (i != plugins.end())
            {
                String name = i->stringValue();
                ret = loadPlugin(name);
                if (ret != T3D_ERR_OK)
                {
                    break;
                }

                ++i;
            }
        } while (0);

        return ret;
    }

    TResult Engine::unloadPlugins()
    {
        TResult ret = T3D_ERR_OK;

        DylibsItr itr = mDylibs.begin();
        while (itr != mDylibs.end())
        {
            DylibPtr dylib = itr->second;
            DLL_STOP_PLUGIN pFunc 
                = (DLL_STOP_PLUGIN)(dylib->getSymbol("dllStopPlugin"));
            if (pFunc != nullptr)
            {
                ret = pFunc();
                if (ret == T3D_ERR_OK)
                {
                    DylibManager::getInstance().unloadDylib(dylib);
                }
            }
            ++itr;
        }

        mDylibs.clear();

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Engine::createRenderWindow()
    {
        TResult ret = T3D_ERR_OK;

        do 
        {
            Settings settings = mSettings["Render"].mapValue();

            // 窗口标题
            String title = settings["Title"].stringValue();
            // 窗口位置
            int32_t x = settings["x"].int32Value();
            int32_t y = settings["y"].int32Value();
            // 窗口大小
            int32_t w = settings["Width"].int32Value();
            int32_t h = settings["Height"].int32Value();
            // 是否全屏
            bool fullscreen = settings["FullScreen"].boolValue();
            // 创建标记位
            uint32_t flags = Window::WINDOW_SHOWN;
            if (fullscreen)
                flags |= Window::WINDOW_FULLSCREEN;

            // 创建窗口
            mWindow = new Window();
            ret = mWindow->create(title.c_str(), x, y, w, h, flags);
            if (ret != T3D_ERR_OK)
            {
                break;
            }
        } while (0);
        
        return ret;
    }
}
