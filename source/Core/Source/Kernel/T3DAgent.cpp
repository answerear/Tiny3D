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

#include "T3DConfig.h"
#include "Kernel/T3DAgent.h"
#include "Kernel/T3DTime.h"
#include "Input/T3DInput.h"
#include "Kernel/T3DArchive.h"
#include "Kernel/T3DArchiveManager.h"
#include "Serializer/T3DSerializerManager.h"
#include "Kernel/T3DPlugin.h"
#include "Resource/T3DResourceManager.h"
#include "Resource/T3DResource.h"
#include "Resource/T3DDylib.h"
#include "Resource/T3DDylibManager.h"
#include "Resource/T3DPrefabManager.h"
#include "Resource/T3DShaderManager.h"
#include "T3DErrorDef.h"
#include "Animation/T3DAnimationPlayerMgr.h"
#include "Kernel/T3DGameObject.h"
#include "Render/T3DRenderWindow.h"
#include "RHI/T3DRHIRenderer.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHIThread.h"
#include "Render/T3DRenderResourceManager.h"
#include "Render/T3DForwardRenderPipeline.h"
#include "Resource/T3DAssetManager.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DMeshManager.h"
#include "Resource/T3DScene.h"
#include "Resource/T3DSceneManager.h"
#include "Resource/T3DTextureManager.h"
#include "Resource/T3DTexture.h"
#include "ImageCodec/T3DImageCodec.h"
#include "ImageCodec/T3DImageCodecBase.h"
#include "Resource/T3DImageManager.h"
#include "Resource/T3DAnimationManager.h"
#include "Resource/T3DSkeletonManager.h"


namespace Tiny3D
{
    #define T3D_VERSION_0_0_0_1_STR         "0.0.0.1"
    #define T3D_VERSION_0_0_0_1_VAL         0x00000001
    #define T3D_VERSION_0_0_0_1_NAME        "Rosetta"

    #define T3D_VERSION_STR                 T3D_VERSION_0_0_0_1_STR
    #define T3D_VERSION_VAL                 T3D_VERSION_0_0_0_1_VAL
    #define T3D_VERSION_NAME                T3D_VERSION_0_0_0_1_NAME

    /// 单帧内 FixedUpdate 的最大步数，防"死亡螺旋"（一帧过卡导致 fixed 次数暴涨）
    static const uint32_t kMaxFixedStepsPerFrame = 8;

    typedef TResult (*DLL_START_PLUGIN)(void);
    typedef TResult (*DLL_STOP_PLUGIN)(void);

    /// 串联两个同一时序槽位的渲染回调，first 先跑；任一为空时直接返回另一个
    static TFunction<void()> combineEngineRender(const TFunction<void()> &first, const TFunction<void()> &second)
    {
        if (first == nullptr)
        {
            return second;
        }

        if (second == nullptr)
        {
            return first;
        }

        return [first, second]() { first(); second(); };
    }
    
    //--------------------------------------------------------------------------
    
    Agent::Agent()
    {
    }

    //--------------------------------------------------------------------------
    
