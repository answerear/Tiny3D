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

#ifndef __T3D_BUILDER_H__
#define __T3D_BUILDER_H__


#include "T3DPrerequisites.h"
#include "T3DSingleton.h"
#include "T3DCommon.h"
#include "Misc/T3DVariant.h"
#include "SceneGraph/T3DSceneManager.h"


namespace Tiny3D
{
    class ImageCodec;

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
        /** 
         * @brief Ĭ�Ϲ��캯��
         * @param [in] appPath : ����ĳ���·����������һ��·�����Ա�·����Ϊ����
         * @param [in] isMemoryTracing : �Ƿ��������ڴ�й©���ܣ��ù��ܻ��ڳ����˳�ʱ���ӡ��й©�Ķ���
         * @param [in] config : ���������ļ�
         */
        Entrance(const String &appPath, bool isMemoryTracing = false, const String &config = "Tiny3D.cfg");

        /** Destructor. */
        virtual ~Entrance();

        Settings &getConfig()
        {
            return mSettings;
        }

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

        void removeRenderer(Renderer *renderer);

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

        bool renderOneFrame();

        void shutdown();

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

        SceneManager *getSceneManager() { return mSceneMgr; }

        /**
         * @brief ����Ӧ�ó���·��
         */
        const String &getAppPath() const { return mAppPath; }

        /**
         * @brief ����Ӧ�ó�������
         */
        const String &getAppName() const { return mAppName; }

        /**
         * @brief ���ز��·��
         */
        const String &getPluginsPath() const { return mPluginsPath; }

    protected:
        void loadPlugins();
        void unloadPlugins();

        void startLogging();
        void stopLogging();

        void initArchives();
        void initResources();

    protected:
        typedef std::list<Plugin*>          PluginList;
        typedef PluginList::iterator        PluginListItr;
        typedef PluginList::const_iterator  PluginListConstItr;

        typedef std::list<DylibPtr>         DylibList;
        typedef DylibList::iterator         DylibListItr;
        typedef DylibList::const_iterator   DylibListConstItr;

        System                  *mSystem;
        Logger                  *mLogger;
        MemoryTracer            *mMemoryTracer;
        WindowEventHandler      *mWindowEventHandler;

        DylibManager            *mDylibMgr;
        ArchiveManager          *mArchiveMgr;
        MaterialManager         *mMaterialMgr;
        ModelManager            *mModelMgr;
        TextureManager          *mTextureMgr;
        FontManager             *mFontMgr;

        Renderer        *mActiveRenderer;

        PluginList      mPluginList;
        RendererList    mRendererList;

        DylibList       mDylibList;

        Settings        mSettings;

        String          mAppPath;
        String          mAppName;
        String          mPluginsPath;

        ApplicationListener *mAppListener;

        SceneManager    *mSceneMgr;

        ImageCodec      *mImageCodec;

        bool            mShutdown;
    };

    #define T3D_ENTRANCE         Entrance::getInstance()
    #define T3D_ENTRANCE_PTR     Entrance::getInstancePtr()
}


#endif  /*__T3D_BUILDER_H__*/

