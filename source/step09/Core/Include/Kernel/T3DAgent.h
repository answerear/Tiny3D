/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_ENGINE_H__
#define __T3D_ENGINE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "ImageCodec/T3DImageCodec.h"


namespace Tiny3D
{
    /**
     * @brief 引擎入口类
     */
    class T3D_ENGINE_API Agent : public Singleton<Agent>
    {
        T3D_DISABLE_COPY(Agent);

    public:
        /**
         * @brief 构造函数
         */
        Agent();

        /**
         * @brief 析构函数
         */
        virtual ~Agent();

        /**
         * @brief 初始化引擎
         * @param [in] appPath : 应用程序路径
         * @param [in] autoCreateWindow : 是否创建渲染窗口
         * @param [in] config : 配置文件
         * @remarks 引擎的一切应用都要在调用本接口之后才有效。
         */
        TResult init(const String &appPath, bool autoCreateWindow,
            const String &config = "Tiny3D.cfg");

        /**
         * @brief 创建渲染窗口
         */
        TResult createDefaultRenderWindow(RenderWindowPtr &window);

        /**
         * @brief 添加渲染窗口到引擎里面
         * @param [in] window : 渲染窗口对象
         * @return 成功返回 T3D_OK
         */
        TResult addRenderWindow(RenderWindowPtr window);

        /**
         * @brief 从引擎移除渲染窗口
         * @param [in] name : 渲染窗口名称
         * @return 成功返回 T3D_OK
         */
        TResult removeRenderWindow(const String &name);

        /**
         * @brief 获取渲染窗口
         * @param [in] name : 渲染窗口名称
         * @return 成功返回渲染窗口对象
         */
        RenderWindowPtr getRenderWindow(const String &name) const;

        /**
         * @brief 获取默认渲染窗口
         * @return 返回默认渲染窗口对象
         */
        RenderWindowPtr getDefaultRenderWindow() const 
        { 
            return mDefaultWindow; 
        }

        /**
         * @brief 运行引擎
         */
        bool run();

        /**
         * @brief 渲染一帧
         */
        void renderOneFrame();

        /**
         * @brief 程序进入后台调用本接口告知引擎
         */
        void appDidEnterBackground();

        /**
         * @brief 程序回到前台调用本接口告知引擎
         */
        void appWillEnterForeground();

        /**
         * @brief 安装插件
         * @param [in] plugin : 对应的插件对象
         * @return 成功返回 T3D_OK
         */
        TResult installPlugin(Plugin *plugin);

        /**
         * @brief 卸载插件
         * @param [in] plugin : 对应的插件对象
         * @return 成功返回 T3D_OK
         */
        TResult uninstallPlugin(Plugin *plugin);

        /**
         * @brief 加载指定名称的插件
         * @return 成功返回 T3D_OK
         */
        TResult loadPlugin(const String &name);

        /**
         * @brief 卸载指定名称的插件
         */
        TResult unloadPlugin(const String &name);

        /**
         * @brief 添加档案结构构造器
         * @return 成功返回 T3D_OK
         */
        TResult addArchiveCreator(ArchiveCreator *creator);

        /**
         * @brief 移除档案结构构造器
         * @return 成功返回 T3D_OK
         */
        TResult removeArchiveCreator(ArchiveCreator *creator);

        /**
         * @brief 获取主资源的档案对象
         * @param [in] path : 档案结构内部相对路径
         * @return 成功返回档案结构对象
         */
        ArchivePtr getMainAssetsArchive(const String &path) const;

        /**
         * @brief 根据输入路径，返回相应资源路径
         * @param [in] path : 资源相对路径
         * @return 返回相关平台的绝对路径
         * @remarks 使用该返回的路径才能访问到对应平台的资源
         */
        String getMainAssetsPath(const String &path) const;

        /**
         * @brief 获取文件所在的档案对象
         * @param [in] filename : 文件名
         * @return 成功返回档案结构对象
         */
        ArchivePtr getAssetsArchive(const String &filename) const;

        /**
         * @brief 添加图像编解码器
         * @param [in] type : 文件类型
         * @param [in] codec : 要添加的图像编解码器对象
         * @return 成功返回 T3D_OK
         */
        TResult addImageCodec(ImageCodecBase::FileType type, 
            ImageCodecBasePtr codec);

        /**
         * @brief 移除图像编解码器
         * @param [in] type : 要移除的编解码器对应的文件类型
         * @return 成功返回 T3D_OK
         */
        TResult removeImageCodec(ImageCodecBase::FileType type);

        /**
         * @brief 枚举所有可用的渲染器
         * @param [in][out] renderers : 返回所有可用渲染器列表
         */
        void enumerateAvailableRenderers(Renderers &renderers) const;