    Agent::~Agent()
    {
        // if (mObjTracer != nullptr)
        // {
        //     mObjTracer->dumpMemoryInfo();
        // }

        // 帧循环已经结束，残留的任务捕获的对象正在被拆掉，不能再让它们跑
        mFrameEndTasks.clear();

        if (mAniPlayerMgr != nullptr)
        {
            mAniPlayerMgr->removeAllPlayers();
        }
        
        stopRenderThread();

        // Wait for GPU to finish all submitted commands before destroying
        // any GPU resources. This prevents Vulkan validation errors caused
        // by destroying resources still in use by pending command buffers.
        // For D3D11/GL4, destroy() is a no-op so this is safe across backends.
        if (mActiveRHIRenderer != nullptr)
        {
            mActiveRHIRenderer->destroy();
        }

        if (mRenderPipeline != nullptr)
        {
            mRenderPipeline->destroy();
            mRenderPipeline = nullptr;
        }

        mDefaultWindow = nullptr;

        // Release Agent's reference to renderer. mRenderers map still holds
        // the renderer alive, so Context/Device won't be destroyed yet.
        // Actual destruction happens in unloadPlugins() when mRenderers is cleared.
        mActiveRHIRenderer = nullptr;

        mRHIRunnable = nullptr;

        // 先释放资源门面，解除对搜索链上各档案的引用
        if (mAssetMgr != nullptr)
        {
            mAssetMgr->unmountAll();
            mAssetMgr = nullptr;
        }

        if (mSkeletonMgr != nullptr)
        {
            mSkeletonMgr->unloadAllResources();
            mSkeletonMgr = nullptr;
        }
        
        if (mSceneMgr != nullptr && mSceneMgr->getCurrentScene() != nullptr)
        {
            // mSceneMgr->getCurrentScene()->removeAll();
            mSceneMgr->unloadScene();
        }

        if (mSceneMgr != nullptr)
        {
            // mSceneMgr->unloadScene();
            mSceneMgr->unloadAllScenes();
        }

        if (mPrefabMgr != nullptr)
        {
            mPrefabMgr->unloadAllResources();
            
        }
        
        GameObject::destroyComponents();
        GameObject::destroyGameObjects();

        // if (mObjTracer != nullptr)
        // {
        //     mObjTracer->dumpMemoryInfo();
        // }
        
        // releaseBuiltinAssets();

        mSceneMgr = nullptr;
        mPrefabMgr = nullptr;

        if (mMeshMgr != nullptr)
        {
            mMeshMgr->unloadAllResources();
            mMeshMgr = nullptr;
        }

        if (mMaterialMgr != nullptr)
        {
            mMaterialMgr->unloadAllResources();
            mMaterialMgr = nullptr;
        }
        
        if (mShaderMgr != nullptr)
        {
            mShaderMgr->unloadAllResources();
            mShaderMgr = nullptr;
        }

        if (mTextureMgr != nullptr)
        {
            mTextureMgr->unloadAllResources();
            mTextureMgr = nullptr;
        }

        if (mImageMgr != nullptr)
        {
            mImageMgr->unloadAllResources();
            mImageMgr = nullptr;
        }

        mRenderPipeline = nullptr;
        mAssignableObjMgr = nullptr;
        mRenderStateMgr = nullptr;
        mRenderBufferMgr = nullptr;
        mRenderWindows.clear();

        if (mAnimationMgr != nullptr)
        {
            mAnimationMgr->unloadAllResources();
            mAnimationMgr = nullptr;
        }

        if (mArchiveMgr != nullptr)
        {
            mArchiveMgr->unloadAllArchives();
        }

        // if (mObjTracer != nullptr)
        // {
        //     mObjTracer->dumpMemoryInfo();
        // }
        
        // 泄漏对象的类型名只能趁模块还在的时候取：插件 DLL 卸载之后，属于它的
        // 对象 vtable / RTTI 随模块解除映射，末尾那次 dumpMemoryInfo 里的 typeid
        // 就会读到一段没有映射的地址
        if (mObjTracer != nullptr)
        {
            mObjTracer->captureTypeNames();
        }

        // 卸载所有插件
        unloadPlugins();

        // if (mObjTracer != nullptr)
        // {
        //     mObjTracer->dumpMemoryInfo();
        // }
        
        if (mDylibMgr != nullptr)
        {
            mDylibMgr->unloadAllResources();
            mDylibMgr = nullptr;
        }

        mImageCodec = nullptr;
        mSerializerMgr = nullptr;
        mArchiveMgr = nullptr;
        mAniPlayerMgr = nullptr;
        
        // 销毁全局 Input 单例（在 Time 之前，查询仍可用）
        if (Application::getInstancePtr() != nullptr && mInput != nullptr)
        {
            Application::getInstance().removeEventListener(mInput);
        }
        T3D_SAFE_DELETE(mInput);

        // 销毁全局 Time 单例（在场景 / 组件清理之后，确保销毁回调仍可读取时间）
        T3D_SAFE_DELETE(mTime);

        T3D_SAFE_DELETE(mEventMgr);

        if (mObjTracer != nullptr)
        {
            mObjTracer->dumpMemoryInfo();
        }
        T3D_SAFE_DELETE(mObjTracer);

        T3D_LOG_SHUTDOWN();
        T3D_SAFE_DELETE(mLogger);

        MemoryManager::getInstance().exit();
    }

    //--------------------------------------------------------------------------
    
    uint32_t Agent::getVersion() const
    {
        return T3D_VERSION_VAL;
    }

    //--------------------------------------------------------------------------

    const char *Agent::getVersionString() const
    {
        return T3D_VERSION_STR;
    }
    
    //--------------------------------------------------------------------------

    const char *Agent::getVersionName() const
    {
        return T3D_VERSION_NAME;
    }
    
    //--------------------------------------------------------------------------

    void Agent::getEditorInfo(void *info) const
    {
        if (mActiveRHIRenderer != nullptr)
        {
            mActiveRHIRenderer->getEditorInfo(info, mDefaultWindow);
        }
    }
    
    //--------------------------------------------------------------------------

