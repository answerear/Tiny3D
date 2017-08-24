/***************************************************************************************************
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
 **************************************************************************************************/

#include "Misc/T3DEntrance.h"
#include "Misc/T3DMemoryTracer.h"
#include "Misc/T3DPlugin.h"
#include "Misc/T3DConfigFile.h"
#include "Misc/T3DWindowEventHandler.h"
#include "Misc/T3DString.h"
#include "SceneGraph/T3DSGTransform2D.h"
#include "SceneGraph/T3DSGText2D.h"
#include "Render/T3DRenderer.h"
#include "Render/T3DRenderWindow.h"
#include "Resource/T3DDylibManager.h"
#include "Resource/T3DArchiveManager.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DModelManager.h"
#include "Resource/T3DTextureManager.h"
#include "Resource/T3DFontManager.h"
#include "Resource/T3DDylib.h"
#include "ImageCodec/T3DImageCodec.h"
#include "Resource/T3DFileSystemArchive.h"
#include "Listener/T3DApplicationListener.h"
#include <algorithm>


namespace Tiny3D
{
    typedef void (*DLL_START_PLUGIN)(void);
    typedef void (*DLL_STOP_PLUGIN)(void);

    T3D_INIT_SINGLETON(Entrance);

    Entrance::Entrance(const String &appPath, bool isMemoryTracing /* = false */, const String &config /* = "Tiny3D.cfg" */)
        : mSystem(new System())
        , mLogger(new Logger())
        , mMemoryTracer(new MemoryTracer(isMemoryTracing))
        , mWindowEventHandler(new WindowEventHandler())
        , mDylibMgr(new DylibManager())
        , mArchiveMgr(new ArchiveManager())
        , mMaterialMgr(new MaterialManager())
        , mModelMgr(new ModelManager())
        , mTextureMgr(new TextureManager())
        , mFontMgr(new FontManager())
        , mActiveRenderer(nullptr)
        , mAppListener(nullptr)
        , mSceneMgr(nullptr)
        , mImageCodec(new ImageCodec())
    {
        StringUtil::split(appPath, mAppPath, mAppName);

        ConfigFile file(mAppPath + config);
        file.loadXML(mSettings);
        startLogging();

        initArchives();
        initResources();
        loadPlugins();

        mImageCodec->startup();

        RendererListItr itr = mRendererList.begin();
        while (itr != mRendererList.end())
        {
            if ((*itr)->getName() == Renderer::DIRECT3D9)
            {
                setActiveRenderer(*itr);
                break;
            }
            ++itr;
        }
    }

    Entrance::~Entrance()
    {
        mFPSText = nullptr;
        mSPFText = nullptr;
        mDrawText = nullptr;

        mActiveRenderer = nullptr;
        unloadPlugins();

        mImageCodec->shutdown();

        T3D_SAFE_DELETE(mSceneMgr);

        T3D_SAFE_DELETE(mFontMgr);
        T3D_SAFE_DELETE(mModelMgr);
        T3D_SAFE_DELETE(mMaterialMgr);
        T3D_SAFE_DELETE(mTextureMgr);
        T3D_SAFE_DELETE(mImageCodec);
        T3D_SAFE_DELETE(mArchiveMgr);
        T3D_SAFE_DELETE(mDylibMgr);
        T3D_SAFE_DELETE(mWindowEventHandler);

        mMemoryTracer->dumpMemoryInfo();
        T3D_SAFE_DELETE(mMemoryTracer);

        stopLogging();
        T3D_SAFE_DELETE(mLogger);
        T3D_SAFE_DELETE(mSystem);
    }

    void Entrance::installPlugin(Plugin *plugin)
    {
        mPluginList.push_back(plugin);

        plugin->install();
        plugin->startup();

        T3D_LOG_TRACE(Logger::E_LEVEL_INFO, "install plugin %s completed !", plugin->getName().c_str());
    }

