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
 * \brief 引擎代理：负责初始化、主循环、插件、渲染器与子系统生命周期
 * \remarks 引擎坐标与矩阵约定：
 *      1. 基于左手坐标系（世界 / 相机 / 裁剪空间）
 *      2. 矩阵 × 列向量
 *      3. NDC 空间 z 轴范围 [-1, 1]
 *      4. 纹理空间：原点在左下角，x 向右，y 向上
 *      5. 欧拉角旋转顺序：Z-X-Y
 */
namespace Tiny3D
{
    // using PollEvents = TFunction<bool()>;
    using Update = TFunction<void()>;
    /// beginRender 之前的回调
    using PreEngineRender = TFunction<void()>;
    /// pipeline.render 之后、endRender 之前的回调
    using OnEngineRender = TFunction<void()>;
    /// endRender 之后、swapBuffers 之前的回调
    using PostEngineRender = TFunction<void()>;
    using FrameEndTask = TFunction<void()>;

    /**
     * \brief 编辑器主循环注入回调（供 runForEditor 使用）
     * \remarks update 在 Agent::update 之后、渲染之前调用；
     *          preRender / onRender / postRender 分别在 RHI beginRender 前、
     *          endRender 前、endRender 后调用。
     *          三个 typedef 底层都是 TFunction<void()>，分开命名只为让调用点看得出时序。
     */
    struct EditorRunningData
    {
        // PollEvents          pollEvents {nullptr};
        Update              update {nullptr};       ///< 可选：自定义逻辑更新
        PreEngineRender     preRender {nullptr};    ///< 可选：beginRender 之前
        OnEngineRender      onRender {nullptr};     ///< 可选：beginRender 内、endRender 之前
        PostEngineRender    postRender {nullptr};   ///< 可选：endRender 之后
    };

    /**
     * \brief 引擎入口单例，串联配置、插件、RHI、场景更新与渲染帧循环
     */
    class T3D_ENGINE_API Agent : public Singleton<Agent>
    {
    public:
        Agent();

        /**
         * \brief 按与初始化大致相反的顺序拆除子系统、插件与 Time 单例
         */
        virtual ~Agent();

        /**
         * \brief 获取整数版本号
         * \return 编译期版本常量（当前为 0x00000001）
         */
        uint32_t getVersion() const;

        /**
         * \brief 获取版本号字符串
         * \return 静态字面量（当前为 "0.0.0.1"）
         */
        const char *getVersionString() const;

        /**
         * \brief 获取版本代号名
         * \return 静态字面量（当前为 "Rosetta"）
         */
        const char *getVersionName() const;

        /// 返回引擎配置常量引用
        const Settings &getSettings() const { return mSettings; }

        /// 返回引擎配置可变引用（调用方可直接修改，无额外校验）
        Settings &getSettings() { return mSettings; }

        /**
         * \brief 向活动 RHI 查询编辑器相关信息
         * \param [in] info : 由 RHI 实现解释的输出缓冲；无活动渲染器时本函数为空操作
         */
        void getEditorInfo(void *info) const;

        /**
         * \brief 从配置文件初始化引擎
         * \param [in] argc : 参数个数，必须 ≥ 1（使用 argv[0] 作为应用路径）
         * \param [in] argv : 命令行参数
         * \param [in] autoCreateWindow : 是否在 initRenderer 后创建并登记默认窗口
         * \param [in] isWindowApp : true 创建真实窗口，false 创建 NullRenderWindow
         * \param [in] config : 配置文件名，默认 "Tiny3D.cfg"
         * \param [in] maxEvent : 事件管理器容量，传给 EventManager
         * \return 调用成功返回 T3D_OK；argc==0 返回 T3D_ERR_INVALID_PARAM；任一步失败返回该步错误码
         */
        TResult init(int32_t argc, char *argv[], bool autoCreateWindow, bool isWindowApp, const String &config = "Tiny3D.cfg", uint32_t maxEvent = 0);

