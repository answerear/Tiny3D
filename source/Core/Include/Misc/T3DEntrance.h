

#ifndef __T3D_BUILDER_H__
#define __T3D_BUILDER_H__


#include "T3DPrerequisites.h"
#include "T3DSingleton.h"
#include "T3DCommon.h"
#include "DataStruct/T3DVariant.h"


namespace Tiny3D
{
    /**
     * @brief The starting point class of the Tiny3D system.
     * @remarks
     *      The Tiny3D::Builder class represents a starting point for
     *      the application. From here, the application can gain access to
     *      fundamentals of the system, listing the renderer available,
     *      management of saved configurations. An instance of Builder must be
     *      created before any other Tiny3D operations are called.
     *      Once an instance has been created, the same instance is accessible
     *      throughout the life of that object by using Builder::getInstance()
     *      (as a reference) or Builder::getInstancePtr() (as a pointer).
     *      And then you should call Builder::run() for starting Tiny3D.
     */
    class T3D_ENGINE_API Entrance : public Singleton<Entrance>
    {
    public:
        /** Default constructor. */
        Entrance(const String &config = "Tiny3D.cfg", 
            const String &log = "Tiny3D.log");

        /** Destructor. */
        virtual ~Entrance();

        void installPlugin(Plugin *plugin);
        void uninstallPlugin(Plugin *plugin);

        bool loadPlugin(const String &name);
        void unloadPlugin(const String &name);

        bool initialize(bool autoCreateWindow, RenderWindow *&renderWindow);

        /**
         * @brief Enumerate all available renderer in list.
         * @param [out] rRendererList
         *      list of all available renderer instances.
         * @return void
         */
        void enumerateAvailableRenderers(RendererList &rRendererList) const;

        /**
         * @brief Set active renderer.
         * @param [in] renderer
         *      pointer to an available renderer instance.
         * @return void
         * @remarks
         *      The renderer must be one of all available renderer. You should
         *      call Builder::enumerateAvailableRenderers() to retrieve all 
         *      available renderer in list.
         */
        void setActiveRenderer(Renderer *renderer);

        Renderer *getActiveRenderer() const;

        Renderer *getRenderer(const String &name) const;

        void addRenderer(Renderer *renderer);

        /**
         * @brief Create render window.
         * @param [in] rkCreatedParam 
         *      reference to RenderWindowCreateParam instance. 
         *      See RenderWindowCreateParam.
         * @param [in] rkCreatedParamEx : 
         *      reference to RenderWindowCreateParamEx instance. 
         *      See RenderWindowCreateParamEx.
         * @return pointer to an instance for RenderWindow if create succeeded.
         * @see RenderWindowCreatedParam
         * @see RenderWindowCreatedParamEx
         */
        RenderWindow *createRenderWindow(
            const RenderWindowCreateParam &rkCreatedParam,
            const RenderWindowCreateParamEx &rkCreatedParamEx);

        /**
         * @brief Run Tiny3D
         * @param void
         * @return void
         */
        bool run();

        void renderOneFrame();

        /**
         * @brief Set application event listener.
         * @param [in] pListener
         *      pointer to application listener instance.
         * @return void
         */
        void setApplicationListener(ApplicationListener *listener);

        ApplicationListener *getApplicationListener();

        void addFrameListener(FrameListener *listener);
        void removeFrameListener(FrameListener *listener);

    protected:
        void loadPlugins();
        void unloadPlugins();

    protected:
        typedef std::list<Plugin*>          PluginList;
        typedef PluginList::iterator        PluginListItr;
        typedef PluginList::const_iterator  PluginListConstItr;

        typedef std::list<Dylib*>           DylibList;
        typedef DylibList::iterator         DylibListItr;
        typedef DylibList::const_iterator   DylibListConstItr;

        System          *mSystem;

        DylibManager    *mDylibMgr;

        Renderer        *mActiveRenderer;

        PluginList      mPluginList;
        RendererList    mRendererList;

        DylibList       mDylibList;

        Settings        mSettings;

        ApplicationListener *mAppListener;
    };

    #define T3D_ENTRANCE         Entrance::getInstance()
    #define T3D_ENTRANCE_PTR     Entrance::getInstancePtr()
}


#endif  /*__T3D_BUILDER_H__*/

