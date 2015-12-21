

#include "Misc/T3DEntrance.h"
#include "Misc/T3DPlugin.h"
#include "Misc/T3DConfigFile.h"
#include "Render/T3DRenderer.h"
#include "Render/T3DRenderWindow.h"
#include "Resource/T3DDylibManager.h"
#include "Resource/T3DDylib.h"
#include "T3DPlatform.h"
#include "T3DLogger.h"
#include <algorithm>


namespace Tiny3D
{
    typedef void (*DLL_START_PLUGIN)(void);
    typedef void (*DLL_STOP_PLUGIN)(void);

    T3D_INIT_SINGLETON(Entrance);

    Entrance::Entrance(const String &config /* = "Tiny3D.cfg" */, 
        const String &log /* = "Tiny3D.log" */)
        : mSystem(new System())
        , mDylibMgr(new DylibManager())
        , mActiveRenderer(nullptr)
        , mAppListener(nullptr)
        , mSceneMgr(nullptr)
    {
        ConfigFile file(config);
        file.loadXML(mSettings);

        loadPlugins();

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
        T3D_SAFE_DELETE(mDylibMgr);
        T3D_SAFE_DELETE(mSystem);
    }

    void Entrance::installPlugin(Plugin *plugin)
    {
        mPluginList.push_back(plugin);

        plugin->install();
        plugin->startup();
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
        bool ret = false;
        Dylib *lib = (Dylib*)DylibManager::getInstance().load(name);

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
            Dylib *lib = *itr;

            if (lib->getName() == name)
            {
                DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)lib->getSymbol("dllStopPlugin");
                pFunc();
                DylibManager::getInstance().unload(lib);
                break;
            }

            ++itr;
        }
    }

    void Entrance::loadPlugins()
    {
        String s("PluginsPath");
        Variant key(s);
        Settings::const_iterator itr = mSettings.find(key);
        if (itr == mSettings.end())
            return;

        String path = itr->second.stringValue();
        
        key.setString("Plugins");
        itr = mSettings.find(key);

        if (itr != mSettings.end())
        {
            const VariantArray &plugins = itr->second.arrayValue();
            VariantArrayConstItr i = plugins.begin();

            while (i != plugins.end())
            {
                String name = i->stringValue();
                String fullpath = path + "/" + name;
                loadPlugin(name);
                ++i;
            }
        }
    }

    void Entrance::unloadPlugins()
    {

    }

    bool Entrance::initialize(bool autoCreateWindow, RenderWindow *&renderWindow)
    {
        if (mActiveRenderer == nullptr)
            return false;

        mSceneMgr = new SceneManager();

        bool ret = mActiveRenderer->initialize();
        
        if (autoCreateWindow)
        {
            RenderWindowCreateParam param;
            param._windowLeft = mSettings["x"].int32Value();
            param._windowTop = mSettings["y"].int32Value();
            param._windowWidth = mSettings["Width"].int32Value();
            param._windowHeight = mSettings["Height"].int32Value();
            param._fullscreen = mSettings["FullScreen"].boolValue();
            param._colorDepth = mSettings["ColorDepth"].int32Value();
            param._windowTitle = mSettings["Title"].stringValue();
            RenderWindowCreateParamEx paramEx;
            paramEx["renderer"].setLong((long_t)mActiveRenderer);
            paramEx["MultiSampleQuality"] = mSettings["MultiSampleQuality"];
            paramEx["vsync"] = mSettings["VSync"];
            renderWindow = mActiveRenderer->createRenderWindow(param, paramEx);
        }

        return ret;
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
        if (mActiveRenderer != nullptr)
        {
            mActiveRenderer->startRendering();
        }

        return true;
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