        /**
         * \brief 用内存中的 Settings 初始化引擎（不读配置文件）
         * \param [in] argc : 参数个数，必须 ≥ 1
         * \param [in] argv : 命令行参数
         * \param [in] autoCreateWindow : 是否创建并登记默认窗口
         * \param [in] isWindowApp : true 创建真实窗口，false 创建 NullRenderWindow
         * \param [in] settings : 直接赋值给 mSettings 的配置
         * \param [in] maxEvent : 事件管理器容量
         * \return 调用成功返回 T3D_OK；argc==0 返回 T3D_ERR_INVALID_PARAM；任一步失败返回该步错误码
         */
        TResult init(int32_t argc, char *argv[], bool autoCreateWindow, bool isWindowApp, const Settings &settings, uint32_t maxEvent = 0);

        /**
         * \brief 按 mSettings.renderSettings 创建默认渲染窗口，并写入 mDefaultWindow
         * \param [out] window : 创建出的窗口
         * \param [in] isWindowApp : true 用 RenderWindow，false 用 NullRenderWindow
         * \return 成功返回 T3D_OK；创建失败返回 T3D_ERR_RENDER_CREATE_WINDOW
         * \note 窗口标题会拼上版本代号与版本号；MSAA 在实现中固定为 Count=4
         */
        TResult createDefaultRenderWindow(RenderWindowPtr &window, bool isWindowApp);

        /**
         * \brief 将窗口按名称加入 mRenderWindows
         * \param [in] window : 窗口对象（实现未判空）
         * \return 成功返回 T3D_OK；无活动 RHI 返回 T3D_ERR_SYS_NOT_INIT
         */
        TResult addRenderWindow(RenderWindowPtr window);

        /**
         * \brief 按名称从 mRenderWindows 移除窗口
         * \param [in] name : 窗口名称
         * \return 成功返回 T3D_OK（名称不存在亦返回 OK）；无活动 RHI 返回 T3D_ERR_SYS_NOT_INIT
         */
        TResult removeRenderWindow(const String &name);

        /**
         * \brief 按名称查找渲染窗口
         * \param [in] name : 窗口名称
         * \return 当前实现查找逻辑已注释，恒返回 nullptr
         */
        RenderWindowPtr getRenderWindow(const String &name) const;

        /// 返回默认渲染窗口（可能为 nullptr）
        RenderWindowPtr getDefaultRenderWindow() const { return mDefaultWindow; }

        /**
         * \brief 运行标准主循环直至 mIsRunning 为 false
         * \return 恒返回 true
         * \remarks 帧序：beginFrame → pollEvents → update → renderOneFrame → endFrame；
         *          退出后调用 applicationWillTerminate。
         */
        bool run();

        /**
         * \brief 运行带编辑器回调的主循环
         * \param [in] updateData : 可选 update / preRender / postRender
         * \return 恒返回 true
         */
        bool runForEditor(const EditorRunningData &updateData);

        /**
         * \brief 是否处于播放态
         * \return mIsPlaying，默认 true
         * \remarks Behaviour 的 onUpdate 等仅在播放态或 executeInEditMode 时执行；
         *          Agent 帧循环本身不读取此标志。
         */
        bool isPlaying() const { return mIsPlaying; }

        /// 进入播放态（mIsPlaying = true）
        void enterPlayMode() { mIsPlaying = true; }

        /// 退出播放态（mIsPlaying = false）
        void exitPlayMode() { mIsPlaying = false; }

        /// 渲染一帧（无 pre/post 回调）
        void renderOneFrame();

        /// 进入后台时通知日志子系统
        void appDidEnterBackground();

        /// 回到前台时通知日志子系统
        void appWillEnterForeground();

        /**
         * \brief 处理应用事件；当前仅响应默认窗口的 RESIZED
         * \param [in] event : 应用事件
         * \return 恒返回 true
         * \note 处理 resize 时未检查 mDefaultWindow 是否为空
         */
        bool processEvents(const AppEvent &event);

