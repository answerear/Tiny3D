

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
    {
        ConfigFile file(config);
        file.loadXML(mSettings);

        loadPlugins();
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

    void Entrance::enumerateAvailableRenderers(RendererList &rRendererList) const
    {
        rRendererList.clear();

        rRendererList = mRendererList;
    }

    void Entrance::setActiveRenderer(Renderer *renderer)
    {
        if (mActiveRenderer != renderer)
        {
            
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
        return nullptr;
    }

    bool Entrance::run()
    {
        if (mActiveRenderer != nullptr)
        {
            mActiveRenderer->startRendering();
        }

        return true;
    }

    void Entrance::setApplicationListener(ApplicationListener *pListener)
    {

    }
}
