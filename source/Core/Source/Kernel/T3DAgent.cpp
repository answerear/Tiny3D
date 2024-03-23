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
#include "Render/T3DRenderWindow.h"
#include "RHI/T3DRHIRenderer.h"
#include "RHI/T3DRHIThread.h"
#include "Render/T3DRenderResourceManager.h"
#include "Render/T3DForwardRenderPipeline.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DMeshManager.h"
#include "Resource/T3DScene.h"
#include "Resource/T3DSceneManager.h"
#include "Resource/T3DTextureManager.h"


namespace Tiny3D
{
    #define T3D_VERSION_0_0_0_1_STR         "0.0.0.1"
    #define T3D_VERSION_0_0_0_1_VAL         0x00000001
    #define T3D_VERSION_0_0_0_1_NAME        "Rosetta"

    #define T3D_VERSION_STR                 T3D_VERSION_0_0_0_1_STR
    #define T3D_VERSION_VAL                 T3D_VERSION_0_0_0_1_VAL
    #define T3D_VERSION_NAME                T3D_VERSION_0_0_0_1_NAME

    typedef TResult (*DLL_START_PLUGIN)(void);
    typedef TResult (*DLL_STOP_PLUGIN)(void);
    
    //--------------------------------------------------------------------------
    
    Agent::Agent()
    {
    }

    //--------------------------------------------------------------------------
    