        /// 返回插件目录路径（loadPlugins 后为 appPath + pluginPath）
        const String &getPluginsPath() const { return mPluginsPath; }

        /**
         * \brief 注册并启动插件（install → startup）
         * \param [in] plugin : 插件实例，不可为 nullptr
         * \return 成功返回 T3D_OK；空指针返回 T3D_ERR_INVALID_POINTER；
         *         重名返回 T3D_ERR_PLG_DUPLICATED；install/startup 失败时回滚 map 并返回对应错误码
         */
        TResult installPlugin(Plugin *plugin);

        /**
         * \brief 关闭并卸载插件（shutdown → uninstall），再从插件表移除
         * \param [in] plugin : 插件实例，不可为 nullptr
         * \return 成功返回 T3D_OK；空指针返回 T3D_ERR_INVALID_POINTER；
         *         shutdown/uninstall 失败时不 erase，返回对应错误码
         */
        TResult uninstallPlugin(Plugin *plugin);

        /**
         * \brief 加载动态库插件并调用 dllStartPlugin
         * \param [in] name : 动态库资源名
         * \return 成功或已加载返回 T3D_OK；加载失败 / 非 dylib / 无符号时返回对应错误码
         */
        TResult loadPlugin(const String &name);

        /**
         * \brief 从指定目录加载动态库插件并调用 dllStartPlugin
         * \param [in] name : 动态库资源名（不含平台前后缀）
         * \param [in] dir : 动态库所在目录；为空时等价于 loadPlugin
         * \return 成功或已加载返回 T3D_OK；加载失败 / 非 dylib / 无符号时返回对应错误码
         * \remarks 供编辑器加载工程业务插件使用，这类插件不在引擎插件目录下
         */
        TResult loadPluginFromPath(const String &name, const String &dir);

        /**
         * \brief 调用 dllStopPlugin 并卸载动态库
         * \param [in] name : 动态库资源名
         * \return 成功返回 T3D_OK；未加载返回 T3D_ERR_PLG_NOT_EXISTS；无符号返回 T3D_ERR_PLG_NO_FUNCTION
         */
        TResult unloadPlugin(const String &name);

        /**
         * \brief 枚举已注册的 RHI 渲染器
         * \param [out] renderers : 先 clear 再赋值为 mRenderers 的拷贝
         */
        void enumerateAvailableRenderers(RHIRenderers &renderers) const;

        /**
         * \brief 切换活动 RHI：停旧线程、destroy 旧渲染器、init 新渲染器并重启 RHI 线程与管线
         * \param [in] renderer : 目标渲染器（实现未判空；与当前相同则直接 OK）
         * \return 成功返回 T3D_OK；init / 渲染线程 / 管线失败返回对应错误码
         */
        TResult setActiveRHIRenderer(RHIRendererPtr renderer);

        /// 返回当前活动 RHI 渲染器（可能为 nullptr）
        RHIRendererPtr getActiveRHIRenderer() const;

        /**
         * \brief 返回活动 RHI 的 Context
         * \return Context 智能指针
         * \note 实现直接解引用 mActiveRHIRenderer，为空时会崩溃
         */
        RHIContextPtr getActiveRHIContext() const;

        /// 设置应用是否在后台继续运行（转发 Application）
        void setRunInBackground(bool enable) { T3D_APPLICATION.setRunInBackground(enable); }

        /// 查询应用是否在后台继续运行
        bool isRunInBackground() const { return T3D_APPLICATION.isRunInBackground(); }

        /**
         * \brief 按名称注册 RHI 渲染器到 mRenderers（不自动设为活动）
         * \param [in] renderer : 渲染器
         * \return 成功返回 T3D_OK；重名返回 T3D_ERR_DUPLICATED_ITEM
         */
        TResult addRHIRenderer(RHIRendererPtr renderer);