    TResult Agent::initSystem(const String &appPath, uint32_t maxEvent)
    {
        TResult ret = T3D_OK;

        do
        {
            if (Dir::getCompany().empty() && Dir::getAppName().empty())
            {
                Dir::setCachePathInfo("Tiny3D", "Tiny3D");
            }
            
#if !defined (T3D_OS_ANDROID)
            // 获取应用程序路径、应用程序名称
            StringUtil::split(appPath, mAppPath, mAppName);
            mAppPath = Dir::getAppPath();
#endif

            // 初始化 CRC 表
            CRC::initCRCTable();

            // 初始化应用程序框架，这个需要放在最前面，否则平台相关接口均不能用
            ret = initApplication();
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            // 初始化日志系统，这个需要放在前面，避免日志无法输出
            ret = initLogSystem();
            if (T3D_FAILED(ret))
            {
                break;
            }

#if defined (T3D_OS_ANDROID)
            mAppPath = Dir::getAppPath();
#endif

            // 初始化事件系统
            ret = initEventSystem(maxEvent);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 初始化对象追踪器
            ret = initObjectTracer();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 初始化各种管理器
            ret = initManagers();
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::init(int32_t argc, char *argv[], bool autoCreateWindow, bool isWindowApp, const String &config, uint32_t maxEvent)
    {
        TResult ret = T3D_OK;

        do
        {
            if (argc == 0)
            {
                ret = T3D_ERR_INVALID_PARAM;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid the number of arguments [%d] !", argc);
                break;
            }
            
            String appPath = argv[0];
            
            // 初始化系统必须的
            ret = initSystem(appPath, maxEvent);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 加载配置文件
            ret = loadConfig(config);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 配置就绪后创建全局 Time 单例
            initTime();

            // 加载配置文件中指定的插件
            ret = loadPlugins();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 初始化渲染器
            ret = initRenderer();
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            if (autoCreateWindow)
            {
                // 创建渲染窗口
                RenderWindowPtr window;
                ret = createDefaultRenderWindow(window, isWindowApp);
                if (T3D_FAILED(ret))
                {
                    break;
                }

                addRenderWindow(window);
            }

            mIsRunning = true;

            Application *theApp = Application::getInstancePtr();
            ret = theApp->applicationDidFinishLaunching(argc, argv);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::init(int32_t argc, char *argv[], bool autoCreateWindow, bool isWindowApp, const Settings &settings, uint32_t maxEvent)
    {
        TResult ret = T3D_OK;

        do
        {
            if (argc == 0)
            {
                ret = T3D_ERR_INVALID_PARAM;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid the number of arguments [%d] !", argc);
                break;
            }
            
            String appPath = argv[0];
            
            // 初始化系统必须的
            ret = initSystem(appPath, maxEvent);
            if (T3D_FAILED(ret))
            {
                break;
            }

            mSettings = settings;

            // 配置就绪后创建全局 Time 单例
            initTime();

            mArchiveMgr = ArchiveManager::create();

            // 加载配置文件中指定的插件
            ret = loadPlugins();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 初始化渲染器
            ret = initRenderer();
            if (T3D_FAILED(ret))
            {
                break;
            }

            if (autoCreateWindow)
            {
                // 创建渲染窗口
                RenderWindowPtr window;
                ret = createDefaultRenderWindow(window, isWindowApp);
                if (T3D_FAILED(ret))
                {
                    break;
                }

                addRenderWindow(window);
            }

            mIsRunning = true;

            Application *theApp = Application::getInstancePtr();
            ret = theApp->applicationDidFinishLaunching(argc, argv);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::createDefaultRenderWindow(RenderWindowPtr &window, bool isWindowApp)
    {
        TResult ret = T3D_OK;

        do
        {
            RenderWindowDesc param;

            param.externalHandle = nullptr;
            
            // 窗口标题
            param.Title = mSettings.renderSettings.title;
            // 窗口位置
            param.Left = mSettings.renderSettings.x;
            param.Top = mSettings.renderSettings.y;
            // 窗口大小
            param.Width = mSettings.renderSettings.width;
            param.Height = mSettings.renderSettings.height;
            // 是否全屏
            param.IsFullscreen = mSettings.renderSettings.fullscreen;
            // 色深，如果是非全屏，自动跟桌面一样
            param.ColorDepth = mSettings.renderSettings.colorDepth;
            // 图标路径
            param.IconPath = mSettings.renderSettings.iconPath;
            // 抗锯齿
            //param.MSAA = mSettings.renderSettings.MSAA;
            param.MSAA.Count = 4;
            param.MSAA.Quality = 0;
            // 垂直同步
            param.IsVsync = mSettings.renderSettings.vsync;
            // 是否可调整大小
            param.IsResizable = mSettings.renderSettings.resizable;

            std::stringstream ss;
            ss << "Tiny3D " << getVersionName() << "(" << getVersionString();
            ss << ")" << " - " << param.Title;
            param.Title = ss.str();
            if (isWindowApp)
            {
                window = RenderWindow::create(param.Title, param);
            }
            else
            {
                window = NullRenderWindow::create(param.Title);
            }
            if (window == nullptr)
            {
                ret = T3D_ERR_RENDER_CREATE_WINDOW;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Create render window failed !");
                break;
            }

            mDefaultWindow = window;
            // T3D_RENDER_PIPELINE.attachRenderTarget(mDefaultWindow);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::addRenderWindow(RenderWindowPtr window)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (mActiveRHIRenderer == nullptr)
            {
                ret = T3D_ERR_SYS_NOT_INIT;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Do not set active renderer !");
                break;
            }

            mRenderWindows.emplace(window->getName(), window);
            // ret = mActiveRHIRenderer->getContext()->attachRenderTarget(window);
        } while (0);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::removeRenderWindow(const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mActiveRHIRenderer == nullptr)
            {
                ret = T3D_ERR_SYS_NOT_INIT;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Do not set active renderer !");
                break;
            }

            mRenderWindows.erase(name);
            // ret = mActiveRHIRenderer->getContext()->detachRenderTarget(name);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    RenderWindowPtr Agent::getRenderWindow(const String &name) const
    {
        RenderWindowPtr window = nullptr;
        
        if (mActiveRHIRenderer == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_ENGINE, "Do not set active renderer !");
        }
        else
        {
            // window = mActiveRHIRenderer->getContext()->getRenderTarget(name);
        }

        return window;
    }

    //--------------------------------------------------------------------------
    
    bool Agent::run()
    {
        Application *theApp = Application::getInstancePtr();

        while (mIsRunning)
        {
            // 帧开始
            beginFrame();
            
            // 轮询系统事件
            mIsRunning = theApp->pollEvents();

            // 更新
            update();

            // 渲染一帧
            renderOneFrame(
                [theApp]() { theApp->onPreRender(); },
                [theApp]() { theApp->onRender(); },
                [theApp]() { theApp->onPostRender(); });

            // 帧结束
            endFrame();
        }

        theApp->applicationWillTerminate();

        return true;
    }

    //--------------------------------------------------------------------------
    
    void Agent::renderOneFrame()
    {
        renderOneFrame(nullptr, nullptr, nullptr);
    }

    //--------------------------------------------------------------------------

    void Agent::renderOneFrame(const PreEngineRender &preRender, const OnEngineRender &onRender, const PostEngineRender &postRender)
    {
        if (preRender != nullptr)
        {
            preRender();
        }

        auto ctx = mActiveRHIRenderer->getContext();

        // 开始一帧渲染（Vulkan: waitFence + acquire + beginCmdBuf）
        ctx->beginRender();

        if (mRenderPipeline != nullptr)
        {
            // 剔除
            mRenderPipeline->cull(mSceneMgr->getCurrentScene());
            
            // 渲染
            mRenderPipeline->render(ctx);
        }

        // 提交之前最后的录制机会：blit / copyBuffer / GPU 读回的 Copy 都在这里发起
        if (onRender != nullptr)
        {
            onRender();
        }

        // 结束一帧渲染（Vulkan: endCmdBuf + submit）
        ctx->endRender();

        if (postRender != nullptr)
        {
            postRender();
        }
        
        for (auto win : mRenderWindows)
        {
            win.second->swapBuffers();
        }

        mRenderStateMgr->GC();
        mRenderBufferMgr->GC();
    }

    //--------------------------------------------------------------------------

    void Agent::flushRHICommands()
    {
#if (T3D_ENABLE_RHI_THREAD)
        // 线程没起来或者已经停了，命令本来就是在主线程同步执行的，队列里不会有东西。
        // 这种情况下还去 resume，只会挂死在等一个永远不会被触发的事件上
        if (mRHIRunnable == nullptr || !mRHIRunnable->isRunning())
        {
            return;
        }

        mRHIRunnable->resume();
        mRHIEvent.wait();
#endif
    }

    //--------------------------------------------------------------------------

    void Agent::drainRHICommands()
    {
        // 一次 flush 只把入队表推去执行，执行完的那条表要等下一次 exchange 才清空。
        // 连做两次，第二次的 exchange 会清掉第一次执行完的命令，两条表就都空了
        flushRHICommands();
        flushRHICommands();
    }

    //--------------------------------------------------------------------------

    void Agent::syncRHIThread()
    {
#if (T3D_ENABLE_RHI_THREAD)
        if (mRHIRunnable == nullptr || !mRHIRunnable->isRunning())
        {
            // 命令本来就在主线程同步执行完了，没有需要推的队列
            return;
        }

        // beginFrame 发出的那批可能还在跑，必须等它自己结束。
        // 不等就 flush，exchange 会在 RHI 线程遍历命令表的同时把表清掉
        waitRHIBatch();

        drainRHICommands();
#endif
    }

    //--------------------------------------------------------------------------

    void Agent::waitRHIBatch()
    {
#if (T3D_ENABLE_RHI_THREAD)
        if (mRHIBatchInFlight)
        {
            mRHIEvent.wait();
            mRHIBatchInFlight = false;
        }
#endif
    }

    //--------------------------------------------------------------------------

    void Agent::postFrameEndTask(FrameEndTask task)
    {
        if (task == nullptr)
        {
            return;
        }

        mFrameEndTasks.push_back(std::move(task));
    }

    //--------------------------------------------------------------------------

    void Agent::runFrameEndTasks()
    {
        if (mFrameEndTasks.empty())
        {
            return;
        }

        // 走到这里 RHI 线程已经空闲，但本帧 renderOneFrame 刚入队的一整帧命令还
        // 一条都没执行，它们要等下一帧 beginFrame 才跑。任务基本都是销毁 GPU 资源，
        // 不先把这批命令推完，下一帧就会拿着已经销毁的资源去绘制
        flushRHICommands();

        // 任务里可能再投递任务（play 触发插件热重载就是这种），本帧只跑已投递的这批
        FrameEndTasks tasks;
        tasks.swap(mFrameEndTasks);

        for (auto &task : tasks)
        {
            task();
        }
    }

    //--------------------------------------------------------------------------

    void Agent::initTime()
    {
        if (mTime == nullptr)
        {
            // 构造即注册 Singleton（Singleton<Time> 在构造函数写入 m_pInstance）
            mTime = new Time();
        }

        // 即便 Agent 是 Time 友元，也一律走 setter 注入，保持不变量 / 校验的单一入口
        mTime->setFixedDeltaTime(mSettings.timeSettings.fixedDeltaTimeMS);
        mTime->setMaximumDeltaTime(mSettings.timeSettings.maximumDeltaTimeMS);
        mTime->setTimeScale(mSettings.timeSettings.timeScalePermille);
        mTime->start();
    }

    //--------------------------------------------------------------------------

    void Agent::beginFrame()
    {
        // 每帧最先推进时间，保证本帧 update() 读到的 dt 已就绪
        if (Time::getInstancePtr() != nullptr)
        {
            T3D_TIME.tick();
        }
#if (T3D_ENABLE_RHI_THREAD)
        T3D_RHI_THREAD.resume();
        mRHIBatchInFlight = true;
#endif
    }

    //--------------------------------------------------------------------------

    void Agent::update()
    {
        // 事件系统派发事件
        T3D_EVENT_MGR.dispatchEvent();

        // 更新场景树。由 Agent 编排帧内调度（stepFixed 为 Time 私有、仅 Agent 友元可调）
        Scene *scene = nullptr;
        if (mSceneMgr != nullptr)
        {
            scene = mSceneMgr->getCurrentScene();
        }
        if (scene != nullptr)
        {
            // 首帧前统一 flush onStart
            scene->flushPendingStart();

            // 固定步长循环（含单帧步数上限）→ Behaviour::onFixedUpdate
            uint32_t steps = 0;
            while (T3D_TIME.stepFixed() && steps++ < kMaxFixedStepsPerFrame)
            {
                scene->fixedUpdate();
            }

            // 普通 Update + LateUpdate（Scene 内部编排）
            scene->update();
        }
    }

    //--------------------------------------------------------------------------

    void Agent::endFrame()
    {
        if (Input::getInstancePtr() != nullptr)
        {
            T3D_INPUT.endFrame();
        }

        // 本帧若已经被 syncRHIThread 等过（GPU 读回路径），这里就不能再等一次
        waitRHIBatch();

        // 更新动画
        if (mAniPlayerMgr != nullptr)
        {
            mAniPlayerMgr->update();
        }
        
        // 异步赋值
        mAssignableObjMgr->assign();

        // 清理要删除的对象
        GameObject::destroyComponents();
        GameObject::destroyGameObjects();

        // RHI 线程空闲，延迟销毁也落实完了，此刻才是销毁 GPU 资源的安全点
        runFrameEndTasks();
    }

    //--------------------------------------------------------------------------

    bool Agent::runForEditor(const EditorRunningData &updateData)
    {
        Application *theApp = Application::getInstancePtr();

        while (mIsRunning)
        {
            // 帧开始
            beginFrame();
            
            // 轮询系统事件
            // if (updateData.pollEvents != nullptr)
            // {
            //     mIsRunning = updateData.pollEvents();
            //     T3D_PLATFORM.poll();
            // }
            
            // 轮询系统事件
            mIsRunning = theApp->pollEvents();
            
            // 更新
            update();

            if (updateData.update != nullptr)
            {
                updateData.update();
            }

            // 渲染一帧。两个来源都非空时先跑编辑器回调，再跑 Application 的
            renderOneFrame(
                combineEngineRender(updateData.preRender, [theApp]() { theApp->onPreRender(); }),
                combineEngineRender(updateData.onRender, [theApp]() { theApp->onRender(); }),
                combineEngineRender(updateData.postRender, [theApp]() { theApp->onPostRender(); }));

            // 帧结束
            endFrame();
        }

        theApp->applicationWillTerminate();

        return true;
    }

    //--------------------------------------------------------------------------

    void Agent::appWillEnterForeground()
    {
        T3D_LOG_ENTER_FOREGROUND();
    }

    //--------------------------------------------------------------------------

    void Agent::appDidEnterBackground()
    {
        T3D_LOG_ENTER_BACKGROUND();
        if (Input::getInstancePtr() != nullptr)
        {
            T3D_INPUT.reset();
        }
    }
    
    //--------------------------------------------------------------------------

    bool Agent::processEvents(const AppEvent &event)
    {
        if (event.type == APP_WINDOWEVENT)
        {
            switch (event.window.event)
            {
            case APP_WINDOWEVENT_RESIZED:
                {
                    mDefaultWindow->resize(event.window.data1, event.window.data2);
                }
                break;
            }
        }
        return true;
    }

    //--------------------------------------------------------------------------
    
    TResult Agent::installPlugin(Plugin *plugin)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (plugin == nullptr)
            {
                // 空指针
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid plugin !!!");
                break;
            }

            auto rval = mPlugins.emplace(PluginsValue(plugin->getName(), plugin));
            if (!rval.second)
            {
                ret = T3D_ERR_PLG_DUPLICATED;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Duplicated plugin [%s] !",
                    plugin->getName().c_str());
                break;
            }

            // 安装插件
            ret = plugin->install();
            if (T3D_FAILED(ret))
            {
                mPlugins.erase(plugin->getName());
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Install plugin [%s] failed !",
                    plugin->getName().c_str());
                break;
            }

            // 启动插件
            ret = plugin->startup();
            if (T3D_FAILED(ret))
            {
                mPlugins.erase(plugin->getName());
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Startup plugin [%s] failed !",
                    plugin->getName().c_str());
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::uninstallPlugin(Plugin *plugin)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (plugin == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid plugin !!!");
                break;
            }

            ret = plugin->shutdown();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Shutdown plugin [%s] failed !",
                    plugin->getName().c_str());
                break;
            }

            ret = plugin->uninstall();
            if (T3D_FAILED(ret))
            {

                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Uninstall plugin [%s] failed !",
                    plugin->getName().c_str())
                break;
            }

            mPlugins.erase(plugin->getName());
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::loadPlugin(const String &name)
    {
        return loadPluginFromPath(name, "");
    }

    //--------------------------------------------------------------------------

    TResult Agent::loadPluginFromPath(const String &name, const String &dir)
    {
        T3D_LOG_INFO(LOG_TAG_ENGINE, "Load plugin %s ...", name.c_str());

        TResult ret = T3D_OK;

        do 
        {
            auto rval = mDylibs.find(name);
            if (rval != mDylibs.end())
            {
                // 已经加载过了，直接返回吧
                T3D_LOG_INFO(LOG_TAG_ENGINE,
                    "Load plugin [%s] but it already loaded !", name.c_str());
                break;
            }
            
            DylibPtr dylib = smart_pointer_cast<Dylib>(T3D_DYLIB_MGR.loadDylib(name, dir));
            if (dylib == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            if (dylib->getType() != Resource::Type::kDylib)
            {
                ret = T3D_ERR_PLG_NOT_DYLIB;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Load plugin [%s] failed !", 
                    name.c_str());
                break;
            }

            DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)(dylib->getSymbol("dllStartPlugin"));
            if (pFunc == nullptr)
            {
                ret = T3D_ERR_PLG_NO_FUNCTION;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, 
                    "Load plugin [%s] get function dllStartPlugin failed !", 
                    name.c_str());
                break;
            }

            ret = pFunc();
            if (T3D_FAILED(ret))
            {
                break;
            }

            mDylibs.insert(DylibsValue(dylib->getName(), dylib));
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::unloadPlugin(const String &name)
    {
        T3D_LOG_INFO(LOG_TAG_ENGINE, "Unload plugin %s ...", name.c_str());

        TResult ret = T3D_OK;

        // FreeLibrary 之前队列必须是空的：命令对象的析构函数属于当初实例化
        // ENQUEUE_UNIQUE_COMMAND 的那个模块，卸载之后再析构就是在已卸载的代码段上跳转
        drainRHICommands();

        do 
        {
            DylibsItr itr = mDylibs.find(name);
            if (itr == mDylibs.end())
            {
                ret = T3D_ERR_PLG_NOT_EXISTS;
                T3D_LOG_ERROR(LOG_TAG_ENGINE,
                    "Unload plugin [%s] , it don't exist !",
                    name.c_str());
                break;
            }

            DylibPtr dylib = itr->second;
            DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(dylib->getSymbol("dllStopPlugin"));
            if (pFunc == nullptr)
            {
                ret = T3D_ERR_PLG_NO_FUNCTION;
                T3D_LOG_ERROR(LOG_TAG_ENGINE,
                    "Unload plugin [%s], get function dllStopPlugin failed !",
                    name.c_str());
                break;
            }

            ret = pFunc();
            if (T3D_FAILED(ret))
            {
                break;
            }

            mDylibs.erase(itr);

            T3D_DYLIB_MGR.unloadDylib(dylib);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::initApplication()
    {
        TResult ret = T3D_OK;

        do
        {
            Application *theApp = Application::getInstancePtr();
            if (theApp == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Application instance did not created !")
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            ret = theApp->init();
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult Agent::initLogSystem()
    {
        TResult ret = T3D_ERR_FAIL;

        mLogger = T3D_NEW Logger();

        if (mLogger != nullptr)
        {
            ret = mLogger->startup(1000, "Agent", true, true);
        }

        T3D_LOG_INFO(LOG_TAG_ENGINE, "Start Tiny3D - %s(%s) ...... version %s", getVersionName(), getVersionString(), T3D_DEVICE_INFO.getSoftwareVersion().c_str());
        T3D_LOG_INFO(LOG_TAG_ENGINE, "System Information : \n%s", T3D_DEVICE_INFO.getSystemInfo().c_str());

#if !defined(T3D_SIMD_DISABLED)
        T3D_LOG_INFO(LOG_TAG_ENGINE, "SIMD is enabled !")
#else
        T3D_LOG_INFO(LOG_TAG_ENGINE, "SIMD is disabled !")
#endif

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult Agent::initEventSystem(uint32_t maxEvent)
    {
        mEventMgr = T3D_NEW EventManager(maxEvent);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Agent::initObjectTracer()
    {
        mObjTracer = T3D_NEW ObjectTracer(true);
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult Agent::initManagers()
    {
        mAssignableObjMgr = AssignableObjectManager::create();
        mAniPlayerMgr = AnimationPlayerMgr::create();
        mImageCodec = ImageCodec::create();
        mSerializerMgr = SerializerManager::create();
        mSerializerMgr->setFileMode(SerializerManager::FileMode::kText);
        mRenderStateMgr = RenderStateManager::create();
        mRenderBufferMgr = RenderBufferManager::create();
        mDylibMgr = DylibManager::create();
        mMeshMgr = MeshManager::create();
        mSkeletonMgr = SkeletonManager::create();
        mAnimationMgr = AnimationManager::create();
        mPrefabMgr = PrefabManager::create();
        mShaderMgr = ShaderManager::create();
        mTextureMgr = TextureManager::create();
        mMaterialMgr = MaterialManager::create();
        mSceneMgr = SceneManager::create();
        mSceneMgr->setSceneManagerImpl(BuiltinSceneManager::create());
        mImageMgr = ImageManager::create();
        mAssetMgr = AssetManager::create();
        mRenderPipeline = ForwardRenderPipeline::create();

        mInput = T3D_NEW Input();
        if (Application::getInstancePtr() != nullptr)
        {
            Application::getInstance().addEventListener(mInput);
        }

        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    TResult Agent::loadConfig(const String &cfgPath)
    {
        TResult ret = T3D_OK;

// #if defined (T3D_OS_ANDROID)
//         // Android，只能读取apk包里面的文件
//         ret = loadPlugin("ZipArchive");
//         if (T3D_FAILED(ret))
//         {
//             return ret;
//         }
//
//         String apkPath = Dir::getAppPath();
//         ArchivePtr archive = mArchiveMgr->loadArchive(apkPath, "Zip");
//         ConfigFile cfgFile("assets/" + cfgPath, archive);
//         ret = cfgFile.loadXML(mSettings);
// #else
//         // 其他不需要从 apk 包里面读取文件的
//         String path = mAppPath + cfgPath;
//         
// #endif

        do
        {
            // ret = loadPlugin("FileSystemArchive");
            // if (T3D_FAILED(ret))
            // {
            //     break;
            // }
            //
            // SerializablePtr res = smart_pointer_cast<Serializable>(T3D_SERIALIZABLE_MGR.load("MetaFileSystemArchive", "Tiny3D.cfg"));
            // mSettings = res->instantiateAsObject<Settings>();

            mArchiveMgr = ArchiveManager::create();

#if defined (T3D_OS_ANDROID)
            // 使用 AAssetManager 读取 assets/ 中的配置
            ret = loadPlugin("AndroidAssetArchive");
            if (T3D_FAILED(ret))
            {
                break;
            }

            ArchivePtr archive = mArchiveMgr->loadArchive(
                "", "AndroidAsset", Archive::AccessMode::kRead);
#else
            // 桌面平台：文件系统
            ret = loadPlugin("FileSystemArchive");
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ArchivePtr archive = mArchiveMgr->loadArchive(mAppPath, "FileSystem", Archive::AccessMode::kRead);
#endif
            ret = archive->read(cfgPath,
                [this](DataStream &stream, const String &name, void *userData)
            {
                return T3D_SERIALIZER_MGR.deserialize(stream, mSettings);
            },
            nullptr);
            
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            // MemoryDataStream stream;
            // ret = archive->read(cfgPath, stream);
            // if (T3D_FAILED(ret))
            // {
            //     break;
            // }
            
            

        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::loadPlugins()
    {
        TResult ret = T3D_OK;

        do 
        {
            const String &pluginPath = mSettings.pluginSettings.pluginPath;
            // 绝对路径直接用：游戏 sln 里的 TinyPlayer 不在编辑器 bin 下，
            // 插件要去 SDK 目录找。相对路径仍按「应用目录 + pluginPath」拼。
            const bool absolute = !pluginPath.empty()
                && (pluginPath[0] == '/' || pluginPath[0] == '\\'
                    || (pluginPath.length() >= 2 && pluginPath[1] == ':'));
            mPluginsPath = absolute
                ? pluginPath
                : (mAppPath + Dir::getNativeSeparator() + pluginPath);

            auto itr = mSettings.pluginSettings.plugins.begin();
            while (itr != mSettings.pluginSettings.plugins.end())
            {
                const String &name = *itr;
                ret = loadPlugin(name);
                if (T3D_FAILED(ret))
                {
                    break;
                }

                ++itr;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::unloadPlugins()
    {
        TResult ret = T3D_OK;

        drainRHICommands();

        DylibsItr itr = mDylibs.begin();
        while (itr != mDylibs.end())
        {
            DylibPtr dylib = itr->second;
            DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(dylib->getSymbol("dllStopPlugin"));
            if (pFunc != nullptr)
            {
                ret = pFunc();
                if (ret == T3D_OK)
                {
                    T3D_DYLIB_MGR.unloadDylib(dylib);
                }
            }
            ++itr;
        }

        mDylibs.clear();

        return ret;
    }
    
    //--------------------------------------------------------------------------

    void Agent::enumerateAvailableRenderers(RHIRenderers &renderers) const
    {
        renderers.clear();
        renderers = mRenderers;
    }

    //--------------------------------------------------------------------------

    TResult Agent::setActiveRHIRenderer(RHIRendererPtr renderer)
    {
        TResult ret = T3D_OK;

        if (mActiveRHIRenderer != renderer)
        {
            if (mRenderPipeline != nullptr)
            {
                mRenderPipeline->destroy();
            }

            stopRenderThread();
            
            if (mActiveRHIRenderer != nullptr)
            {
                mActiveRHIRenderer->destroy();
            }

            mRHIRunnable = nullptr;

            ret = renderer->init();

            if (ret == T3D_OK)
            {
                mActiveRHIRenderer = renderer;
            }

            ret = initRenderThread();
            if (T3D_FAILED(ret))
            {
                mActiveRHIRenderer = nullptr;
                return ret;
            }

            if (mRenderPipeline != nullptr)
            {
                ret = mRenderPipeline->init();
                if (T3D_FAILED(ret))
                {
                    stopRenderThread();
                    mActiveRHIRenderer = nullptr;
                    mRHIRunnable = nullptr;
                    return ret;
                }
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    RHIRendererPtr Agent::getActiveRHIRenderer() const
    {
        return mActiveRHIRenderer;
    }

    //--------------------------------------------------------------------------

    RHIContextPtr Agent::getActiveRHIContext() const 
    { 
        return mActiveRHIRenderer->getContext();
    }

    //--------------------------------------------------------------------------

    TResult Agent::addRHIRenderer(RHIRendererPtr renderer)
    {
        const String &name = renderer->getName();
        auto r = mRenderers.insert(RHIRenderersValue(name, renderer));
        if (r.second)
        {
            return T3D_OK;
        }

        return T3D_ERR_DUPLICATED_ITEM;
    }

    //--------------------------------------------------------------------------

    TResult Agent::removeRHIRenderer(RHIRendererPtr renderer)
    {
        auto itr = mRenderers.find(renderer->getName());

        if (itr == mRenderers.end())
        {
            return T3D_ERR_NOT_FOUND;
        }

        mRenderers.erase(itr);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIRendererPtr Agent::getRHIRenderer(const String &name) const
    {
        RHIRendererPtr renderer = nullptr;
        auto itr = mRenderers.find(name);

        if (itr != mRenderers.end())
        {
            renderer = itr->second;
        }

        return renderer;
    }

    //--------------------------------------------------------------------------

    TResult Agent::initRenderer()
    {
        TResult ret = T3D_OK;

        do 
        {
            const String &rendererName = mSettings.renderSettings.renderer;

            // 设置当前要使用的渲染器
            RHIRendererPtr renderer = getRHIRenderer(rendererName);
            if (renderer == nullptr)
            {
                ret = T3D_ERR_PLG_NOT_LOADED;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Renderer [%s] did not load !",
                    rendererName.c_str());
                break;
            }

            ret = setActiveRHIRenderer(renderer);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult Agent::initRenderThread()
    {
        TResult ret = T3D_OK;

        do
        {
            mRHIRunnable = RHIThread::create();
#if (T3D_ENABLE_RHI_THREAD)
            ret = mRHIThread.start(mRHIRunnable, "RHIThread");
            T3D_LOG_INFO(LOG_TAG_ENGINE, "Start RHI Thread [%d] !", ret);
#else
            T3D_LOG_INFO(LOG_TAG_ENGINE, "Disable RHI Thread !");
#endif
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void Agent::stopRenderThread()
    {
        if (mRHIRunnable != nullptr)
        {
#if (T3D_ENABLE_RHI_THREAD)
            // flush 最后一帧入队但未执行的 command，避免退出时 writeBuffer
            // 等深拷贝命令的 Buffer 在 command 析构时未释放导致内存泄漏
            T3D_RHI_THREAD.resume();
            mRHIEvent.wait();
#endif
            mRHIRunnable->stop();
            mRHIThread.wait();
        }
    }

    //--------------------------------------------------------------------------

    void Agent::resumeEngineThread()
    {
#if (T3D_ENABLE_RHI_THREAD)
        mRHIEvent.trigger();
#endif
    }

    //--------------------------------------------------------------------------

    TResult Agent::addImageCodec(ImageCodecBase::FileType type, ImageCodecBase *codec)
    {
        return mImageCodec->addImageCodec(type, codec);
    }

    //--------------------------------------------------------------------------

    TResult Agent::removeImageCodec(ImageCodecBase::FileType type)
    {
        return mImageCodec->removeImageCodec(type);
    }

    //--------------------------------------------------------------------------

    T3D_INSTANTIATE_SINGLETON(Agent)
}
