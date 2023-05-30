/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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

#ifndef __T3D_AGENT_H__
#define __T3D_AGENT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DSettings.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Agent : public Singleton<Agent>
    {
    public:
        Agent();

        virtual ~Agent();

        /**
         * \brief 获取版本号
         * \return 返回整数型版本号
         */
        uint32_t getVersion() const;

        const char *getVersionString() const;

        const char *getVersionName() const;

        TResult init(const String &appPath, bool autoCreateWindow,
                     const String &config = "Tiny3D.cfg");

        TResult createDefaultRenderWindow(RHIRenderWindowPtr &window);

        TResult addRenderWindow(RHIRenderWindowPtr window);

        TResult removeRenderWindow(const String &name);

        RHIRenderWindowPtr getRenderWindow(const String &name) const;

        // RenderWindowPtr getDefaultRenderWindow() const 
        // { 
        //     return mDefaultWindow; 
        // }

        bool run();

        void renderOneFrame();

        void appDidEnterBackground();

        void appWillEnterForeground();
     
        const String &getPluginsPath() const { return mPluginsPath; }

        TResult installPlugin(Plugin *plugin);

        TResult uninstallPlugin(Plugin *plugin);

        TResult loadPlugin(const String &name);

        TResult unloadPlugin(const String &name);

        void enumerateAvailableRenderers(RHIRenderers &renderers) const;

        TResult setActiveRHIRenderer(RHIRendererPtr renderer);

        RHIRendererPtr getActiveRHIRenderer() const;
     
        /**
         * @fn  TResult Agent::addRenderer(RenderContextPtr renderer);
         * @brief   添加渲染器
         * @param [in]  renderer    : 要添加的渲染器对象.
         * @return  成功返回 T3D_OK.
         */
        TResult addRHIRenderer(RHIRendererPtr renderer);

        /**
         * @fn  TResult Agent::removeRenderer(RenderContextPtr renderer);
         * @brief   移除渲染器
         * @param [in]  renderer    : 要移除的渲染器对象.
         * @return  成功返回 T3D_OK.
         */
        TResult removeRHIRenderer(RHIRendererPtr renderer);

        /**
         * @fn  RenderContextPtr Agent::getRenderer(const String &name) const;
         * @brief   获取渲染器对象
         * @param [in]  name    : 渲染器名称.
         * @return  成功返回渲染器对象.
         */
        RHIRendererPtr getRHIRenderer(const String &name) const;

        ArchivePtr getInternalArchive() const { return mInternalArchive; }

        ArchivePtr getProjectArchive() const { return mProjectArchive; }
        
    protected:
        TResult initApplication();

        TResult initLogSystem();

        TResult initEventSystem();

        TResult initObjectTracer();

        TResult initManagers();

        TResult loadConfig(const String &cfgPath);

        TResult loadPlugins();

        TResult unloadPlugins();

        TResult initAssets();

        TResult initRenderer();

        TResult initSceneManager();
     
    protected:
        typedef TMap<String, Plugin*>       Plugins;
        typedef Plugins::iterator           PluginsItr;
        typedef Plugins::const_iterator     PluginsConstItr;
        typedef Plugins::value_type         PluginsValue;

        typedef TMap<String, DylibPtr>      Dylibs;
        typedef Dylibs::iterator            DylibsItr;
        typedef Dylibs::const_iterator      DylibsConstItr;
        typedef Dylibs::value_type          DylibsValue;

        Logger                  *mLogger;           /**< 日志对象 */
        EventManager            *mEventMgr;         /**< 事件管理器对象 */
        ObjectTracer            *mObjTracer;        /**< 对象内存跟踪 */

        RHIRenderWindowPtr      mDefaultWindow;     /**< 默认渲染窗口 */
        RHIRendererPtr          mActiveRHIRenderer; /**< 当前使用的 RHI 渲染器 */

        ArchiveManagerPtr       mArchiveMgr;
        SerializerManagerPtr    mSerializerMgr;
        DylibManagerPtr         mDylibMgr;
        SerializableManagerPtr  mSerializableMgr;

        ArchivePtr              mInternalArchive;
        ArchivePtr              mProjectArchive;
        
        Plugins                 mPlugins;           /**< 安装的插件列表 */
        Dylibs                  mDylibs;            /**< 加载的动态库列表 */
        RHIRenderers            mRenderers;         /**< 渲染器列表 */
     
        String                  mAppPath;           /**< 程序路径 */
        String                  mAppName;           /**< 程序名称 */
        String                  mPluginsPath = ".";

        Settings                mSettings;          /**< 引擎配置项 */

        bool                    mIsRunning = false;         /**< 引擎是否在运行中 */
    };

    #define T3D_AGENT   Agent::getInstance()
}


#endif    /*__T3D_AGENT_H__*/