        /**
         * \brief 按名称从 mRenderers 移除渲染器（不 destroy 对象）
         * \param [in] renderer : 渲染器
         * \return 成功返回 T3D_OK；未找到返回 T3D_ERR_NOT_FOUND
         */
        TResult removeRHIRenderer(RHIRendererPtr renderer);

        /**
         * \brief 按名称查找已注册 RHI 渲染器
         * \param [in] name : 渲染器名称
         * \return 命中返回对象，否则返回 nullptr
         */
        RHIRendererPtr getRHIRenderer(const String &name) const;

        /**
         * \brief 在启用 RHI 线程时 trigger mRHIEvent，用于唤醒/通知渲染线程
         */
        void resumeEngineThread();

        /**
         * \brief 向 ImageCodec 注册指定文件类型的编解码器
         * \param [in] type : 文件类型
         * \param [in] codec : 编解码器实例
         * \return 委托 mImageCodec->addImageCodec 的返回值
         */
        TResult addImageCodec(ImageCodecBase::FileType type, ImageCodecBase *codec);

        /**
         * \brief 移除指定文件类型的编解码器
         * \param [in] type : 文件类型
         * \return 委托 mImageCodec->removeImageCodec 的返回值
         */
        TResult removeImageCodec(ImageCodecBase::FileType type);

        /**
         * \brief 同步刷新 RHI 命令队列（启用 RHI 线程时 resume + wait）
         * \remarks 用于需确保已入队 RHI 命令执行完毕的场景（如编辑器初始化 ImGui 前）
         * \remarks 只能在 RHI 线程空闲时调用（帧末 wait 之后，或帧循环之外）。
         *          resume 内部的 exchange 会清空正在被 RHI 线程遍历的那条命令表，
         *          帧中间调用等于边遍历边析构命令对象，还会打乱事件握手
         */
        void flushRHICommands();

        /**
         * \brief 排空 RHI 命令队列，返回时两条命令表都为空
         * \remarks flush 只保证命令执行完毕，命令对象要到下一次 exchange 才析构。
         *          卸载动态库前必须排空，否则队列里由该模块实例化的命令对象会在
         *          FreeLibrary 之后才析构，虚调用直接踩到已卸载的代码段
         */
        void drainRHICommands();

        /**
         * \brief 在帧中间把 RHI 线程拉到空闲并排空命令队列
         * \remarks 与 drainRHICommands 的区别：本函数先等 beginFrame 那批命令自己跑完，
         *          再 drain 本帧刚入队的命令。GPU 读回的 unmap 必须用它——
         *          onPostRender 发生在 endFrame 的 wait 之前，本帧的 Copy 还躺在
         *          入队表里没执行，直接 drain 等于在 RHI 线程遍历命令表的同时 exchange。
         *          等过的那次会记账，endFrame 不会重复 wait。
         */
        void syncRHIThread();

        /**
         * \brief 投递一个在帧末安全点执行的任务
         * \param [in] task : 待执行任务，空任务被忽略
         * \remarks 帧中间 RHI 线程正在执行上一帧的命令，此时销毁 GPU 资源就是跨线程
         *          的 use-after-free；而本帧刚入队的命令要到下一帧才执行，只等一次
         *          帧同步也不够。凡是要销毁场景 / 渲染目标 / 插件的操作都应该投递到
         *          这里，由 endFrame 在 RHI 线程空闲且命令排空之后执行
         */
        void postFrameEndTask(FrameEndTask task);

    protected:
        /**
         * \brief 初始化平台路径、CRC、Application、日志、事件、ObjectTracer 与各 Manager
         * \param [in] appPath : 应用路径（通常为 argv[0]）
         * \param [in] maxEvent : 事件队列容量
         * \return 成功返回 T3D_OK；任一步失败返回该步错误码
         */
        TResult initSystem(const String &appPath, uint32_t maxEvent);

        /**
         * \brief 初始化 Application 单例
         * \return 成功返回 Application::init 结果；单例不存在返回 T3D_ERR_INVALID_POINTER
         */
        TResult initApplication();

