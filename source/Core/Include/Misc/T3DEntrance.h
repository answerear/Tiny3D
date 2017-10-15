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
     * @class Entrance
     * @brief Tiny3D入口类
     */
    class T3D_ENGINE_API Entrance : public Singleton<Entrance>
    {
    public:
        /** 
         * @brief 默认构造函数
         * @param [in] appPath : 传入的程序路径，引擎中一切路径均以本路径作为参照
         * @param [in] isMemoryTracing : 是否开启跟踪内存泄漏功能，该功能会在程序退出时候打印出泄漏的对象
         * @param [in] config : 引擎配置文件
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

        bool initialize(bool autoCreateWindow, RenderWindow *&renderWindow, bool showStat = false);

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
         * @brief 设置窗口事件处理对象
         */
        void setWindowEventHandler(WindowEventHandler *handler);

        /**
         * @brief 获取窗口事件处理对象
         */
        WindowEventHandler *getWindowEventHandler();

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
         * @brief 返回应用程序路径
         */
        const String &getAppPath() const { return mAppPath; }

        /**
         * @brief 返回应用程序名称
         */
        const String &getAppName() const { return mAppName; }

        /**
         * @brief 返回插件路径
         */
        const String &getPluginsPath() const { return mPluginsPath; }

        void addBatchCounter() { mBatchCounter++; }

    protected:
        /**
         * @brief 根据配置文件自动加载所有插件
         */
        void loadPlugins();

        /**
         * @brief 卸载所有插件
         */
        void unloadPlugins();

        /**
         * @brief 启动日志系统，可以开始打印日志
         */
        void startLogging();

        /**
         * @brief 关闭日志系统，在之后无法打印日志
         */
        void stopLogging();

        /**
         * @brief 创建和初始化所有档案结构对象
         */
        void initArchives();

        /**
         * @brief 初始化配置文件里面指定的资源
         */
        void initResources();

        /**
         * @brief 初始化统计系统
         */
        void initStatistics();

        /**
         * @brief 计算性能指标数据
         */
        void calculatePerformance();

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

        DylibManager            *mDylibMgr;
        ArchiveManager          *mArchiveMgr;
        MaterialManager         *mMaterialMgr;
        ModelManager            *mModelMgr;
        TextureManager          *mTextureMgr;
        FontManager             *mFontMgr;

        Renderer                *mActiveRenderer;
        WindowEventHandler      *mWindowEventHandler;

        PluginList      mPluginList;
        RendererList    mRendererList;

        DylibList       mDylibList;

        Settings        mSettings;

        String          mAppPath;
        String          mAppName;
        String          mPluginsPath;

        ApplicationListener *mAppListener;

        SceneManager    *mSceneMgr;

        SGText2DPtr     mFPSText;           /// 用于显示FPS信息
        SGText2DPtr     mSPFText;           /// 用于显示SPF信息
        SGText2DPtr     mDrawText;          /// 用于显示draw call信息

        ImageCodec      *mImageCodec;

        Real            mFrameRate;         /// 帧率
        uint64_t        mFrames;            /// 总帧数
        uint64_t        mDeltaTime;         /// 帧间隔时间
        uint64_t        mLastTime;          /// 记录上一帧时刻
        uint32_t        mBatchCounter;      /// 批次技术

        bool            mShutdown;
    };

    #define T3D_ENTRANCE         Entrance::getInstance()
    #define T3D_ENTRANCE_PTR     Entrance::getInstancePtr()
}


#endif  /*__T3D_BUILDER_H__*/

