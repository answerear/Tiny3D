/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef __T3D_AGENT_H__
#define __T3D_AGENT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DSettings.h"
#include "ImageCodec/T3DImageCodecBase.h"


/**
 * \brief   T3D引擎代理类
 * \remarks 引擎一些规范
 *      1. 基于左手坐标系
 *      2. 矩阵 x 列向量的方式计算
 *      3. NDC 空间 z 轴范围是 [-1, 1]
 *      4. 纹理空间：原点位于纹理左下角， x 正向向右，y 正向向上
 */
namespace Tiny3D
{
    // using PollEvents = TFunction<bool()>;
    using Update = TFunction<void()>;
    using PreEngineRender = TFunction<void()>;
    using PostEngineRender = TFunction<void()>;

    struct EditorRunningData
    {
        // PollEvents          pollEvents {nullptr};
        Update              update {nullptr};
        PreEngineRender     preRender {nullptr};
        PostEngineRender    postRender {nullptr};
    };
    
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

        const Settings &getSettings() const { return mSettings; }

        Settings &getSettings() { return mSettings; }

        void getEditorInfo(void *info) const;

        TResult init(int32_t argc, char *argv[], bool autoCreateWindow, bool isWindowApp, const String &config = "Tiny3D.cfg", uint32_t maxEvent = 0);

        TResult init(int32_t argc, char *argv[], bool autoCreateWindow, bool isWindowApp, const Settings &settings, uint32_t maxEvent = 0);

        TResult createDefaultRenderWindow(RenderWindowPtr &window, bool isWindowApp);

        TResult addRenderWindow(RenderWindowPtr window);

        TResult removeRenderWindow(const String &name);

        RenderWindowPtr getRenderWindow(const String &name) const;

        RenderWindowPtr getDefaultRenderWindow() const 
        { 
            return mDefaultWindow; 
        }

        bool run();

        bool runForEditor(const EditorRunningData &updateData);

        void renderOneFrame();

        void appDidEnterBackground();

        void appWillEnterForeground();

        bool processEvents(const AppEvent &event);
     
        const String &getPluginsPath() const { return mPluginsPath; }

        TResult installPlugin(Plugin *plugin);

        TResult uninstallPlugin(Plugin *plugin);

        TResult loadPlugin(const String &name);

        TResult unloadPlugin(const String &name);

        void enumerateAvailableRenderers(RHIRenderers &renderers) const;

        TResult setActiveRHIRenderer(RHIRendererPtr renderer);

        RHIRendererPtr getActiveRHIRenderer() const;

        RHIContextPtr getActiveRHIContext() const;

        void setRunInBackground(bool enable) { T3D_APPLICATION.setRunInBackground(enable); }

        bool isRunInBackground() const { return T3D_APPLICATION.isRunInBackground(); }
     
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

        void resumeEngineThread();

        TResult addImageCodec(ImageCodecBase::FileType type, ImageCodecBase *codec);

        TResult removeImageCodec(ImageCodecBase::FileType type);

    protected:
        TResult initSystem(const String &appPath, uint32_t maxEvent);
        
        TResult initApplication();

        TResult initLogSystem();

        TResult initEventSystem(uint32_t maxEvent);

        TResult initObjectTracer();

        TResult initManagers();

        TResult loadConfig(const String &cfgPath);

        TResult loadPlugins();

        TResult unloadPlugins();

        TResult initRenderer();

        TResult initSceneManager();

        TResult initRenderThread();

        void stopRenderThread();

        void renderOneFrame(const PreEngineRender &preRender, const PostEngineRender &postRender);

        void update();

        void beginFrame();

        void endFrame();
        
    protected:
        typedef TMap<String, Plugin*>       Plugins;
        typedef Plugins::iterator           PluginsItr;
        typedef Plugins::const_iterator     PluginsConstItr;
        typedef Plugins::value_type         PluginsValue;

        typedef TMap<String, DylibPtr>      Dylibs;
        typedef Dylibs::iterator            DylibsItr;
        typedef Dylibs::const_iterator      DylibsConstItr;
        typedef Dylibs::value_type          DylibsValue;

        using RenderWindows = TMap<String, RenderWindowPtr>;

        /// 异步赋值对象管理器
        AssignableObjectManagerPtr  mAssignableObjMgr {nullptr};

        /// 日志对象
        Logger                  *mLogger {nullptr};
        /// 事件管理器对象
        EventManager            *mEventMgr {nullptr};
        /// 对象内存跟踪
        ObjectTracer            *mObjTracer {nullptr};

        /// 渲染管线
        RenderPipelinePtr       mRenderPipeline {nullptr};
        /// 默认渲染窗口
        RenderWindowPtr         mDefaultWindow {nullptr};
        /// 当前使用的 RHI 渲染器
        RHIRendererPtr          mActiveRHIRenderer {nullptr};

        ArchiveManagerPtr       mArchiveMgr {nullptr};
        SerializerManagerPtr    mSerializerMgr {nullptr};
        DylibManagerPtr         mDylibMgr {nullptr};
        MeshManagerPtr          mMeshMgr {nullptr};
        PrefabManagerPtr        mPrefabMgr {nullptr};
        ShaderManagerPtr        mShaderMgr {nullptr};
        MaterialManagerPtr      mMaterialMgr {nullptr};
        TextureManagerPtr       mTextureMgr {nullptr};
        SceneManagerPtr         mSceneMgr {nullptr};
        ImageCodecPtr           mImageCodec {nullptr};
        ImageManagerPtr         mImageMgr {nullptr};

        RenderStateManagerPtr   mRenderStateMgr {nullptr};
        RenderBufferManagerPtr  mRenderBufferMgr {nullptr};

        /// 安装的插件列表
        Plugins                 mPlugins {};
        /// 加载的动态库列表
        Dylibs                  mDylibs {};
        /// 渲染器列表
        RHIRenderers            mRenderers {};

        /// 所有的渲染窗口
        RenderWindows           mRenderWindows {};

        /// 程序路径
        String                  mAppPath {};
        /// 程序名称
        String                  mAppName {};
        /// 插件路径
        String                  mPluginsPath {"."};
        /// 游戏项目路径，用于资源读写
        String                  mProjectPath {};

        Settings                mSettings {};           /**< 引擎配置项 */

        RunnableThread          mRHIThread {};
        RHIThreadPtr            mRHIRunnable {nullptr};
        Event                   mRHIEvent {};

        // Texture2DPtr            mDefaultAlbedoTex {nullptr};
        
        bool                    mIsRunning {false};     /**< 引擎是否在运行中 */
    };

    #define T3D_AGENT   Agent::getInstance()
}


#endif    /*__T3D_AGENT_H__*/