        /**
         * \brief 创建 Logger 并 startup
         * \return Logger 创建失败为 T3D_ERR_FAIL，否则为 startup 返回值
         */
        TResult initLogSystem();

        /**
         * \brief 创建 EventManager
         * \param [in] maxEvent : 事件容量
         * \return 恒返回 T3D_OK
         */
        TResult initEventSystem(uint32_t maxEvent);

        /**
         * \brief 创建 ObjectTracer（启用追踪）
         * \return 恒返回 T3D_OK
         */
        TResult initObjectTracer();

        /**
         * \brief 创建资源 / 渲染 / 序列化等全部 Manager 与默认 ForwardRenderPipeline
         * \return 恒返回 T3D_OK（实现不做失败检查）
         */
        TResult initManagers();

        /**
         * \brief 加载平台档案插件，读取配置文件反序列化到 mSettings
         * \param [in] cfgPath : 配置文件路径
         * \return 成功返回 T3D_OK；插件 / 档案 / 反序列化失败返回对应错误码
         */
        TResult loadConfig(const String &cfgPath);

        /**
         * \brief 设置 mPluginsPath，并按 pluginSettings.plugins 逐个 loadPlugin
         * \return 成功返回 T3D_OK；首个失败插件的错误码
         */
        TResult loadPlugins();

        /**
         * \brief 对已加载 dylib 调用 dllStopPlugin 并 unload，最后 clear mDylibs
         * \return 最后一次 dllStopPlugin 的结果（或 T3D_OK）
         */
        TResult unloadPlugins();

        /**
         * \brief 按 renderSettings.renderer 名称取渲染器并 setActiveRHIRenderer
         * \return 未找到返回 T3D_ERR_PLG_NOT_LOADED，否则返回 setActiveRHIRenderer 结果
         */
        TResult initRenderer();

        /**
         * \brief 场景管理器初始化（头文件声明；当前无对应 .cpp 实现）
         * \return 待确认
         */
        TResult initSceneManager();

        /**
         * \brief 创建 Time 单例（若尚未创建），注入 timeSettings，并调用 start()
         */
        void initTime();

        /**
         * \brief 创建 RHIThread runnable；启用 RHI 线程时启动工作线程
         * \return 线程 start 的返回值（禁用 RHI 线程时为 T3D_OK）
         */
        TResult initRenderThread();

        /**
         * \brief 停止并 join RHI 线程（可选先 flush 已入队命令）
         */
        void stopRenderThread();

        /**
         * \brief 执行单帧渲染：可选 pre/on/post 回调、管线 cull/render、swapBuffers、渲染资源 GC
         * \param [in] preRender : beginRender 之前，可为空
         * \param [in] onRender : pipeline.render 之后、endRender 之前，可为空
         * \param [in] postRender : endRender 之后、swapBuffers 之前，可为空
         */
        void renderOneFrame(const PreEngineRender &preRender, const OnEngineRender &onRender, const PostEngineRender &postRender);

        /**
         * \brief 派发事件，并对当前 Scene 执行 FixedUpdate（最多 8 步）与 Update/LateUpdate
         */
        void update();

        /**
         * \brief 帧开始：Time::tick，并在启用 RHI 线程时 resume
         */
        void beginFrame();

        /**
         * \brief 帧结束：等待 RHI、更新动画、AssignableObject 赋值、延迟销毁 Component/GameObject
         */
        void endFrame();

        /**
         * \brief 在帧末安全点排空 RHI 命令队列并执行已投递的延迟任务
         * \remarks 只能由 endFrame 在 mRHIEvent.wait() 之后调用
         */
        void runFrameEndTasks();

        /**
         * \brief 等 beginFrame 发出的那批 RHI 命令执行完毕，最多等一次
         * \remarks 用 mRHIBatchInFlight 记账，endFrame 与 syncRHIThread 谁先调到谁等，
         *          另一个直接跳过，避免多等一次 mRHIEvent 把自己挂死
         */
        void waitRHIBatch();

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