        /**
         * @brief 设置当前可用的渲染器
         * @param [in] renderer : 渲染器对象
         * @return 成功返回 T3D_OK
         */
        TResult setActiveRenderer(RendererPtr renderer);

        /**
         * @brief 获取当前可用的渲染器
         * @return 成功返回当前渲染器对象
         */
        RendererPtr getActiveRenderer() const;

        /**
         * @brief 添加渲染器
         * @param [in] renderer : 要添加的渲染器对象
         * @return 成功返回 T3D_OK
         */
        TResult addRenderer(RendererPtr renderer);

        /**
         * @brief 移除渲染器
         * @param [in] renderer : 要移除的渲染器对象
         * @return 成功返回 T3D_OK
         */
        TResult removeRenderer(RendererPtr renderer);

        /**
         * @brief 获取渲染器对象
         * @param [in] name : 渲染器名称
         * @return 成功返回渲染器对象
         */
        RendererPtr getRenderer(const String &name) const;

        /**
         * @brief 设置场景管理器
         * @param [in] mgr : 场景管理器对象
         * @return 成功返回 T3D_OK
         * @remarks 如果没有外部插件设置特有的场景管理器，则内部会用默认场景
         *      管理器来使用
         */
        TResult setSceneManager(SceneManagerBasePtr mgr);

        /**
         * @brief 获取应用程序路径，不包含程序名称
         */
        const String &getAppPath() const { return mAppPath; }

        /**
         * @brief 获取应用程序名称，不包含路径
         */
        const String &getAppName() const { return mAppName; }

        /**
         * @brief 获取插件路径
         */
        const String &getPluginsPath() const { return mPluginsPath; }

    protected:
        /**
         * @brief 初始化应用程序
         */
        TResult initApplication();

        /** 
         * @brief 初始化日志系统 
         */
        TResult initLogSystem();

        /**
         * @brief 初始化事件系统
         */
        TResult initEventSystem();

        /**
         * @brief 初始化对象跟踪器
         */
        TResult initObjectTracer();

        /**
         * @brief 初始化各种管理器
         */
        TResult initManagers();

        /**
         * @brief 加载配置文件
         * @param [in] cfgPath : 配置文件名
         * @return 调用成功返回 T3D_OK
         */
        TResult loadConfig(const String &cfgPath);

        /**
         * @brief 加载配置文件中指定的插件
         * @return 调用成功返回 T3D_OK
         */
        TResult loadPlugins();

        /**
         * @brief 卸载所有插件
         * @return 调用成功返回 T3D_OK
         */
        TResult unloadPlugins();

        /**
         * @brief 初始化资源
         * @return 调用成功返回 T3D_OK
         */
        TResult initAssets();

        /**
         * @brief 初始化渲染器
         * @return 成功返回 T3D_OK
         */
        TResult initRenderer();

        /**
         * @brief 初始化场景管理器
         * @return 成功返回 T3D_OK
         */
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

        RenderWindowPtr         mDefaultWindow;     /**< 默认渲染窗口 */

        ArchiveManagerPtr           mArchiveMgr;        /**< 档案管理对象 */
        DylibManagerPtr             mDylibMgr;          /**< 动态库管理对象 */
        MaterialManagerPtr          mMaterialMgr;       /**< 材质资源管理对象 */
        TextureManagerPtr           mTextureMgr;        /**< 纹理资源管理对象 */
        ShaderManagerPtr            mShaderMgr;         /**< 着色器管理对象 */
        GPUProgramManagerPtr        mGPUProgramMgr;     /**< GPU程序管理对象 */
        GPUConstBufferManagerPtr    mGPUConstBufferMgr; /**< Manager for GPU constant buffer */

        ImageCodecPtr           mImageCodec;        /**< 图像编解码器对象 */

        RendererPtr             mActiveRenderer;    /**< 当前渲染器对象 */
        SceneManagerPtr         mSceneMgr;

        ScriptParserPtr         mScriptParser;

        Plugins                 mPlugins;           /**< 安装的插件列表 */
        Dylibs                  mDylibs;            /**< 加载的动态库列表 */
        Renderers               mRenderers;         /**< 渲染器列表 */

        String                  mAppPath;           /**< 程序路径 */
        String                  mAppName;           /**< 程序名称 */
        String                  mPluginsPath;       /**< 插件路径 */

        Settings                mSettings;          /**< 引擎配置项 */

        bool                    mIsRunning;         /**< 引擎是否在运行中 */
    };

    #define T3D_AGENT      (Agent::getInstance())
}


#endif  /*__T3D_APP_EVENT_LISTENER_H__*/