    Agent::~Agent()
    {
        mRHIRunnable->stop();
        mRHIThread.wait();
        mRHIRunnable = nullptr;

        // mRenderPipeline->detachAllRenderTargets();
        
        mInternalArchive = nullptr;
        mProjectArchive = nullptr;
        
        mDefaultWindow = nullptr;
        mActiveRHIRenderer = nullptr;
        
        if (mPrefabMgr != nullptr)
        {
            mPrefabMgr->unloadAllResources();
            mPrefabMgr = nullptr;
        }

        if (mSceneMgr != nullptr)
        {
            mSceneMgr->unloadScene();
            mSceneMgr = nullptr;
        }

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

        if (mArchiveMgr != nullptr)
        {
            mArchiveMgr->unloadAllArchives();
        }

        mRenderPipeline = nullptr;
        mAssignableObjMgr = nullptr;
        mRenderStateMgr = nullptr;
        mRenderBufferMgr = nullptr;
        mRenderWindows.clear();

        // 卸载所有插件
        unloadPlugins();

        if (mDylibMgr != nullptr)
        {
            mDylibMgr->unloadAllResources();
            mDylibMgr = nullptr;
        }

        mSerializerMgr = nullptr;
        mArchiveMgr = nullptr;
        
        T3D_SAFE_DELETE(mEventMgr);

        if (mObjTracer != nullptr)
        {
            mObjTracer->dumpMemoryInfo();
        }
        T3D_SAFE_DELETE(mObjTracer);

        T3D_LOG_SHUTDOWN();
        T3D_SAFE_DELETE(mLogger);
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

    TResult Agent::initSystem(const String &appPath)
    {
        TResult ret = T3D_OK;

        do
        {
#if !defined (T3D_OS_ANDROID)
            // 获取应用程序路径、应用程序名称
            StringUtil::split(appPath, mAppPath, mAppName);
#endif

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

            // Android 单独设置插件路径，不使用配置文件里面设置的路径
            // 因为android的插件在/data/data/appname/lib文件下
            mPluginsPath = Dir::getLibraryPath();
#endif

            // 初始化事件系统
            ret = initEventSystem();
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

    TResult Agent::init(int32_t argc, char *argv[], bool autoCreateWindow, bool isWindowApp, const String &config)
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
            ret = initSystem(appPath);
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

            ret = initRenderThread();
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
            
            //ShaderPtr shader = mShaderMgr->createShader("test.shader");
            //ShaderKeyword keyword;
            //keyword.addKeyword("TEST1");
            //keyword.generate();
            //String code = 
            //    "static float4 gl_Position;\n"
            //    "struct SPIRV_Cross_Output\n"
            //    "{\n"
            //    "    float4 gl_Position : SV_Position;\n"
            //    "};\n"
            //    "\n"
            //    "void vert_main()\n"
            //    "{\n"
            //    "    gl_Position = float4(1.0f, 2.0f, 3.0f, 4.0f);\n"
            //    "}\n"
            //    "\n"
            //    "SPIRV_Cross_Output main()\n"
            //    "{\n"
            //    "    vert_main();\n"
            //    "    SPIRV_Cross_Output stage_output;\n"
            //    "    stage_output.gl_Position = gl_Position;\n"
            //    "    return stage_output;\n"
            //    "}\n";
            //Matrix4 mat;
            //ShaderConstantParamPtr constParam = ShaderConstantParam::create("WorldMatrix", &mat, sizeof(mat), 0, 0, ShaderConstantParam::DATA_TYPE::DT_FLOAT);
            //ShaderVariantPtr variant = ShaderVariant::create(std::forward<ShaderKeyword>(keyword), code);
            //variant->addConstant(constParam);

            //ShaderKeyword keyword1;
            //keyword1.addKeyword("TEST2");
            //keyword1.generate();
            //mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 2.0f;
            //ShaderConstantParamPtr constParam1 = ShaderConstantParam::create("ViewMatrix", &mat, sizeof(mat), 0, 0, ShaderConstantParam::DATA_TYPE::DT_FLOAT);
            //ShaderVariantPtr variant1 = ShaderVariant::create(std::forward<ShaderKeyword>(keyword1), code);
            //variant1->addConstant(constParam1);

            //shader->addShaderVariant(variant->getShaderKeyword(), variant);
            //shader->addShaderVariant(variant1->getShaderKeyword(), variant1);
            //shader->enableKeyword("TEST1");
            //
            //MemoryDataStream os(1024*1024);
            //T3D_SERIALIZER_MGR.serialize(os, shader);
            //size_t pos = os.tell();
            //uint8_t *data = nullptr;
            //size_t dataSize = 0;
            //os.getBuffer(data, dataSize);

            //FileDataStream fs;
            //if (fs.open("1.json", FileDataStream::E_MODE_TRUNCATE | FileDataStream::E_MODE_READ_WRITE | FileDataStream::E_MODE_TEXT))
            //{
            //    fs.write(data, dataSize);
            //    char c = 0;
            //    fs.write(&c, sizeof(char));
            //    fs.close();
            //}

            //MemoryDataStream is(data, pos, false);
            //ShaderPtr shader1 = T3D_SERIALIZER_MGR.deserialize<Shader>(is);

            //if (fs.open("2.json", FileDataStream::E_MODE_TRUNCATE | FileDataStream::E_MODE_READ_WRITE | FileDataStream::E_MODE_TEXT))
            //{
            //    fs.write(data, dataSize);
            //    char c = 0;
            //    fs.write(&c, sizeof(char));
            //    fs.close();
            //}

            mIsRunning = true;

            Application *theApp = Application::getInstancePtr();
            theApp->applicationDidFinishLaunching(argc, argv);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::init(int32_t argc, char *argv[], bool autoCreateWindow, bool isWindowApp, const Settings &settings)
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
            ret = initSystem(appPath);
            if (T3D_FAILED(ret))
            {
                break;
            }

            mSettings = settings;

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

            ret = initRenderThread();
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
            theApp->applicationDidFinishLaunching(argc, argv);
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
#if (T3D_ENABLE_RHI_THREAD)
            T3D_RHI_THREAD.resume();
#endif
            
            // 轮询系统事件
            mIsRunning = theApp->pollEvents();

            // if (!mIsRunning)
            //     break;

            // 事件系统派发事件
            T3D_EVENT_MGR.dispatchEvent();

            // 更新场景树
            if (mSceneMgr != nullptr && mSceneMgr->getCurrentScene() != nullptr)
            {
                mSceneMgr->getCurrentScene()->update();
            }

            // 渲染一帧
            renderOneFrame();

#if (T3D_ENABLE_RHI_THREAD)
            mRHIEvent.wait();
#endif

            // 异步赋值
            mAssignableObjMgr->assign();
        }

        theApp->applicationWillTerminate();

        return true;
    }

    void Agent::renderOneFrame()
    {
        if (mRenderPipeline != nullptr)
        {
            // 剔除
            mRenderPipeline->cull(mSceneMgr->getCurrentScene());
            
            // 渲染
            mRenderPipeline->render(mActiveRHIRenderer->getContext());
        }

        for (auto win : mRenderWindows)
        {
            win.second->swapBuffers();
        }

        mRenderStateMgr->GC();
        mRenderBufferMgr->GC();
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

            auto rval = mPlugins.insert(PluginsValue(plugin->getName(), plugin));
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
            
            DylibPtr dylib = smart_pointer_cast<Dylib>(T3D_DYLIB_MGR.loadDylib(name));
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
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            ret = theApp->init();
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (0);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult Agent::initLogSystem()
    {
        TResult ret = T3D_ERR_FAIL;

        mLogger = new Logger();

        if (mLogger != nullptr)
        {
            ret = mLogger->startup(1000, "Agent", true, true);
        }

        T3D_LOG_INFO(LOG_TAG_ENGINE, 
            "Start Tiny3D - %s(%s) ...... version %s",
            getVersionName(), getVersionString(),
            T3D_DEVICE_INFO.getSoftwareVersion().c_str());

        T3D_LOG_INFO(LOG_TAG_ENGINE, "System Information : \n%s",
            T3D_DEVICE_INFO.getSystemInfo().c_str());

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult Agent::initEventSystem()
    {
        mEventMgr = new EventManager(10);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Agent::initObjectTracer()
    {
        mObjTracer = new ObjectTracer(true);
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult Agent::initManagers()
    {
        mAssignableObjMgr = AssignableObjectManager::create();
        mRenderPipeline = ForwardRenderPipeline::create();
        mArchiveMgr = ArchiveManager::create();
        mSerializerMgr = SerializerManager::create();
        mSerializerMgr->setFileMode(SerializerManager::FileMode::kText);
        mRenderStateMgr = RenderStateManager::create();
        mRenderBufferMgr = RenderBufferManager::create();
        mDylibMgr = DylibManager::create();
        mMeshMgr = MeshManager::create();
        mPrefabMgr = PrefabManager::create();
        mShaderMgr = ShaderManager::create();
        mTextureMgr = TextureManager::create();
        mMaterialMgr = MaterialManager::create();
        mSceneMgr = SceneManager::create();
        mSceneMgr->setSceneManagerImpl(BuiltinSceneManager::create());
        
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
#if defined (T3D_OS_ANDROID)
            // Android，只能读取apk包里面的文件
            ret = loadPlugin("ZipArchive");
            if (T3D_FAILED(ret))
            {
                return ret;
            }
            
            String apkPath = Dir::getAppPath();
            ArchivePtr archive = mArchiveMgr->loadArchive(apkPath, "Zip", Archive::AccessMode::kRead);
#else
            // 其他不需要从 apk 包里读取文件的
            ret = loadPlugin("FileSystemArchive");
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ArchivePtr archive = mArchiveMgr->loadArchive(mAppPath, "FileSystem", Archive::AccessMode::kRead);
#endif

            MemoryDataStream stream;
            ret = archive->read(cfgPath, stream);
            if (T3D_FAILED(ret))
            {
                break;
            }

#if 1
            //RTTRVariant var(mSettings);
            //T3D_SERIALIZER_MGR.deserializeObject(stream, var);
            T3D_SERIALIZER_MGR.deserialize(stream, mSettings);
#else
            Settings *settings = T3D_SERIALIZER_MGR.deserialize<Settings>(stream);
            if (settings == nullptr)
            {
                break;
            }

            mSettings = *settings;
            T3D_SAFE_DELETE(settings);
#endif
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Agent::loadPlugins()
    {
        TResult ret = T3D_OK;

        do 
        {
#if !defined (T3D_OS_ANDROID)
            mPluginsPath = mAppPath + Dir::getNativeSeparator() + mSettings.pluginSettings.pluginPath;
#endif

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
            if (mActiveRHIRenderer != nullptr)
            {
                mActiveRHIRenderer->destroy();
            }

            ret = renderer->init();

            if (ret == T3D_OK)
            {
                mActiveRHIRenderer = renderer;
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
        } while (0);

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
            T3D_LOG_INFOLOG_TAG_ENGINE, "Disable RHI Thread !");
#endif
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void Agent::resumeEngineThread()
    {
#if (T3D_ENABLE_RHI_THREAD)
        mRHIEvent.trigger();
#endif
    }

    //--------------------------------------------------------------------------
}