        using FrameEndTasks = TList<FrameEndTask>;

        AssignableObjectManagerPtr  mAssignableObjMgr {nullptr};    ///< 异步赋值对象管理器

        Logger                  *mLogger {nullptr};                 ///< 日志对象
        EventManager            *mEventMgr {nullptr};               ///< 事件管理器
        ObjectTracer            *mObjTracer {nullptr};              ///< 对象内存跟踪

        RenderPipelinePtr       mRenderPipeline {nullptr};          ///< 渲染管线
        RenderWindowPtr         mDefaultWindow {nullptr};           ///< 默认渲染窗口
        RHIRendererPtr          mActiveRHIRenderer {nullptr};       ///< 当前活动 RHI

        AnimationPlayerMgrPtr   mAniPlayerMgr {nullptr};            ///< 动画播放器管理器
        ArchiveManagerPtr       mArchiveMgr {nullptr};              ///< 档案管理器
        SerializerManagerPtr    mSerializerMgr {nullptr};           ///< 序列化管理器
        DylibManagerPtr         mDylibMgr {nullptr};                ///< 动态库管理器
        MeshManagerPtr          mMeshMgr {nullptr};                 ///< 网格资源管理器
        SkeletonManagerPtr      mSkeletonMgr {nullptr};             ///< 骨架资源管理器
        AnimationManagerPtr     mAnimationMgr {nullptr};            ///< 动画资源管理器
        PrefabManagerPtr        mPrefabMgr {nullptr};               ///< 预制体资源管理器
        ShaderManagerPtr        mShaderMgr {nullptr};               ///< 着色器资源管理器
        MaterialManagerPtr      mMaterialMgr {nullptr};             ///< 材质资源管理器
        TextureManagerPtr       mTextureMgr {nullptr};              ///< 纹理资源管理器
        SceneManagerPtr         mSceneMgr {nullptr};                ///< 场景资源管理器
        AssetManagerPtr         mAssetMgr {nullptr};                ///< 资源加载门面
        ImageCodecPtr           mImageCodec {nullptr};              ///< 图像编解码器
        ImageManagerPtr         mImageMgr {nullptr};                ///< 图像管理器

        RenderStateManagerPtr   mRenderStateMgr {nullptr};          ///< 渲染状态管理器
        RenderBufferManagerPtr  mRenderBufferMgr {nullptr};         ///< 渲染缓冲区管理器

        Plugins                 mPlugins {};                        ///< 已 install 的插件表
        Dylibs                  mDylibs {};                         ///< 已加载的动态库
        RHIRenderers            mRenderers {};                      ///< 已注册的 RHI 渲染器

        RenderWindows           mRenderWindows {};                  ///< 已登记的渲染窗口

        String                  mAppPath {};                        ///< 程序路径
        String                  mAppName {};                        ///< 程序名称
        String                  mPluginsPath {"."};                 ///< 插件目录
        String                  mProjectPath {};                    ///< 游戏项目路径

        Settings                mSettings {};                       ///< 引擎配置

        Time                   *mTime {nullptr};                    ///< 全局时间单例
        Input                  *mInput {nullptr};                   ///< 全局输入单例
        bool                    mIsPlaying {true};                  ///< 播放态标志

        RunnableThread          mRHIThread {};                      ///< RHI 工作线程
        RHIThreadPtr            mRHIRunnable {nullptr};             ///< RHI 线程执行体
        Event                   mRHIEvent {};                       ///< 与 RHI 线程同步的事件
        bool                    mRHIBatchInFlight {false};          ///< beginFrame 发出的那批命令是否还没等过

        FrameEndTasks           mFrameEndTasks {};                  ///< 帧末安全点待执行的任务

        bool                    mIsRunning {false};                 ///< 主循环是否在运行
    };

    #define T3D_AGENT   Agent::getInstance()

    T3D_EXTERN_SINGLETON(Agent)
}


#endif    /*__T3D_AGENT_H__*/