    void Entrance::uninstallPlugin(Plugin *plugin)
    {
        PluginListItr itr = 
            std::find(mPluginList.begin(), mPluginList.end(), plugin);

        if (itr != mPluginList.end())
        {
            plugin->shutdown();
            plugin->uninstall();
            mPluginList.erase(itr);
        }
    }

    bool Entrance::loadPlugin(const String &name)
    {
        T3D_LOG_TRACE(Logger::E_LEVEL_INFO, "load plugin %s ...", name.c_str());
        bool ret = false;
        DylibPtr lib = DylibManager::getInstance().loadDylib(name);

        if (lib->getType() == Resource::E_TYPE_DYLIB)
        {
            mDylibList.push_back(lib);

            DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)lib->getSymbol("dllStartPlugin");
            if (pFunc != nullptr)
            {
                pFunc();
            }
        }

        return ret;
    }

    void Entrance::unloadPlugin(const String &name)
    {
        DylibListItr itr = mDylibList.begin();

        while (itr != mDylibList.end())
        {
            DylibPtr &lib = *itr;

            if (lib->getName() == name)
            {
                DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)lib->getSymbol("dllStopPlugin");
                pFunc();
                DylibManager::getInstance().unloadDylib(lib);
                break;
            }

            ++itr;
        }
    }

    void Entrance::loadPlugins()
    {
        Settings pluginSettings = mSettings["Plugins"].mapValue();
        String s("Path");
        Variant key(s);
        Settings::const_iterator itr = pluginSettings.find(key);
        if (itr == pluginSettings.end())
            return;

        mPluginsPath = itr->second.stringValue();
        
        key.setString("List");
        itr = pluginSettings.find(key);

        if (itr != pluginSettings.end())
        {
            const VariantArray &plugins = itr->second.arrayValue();
            VariantArrayConstItr i = plugins.begin();

            while (i != plugins.end())
            {
                String name = i->stringValue();
                bool ret = loadPlugin(name);
                T3D_LOG_INFO("Load plugin %s result %d", name.c_str(), ret);
                ++i;
            }
        }
    }

    void Entrance::unloadPlugins()
    {
        DylibListItr itr = mDylibList.begin();

        while (itr != mDylibList.end())
        {
            DylibPtr &lib = *itr;
            DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)lib->getSymbol("dllStopPlugin");
            pFunc();
            DylibManager::getInstance().unloadDylib(lib);
            ++itr;
        }
    }

    void Entrance::startLogging()
    {
        Settings &logSettings = mSettings["Log"].mapValue();
        Logger::Level level = T3D_LOG_TO_LEVEL_VALUE(logSettings["Level"].stringValue());
        T3D_LOG_SET_LEVEL(level);
        T3D_LOG_SET_EXPIRED(logSettings["Expired"].uint32Value());
        T3D_LOG_SET_MAX_CACHE_SIZE(logSettings["MaxCacheSize"].uint32Value());
        T3D_LOG_SET_MAX_CACHE_TIME(logSettings["MaxCacheTime"].uint32Value());
        T3D_LOG_STARTUP(logSettings["AppID"].uint32Value(), logSettings["Tag"].stringValue(), false, true);

        T3D_LOG_TRACE(Logger::E_LEVEL_INFO, "**************************** Tiny3D started *************************");
    }

    void Entrance::stopLogging()
    {
        T3D_LOG_TRACE(Logger::E_LEVEL_INFO, "---------------------------- Tiny3D shutdown ------------------------");
        T3D_LOG_SHUTDOWN();
    }

    void Entrance::initArchives()
    {
        FileSystemArchiveCreator *creator = new FileSystemArchiveCreator();
        mArchiveMgr->addArchiveCreator(creator);
    }

    void Entrance::initResources()
    {
        Settings &resSettings = mSettings["Resources"].mapValue();

        String s("Path");
        Variant key(s);
        Settings::const_iterator itr = resSettings.find(key);
        if (itr == resSettings.end())
            return;

        const VariantMap &pathes = itr->second.mapValue();
        auto i = pathes.begin();
        while (i != pathes.end())
        {
            const String &path = i->first.stringValue();
            const String &type = i->second.stringValue();
            mArchiveMgr->loadArchive(path, type);
            ++i;
        }

        // 设置默认字体
        itr = resSettings.find("DefaultFont");
        if (itr != resSettings.end())
        {
            const String &fontName = itr->second.stringValue();
            T3D_FONT_MGR.setDefaultFontName(fontName);
        }
    }

    void Entrance::initStatistics()
    {
        SGNodePtr root = mSceneMgr->getRoot();

        RenderTarget *renderTarget = mActiveRenderer->getRenderTarget("MainWindow");

        mFrameRate = Real(0.0);
        mFrames = 0;
        mLastTime = DateTime::currentMSecsSinceEpoch();
        mDeltaTime = 0;
        mBatchCounter = 0;
        
        Real x = 0;
        Real y = 0;
        Real width = renderTarget->getWidth();
        Real height = renderTarget->getHeight();
        Real halfW = width * Real(0.5);
        Real halfH = height * Real(0.5);
        Real rightMargin = 10;
        Real lineSpace = 10;

        size_t fontSize = 40;

        // 显示FPS
        SGTransform2DPtr node = SGTransform2D::create();
        root->addChild(node);
        mFPSText = SGText2D::create("00.000", fontSize);
        node->addChild(mFPSText);
        mFPSText->setAnchorPos(Vector2(1.0, 1.0));
        Size textSize = mFPSText->getSize();
        x = halfW - rightMargin;
        y = -halfH + textSize.height + lineSpace;
        node->setPosition(x, y);

        // 显示SPF
        node = SGTransform2D::create();
        root->addChild(node);
        mSPFText = SGText2D::create("0.000", fontSize);
        node->addChild(mSPFText);
        mSPFText->setAnchorPos(Vector2(1.0, 1.0));
        textSize = mSPFText->getSize();
        x = halfW - rightMargin;
        y = y + textSize.height + lineSpace;
        node->setPosition(x, y);

        // 显示draw call
        node = SGTransform2D::create();
        root->addChild(node);
        mDrawText = SGText2D::create("000", fontSize);
        node->addChild(mDrawText);
        mDrawText->setAnchorPos(Vector2(1.0, 1.0));
        textSize = mDrawText->getSize();
        x = halfW - rightMargin;
        y = y + textSize.height + lineSpace;
        node->setPosition(x, y);
    }

    void Entrance::calculatePerformance()
    {
        uint64_t current = DateTime::currentMSecsSinceEpoch();
        mFrames++;
        uint64_t dt = current - mLastTime;
        mDeltaTime += dt;

        char szText[32];

        if (mDeltaTime >= 1000)
        {
            mFrameRate = Real(mFrames) / (Real(mDeltaTime) / Real(1000.0));
            snprintf(szText, sizeof(szText), "%.3f", mFrameRate);
            mFPSText->setText(szText);

            Real spf = Real(1.0) / mFrameRate;
            snprintf(szText, sizeof(szText), "%.3f", spf);
            mSPFText->setText(szText);

            mDeltaTime = 0;
            mFrames = 0;
        }

        snprintf(szText, sizeof(szText), "%4lu", mBatchCounter);
        mDrawText->setText(szText);

        mBatchCounter = 0;
        mLastTime = DateTime::currentMSecsSinceEpoch();
    }

    bool Entrance::initialize(bool autoCreateWindow, RenderWindow *&renderWindow)
    {
        if (mActiveRenderer == nullptr)
            return false;

        mSceneMgr = new SceneManager();
        mSceneMgr->setRenderer(mActiveRenderer);

        if (autoCreateWindow)
        {
            RenderWindowCreateParam param;
            Settings renderSettings = mSettings["Render"].mapValue();
            param._windowLeft = renderSettings["x"].int32Value();
            param._windowTop = renderSettings["y"].int32Value();
            param._windowWidth = renderSettings["Width"].int32Value();
            param._windowHeight = renderSettings["Height"].int32Value();
            param._fullscreen = renderSettings["FullScreen"].boolValue();
            param._colorDepth = renderSettings["ColorDepth"].int32Value();
            param._windowTitle = renderSettings["Title"].stringValue();
            RenderWindowCreateParamEx paramEx;
            paramEx["renderer"].setLong((long_t)mActiveRenderer);
            paramEx["MultiSampleQuality"] = renderSettings["MultiSampleQuality"];
            paramEx["vsync"] = renderSettings["VSync"];
            renderWindow = mActiveRenderer->createRenderWindow(param, paramEx);
        }

        initStatistics();

        return (renderWindow != nullptr);
    }

    void Entrance::enumerateAvailableRenderers(RendererList &rRendererList) const
    {
        rRendererList.clear();

        rRendererList = mRendererList;
    }

    void Entrance::setActiveRenderer(Renderer *renderer)
    {
        if (mActiveRenderer != renderer)
        {
            if (mActiveRenderer != nullptr)
                mActiveRenderer->uninitialize();
            renderer->initialize();
        }

        mActiveRenderer = renderer;
    }

    Renderer *Entrance::getActiveRenderer() const
    {
        return mActiveRenderer;
    }

    Renderer *Entrance::getRenderer(const String &name) const
    {
        Renderer *renderer = nullptr;
        RendererListConstItr itr = mRendererList.begin();
        while (itr != mRendererList.end())
        {
            if ((*itr)->getName() == name)
            {
                renderer = *itr;
                break;
            }
            ++itr;
        }

        return renderer;
    }

    void Entrance::addRenderer(Renderer *renderer)
    {
        mRendererList.push_back(renderer);
    }

    void Entrance::removeRenderer(Renderer *renderer)
    {
        for (auto itr = mRendererList.begin(); itr != mRendererList.end(); ++itr)
        {
            if (*itr == renderer)
            {
                mRendererList.erase(itr);
                break;
            }
        }
    }

    RenderWindow *Entrance::createRenderWindow(
        const RenderWindowCreateParam &rkCreatedParam, 
        const RenderWindowCreateParamEx &rkCreatedParamEx
        )
    {
        RenderWindow *window = nullptr;

        if (mActiveRenderer != nullptr)
        {
            window = mActiveRenderer->createRenderWindow(rkCreatedParam, 
                rkCreatedParamEx);
        }

        return window;
    }

    bool Entrance::run()
    {
        if (mAppListener != nullptr)
        {
            mAppListener->applicationDidFinishLaunching();
        }

        mShutdown = false;

        while (!mShutdown)
        {
            WindowEventHandler::getInstance().handleWindowMessage();
            if (!renderOneFrame())
                break;
        }

        return true;
    }

    bool Entrance::renderOneFrame()
    {
        bool ret = false;

        if (mActiveRenderer != nullptr)
        {
            ret = mActiveRenderer->renderOneFrame();
        }

        calculatePerformance();

        return ret;
    }

    void Entrance::shutdown()
    {
        mShutdown = true;
    }

    void Entrance::setApplicationListener(ApplicationListener *listener)
    {
        mAppListener = listener;
    }

    ApplicationListener *Entrance::getApplicationListener()
    {
        return mAppListener;
    }

    void Entrance::addFrameListener(FrameListener *listener)
    {
        T3D_ASSERT(mActiveRenderer != nullptr);

        mActiveRenderer->addFrameListener(listener);
    }

    void Entrance::removeFrameListener(FrameListener *listener)
    {
        T3D_ASSERT(mActiveRenderer != nullptr);
        mActiveRenderer->removeFrameListener(listener);
    }
}
