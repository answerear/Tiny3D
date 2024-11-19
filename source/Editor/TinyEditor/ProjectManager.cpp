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

    const char *ProjectManager::BUILTIN_CUBE_MESH_NAME = "cube.tmesh";
    const char *ProjectManager::BUILTIN_CUBE_SUBMESH_NAME = "#0";
    
    //--------------------------------------------------------------------------

    ProjectManager::ProjectManager()
    {
        // mFSMonitor = new FileSystemMonitor();
    }
    
    //--------------------------------------------------------------------------

    ProjectManager::~ProjectManager()
    {
        // T3D_SAFE_DELETE(mFSMonitor);
        closeProject();
        AssetNode::GC();
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

            mBuiltinPath = dstPath;
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
            EDITOR_SCENE.getRuntimeRootTransform()->addChild(scene->getRootTransform());

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
            // if (!Dir::makeDir(scenePath))
            // {
            //     EDITOR_LOG_ERROR("Failed to create scene directory [%s]", scenePath.c_str());
            //     ret = T3D_ERR_FAIL;
            //     break;
            // }

            AssetNode *folderNode = nullptr;
            ret = makeFolder(mAssetRoot, scenePath, folderNode);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create scene directory [%s]", scenePath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(assetsPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kTruncate);
            if (archive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load archive [%s]", assetsPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 保存场景，作为新生成的场景文件
            String filename = sceneFolder + Dir::getNativeSeparator() + scene->getName() + "." + Resource::EXT_SCENE;
            ret = T3D_SCENE_MGR.saveScene(archive, filename, scene);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to save scene [%s]", filename.c_str());
                break;
            }

            AssetNode *node = nullptr;
            String metaPath = filename + ".meta";
            ret = addFile(folderNode, metaPath, node);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to add file [%s]", filename.c_str());
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

            mPath = projectPath;
            mName = name;
            mAssetsPath = assetsPath;
            mTempPath = tempPath;

            mProjectSettings.ensure();

            // 构建工程文件树
            ret = populate();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to populate project tree [%s] !", mAssetsPath.c_str());
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

            // 保存工程设置
            ret = mProjectSettings.save();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to save project settings !");
            }

            mAssetRoot->debugOutput();
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

            mProjectSettings.ensure();

            // 加載工程設置
            ret = mProjectSettings.load();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to load project settings !");
                break;
            }


            // 构建工程文件树
            ret = populate();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to populate project tree [%s] !", mAssetsPath.c_str());
                break;
            }
            
#if !defined (TEST_SCENE_ENABLE)
            // 加载启动场景
            ret = loadStartupScene();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to load startup scene !");
                break;
            }

            mAssetRoot->debugOutput();
