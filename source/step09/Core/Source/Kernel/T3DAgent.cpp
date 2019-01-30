﻿/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
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

#include "Kernel/T3DAgent.h"
#include "Kernel/T3DPlugin.h"
#include "Kernel/T3DConfigFile.h"

#include "ImageCodec/T3DImageCodec.h"

#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveCreator.h"
#include "Resource/T3DArchiveManager.h"
#include "Resource/T3DDylib.h"
#include "Resource/T3DDylibManager.h"

#include "DataStruct/T3DString.h"

#include "Memory/T3DObjectTracer.h"

#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    typedef TResult (*DLL_START_PLUGIN)(void);
    typedef TResult (*DLL_STOP_PLUGIN)(void);

    //--------------------------------------------------------------------------

    T3D_INIT_SINGLETON(Agent);

    //--------------------------------------------------------------------------

    Agent::Agent()
        : mLogger(nullptr)
        , mEventMgr(nullptr)
        , mObjTracer(nullptr)
        , mWindow(nullptr)
        , mArchiveMgr(nullptr)
        , mDylibMgr(nullptr)
        , mImageCodec(nullptr)
        , mActiveRenderer(nullptr)
        , mIsRunning(false)
    {
    }

    Agent::~Agent()
    {
        mActiveRenderer = nullptr;

        unloadPlugins();

        mDylibMgr = nullptr;
        mArchiveMgr = nullptr;
        mImageCodec = nullptr;

        T3D_SAFE_DELETE(mWindow);
        T3D_SAFE_DELETE(mEventMgr);

        mObjTracer->dumpMemoryInfo();
        T3D_SAFE_DELETE(mObjTracer);

        mLogger->shutdown();
        T3D_SAFE_DELETE(mLogger);
    }

    //--------------------------------------------------------------------------

    TResult Agent::init(const String &appPath, bool autoCreateWindow,
        const String &rendererName, const String &config /* = "Tiny3D.cfg" */)
    {
        TResult ret = T3D_OK;

        do
        {
#if !defined (T3D_OS_ANDROID)
            // 获取应用程序路径、应用程序名称
            StringUtil::split(appPath, mAppPath, mAppName);
#endif

            // 初始化应用程序框架，这个需要放在最前面，否则平台相关接口均不能用
            ret = initApplication();
            if (ret != T3D_OK)
            {
                break;
            }

            // 初始化日志系统，这个需要放在前面，避免日志无法输出
            ret = initLogSystem();
            if (ret != T3D_OK)
            {
                break;
            }

#if defined (T3D_OS_ANDROID)
            mAppPath = Dir::getAppPath();

            // Android 单独设置插件路径，不使用配置文件里面设置的路径
            // 因为android的插件在/data/data/appname/lib文件下
            mPluginsPath = Dir::getLibraryPath();
#endif

            // 初始化事件系统
            ret = initEventSystem();
            if (ret != T3D_OK)
            {
                break;
            }

            // 初始化对象追踪器
            ret = initObjectTracer();
            if (ret != T3D_OK)
            {
                break;
            }

            // 初始化各种管理器
            ret = initManagers();
            if (ret != T3D_OK)
            {
                break;
            }

            // 加载配置文件
            ret = loadConfig(config);
            if (ret != T3D_OK)
            {
                break;
            }

            // 加载配置文件中指定的插件
            ret = loadPlugins();
            if (ret != T3D_OK)
            {
                break;
            }

            // 初始化资源
            ret = initAssets();
            if (ret != T3D_OK)
            {
                break;
            }

            // 初始化渲染器
            ret = initRenderer(rendererName);
            if (ret != T3D_OK)
            {
                break;
            }

            if (autoCreateWindow)
            {
                // 创建渲染窗口
                RenderWindowPtr window;
                ret = createDefaultRenderWindow(window);
                if (ret != T3D_OK)
                {
                    break;
                }

                addRenderWindow(window);
            }

            mIsRunning = true;
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::createDefaultRenderWindow(RenderWindowPtr &window)
    {
        TResult ret = T3D_OK;

        do
        {
            Settings settings = mSettings["Render"].mapValue();

            RenderWindowCreateParam param;
            // 窗口标题
            param.windowTitle = settings["Title"].stringValue();
            // 窗口位置
            param.windowLeft = settings["x"].int32Value();
            param.windowTop = settings["y"].int32Value();
            // 窗口大小
            param.windowWidth = settings["Width"].int32Value();
            param.windowHeight = settings["Height"].int32Value();
            // 是否全屏
            param.fullscreen = settings["FullScreen"].boolValue();
            // 图标路径
            param.iconPath = settings["Icon"].stringValue();

            RenderWindowCreateParamEx paramEx;

            window = mActiveRenderer->createRenderWindow(
                param.windowTitle, param, paramEx);
            if (window == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Create render window failed !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::addRenderWindow(RenderWindowPtr window)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (mActiveRenderer == nullptr)
            {
                ret = T3D_ERR_SYS_NOT_INIT;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Do not set active renderer !");
                break;
            }

            ret = mActiveRenderer->attachRenderTarget(window);
        } while (0);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::removeRenderWindow(const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mActiveRenderer == nullptr)
            {
                ret = T3D_ERR_SYS_NOT_INIT;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Do not set active renderer !");
                break;
            }

            ret = mActiveRenderer->detachRenderTarget(name);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    RenderWindowPtr Agent::getRenderWindow(const String &name) const
    {
        RenderWindowPtr window = nullptr;
        
        if (mActiveRenderer == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_ENGINE, "Do not set active renderer !");
        }
        else
        {
            window = mActiveRenderer->getRenderTarget(name);
        }

        return window;
    }

    //--------------------------------------------------------------------------

    bool Agent::run()
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

    void Agent::renderOneFrame()
    {
        if (mActiveRenderer != nullptr)
        {
            mActiveRenderer->render();
        }
    }

    //--------------------------------------------------------------------------

    void Agent::appWillEnterForeground()
    {
        T3D_LOG_ENTER_FOREGROUND();
    }

    void Agent::appDidEnterBackground()
    {
        T3D_LOG_ENTER_BACKGROUND();
    }

    //--------------------------------------------------------------------------

    TResult Agent::installPlugin(Plugin *plugin)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (plugin == nullptr)
            {
                // 空指针
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid plugin !!!");
                break;
            }

            auto rval 
                = mPlugins.insert(PluginsValue(plugin->getName(), plugin));
            if (!rval.second)
            {
                ret = T3D_ERR_PLG_DUPLICATED;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Duplicated plugin [%s] !",
                    plugin->getName().c_str());
                break;
            }

            // 安装插件
            ret = plugin->install();
            if (ret != T3D_OK)
            {
                mPlugins.erase(plugin->getName());
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Install plugin [%s] failed !",
                    plugin->getName().c_str());
                break;
            }

            // 启动插件
            ret = plugin->startup();
            if (ret != T3D_OK)
            {
                mPlugins.erase(plugin->getName());
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Startup plugin [%s] failed !",
                    plugin->getName().c_str());
                break;
            }
        } while (0);

        return ret;
    }

    TResult Agent::uninstallPlugin(Plugin *plugin)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (plugin == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid plugin !!!");
                break;
            }

            ret = plugin->shutdown();
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Shutdown plugin [%s] failed !",
                    plugin->getName().c_str());
                break;
            }

            ret = plugin->uninstall();
            if (ret != T3D_OK)
            {

                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Uninstall plugin [%s] failed !",
                    plugin->getName().c_str())
                break;
            }

            mPlugins.erase(plugin->getName());
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::loadPlugin(const String &name)
    {
        T3D_LOG_INFO(LOG_TAG_ENGINE, "Load plugin %s ...", name.c_str());

        TResult ret = T3D_OK;

        do 
        {
            auto rval = mDylibs.find(name);
            if (rval != mDylibs.end())
            {
                // 已经加载过了，直接返回吧
                T3D_LOG_INFO(LOG_TAG_ENGINE, "Load plugin [%s] , \
                    but it already loaded !", name.c_str());
                break;
            }
            
            DylibPtr dylib = DylibManager::getInstance().loadDylib(name);
            if (dylib == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            if (dylib->getType() != Resource::E_TYPE_DYLIB)
            {
                ret = T3D_ERR_PLG_NOT_DYLIB;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Load plugin [%s] failed !", 
                    name.c_str());
                break;
            }

            DLL_START_PLUGIN pFunc 
                = (DLL_START_PLUGIN)(dylib->getSymbol("dllStartPlugin"));
            if (pFunc == nullptr)
            {
                ret = T3D_ERR_PLG_NO_FUNCTION;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, 
                    "Load plugin [%s] get function dllStartPlugin failed !", 
                    name.c_str());
                break;
            }

            ret = pFunc();
            if (ret != T3D_OK)
            {
                break;
            }

            mDylibs.insert(DylibsValue(dylib->getName(), dylib));
        } while (0);

        return ret;
    }

    TResult Agent::unloadPlugin(const String &name)
    {
        T3D_LOG_INFO(LOG_TAG_ENGINE, "Unload plugin %s ...", name.c_str());

        TResult ret = T3D_OK;

        do 
        {
            DylibsItr itr = mDylibs.find(name);
            if (itr == mDylibs.end())
            {
                ret = T3D_ERR_PLG_NOT_EXISTS;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Unload plugin [%s] , \
                    it don't exist !", name.c_str());
                break;
            }

            DylibPtr dylib = itr->second;
            DLL_STOP_PLUGIN pFunc 
                = (DLL_STOP_PLUGIN)(dylib->getSymbol("dllStopPlugin"));
            if (pFunc == nullptr)
            {
                ret = T3D_ERR_PLG_NO_FUNCTION;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Unload plugin [%s], \
                    get function dllStopPlugin failed !", name.c_str());
                break;
            }

            ret = pFunc();
            if (ret != T3D_OK)
            {
                break;
            }

            mDylibs.erase(itr);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::addArchiveCreator(ArchiveCreator *creator)
    {
        TResult ret = T3D_OK;
        mArchiveMgr->addArchiveCreator(creator);
        return ret;
    }

    TResult Agent::removeArchiveCreator(ArchiveCreator *creator)
    {
        TResult ret = T3D_OK;
        mArchiveMgr->removeArchiveCreator(creator->getType());
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::addImageCodec(ImageCodecBase::FileType type,
        ImageCodecBasePtr codec)
    {
        return mImageCodec->addImageCodec(type, codec);
    }

    TResult Agent::removeImageCodec(ImageCodecBase::FileType type)
    {
        return mImageCodec->removeImageCodec(type);
    }

    //--------------------------------------------------------------------------

    void Agent::enumerateAvailableRenderers(Renderers &renderers) const
    {
        renderers.clear();
        renderers = mRenderers;
    }

    TResult Agent::setActiveRenderer(RendererPtr renderer)
    {
        TResult ret = T3D_OK;

        if (mActiveRenderer != renderer)
        {
            if (mActiveRenderer != nullptr)
            {
                mActiveRenderer->destroy();
            }

            ret = renderer->init();

            if (ret == T3D_OK)
            {
                mActiveRenderer = renderer;
            }
        }

        return ret;
    }

    RendererPtr Agent::getActiveRenderer() const
    {
        return mActiveRenderer;
    }

    //--------------------------------------------------------------------------

    TResult Agent::addRenderer(RendererPtr renderer)
    {
        const String &name = renderer->getName();
        auto r = mRenderers.insert(RenderersValue(name, renderer));
        if (r.second)
        {
            return T3D_OK;
        }

        return T3D_ERR_DUPLICATED_ITEM;
    }

    TResult Agent::removeRenderer(RendererPtr renderer)
    {
        auto itr = mRenderers.find(renderer->getName());

        if (itr == mRenderers.end())
        {
            return T3D_ERR_NOT_FOUND;
        }

        mRenderers.erase(itr);

        return T3D_OK;
    }

    RendererPtr Agent::getRenderer(const String &name) const
    {
        RendererPtr renderer = nullptr;
        auto itr = mRenderers.find(name);

        if (itr != mRenderers.end())
        {
            renderer = itr->second;
        }

        return renderer;
    }

    //--------------------------------------------------------------------------

    TResult Agent::initApplication()
    {
        TResult ret = T3D_OK;

        do
        {
            Application *theApp = Application::getInstancePtr();
            if (theApp == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            ret = theApp->init();
            if (ret != T3D_OK)
            {
                break;
            }
        } while (0);

        return ret;
    }

    TResult Agent::initLogSystem()
    {
        TResult ret = T3D_ERR_FAIL;

        mLogger = new Logger();

        if (mLogger != nullptr)
        {
            ret = mLogger->startup(1000, "Agent", true, true);
        }

        T3D_LOG_INFO(LOG_TAG_ENGINE, "Start Tiny3D ...... version %s",
            T3D_DEVICE_INFO.getSoftwareVersion().c_str());

        T3D_LOG_INFO(LOG_TAG_ENGINE, "System Information : \n%s",
            T3D_DEVICE_INFO.getSystemInfo().c_str());

        return ret;
    }

    TResult Agent::initEventSystem()
    {
        mEventMgr = new EventManager(10);
        return T3D_OK;
    }

    TResult Agent::initObjectTracer()
    {
        mObjTracer = new ObjectTracer();
        return T3D_OK;
    }

    TResult Agent::initManagers()
    {
        mImageCodec = ImageCodec::create();
        mArchiveMgr = ArchiveManager::create();
        mDylibMgr = DylibManager::create();

        return T3D_OK;
    }

    TResult Agent::loadConfig(const String &cfgPath)
    {
        TResult ret = T3D_OK;

#if defined (T3D_OS_ANDROID)
        // Android，只能读取apk包里面的文件
        ret = loadPlugin("ZipArchive");
        if (ret != T3D_OK)
        {
            return ret;
        }

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

    TResult Agent::loadPlugins()
    {
        TResult ret = T3D_OK;

        do 
        {
            Settings pluginSettings = mSettings["Plugins"].mapValue();
            String s("Path");
            Variant key(s);
            Settings::const_iterator itr = pluginSettings.find(key);
            if (itr == pluginSettings.end())
            {
                ret = T3D_ERR_PLG_NO_PATH;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Load plguins - \
                    The plugin path don't set !");
                break;
            }

#if !defined (T3D_OS_ANDROID)
            mPluginsPath = mAppPath + Dir::NATIVE_SEPARATOR 
                + itr->second.stringValue();
#endif

            key.setString("List");
            itr = pluginSettings.find(key);

            if (itr == pluginSettings.end())
            {
                // 虽然没有获取到任何插件，但是仍然是合法的，正常返回
                ret = T3D_OK;
                break;
            }

            const VariantArray &plugins = itr->second.arrayValue();
            VariantArrayConstItr i = plugins.begin();

            while (i != plugins.end())
            {
                String name = i->stringValue();
                ret = loadPlugin(name);
                if (ret != T3D_OK)
                {
                    break;
                }

                ++i;
            }
        } while (0);

        return ret;
    }

    TResult Agent::unloadPlugins()
    {
        TResult ret = T3D_OK;

        DylibsItr itr = mDylibs.begin();
        while (itr != mDylibs.end())
        {
            DylibPtr dylib = itr->second;
            DLL_STOP_PLUGIN pFunc 
                = (DLL_STOP_PLUGIN)(dylib->getSymbol("dllStopPlugin"));
            if (pFunc != nullptr)
            {
                ret = pFunc();
                if (ret == T3D_OK)
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

    TResult Agent::initAssets()
    {
        TResult ret = T3D_OK;

        do 
        {
            Settings &assets = mSettings["Assets"].mapValue();

            // Assets 资源路径
            String str("Path");
            Variant key(str);
            Settings::const_iterator itr = assets.find(key);
            if (itr == assets.end())
            {
                ret = T3D_ERR_SETTINGS_NOT_FOUND;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Could not find the assets path \
                    settings item !");
                break;
            }

            // 所有资源文件
            const VariantMap &pathes = itr->second.mapValue();
            auto i = pathes.begin();
            while (i != pathes.end())
            {
                const String &path = i->first.stringValue();
                const String &type = i->second.stringValue();
#if defined (T3D_OS_ANDROID)
                String fullpath = getAppPath() + Dir::NATIVE_SEPARATOR + path;
#else
                const String &fullpath = path;
#endif
                ArchivePtr archive = mArchiveMgr->loadArchive(fullpath, type);
                if (archive == nullptr)
                {
                    ret = T3D_ERR_RES_LOAD_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_ENGINE, "Load archive failed ! \
                        Path :%s", path.c_str());
                    break;
                }
                ++i;
            }

            if (ret != T3D_OK)
            {
                break;
            }


        } while (0);

        return ret;
    }

    TResult Agent::initRenderer(const String &rendererName)
    {
        TResult ret = T3D_OK;

        do 
        {
            // 设置当前要使用的渲染器
            RendererPtr renderer = getRenderer(rendererName);
            if (renderer == nullptr)
            {
                ret = T3D_ERR_PLG_NOT_LOADED;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Renderer [%s] did not load !",
                    rendererName.c_str());
                break;
            }

            setActiveRenderer(renderer);
        } while (0);

        return ret;
    }
}