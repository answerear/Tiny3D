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


#include "ProjectManager.h"
#include "EditorSceneImpl.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)
    
    //--------------------------------------------------------------------------

    const char *ProjectManager::ASSETS = "Assets";
    const char *ProjectManager::SCENES = "Scenes";
    const char *ProjectManager::TEMP = "Temp";
    const char *ProjectManager::PROJECT_SETTINGS_NAME = "ProjectSettings.tdat";
    
    //--------------------------------------------------------------------------

    ProjectManager::ProjectManager()
    {
        // mFSMonitor = new FileSystemMonitor();
    }
    
    //--------------------------------------------------------------------------

    ProjectManager::~ProjectManager()
    {
        // T3D_SAFE_DELETE(mFSMonitor);
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::setupBuiltinAssets(const String &tempPath)
    {
        TResult ret = T3D_OK;

        do
        {
            // 复制 builtin assets 到 temp 文件夹
            String builtinPath = Dir::getAppPath() + Dir::getNativeSeparator() + "Editor" + Dir::getNativeSeparator() + "builtin";
            
            // textures
            String srcPath = builtinPath + Dir::getNativeSeparator() + "textures";
            String dstPath = tempPath + Dir::getNativeSeparator() + "builtin";
            bool rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                EDITOR_LOG_ERROR("Copy builtin textures from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }
            
            // shaders
            srcPath = builtinPath + Dir::getNativeSeparator() + "shaders";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                EDITOR_LOG_ERROR("Copy builtin shaders from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }
            
            // materials
            srcPath = builtinPath + Dir::getNativeSeparator() + "materials";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                EDITOR_LOG_ERROR("Copy builtin materials from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }

            // meshes
            srcPath = builtinPath + Dir::getNativeSeparator() + "meshes";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                EDITOR_LOG_ERROR("Copy builtin meshes from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }

            // Builtin 资源档案系统
            mBuiltinArchive = T3D_ARCHIVE_MGR.loadArchive(dstPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadTxtTruncate);
            if (mBuiltinArchive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load builtin fs archive [%s]", builtinPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::createSimpleScene(const String &assetsPath)
    {
        TResult ret = T3D_OK;

        do
        {
            ScenePtr scene = T3D_SCENE_MGR.createScene("SimpleScene");
            EDITOR_SCENE.setRuntimeScene(scene);
            scene->init();

            // 根节点
            EDITOR_SCENE.getEditorRootTransform()->addChild(scene->getRootTransform());

            Transform3D *gameNode = scene->getRootTransform();

            // render window for render target in camera
            RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
            RenderTargetPtr rt = RenderTarget::create(rw);

            // transform node for camera
            GameObjectPtr go = GameObject::create("Camera");
            Transform3DPtr xform = go->addComponent<Transform3D>();
            gameNode->addChild(xform);
        
            // camera component
            CameraPtr camera = go->addComponent<Camera>();
            camera->setOrder(0);
            Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
            camera->setViewport(vp);
            camera->setClearColor(ColorRGB(0.133f, 0.231f, 0.329f));
            camera->setRenderTarget(rt);
            Real as = Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
            camera->setAspectRatio(as);
        
            // camera for perspective
            camera->setProjectionType(Camera::Projection::kPerspective);
            camera->setFovY(Radian(Math::PI / 3.0f));
            camera->setNearPlaneDistance(0.1f);
            camera->setFarPlaneDistance(1000.0f);
        
            // construct camera position & orientation & scaling
            Vector3 eye(2.0f, 2.0f, -4.0f);
            Vector3 obj(0.0f, 0.0f, 0.0f);
            camera->lookAt(eye, obj, Vector3::UP);

            // construct frustum bound
            auto frustum = go->addComponent<FrustumBound>();
            T3D_ASSERT(frustum != nullptr);

            const String sceneFolder = "Scenes";
            String scenePath = assetsPath + Dir::getNativeSeparator() + sceneFolder;
            if (!Dir::makeDir(scenePath))
            {
                EDITOR_LOG_ERROR("Failed to create scene directory [%s]", scenePath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(assetsPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kTruncate);
            if (archive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load archive [%s]", scenePath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 保存场景，作为新生成的场景文件
            String filename = sceneFolder + Dir::getNativeSeparator() + scene->getName() + "." + Resource::EXT_SCENE;
            ret = T3D_SCENE_MGR.saveScene(archive, filename, scene);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to save scene [%s]", scenePath.c_str());
                break;
            }

            // 更新工程设置
            mProjectSettings.StartupSceneUUID = scene->getUUID();
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::createProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            // 创建工程文件夹
            String projectPath = path + Dir::getNativeSeparator() + name;
            if (!Dir::makeDir(projectPath))
            {
                EDITOR_LOG_ERROR("Failed to create project folder (%s) !", projectPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 创建资产文件夹
            String assetsPath = projectPath + Dir::getNativeSeparator() + ASSETS;
            if (!Dir::makeDir(assetsPath))
            {
                EDITOR_LOG_ERROR("Failed to create assets folder (%s) !", assetsPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // Assets 档案系统
            mAssetsArchive = T3D_ARCHIVE_MGR.loadArchive(assetsPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadTruncate);
            if (mAssetsArchive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load assets fs archive [%s]", assetsPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            // 创建 Temp 文件夹
            String tempPath = projectPath + Dir::getNativeSeparator() + TEMP;
            if (!Dir::makeDir(tempPath))
            {
                EDITOR_LOG_ERROR("Failed to create temporary folder (%s) !", tempPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 设置内置资产
            ret = setupBuiltinAssets(tempPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to setup builtin assets !");
                break;
            }

            // 预编译工程 shaders 和内置 shaders
            ret = compileAllShaders(tempPath, assetsPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to compile shaders !");
                break;
            }

#if !defined (TEST_SCENE_ENABLE)
            // 创建简单的场景
            ret = createSimpleScene(assetsPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create simple scene !");
                break;
            }
#endif
            
            mPath = projectPath;
            mName = name;
            mAssetsPath = assetsPath;
            mTempPath = tempPath;

            // 保存工程设置
            ret = saveProjectSettings();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to save project settings !");
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::openProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            // 工程目录
            String projectPath = path + Dir::getNativeSeparator() + name;
            if (!Dir::exists(projectPath))
            {
                // 工程目录不存在
                EDITOR_LOG_ERROR("Open project [%s] failed !", projectPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            // 资源目录
            String assetsPath = projectPath + Dir::getNativeSeparator() + ASSETS;
            if (!Dir::exists(assetsPath))
            {
                // 资源目录不存在
                EDITOR_LOG_ERROR("Assets folder [%s] did not exist !", assetsPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            // Assets 档案系统
            mAssetsArchive = T3D_ARCHIVE_MGR.loadArchive(assetsPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadTxtTruncate);
            if (mAssetsArchive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load assets fs archive [%s]", assetsPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            // 临时目录
            String tempPath = projectPath + Dir::getNativeSeparator() + TEMP;
            if (!Dir::exists(tempPath))
            {
                // 临时目录不存在，重新创建
                EDITOR_LOG_WARNING("Temporary folder [%s] did not exist ! Create that !", tempPath.c_str());
                if (!Dir::makeDir(tempPath))
                {
                    EDITOR_LOG_ERROR("Failed to create temporary folder [%s] !", tempPath.c_str());
                    break;
                }
            }

            // 设置内置资产
            ret = setupBuiltinAssets(tempPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to setup builtin assets !");
                break;
            }

            // 预编译工程 shaders 和内置 shaders
            ret = compileAllShaders(tempPath, assetsPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to compile shaders !");
                break;
            }

            mPath = projectPath;
            mName = name;
            mAssetsPath = assetsPath;
            mTempPath = tempPath;

            // 加載工程設置
            ret = loadProjectSettings();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to load project settings !");
                break;
            }

            // 加载启动场景
            ret = loadStartupScene();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to load startup scene !");
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::closeProject()
    {
        T3D_SCENE_MGR.unloadScene();
        EDITOR_SCENE.setRuntimeScene(nullptr);
        
        mPath.clear();
        mName.clear();
        mAssetsPath.clear();
        mTempPath.clear();
        mCompiledShadersPath.clear();

        T3D_ARCHIVE_MGR.unloadArchive(mAssetsArchive);
        mAssetsArchive = nullptr;

        T3D_ARCHIVE_MGR.unloadArchive(mBuiltinArchive);
        mBuiltinArchive = nullptr;

        T3D_ARCHIVE_MGR.unloadArchive(mCompiledShadersArchive);
        mCompiledShadersArchive = nullptr;
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::saveProjectSettings()
    {
        TResult ret = T3D_OK;

        do
        {
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(mPath, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            if (archive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load archive [%s]", mPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

            ret = archive->write(PROJECT_SETTINGS_NAME,
                [this](DataStream &stream, const String &filename, void *userData)
                {
                    return T3D_SERIALIZER_MGR.serialize(stream, mProjectSettings);
                },
                nullptr);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::loadProjectSettings()
    {
        TResult ret = T3D_OK;

        do
        {
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(mPath, ARCHIVE_TYPE_FS, Archive::AccessMode::kRead);
            if (archive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load archive [%s] !", mPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

            ret = archive->read(PROJECT_SETTINGS_NAME,
                [this](DataStream &stream, const String &filename, void *userData)
                {
                    return T3D_SERIALIZER_MGR.deserialize(stream, mProjectSettings);
                },
                nullptr);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::loadStartupScene()
    {
        TResult ret = T3D_OK;

        do
        {
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(mAssetsPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kRead);
            if (archive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load archive [%s] !", mPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            ScenePtr scene = T3D_SCENE_MGR.loadScene(archive, mProjectSettings.StartupSceneUUID);
            if (scene == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load scene (uuid: %s) !", mProjectSettings.StartupSceneUUID.toString().c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            // EDITOR_SCENE.setRuntimeScene(scene);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::compileAllShaders(const String &tempPath, const String &assetsPath)
    {
        TResult ret = T3D_OK;

        do
        {
            // 编译后 Shaders 存放路径
            String compiledShadersPath = tempPath + Dir::getNativeSeparator() + "shaders";
            if (!Dir::exists(compiledShadersPath))
            {
                if (!Dir::makeDir(compiledShadersPath))
                {
                    EDITOR_LOG_ERROR("Failed to create shaders folder (%s) !", compiledShadersPath.c_str());
                    ret = T3D_ERR_FAIL;
                    break;
                }
            }

            // 编译内置 shaders
            String builtinPath = tempPath + Dir::getNativeSeparator() + "builtin";
            ret = compileShaders(builtinPath, compiledShadersPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to compile builtin shaders !");
                break;
            }

            // 编译工程 shaders
            ret = compileShaders(assetsPath, compiledShadersPath);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to compile project shaders !");
                break;
            }

            // 编译后 shaders 档案系统
            mCompiledShadersArchive = T3D_ARCHIVE_MGR.loadArchive(compiledShadersPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadTxtTruncate);
            if (mCompiledShadersArchive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load compiled shaders fs archive [%s]", compiledShadersPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

            mCompiledShadersPath = compiledShadersPath;
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::compileShaders(const String &inputPath, const String &outputPath)
    {
        TResult ret = T3D_OK;

        String searchPath = inputPath + Dir::getNativeSeparator() + "*.shader";
        Dir dir;
        bool working = dir.findFile(searchPath);

        while (working)
        {
            if (dir.isDots())
            {
                // . or ..
            }
            else if (dir.isDirectory())
            {
                // directory
                ret = compileShaders(dir.getFilePath(), outputPath);
            }
            else
            {
                // file
                const String filePath = dir.getFilePath();
                ret = compileShader(filePath, outputPath);
            }

            if (T3D_FAILED(ret))
            {
                break;
            }
            
            working = dir.findNextFile();
        }

        dir.close();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::compileShader(const String &inputPath, const String &outputPath)
    {
        TResult ret = T3D_OK;

        do
        {
            EDITOR_LOG_INFO("Begin compiling shader %s ...", inputPath.c_str());
            
            // 从 meta 中读取出 UUID
            String metaPath = inputPath + ".meta";
            FileDataStream fs;
            if (!fs.open(metaPath.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
            {
                EDITOR_LOG_ERROR("Failed to open meta file [%s] !", metaPath.c_str());
                break;
            }

            MetaPtr meta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
            T3D_ASSERT(meta->getType() == Meta::Type::kShaderLab);
            
            fs.close();

            MetaShaderLab *metaShaderLab = (MetaShaderLab*)meta.get();
            
            // 使用 shader cross compiler 工具生成临时编译生成的 shader 文件
#if defined (T3D_OS_WINDOWS)
            String appPath = Dir::getAppPath() + Dir::getNativeSeparator() + "scc.exe";
            String cmdLine =  inputPath + " -t hlsl" + " -o " + outputPath + " -u " + metaShaderLab->getShaderUUID().toString();
#elif defined (T3D_OS_LINUX)
#elif defined (T3D_OS_MAC)
#endif

            Process process;
            ret = process.start(appPath, cmdLine);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to start scc.exe from source file (%s) ! ERROR [%d]", inputPath.c_str(), ret);
                break;
            }

            // 等待编译结束
            ret = process.wait();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to wait process exiting from source file (%s) ! ERROR [%d]", inputPath.c_str(), ret);
                break;
            }

            uint32_t exitCode = process.getExitCode();
            if (exitCode != 0)
            {
                // 编译出错了，只能退出
                EDITOR_LOG_ERROR("Failed to compile shader (%s) ! ERROR [%d]", inputPath.c_str(), ret);
                break;
            }

            EDITOR_LOG_INFO("Completed compiling shader !");
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
    
    NS_END
}