#endif
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::closeProject()
    {
        if (mAssetRoot != nullptr)
        {
            mAssetRoot->destroy();
            mAssetRoot = nullptr;
        }
        
        T3D_SCENE_MGR.unloadScene();
        EDITOR_SCENE.setRuntimeScene(nullptr);
        
        mPath.clear();
        mName.clear();
        mAssetsPath.clear();
        mTempPath.clear();
        mCompiledShadersPath.clear();

        if (mAssetsArchive != nullptr)
        {
            T3D_ARCHIVE_MGR.unloadArchive(mAssetsArchive);
            mAssetsArchive = nullptr;
        }

        if (mBuiltinArchive != nullptr)
        {
            T3D_ARCHIVE_MGR.unloadArchive(mBuiltinArchive);
            mBuiltinArchive = nullptr;
        }

        if (mCompiledShadersArchive != nullptr)
        {
            T3D_ARCHIVE_MGR.unloadArchive(mCompiledShadersArchive);
            mCompiledShadersArchive = nullptr;
        }
        
        return T3D_OK;
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

            // 根节点
            EDITOR_SCENE.getRuntimeRootTransform()->addChild(scene->getRootTransform());
            
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

    TResult ProjectManager::populate()
    {
        String path, name;
        Dir::parsePath(mAssetsPath, path, name);
        mAssetRoot = new AssetNode(name, path, MetaFolder::create(UUID::generate()));
        AssetNode *child = nullptr;
        return populate(mAssetsPath, mAssetRoot, false, child);
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::populate(const String &path, AssetNode *parent, bool generateFolderNode, AssetNode *&node)
    {
        // 先生成一个节点
        String folderMetaFilePath = path + ".meta";
        TResult ret = T3D_OK;

        if (generateFolderNode)
        {
            ret = generateAssetNode(folderMetaFilePath, parent, node);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to generate asset node (%s) ! ERROR [%d]", folderMetaFilePath.c_str(), ret);
                return ret;
            }
        }
        else
        {
            node = parent;
        }

        // 递归文件夹生成节点
        String searchPath = path + Dir::getNativeSeparator() + "*.*";
        Dir dir;
        bool working = dir.findFile(searchPath);

        using PathSet = TUnorderedMap<String, AssetNode*>;
        PathSet pathSet;

        auto queryAssetNode = [&pathSet](const String &path, AssetNode *&node) -> bool
        {
            bool ret = false;
            auto it = pathSet.find(path);
            if (it == pathSet.end())
            {
                ret = true;
            }
            else
            {
                node = it->second;
            }
            return ret;
        };

        while (working)
        {
            AssetNode *child = nullptr;
            String metaPath;
            
            if (dir.isDots())
            {
                // . or ..
            }
            else if (dir.isDirectory())
            {
                // directory
                metaPath = dir.getFilePath() + ".meta";
                bool noFolderNode = queryAssetNode(metaPath, child);
                ret = populate(dir.getFilePath(), node, noFolderNode, child);
            }
            else
            {
                // file
                const String filePath = dir.getFilePath();
                if (filePath != folderMetaFilePath)
                {
                    String::size_type pos = filePath.length();
                    if (filePath.at(pos-5) == '.'
                        && filePath.at(pos-4) == 'm'
                        && filePath.at(pos-3) == 'e'
                        && filePath.at(pos-2) == 't'
                        && filePath.at(pos-1) == 'a')
                    {
                        if (queryAssetNode(dir.getFilePath(), child))
                        {
                            metaPath = dir.getFilePath();
                            ret = generateAssetNode(dir.getFilePath(), node, child);
                        }

                        // 源文件不在，删掉 meta 文件
                        String originalPath, originalName, ext;
                        Dir::parsePath(filePath, originalPath, originalName, ext);
                        originalPath = originalPath + Dir::getNativeSeparator() + originalName;
                        if (!Dir::exists(originalPath))
                        {
                            Dir::remove(filePath);
                        }
                    }
                }
            }

            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to populate ! ERROR [%d]", ret);
                break;
            }

            if (child != nullptr)
            {
                pathSet.emplace(metaPath, child);
            }
            
            working = dir.findNextFile();
        }

        dir.close();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::generateAssetNode(const String &path, AssetNode *parent, AssetNode *&node)
    {
        EDITOR_LOG_DEBUG("Path : %s", path.c_str());
        
        TResult ret = T3D_OK;

        do
        {
            String dir, name;
            if (!Dir::parsePath(path, dir, name))
            {
                ret = T3D_ERR_FAIL;
                EDITOR_LOG_ERROR("Failed to parse path (%s) ! ERROR [%d]", path.c_str(), ret);
                break;
            }

            T3D_ASSERT(parent != nullptr);
            node = new AssetNode(name);
            
            if (!parent->addChild(node))
            {
                ret = T3D_ERR_FAIL;
                EDITOR_LOG_ERROR("Failed to add asset node (%s) ! ERROR [%d]", name.c_str(), ret);
                T3D_SAFE_DELETE(node);
                break;
            }

            if (!node->readMeta())
            {
                ret = T3D_ERR_FAIL;
                EDITOR_LOG_ERROR("Failed to read meta (%s) ! ERROR [%d]", node->getPath().c_str(), ret);
                T3D_SAFE_DELETE(node);
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ProjectManager::update()
    {
        AssetNode::GC();
    }

    //--------------------------------------------------------------------------

    void ProjectManager::applicationDidEnterBackground()
    {
    }

    //--------------------------------------------------------------------------

    void ProjectManager::applicationWillEnterForeground()
    {
        mAssetRoot->destroy();
        populate();
    }

    //--------------------------------------------------------------------------

    void ProjectManager::applicationFocusGained()
    {
        mAssetRoot->destroy();
        populate();
    }

    //--------------------------------------------------------------------------

    void ProjectManager::applicationFocusLost()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::makeFolder(AssetNode *parent, const String &path, AssetNode *&node)
    {
        TResult ret = T3D_OK;

        do
        {
            // 创建文件夹
            if (!Dir::makeDir(path))
            {
                EDITOR_LOG_ERROR("Failed to create folder [%s] !", path.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 创建文件夹对应的 meta 文件
            MetaFolderPtr meta = MetaFolder::create(UUID::generate());
            String metaPath = path + ".meta";
            FileDataStream fs;
            if (!fs.open(metaPath.c_str(), FileDataStream::EOpenMode::E_MODE_TRUNCATE))
            {
                EDITOR_LOG_ERROR("Failed to create meta file [%s] !", metaPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }
            
            ret = T3D_SERIALIZER_MGR.serialize(fs, meta);
            if (T3D_FAILED(ret))
            {
                fs.close();
                EDITOR_LOG_ERROR("Failed to serialize meta file [%s] ! ERROR [%d]", metaPath.c_str(), ret);
                break;
            }

            fs.close();

            // 生成文件夹节点
            ret = generateAssetNode(metaPath, parent, node);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create folder asset node [%s] ! ERROR [%d]",path.c_str(), ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::removeFolder(AssetNode *node)
    {
        TResult ret = T3D_OK;

        do
        {
            String path = node->getFullPath();
            if (!Dir::removeDir(path))
            {
                EDITOR_LOG_ERROR("Failed to delete folder [%s] !", path.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 删除对应的 meta 文件
            String metaPath = node->getMetaName();
            if (!Dir::remove(metaPath))
            {
                EDITOR_LOG_ERROR("Failed to delete meta file [%s] !", metaPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 删除节点
            if (node->getParent() != nullptr)
            {
                node->getParent()->removeChild(node);
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::addFile(AssetNode *parent, const String &path, AssetNode *&node)
    {
        return generateAssetNode(path, parent, node);
    }

    //--------------------------------------------------------------------------

    AssetNode::WaitingForDestroyNodesLUT AssetNode::msWaitingForDestroyNodesLUT;
    AssetNode::WaitingForDestroyNodes AssetNode::msWaitingForDestroyNodes;
    
    //--------------------------------------------------------------------------

    AssetNode::AssetNode(const String &name)
        : TreeNode()
        , mMetaName(name)
    {
        String dir, title, ext;
        Dir::parsePath(mMetaName, dir, title, ext);
        mFilename = title;
        Dir::parsePath(mFilename, dir, title, ext);
        mTitle = title;
    }

    //--------------------------------------------------------------------------

    AssetNode::AssetNode(const String &name, const String &path, Meta *meta)
        : TreeNode()
        , mPath(path)
        , mMetaName(name)
        , mMeta(meta)
    {
        String dir, title, ext;
        Dir::parsePath(mMetaName, dir, title, ext);
        mFilename = title;
        Dir::parsePath(mFilename, dir, title, ext);
        mTitle = title;
    }

    //--------------------------------------------------------------------------

    void AssetNode::buildPath() const
    {
        if (mIsPathDirty)
        {
            const AssetNode *node = this;
            if (getParent() != nullptr)
            {
                const AssetNode *parent = getParent();
                mPath = mMetaName;
                while (parent != nullptr)
                {
                    mPath = parent->getFilename() + Dir::getNativeSeparator() + mPath;
                    node = parent;
                    parent = parent->getParent();
                }

                mPath = node->getPath() + Dir::getNativeSeparator() + mPath;
            }
            
            mIsPathDirty = false;
        }
    }

    //--------------------------------------------------------------------------

    bool AssetNode::addChild(AssetNode *node, const Action &action)
    {
        bool ret = TreeNode::addChild(node, action);
        if (ret)
        {
            mIsPathDirty = true;
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    bool AssetNode::insertAfterChild(AssetNode *prevNode, AssetNode *node, const Action &action)
    {
        bool ret = TreeNode::insertAfterChild(prevNode, node, action);
        if (ret)
        {
            mIsPathDirty = true;
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    bool AssetNode::removeChild(AssetNode *node, const Action &action)
    {
        bool ret = TreeNode::removeChild(node, action);
        if (ret)
        {
            mIsPathDirty = true;
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    AssetNode *AssetNode::removeChild(const String &name, bool destroy)
    {
        AssetNode *node = TreeNode::removeChild(name);
        if (destroy)
        {
            node->destroy();
        }
        return node;
    }

    //--------------------------------------------------------------------------

    void AssetNode::removeAllChildren(const Action &removeAction, const Action &deleteAction)
    {
        TreeNode::removeAllChildren(removeAction, deleteAction);
        mIsPathDirty = true;
    }

    //--------------------------------------------------------------------------

    void AssetNode::removeAllChildren()
    {
        removeAllChildren(nullptr,
                [](AssetNode *node)
                {
                    node->destroy();
                });
    }

    //--------------------------------------------------------------------------

    void AssetNode::putWaitingForDestroyNode(AssetNode *node)
    {
        auto it = msWaitingForDestroyNodesLUT.find(node->getPath());
        if (it == msWaitingForDestroyNodesLUT.end())
        {
            msWaitingForDestroyNodesLUT.emplace(node->getPath(), node);
            msWaitingForDestroyNodes.emplace_back(node);
        }
    }

    //--------------------------------------------------------------------------

    void AssetNode::GC()
    {
        for (auto node : msWaitingForDestroyNodes)
        {
            if (node->getParent() != nullptr)
            {
                node->getParent()->removeChild(node);
            }
            T3D_SAFE_DELETE(node);
        }

        msWaitingForDestroyNodesLUT.clear();
        msWaitingForDestroyNodes.clear();
    }
    
    //--------------------------------------------------------------------------

    void AssetNode::destroy()
    {
        for (auto child : getChildren())
        {
            child->destroy();
        }

        putWaitingForDestroyNode(this);
    }

    //--------------------------------------------------------------------------

    bool AssetNode::readMeta()
    {
        bool ret = true;

        do
        {
            FileDataStream fs;
            
            if (!fs.open(getPath().c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
            {
                EDITOR_LOG_ERROR("Failed to open file [%s] !", getPath().c_str());
                ret = false;
                break;
            }

            mMeta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
            fs.close();
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void AssetNode::debugOutput(int32_t depth)
    {
        std::stringstream ss;
        for (int32_t i = 0; i < depth; i++)
        {
            ss << "\t";
        }
        
        EDITOR_LOG_DEBUG("%sNode - Path : %s, Filename : %s, MetaName : %s", ss.str().c_str(),
            getPath().c_str(), getFilename().c_str(), getMetaName().c_str());

        for (auto itr = child_begin(); itr != child_end(); ++itr)
        {
            (*itr)->debugOutput(depth + 1);
        }
    }

    //--------------------------------------------------------------------------

    NS_END
}

