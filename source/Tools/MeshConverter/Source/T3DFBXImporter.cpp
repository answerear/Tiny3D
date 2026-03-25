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


#include "T3DFBXImporter.h"
#include "T3DFBXDataStream.h"
#include "T3DMeshConverterError.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    // 骨骼权重信息结构体（用于提取蒙皮网格的混合权重）
    //--------------------------------------------------------------------------
    struct BoneWeightInfo
    {
        uint8_t boneIndex {0xFF};
        float32_t weight {0.0f};
    };

    //--------------------------------------------------------------------------

    template<typename T>
    T evaluatePropertyInitialValue(FbxPropertyT<T> & p)
    {
        FbxTime t = 0;
        if (p.IsAnimated())
        {
            auto evaluator = p.GetAnimationEvaluator();
            if (evaluator)
                t = evaluator->ValidateTime(t);
        }
        return p.EvaluateValue(t);
    }
    
    //--------------------------------------------------------------------------

    FBXImporterPtr FBXImporter::create()
    {
        return new FBXImporter();
    }

    //--------------------------------------------------------------------------

    FBXImporter::FBXImporter()
    {
        
    }

    //--------------------------------------------------------------------------

    FBXImporter::~FBXImporter()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::run(const ConverterOptions &opts, Assets &resources)
    {
        TResult ret = T3D_OK;
        FileDataStream fs;
        String tempPath;
        
        do
        {
            mOutputName = opts.dstTitle;
            
            // 初始化 FBX 对象
            ret = initFbxObjects();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 创建 FBX 场景
            FbxScene *lFbxScene = FbxScene::Create(mFbxManager, "My Scene");
            if (lFbxScene == nullptr)
            {
                MCONV_LOG_ERROR("Failed to create FbxScene.")
                ret = T3D_ERR_FBX_SCENE_CREATE_FAILED;
                break;
            }

            // 打开 FBX 文件
            if (!fs.open(opts.srcPath.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
            {
                MCONV_LOG_ERROR("Failed to open file: %s", opts.srcPath.c_str())
                ret = T3D_ERR_FBX_FILE_OPEN_FAILED;
                break;
            }

            // 导入场景
            ret = importScene(fs, lFbxScene);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to import FBX file.")
                break;
            }

            String filename;
            Dir::parsePath(opts.srcPath, mInputDir, filename);
            
            fs.close();

            // 设置度量系统
            ret = setupMetricSystem(lFbxScene);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to setup metric system.")
                break;
            }

            if (opts.compiledShaderPath.empty())
            {
                // 没有传入编译后的着色器路径，自动从资源根路径查找所有shader lab 并编译
                tempPath = Dir::getAppPath() + Dir::getNativeSeparator() + "temp";
                Dir::makeDir(tempPath);
                
                ret = compileAllShaders(tempPath, opts.defaultResourcePath);
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("Failed to compile shaders.")
                    break;
                }
            }
            else
            {
                // 传入了编译后的着色器路径，只记录路径
                ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(opts.compiledShaderPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kRead);
            }
            
            // 加载默认材质
            ret = loadDefaultMaterial(opts.defaultResourcePath, opts.defaultMaterialPath);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to load default material ! ERROR [%d]", ret)
                break;
            }

            bool hasSkeleton = false;

            // 先处理 FBX 场景，收集场景结点和 mesh 对象
            ret = processFbxScene(lFbxScene);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // for (const auto lFbxMesh : mFbxMeshes)
            // {
            //     ret = processFbxMesh(lFbxMesh);
            //     if (T3D_FAILED(ret))
            //     {
            //         break;
            //     }
            // }

            // 要优先处理骨架，否则，后面处理 mesh 的时候，根据 mesh 的 cluster 找骨骼会找不到
            if (opts.dstFileType & MeshFileType::kTSkel)
            {
                // 导出骨架数据
                ret = processSkeleton(lFbxScene);
                if (T3D_FAILED(ret))
                {
                    break;
                }
                hasSkeleton = true;
            }
            
            if (opts.dstFileType & MeshFileType::kTMesh)
            {
                // 导出静态网格数据
                ret = processStaticMesh(lFbxScene);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
            
            if (opts.dstFileType & MeshFileType::kTSkin)
            {
                // 导出蒙皮数据
                ret = processSkinnedMesh(lFbxScene);
                if (T3D_FAILED(ret))
                {
                    break;
                }
                hasSkeleton = true;
            }
            
            if (opts.dstFileType & MeshFileType::kTAni)
            {
                // 导出动画数据
                ret = processAnimation(lFbxScene);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            if (hasSkeleton)
            {
                // 创建骨架
                ret = createSkeleton();
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("Failed to create skeleton.")
                    break;
                }
            }
            
            // 创建网格
            ret = createMesh();
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to create mesh.")
                break;
            }

            // 把结果放到 resources 中
            resources = mResources;
        } while (false);

        if (fs.isOpened())
        {
            fs.close();
        }

        if (!tempPath.empty())
        {
            Dir::remove(tempPath);
        }
        
        // 释放 FBX 对象
        destroyFbxObjects();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::initFbxObjects()
    {
        TResult ret = T3D_OK;

        do
        {
            // 创建 FBX 管理器
            mFbxManager = FbxManager::Create();
            if (mFbxManager == nullptr)
            {
                MCONV_LOG_ERROR("Failed to create FbxManager.")
                ret = T3D_ERR_FBX_SDK_CREATE_FAILED;
                break;
            }

            // 创建 FBX IO 设置
            FbxIOSettings *ioSettings = FbxIOSettings::Create(mFbxManager, IOSROOT);
            ioSettings->SetBoolProp(IMP_FBX_AUDIO, false);
            ioSettings->SetBoolProp(IMP_FBX_CHARACTER, false);
            ioSettings->SetBoolProp(IMP_FBX_POLYGROUP, false);
            ioSettings->SetBoolProp(IMP_FBX_USERDATA, false);
            ioSettings->SetBoolProp(IMP_FBX_EDGECREASE, false);
            ioSettings->SetBoolProp(IMP_FBX_VERTEXCREASE, false);
            ioSettings->SetBoolProp(IMP_FBX_HOLE, false);
            ioSettings->SetBoolProp(IMP_FBX_SHAPE, true);
            ioSettings->SetBoolProp(IMP_FBX_ANIMATION, true);
            ioSettings->SetBoolProp(IMP_FBX_NORMAL, true);
            ioSettings->SetBoolProp(IMP_FBX_BINORMAL, false);
            ioSettings->SetBoolProp(IMP_FBX_TANGENT, false);
            ioSettings->SetBoolProp(IMP_FBX_VISIBILITY, true);
            ioSettings->SetBoolProp(IMP_FBX_CONSTRAINT, false);
            ioSettings->SetBoolProp(IMP_FBX_LINK, true);
            ioSettings->SetIntProp(IMP_CACHE_SIZE, 256); // Set a large cache to avoid many OS level file reads.
            ioSettings->SetBoolProp(IMP_RELAXED_FBX_CHECK, true);
            mFbxManager->SetIOSettings(ioSettings);

            // 加载 FBX 插件动态库
            FbxString lPath = FbxGetApplicationDirectory();
            if (!mFbxManager->LoadPluginsDirectory(lPath.Buffer()))
            {
                MCONV_LOG_ERROR("Failed to load plugins directory.")
                ret = T3D_ERR_FBX_PLUGIN_LOAD_FAILED;
                break;
            }

            int lSDKMajor, lSDKMinor, lSDKRevision;
            FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);
            MCONV_LOG_INFO("FBX SDK Version: %d.%d.%d", lSDKMajor, lSDKMinor, lSDKRevision)
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult FBXImporter::destroyFbxObjects()
    {
        if (mFbxManager != nullptr)
        {
            mFbxManager->Destroy();
            mFbxManager = nullptr;
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::importScene(DataStream &stream, FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            // 创建 FBX importer
            FbxImporter *lImporter = FbxImporter::Create(mFbxManager, "");
            FBXDataStream lStream(stream, mFbxManager, true);
            if (!lImporter->Initialize(&lStream, nullptr, -1, mFbxManager->GetIOSettings()))
            {
                MCONV_LOG_ERROR("Failed to initialize FBX importer.")
                ret = T3D_ERR_FBX_IMPORTER_INIT_FAILED;
                break;
            }

            // 获取 FBX 文件版本
            int lFileMajor, lFileMinor, lFileRevision;
            lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);
            MCONV_LOG_INFO("FBX File Version: %d.%d.%d", lFileMajor, lFileMinor, lFileRevision)
            
            // 导入 FBX 场景
            if (!lImporter->Import(lFbxScene))
            {
                MCONV_LOG_ERROR("Failed to import FBX scene.")
                ret = T3D_ERR_FBX_SCENE_IMPORT_FAILED;
                break;
            }

            // 销毁 FBX importer
            lImporter->Destroy();
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::setupMetricSystem(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            FbxGlobalSettings &lFbxGlobalSettings = lFbxScene->GetGlobalSettings();

            // 统一转成 DirectX 坐标系
            FbxAxisSystem lSceneAxisSystem = lFbxGlobalSettings.GetAxisSystem();
            FbxAxisSystem  tinyAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
            // FbxAxisSystem  tinyAxisSystem(FbxAxisSystem::eYAxis, (FbxAxisSystem::EFrontVector)-FbxAxisSystem::eParityOdd, FbxAxisSystem::eLeftHanded);
            if (lSceneAxisSystem != tinyAxisSystem)
            {
                MCONV_LOG_INFO("Start converting scene to Tiny3D coordinate system ...")
                tinyAxisSystem.ConvertScene(lFbxScene);
                MCONV_LOG_INFO("Completed converting to Tiny3D coordinate system.")
                mIsOpenGLMetric = true;
            }
            // FbxAxisSystem unityAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
            // if (lSceneAxisSystem != unityAxisSystem)
            // {
            //     unityAxisSystem.ConvertScene(lFbxScene);
            // }

            // 统一转成米制单位
            float sceneScaleFactor = (float)lFbxGlobalSettings.GetSystemUnit().GetScaleFactor();
            if (sceneScaleFactor != 1.0f)
            {
                MCONV_LOG_INFO("Start converting unit to meter ......")
                FbxSystemUnit::cm.ConvertScene(lFbxScene);
                MCONV_LOG_INFO("Completed converting unit !")
            }

            // 不是三角形为面的mesh，统一转换成三角形为面的mesh
            MCONV_LOG_INFO("Start converting face to triangles ......")
            FbxGeometryConverter converter(mFbxManager);
            if (!converter.Triangulate(lFbxScene, true))
            {
                MCONV_LOG_ERROR("Failed to convert face to triangles.")
                ret = T3D_ERR_FBX_FACE_TO_TRIANGLE_FAILED;
                break;
            }
            MCONV_LOG_INFO("Completed converting face to triangles !")
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::compileAllShaders(const String &tempPath, const String &assetsPath)
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
                    MCONV_LOG_ERROR("Failed to create shaders folder (%s) !", compiledShadersPath.c_str());
                    ret = T3D_ERR_FAIL;
                    break;
                }
            }
            
            // 编译资源中所有 shaders
            ret = compileShaders(assetsPath, compiledShadersPath);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to compile project shaders !");
                break;
            }

            // 编译后 shaders 档案系统
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(compiledShadersPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadTxtTruncate);
            if (archive == nullptr)
            {
                MCONV_LOG_ERROR("Failed to load compiled shaders fs archive [%s]", compiledShadersPath.c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::compileShaders(const String &inputPath, const String &outputPath)
    {
        TResult ret = T3D_OK;

        String searchPath = inputPath + Dir::getNativeSeparator() + "*.*";
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
                String path, title, ext;
                Dir::parsePath(dir.getFilePath(), path, title, ext);
                StringUtil::toLowerCase(ext);
                if (ext == "shader")
                {
                    const String filePath = dir.getFilePath();
                    ret = compileShader(filePath, outputPath);
                }
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

    TResult FBXImporter::compileShader(const String &inputPath, const String &outputPath)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start compiling shader %s ...", inputPath.c_str());
            
            // 从 meta 中读取出 UUID
            String metaPath = inputPath + ".meta";
            FileDataStream fs;
            if (!fs.open(metaPath.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
            {
                MCONV_LOG_ERROR("Failed to open meta file [%s] !", metaPath.c_str());
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
                MCONV_LOG_ERROR("Failed to start scc.exe from source file (%s) ! ERROR [%d]", inputPath.c_str(), ret);
                break;
            }

            // 等待编译结束
            ret = process.wait();
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to wait process exiting from source file (%s) ! ERROR [%d]", inputPath.c_str(), ret);
                break;
            }

            uint32_t exitCode = process.getExitCode();
            if (exitCode != 0)
            {
                // 编译出错了，只能退出
                MCONV_LOG_ERROR("Failed to compile shader (%s) ! ERROR [%d]", inputPath.c_str(), ret);
                break;
            }

            MCONV_LOG_INFO("Completed compiling shader !");
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult FBXImporter::loadDefaultMaterial(const String &rootPath, const String &relativePath)
    {
        TResult ret = T3D_OK;

        do
        {
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(rootPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kRead);
            T3D_ASSERT(archive != nullptr);
            mDefaultMaterial = T3D_MATERIAL_MGR.loadMaterial(archive, relativePath);
            
            //FileDataStream fs;
            //if (!fs.open(path.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
            //{
            //    ret = T3D_ERR_FILE_NOT_EXIST;
            //    MCONV_LOG_ERROR("Failed to open file: %s", path.c_str())
            //    break;
            //}

            //mDefaultMaterial = T3D_SERIALIZER_MGR.deserialize<Material>(fs);
            //fs.close();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::processFbxScene(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            for (int32_t i = 0; i < lFbxScene->GetRootNode()->GetChildCount(); i++)
            {
                FbxNode *lFbxNode = lFbxScene->GetRootNode()->GetChild(i);
                ret = processFbxNode(lFbxNode, nullptr);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    Vector3 FBXImporter::FbxPointToTinyVector3Remap(const FbxVector4 &lFbxPos) const
    {
        return Vector3(static_cast<float>(-lFbxPos[0]), static_cast<float>(lFbxPos[1]), static_cast<float>(lFbxPos[2]));
        // return Vector3(static_cast<float>(lFbxPos[0]), static_cast<float>(lFbxPos[1]), static_cast<float>(lFbxPos[2]));
    }

    //--------------------------------------------------------------------------
    
    Quaternion FBXImporter::FbxEulerToTinyQuaternion(const FbxVector4 &lFbxEuler) const
    {
        FbxVector4 lFbxFixedEuler(lFbxEuler[0], -lFbxEuler[1], -lFbxEuler[2], lFbxEuler[3]);
        // FbxVector4 lFbxFixedEuler(lFbxEuler[0], lFbxEuler[1], lFbxEuler[2], lFbxEuler[3]);

        FbxAMatrix lFbxRotationM;
        lFbxRotationM.SetROnly(lFbxFixedEuler);

        FbxQuaternion lFbxQ = lFbxRotationM.GetQ();
        return Quaternion(static_cast<float>(lFbxQ[3]), static_cast<float>(lFbxQ[0]), static_cast<float>(lFbxQ[1]), static_cast<float>(lFbxQ[2]));
    }
    
    //--------------------------------------------------------------------------

    Vector3 FBXImporter::FbxPointToTinyVector3(const FbxVector4 &lFbxPos) const
    {
        return Vector3(static_cast<float>(lFbxPos[0]), static_cast<float>(lFbxPos[1]), static_cast<float>(lFbxPos[2]));
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::processFbxNode(FbxNode *lFbxNode, GameObject *goParent)
    {
        TResult ret = T3D_OK;

        do
        {
            // 获取平移、旋转、缩放
            FbxAMatrix lFbxLocalMat = lFbxNode->EvaluateLocalTransform(FBXSDK_TIME_ZERO);
            FbxVector4 lFbxLocalT = lFbxLocalMat.GetT();
            FbxVector4 lFbxLocalS = lFbxLocalMat.GetS();
            FbxVector4 lFbxLocalR = lFbxLocalMat.GetR();
            FbxQuaternion lFbxLocalQ = lFbxLocalMat.GetQ();

            // 转换成 tiny3d 的坐标系
            Vector3 localT = FbxPointToTinyVector3Remap(lFbxLocalT);
            Quaternion localQ = FbxEulerToTinyQuaternion(lFbxLocalR);
            Vector3 localS = FbxPointToTinyVector3(lFbxLocalS);

            // if (lFbxNode->GetNodeAttribute() != nullptr)
            // {
            //     switch (lFbxNode->GetNodeAttribute()->GetAttributeType())
            //     {
            //     case FbxNodeAttribute::eMesh:
            //         {
            //             // 收集 fbx mesh 对象，后续处理
            //             FbxMesh *lFbxMesh = lFbxNode->GetMesh();
            //             T3D_ASSERT(lFbxMesh != nullptr);
            //             mFbxMeshes.emplace_back(lFbxMesh);
            //         }
            //         break;
            //     default:
            //         break;
            //     }
            // }
            // else
            // {
            //     // 没有 mesh 的时候，创建一个 game object，相当于骨骼
            //     GameObjectPtr go = GameObject::createWithTransform(lFbxNode->GetName());
            //     Transform3D *transform = static_cast<Transform3D *>(go->getTransformNode());
            //     transform->setPosition(localT);
            //     transform->setOrientation(localQ);
            //     transform->setScaling(localS);
            //
            //     if (lFbxNode == nullptr && goParent == nullptr)
            //     {
            //         // 根节点放入映射表里面，方便后面查找
            //         mFbxNodesToGameObjects.emplace(lFbxNode, go);
            //     }
            //     else
            //     {
            //         TransformNode *parent = goParent->getTransformNode();
            //         parent->addChild(transform);
            //     }
            // }

            Matrix3 localM;
            localQ.toRotationMatrix(localM);
            Radian xAngle, yAngle, zAngle;
            localM.toEulerAnglesYXZ(yAngle, xAngle, zAngle);
            
            MCONV_LOG_DEBUG("Fbx node: %s, T: (%f, %f, %f, %f), Q: (%f, %f, %f, %f), E: (%f, %f, %f, %f), S: (%f, %f, %f, %f)",
                lFbxNode->GetName(),
                lFbxLocalT[0], lFbxLocalT[1], lFbxLocalT[2], lFbxLocalT[3],
                lFbxLocalQ[3], lFbxLocalQ[0], lFbxLocalQ[1], lFbxLocalQ[2],
                lFbxLocalR[0], lFbxLocalR[1], lFbxLocalR[2], lFbxLocalR[3],
                lFbxLocalS[0], lFbxLocalS[1], lFbxLocalS[2], lFbxLocalS[3])
            
            MCONV_LOG_DEBUG("Tiny3D node: %s, T: %s, Q: %s, E: (%f, %f, %f), S: %s",
                lFbxNode->GetName(),
                localT.getDebugString().c_str(),
                localQ.getDebugString().c_str(),
                xAngle.valueDegrees(), yAngle.valueDegrees(), zAngle.valueDegrees(),
                localS.getDebugString().c_str())

            for (int32_t i = 0; i < lFbxNode->GetChildCount(); i++)
            {
                ret = processFbxNode(lFbxNode->GetChild(i), nullptr);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::processFbxMesh(FbxNode *lFbxNode)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Starting processing fbx mesh node ...")

            FbxMesh *lFbxMesh = lFbxNode->GetMesh();

            // 三角形数量
            int32_t triangleCount = lFbxMesh->GetPolygonCount();

            MCONV_LOG_INFO("FBX mesh triangle count: %d", triangleCount)

            // 获取三角形材质索引，让相同材质的三角形在一起，形成一个 submesh
            TArray<int32_t> triangleMaterialIndices(triangleCount, -1);
            getFbxTriangleMaterialIndices(lFbxMesh, triangleCount, triangleMaterialIndices);

            // 获取三角形平滑组索引，相同平滑组的顶点合并在一起，使用面法线的平均值作为法线
            TArray<int32_t> triangleSmGroupIndices(triangleCount, -1);
            getFbxTriangleSmoothGroupIndices(lFbxMesh, triangleCount, triangleSmGroupIndices);

            int32_t texUVCount = lFbxMesh->GetElementUVCount();

            int32_t materialCount = lFbxNode->GetMaterialCount();

            int32_t vertexCount = triangleCount * 3;
            mVertices.clear();
            mNormals.clear();
            mNormals.reserve(vertexCount);
            mSmoothingGroups.clear();
            mSmoothingGroups.reserve(vertexCount);
            mTangents.clear();
            mTangents.reserve(vertexCount);
            mBinormals.clear();
            mBinormals.reserve(vertexCount);
            mVertexToControlPointMap.clear();  // 清空控制点索引映射
            mVertexToControlPointMap.reserve(vertexCount);
            for (int32_t l = 0; l < T3D_MAX_TEXTURE_LEVEL; l++)
            {
                mTexCoords[l].clear();
                mTexCoords[l].reserve(vertexCount);
            }
            mIndices.reserve(vertexCount);
            
            for (int32_t k = 0; k < materialCount; k++)
            {
                FbxSurfaceMaterial *lMaterial = lFbxNode->GetMaterial(k);
                
                MCONV_LOG_INFO("Material [%d]: %s", k, lMaterial->GetName())

                mIndices.clear();

                for (int32_t i = 0; i < triangleCount; i++)
                {
                    if (triangleMaterialIndices[i] == k)
                    {
                        // 处理三角形

                        for (int32_t j = 0; j < 3; j++)
                        {
                            // 获取顶点
                            Vector3 V;
                            int32_t ctrlPointIndex = lFbxMesh->GetPolygonVertex(i, j);
                            readVertex(lFbxMesh, ctrlPointIndex, V);

                            // 获取 uv
                            Vector2Array texCoords;
                            texCoords.reserve(texUVCount);
                            for (int32_t l = 0; l < texUVCount; l++)
                            {
                                Vector2 UV;
                                readUV(lFbxMesh, ctrlPointIndex, lFbxMesh->GetTextureUVIndex(i, j), l, UV);
                                texCoords.emplace_back(UV);
                            }

                            // 获取顶点颜色
                            ColorRGBA C;
                            readColor(lFbxMesh, ctrlPointIndex, j + 3 * i, C);

                            // 获取法线
                            Vector3 N, T, B;
                            readNormal(lFbxMesh, ctrlPointIndex, j + 3 * i, N);
                            if (texUVCount > 0)
                            {
                                readTangent(lFbxMesh, ctrlPointIndex, j + 3 * i, T);
                                readBinormal(lFbxMesh, ctrlPointIndex, j + 3 * i, B);
                            }

                            // 查找已经收集的顶点中，是否有和当前顶点属于一个光滑组的点
                            // 如果同一个光滑组，则累计光滑组的法线、副法线、切线，用于计算平均值
                            uint32_t v = 0;
                            for (v = 0; v < mVertices.size(); v++)
                            {
                                if (mVertices[v] == V && mSmoothingGroups[v] == triangleSmGroupIndices[i])
                                {
                                    N += mNormals[v];
                                    mNormals[v] = N;

                                    if (texUVCount > 0)
                                    {
                                        T += mTangents[v];
                                        mTangents[v] = T;

                                        B += mBinormals[v];
                                        mBinormals[v] = B;
                                    }
                                }
                            }

                            // 查看这个顶点是否已经存在（顶点位置、光滑组、UV都相同则为相同）
                            for (v = 0; v < mVertices.size(); v++)
                            {
                                if (mVertices[v] == V && mSmoothingGroups[v] == triangleSmGroupIndices[i])
                                {
                                    // 如果已经存在，则不添加
                                    int32_t layer = 0;
                                    for (layer = 0; layer < texUVCount; layer++)
                                    {
                                        if (mTexCoords[layer][v] != texCoords[layer])
                                        {
                                            break;
                                        }
                                    }
                                    if (layer == texUVCount)
                                    {
                                        break;
                                    }
                                }
                            }

                            if (v == mVertices.size())
                            {
                                // 跑到这里表示没有找到相同的顶点，添加

                                // 位置
                                mVertices.emplace_back(V);
                                // 颜色
                                mColors.emplace_back(C);
                                // 光滑组
                                mSmoothingGroups.emplace_back(triangleSmGroupIndices[i]);
                                // 纹理
                                for (int32_t layer = 0; layer < texUVCount; layer++)
                                {
                                    mTexCoords[layer].emplace_back(texCoords[layer]);
                                }
                                // 法线
                                mNormals.emplace_back(N);
                                // 切线
                                mTangents.emplace_back(T);
                                // 副法线
                                mBinormals.emplace_back(B);
                                // 保存该顶点对应的控制点索引
                                mVertexToControlPointMap.emplace_back(ctrlPointIndex);
                            }
                            else
                            {
                                // 顶点已存在，但需要更新控制点索引映射（如果当前控制点索引更合适）
                                // 注意：如果同一个顶点被多个控制点使用，我们保留第一个遇到的
                                // 如果需要更精确的处理，可以在这里做额外的判断
                            }

                            // 索引
                            mIndices.emplace_back(v);
                        }
                    }
                }
                
                String name = lFbxNode->GetName();
                if (materialCount > 0)
                {
                    name += String("_") + lFbxNode->GetMaterial(k)->GetName();
                }

                // 创建对应的材质
                MaterialPtr material;
                FbxSurfaceMaterial *lFbxMaterial = lFbxNode->GetMaterial(k);
                ret = createMaterial(lFbxMaterial, material);
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("Failed to create material.")
                    break;
                }

                String materialName = material->getName() + "." + Resource::EXT_MATERIAL;
                mResources.emplace(materialName, material);

                // 创建子网格
                SubMeshPtr submesh;
                ret = createSubMesh(name, material, submesh);
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("failed to create submesh.")
                    break;
                }

                mSubMeshes.emplace(name, submesh);
            }
            
            MCONV_LOG_INFO("Completed processing fbx mesh node.")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::processStaticMesh(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Starting processing static mesh ...")

            ret = getFbxMeshNode(lFbxScene->GetRootNode(), false);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to process static mesh ! ERROR [%d]", ret)
                break;
            }
            
            MCONV_LOG_INFO("Completed processing static mesh !")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::getFbxMeshNode(FbxNode *lFbxNode, bool hasSkin)
    {
        TResult ret = T3D_OK;

        do
        {
            if (lFbxNode->GetNodeAttribute() != nullptr)
            {
                switch (lFbxNode->GetNodeAttribute()->GetAttributeType())
                {
                case FbxNodeAttribute::eMesh:
                    {
                        ret = processFbxMeshNode(lFbxNode);
                        if (T3D_FAILED(ret))
                        {
                            break;
                        }

                        if (hasSkin)
                        {
                            FbxMesh *lFbxMesh = lFbxNode->GetMesh();
                            ret = getOffsetMatrix(lFbxMesh);
                        }
                    }
                    break;
                default:
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                break;
            }

            for (int32_t i = 0; i < lFbxNode->GetChildCount(); i++)
            {
                ret = getFbxMeshNode(lFbxNode->GetChild(i), hasSkin);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::processFbxMeshNode(FbxNode *lFbxNode)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Starting processing fbx mesh node ...")

            FbxMesh *lFbxMesh = lFbxNode->GetMesh();

            // 三角形数量
            int32_t triangleCount = lFbxMesh->GetPolygonCount();

            MCONV_LOG_INFO("FBX mesh triangle count: %d", triangleCount)

            // 获取三角形材质索引，让相同材质的三角形在一起，形成一个 submesh
            TArray<int32_t> triangleMaterialIndices(triangleCount, -1);
            getFbxTriangleMaterialIndices(lFbxMesh, triangleCount, triangleMaterialIndices);

            // 获取三角形平滑组索引，相同平滑组的顶点合并在一起，使用面法线的平均值作为法线
            TArray<int32_t> triangleSmGroupIndices(triangleCount, -1);
            getFbxTriangleSmoothGroupIndices(lFbxMesh, triangleCount, triangleSmGroupIndices);

            int32_t texUVCount = lFbxMesh->GetElementUVCount();

            int32_t materialCount = lFbxNode->GetMaterialCount();

            int32_t vertexCount = triangleCount * 3;
            mVertices.clear();
            mNormals.clear();
            mNormals.reserve(vertexCount);
            mSmoothingGroups.clear();
            mSmoothingGroups.reserve(vertexCount);
            mTangents.clear();
            mTangents.reserve(vertexCount);
            mBinormals.clear();
            mBinormals.reserve(vertexCount);
            mVertexToControlPointMap.clear();  // 清空控制点索引映射
            mVertexToControlPointMap.reserve(vertexCount);
            for (int32_t l = 0; l < T3D_MAX_TEXTURE_LEVEL; l++)
            {
                mTexCoords[l].clear();
                mTexCoords[l].reserve(vertexCount);
            }
            mIndices.reserve(vertexCount);
            
            for (int32_t k = 0; k < materialCount; k++)
            {
                FbxSurfaceMaterial *lMaterial = lFbxNode->GetMaterial(k);
                
                MCONV_LOG_INFO("Material [%d]: %s", k, lMaterial->GetName())

                mIndices.clear();

                for (int32_t i = 0; i < triangleCount; i++)
                {
                    if (triangleMaterialIndices[i] == k)
                    {
                        // 处理三角形

                        for (int32_t j = 0; j < 3; j++)
                        {
                            // 获取顶点
                            Vector3 V;
                            int32_t ctrlPointIndex = lFbxMesh->GetPolygonVertex(i, j);
                            readVertex(lFbxMesh, ctrlPointIndex, V);

                            // 获取 uv
                            Vector2Array texCoords;
                            texCoords.reserve(texUVCount);
                            for (int32_t l = 0; l < texUVCount; l++)
                            {
                                Vector2 UV;
                                readUV(lFbxMesh, ctrlPointIndex, lFbxMesh->GetTextureUVIndex(i, j), l, UV);
                                texCoords.emplace_back(UV);
                            }

                            // 获取顶点颜色
                            ColorRGBA C;
                            readColor(lFbxMesh, ctrlPointIndex, j + 3 * i, C);

                            // 获取法线
                            Vector3 N, T, B;
                            readNormal(lFbxMesh, ctrlPointIndex, j + 3 * i, N);
                            if (texUVCount > 0)
                            {
                                readTangent(lFbxMesh, ctrlPointIndex, j + 3 * i, T);
                                readBinormal(lFbxMesh, ctrlPointIndex, j + 3 * i, B);
                            }

                            // 查找已经收集的顶点中，是否有和当前顶点属于一个光滑组的点
                            // 如果同一个光滑组，则累计光滑组的法线、副法线、切线，用于计算平均值
                            uint32_t v = 0;
                            for (v = 0; v < mVertices.size(); v++)
                            {
                                if (mVertices[v] == V && mSmoothingGroups[v] == triangleSmGroupIndices[i])
                                {
                                    N += mNormals[v];
                                    mNormals[v] = N;

                                    if (texUVCount > 0)
                                    {
                                        T += mTangents[v];
                                        mTangents[v] = T;

                                        B += mBinormals[v];
                                        mBinormals[v] = B;
                                    }
                                }
                            }

                            // 查看这个顶点是否已经存在（顶点位置、光滑组、UV都相同则为相同）
                            for (v = 0; v < mVertices.size(); v++)
                            {
                                if (mVertices[v] == V && mSmoothingGroups[v] == triangleSmGroupIndices[i])
                                {
                                    // 如果已经存在，则不添加
                                    int32_t layer = 0;
                                    for (layer = 0; layer < texUVCount; layer++)
                                    {
                                        if (mTexCoords[layer][v] != texCoords[layer])
                                        {
                                            break;
                                        }
                                    }
                                    if (layer == texUVCount)
                                    {
                                        break;
                                    }
                                }
                            }

                            if (v == mVertices.size())
                            {
                                // 跑到这里表示没有找到相同的顶点，添加

                                // 位置
                                mVertices.emplace_back(V);
                                // 颜色
                                mColors.emplace_back(C);
                                // 光滑组
                                mSmoothingGroups.emplace_back(triangleSmGroupIndices[i]);
                                // 纹理
                                for (int32_t layer = 0; layer < texUVCount; layer++)
                                {
                                    mTexCoords[layer].emplace_back(texCoords[layer]);
                                }
                                // 法线
                                mNormals.emplace_back(N);
                                // 切线
                                mTangents.emplace_back(T);
                                // 副法线
                                mBinormals.emplace_back(B);
                                // 保存该顶点对应的控制点索引
                                mVertexToControlPointMap.emplace_back(ctrlPointIndex);
                            }
                            else
                            {
                                // 顶点已存在，但需要更新控制点索引映射（如果当前控制点索引更合适）
                                // 注意：如果同一个顶点被多个控制点使用，我们保留第一个遇到的
                                // 如果需要更精确的处理，可以在这里做额外的判断
                            }

                            // 索引
                            mIndices.emplace_back(v);
                        }
                    }
                }

                String name = lFbxNode->GetName();
                if (materialCount > 0)
                {
                    name += String("_") + lFbxNode->GetMaterial(k)->GetName();
                }

                // 创建对应的材质
                MaterialPtr material;
                FbxSurfaceMaterial *lFbxMaterial = lFbxNode->GetMaterial(k);
                ret = createMaterial(lFbxMaterial, material);
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("Failed to create material.")
                    break;
                }

                String materialName = material->getName() + "." + Resource::EXT_MATERIAL;
                mResources.emplace(materialName, material);

                // 创建子网格
                SubMeshPtr submesh;
                ret = createSubMesh(name, material, submesh);
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("failed to create submesh.")
                    break;
                }

                mSubMeshes.emplace(name, submesh);
            }
            
            MCONV_LOG_INFO("Completed processing fbx mesh node.")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void FBXImporter::getFbxTriangleSmoothGroupIndices(FbxGeometryBase *lFbxGeometry, int32_t triangleCount, TArray<int32_t> &triangleSmGroupIndices) const
    {
        FbxLayerElementSmoothing *lFbxSmoothing = lFbxGeometry->GetElementSmoothing();
        
        if (lFbxSmoothing != nullptr)
        {
            bool bDirectSm = (lFbxSmoothing->GetReferenceMode() == FbxLayerElement::eDirect);

            for(int triangleIndex = 0 ; triangleIndex < triangleCount ; ++triangleIndex)  
            {  
                int32_t SmIndex = bDirectSm ? triangleIndex : lFbxSmoothing->GetIndexArray().GetAt(triangleIndex);
                int32_t iSmoothing = lFbxSmoothing->GetDirectArray().GetAt(SmIndex);
                triangleSmGroupIndices[triangleIndex] = iSmoothing;  
            }
        }
    }

    //--------------------------------------------------------------------------

    void FBXImporter::getFbxTriangleMaterialIndices(FbxGeometryBase *lFbxGeometry, int32_t triangleCount, TArray<int32_t> &triangleMaterialIndices) const
    {
        FbxLayerElementMaterial *lFbxMaterial = lFbxGeometry->GetElementMaterial();
        if (lFbxMaterial == nullptr)
        {
            return;
        }

        for(int triangleIndex = 0 ; triangleIndex < triangleCount ; ++triangleIndex)  
        {  
            int32_t materialIndex = lFbxMaterial->GetIndexArray().GetAt(triangleIndex);
            triangleMaterialIndices[triangleIndex] = materialIndex;  
        }
    }

    //--------------------------------------------------------------------------

    void FBXImporter::readVertex(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, Vector3 &vertex)
    {
        FbxVector4 *lFbxCtrlPoints = lFbxGeometry->GetControlPoints();

        vertex[0] = static_cast<float32_t>(lFbxCtrlPoints[ctrlPointIndex][0]);
        vertex[1] = static_cast<float32_t>(lFbxCtrlPoints[ctrlPointIndex][1]);
        vertex[2] = static_cast<float32_t>(lFbxCtrlPoints[ctrlPointIndex][2]);
    }

    //--------------------------------------------------------------------------

    void FBXImporter::readNormal(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &normal)
    {
        if (lFbxGeometry->GetElementNormalCount() <= 0)
        {
            return;
        }

        FbxGeometryElementNormal *lFbxNormal = lFbxGeometry->GetElementNormal(0);
        switch (lFbxNormal->GetMappingMode())
        {
        case FbxGeometryElement::eByControlPoint:
            {
                switch (lFbxNormal->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        normal[0] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(ctrlPointIndex)[0]);
                        normal[1] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(ctrlPointIndex)[1]);
                        normal[2] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(ctrlPointIndex)[2]);
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxNormal->GetIndexArray().GetAt(ctrlPointIndex);
                        normal[0] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[0]);
                        normal[1] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[1]);
                        normal[2] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[2]);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case FbxGeometryElement::eByPolygonVertex:
            {
                switch (lFbxNormal->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        normal[0] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(vertexCounter)[0]);
                        normal[1] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(vertexCounter)[1]);
                        normal[2] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(vertexCounter)[2]);
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxNormal->GetIndexArray().GetAt(vertexCounter);
                        normal[0] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[0]);
                        normal[1] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[1]);
                        normal[2] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[2]);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    void FBXImporter::readTangent(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &tangent)
    {
        if (lFbxGeometry->GetElementTangentCount() <= 0)
        {
            return;
        }

        FbxGeometryElementTangent *lFbxTangent = lFbxGeometry->GetElementTangent(0);
        switch (lFbxTangent->GetMappingMode())
        {
        case FbxGeometryElement::eByControlPoint:
            {
                switch (lFbxTangent->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        tangent[0] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(ctrlPointIndex)[0]);
                        tangent[1] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(ctrlPointIndex)[1]);
                        tangent[2] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(ctrlPointIndex)[2]);
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxTangent->GetIndexArray().GetAt(ctrlPointIndex);
                        tangent[0] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[0]);
                        tangent[1] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[1]);
                        tangent[2] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[2]);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case FbxGeometryElement::eByPolygonVertex:
            {
                switch (lFbxTangent->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        tangent[0] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(vertexCounter)[0]);
                        tangent[1] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(vertexCounter)[1]);
                        tangent[2] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(vertexCounter)[2]);
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxTangent->GetIndexArray().GetAt(vertexCounter);
                        tangent[0] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[0]);
                        tangent[1] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[1]);
                        tangent[2] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[2]);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    void FBXImporter::readBinormal(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &binormal)
    {
        if (lFbxGeometry->GetElementBinormalCount() <= 0)
        {
            return;
        }

        FbxGeometryElementBinormal *lFbxBinormal = lFbxGeometry->GetElementBinormal(0);
        switch (lFbxBinormal->GetMappingMode())
        {
        case FbxGeometryElement::eByControlPoint:
            {
                switch (lFbxBinormal->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        binormal[0] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(ctrlPointIndex)[0]);
                        binormal[1] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(ctrlPointIndex)[1]);
                        binormal[2] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(ctrlPointIndex)[2]);
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxBinormal->GetIndexArray().GetAt(ctrlPointIndex);
                        binormal[0] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[0]);
                        binormal[1] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[1]);
                        binormal[2] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[2]);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case FbxGeometryElement::eByPolygonVertex:
            {
                switch (lFbxBinormal->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        binormal[0] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(vertexCounter)[0]);
                        binormal[1] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(vertexCounter)[1]);
                        binormal[2] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(vertexCounter)[2]);
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxBinormal->GetIndexArray().GetAt(vertexCounter);
                        binormal[0] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[0]);
                        binormal[1] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[1]);
                        binormal[2] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[2]);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    void FBXImporter::readColor(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, ColorRGBA &color)
    {
        if (lFbxGeometry->GetElementVertexColorCount() <= 0)
        {
            return;
        }

        FbxGeometryElementVertexColor *lFbxColor = lFbxGeometry->GetElementVertexColor(0);
        switch (lFbxColor->GetMappingMode())
        {
        case FbxGeometryElement::eByControlPoint:
            {
                switch (lFbxColor->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        color.red() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(ctrlPointIndex).mRed);
                        color.green() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(ctrlPointIndex).mGreen);
                        color.blue() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(ctrlPointIndex).mBlue);
                        color.alpha() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(ctrlPointIndex).mAlpha);
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxColor->GetIndexArray().GetAt(ctrlPointIndex);
                        color.red() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(id).mRed);
                        color.green() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(id).mGreen);
                        color.blue() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(id).mBlue);
                        color.alpha() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(id).mAlpha);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case FbxGeometryElement::eByPolygonVertex:
            {
                switch (lFbxColor->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        color.red() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(vertexCounter).mRed);
                        color.green() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(vertexCounter).mGreen);
                        color.blue() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(vertexCounter).mBlue);
                        color.alpha() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(vertexCounter).mAlpha);
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxColor->GetIndexArray().GetAt(vertexCounter);
                        color.red() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(id).mRed);
                        color.green() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(id).mGreen);
                        color.blue() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(id).mBlue);
                        color.alpha() = static_cast<float32_t>(lFbxColor->GetDirectArray().GetAt(id).mAlpha);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    void FBXImporter::readUV(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t texUVIndex, int32_t uvLayer, Vector2 &uv)
    {
        FbxGeometryElementUV *lFbxUV = lFbxGeometry->GetElementUV(uvLayer);

        switch (lFbxUV->GetMappingMode())
        {
        case FbxGeometryElement::eByControlPoint:
            {
                switch (lFbxUV->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        uv[0] = static_cast<float32_t>(lFbxUV->GetDirectArray().GetAt(ctrlPointIndex)[0]);
                        uv[1] = static_cast<float32_t>(lFbxUV->GetDirectArray().GetAt(ctrlPointIndex)[1]);
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxUV->GetIndexArray().GetAt(ctrlPointIndex);
                        uv[0] = static_cast<float32_t>(lFbxUV->GetDirectArray().GetAt(id)[0]);
                        uv[1] = static_cast<float32_t>(lFbxUV->GetDirectArray().GetAt(id)[1]);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case FbxGeometryElement::eByPolygonVertex:
            {
                switch (lFbxUV->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        uv[0] = static_cast<float32_t>(lFbxUV->GetDirectArray().GetAt(texUVIndex)[0]);
                        uv[1] = static_cast<float32_t>(lFbxUV->GetDirectArray().GetAt(texUVIndex)[1]);
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        uv[0] = static_cast<float32_t>(lFbxUV->GetDirectArray().GetAt(texUVIndex)[0]);
                        uv[1] = static_cast<float32_t>(lFbxUV->GetDirectArray().GetAt(texUVIndex)[1]);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::createMaterial(FbxSurfaceMaterial *lFbxMaterial, MaterialPtr &material)
    {
        TResult ret = T3D_OK;

        do
        {
            const auto lookForImplementation = [](FbxSurfaceMaterial *lFbxMaterial)
            {
                const FbxImplementation *lFbxImplementation = nullptr;
                if (!lFbxImplementation)
                    lFbxImplementation = GetImplementation(lFbxMaterial, FBXSDK_IMPLEMENTATION_CGFX);
                if (!lFbxImplementation)
                    lFbxImplementation = GetImplementation(lFbxMaterial, FBXSDK_IMPLEMENTATION_HLSL);
                if (!lFbxImplementation)
                    lFbxImplementation = GetImplementation(lFbxMaterial, FBXSDK_IMPLEMENTATION_SFX);
                if (!lFbxImplementation)
                    lFbxImplementation = GetImplementation(lFbxMaterial, FBXSDK_IMPLEMENTATION_OGS);
                if (!lFbxImplementation)
                    lFbxImplementation = GetImplementation(lFbxMaterial, FBXSDK_IMPLEMENTATION_SSSL);
                return lFbxImplementation; 
            };

            String materialName = lFbxMaterial->GetName();
            materialName = mOutputName + "-" + materialName;

            const FbxImplementation *lFbxImplementation = lookForImplementation(lFbxMaterial);
            if (lFbxImplementation != nullptr)
            {
                // FBX 是自定义的材质，直接使用默认材质代替
                material = T3D_MATERIAL_MGR.clone(materialName, mDefaultMaterial);
            }
            else if (lFbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
            {
                // Phong 材质
                material = T3D_MATERIAL_MGR.clone(materialName, mDefaultMaterial);

                FbxSurfacePhong *lFbxPhong = static_cast<FbxSurfacePhong *>(lFbxMaterial);

                // // Diffuse color
                // ColorRGBA diffuse(
                //     static_cast<float32_t>(lFbxPhong->Diffuse.Get()[0]),
                //     static_cast<float32_t>(lFbxPhong->Diffuse.Get()[1]),
                //     static_cast<float32_t>(lFbxPhong->Diffuse.Get()[2]));
                // material->setColor("Diffuse", diffuse);
                //
                // // Specular color
                // ColorRGBA specular(
                //     static_cast<float32_t>(lFbxPhong->Specular.Get()[0]),
                //     static_cast<float32_t>(lFbxPhong->Specular.Get()[1]),
                //     static_cast<float32_t>(lFbxPhong->Specular.Get()[2]));
                // material->setColor("Specular", specular);
                //
                // // Shininess
                // float32_t shininess = static_cast<float32_t>(lFbxPhong->Shininess.Get());
                // material->setFloat("Shininess", shininess);
            }
            else if (lFbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
            {
                // Lambert 材质
                material = T3D_MATERIAL_MGR.clone(materialName, mDefaultMaterial);
            }
            else
            {
                // 其他材质
                material = T3D_MATERIAL_MGR.clone(materialName, mDefaultMaterial);
            }

            int lTextureIndex;
            FBXSDK_FOR_EACH_TEXTURE(lTextureIndex)
            {
                FbxProperty lFbxProperty = lFbxMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[lTextureIndex]);
                TexturePtr texture;
                ret = createTexture(lFbxProperty, texture);
                if (T3D_FAILED(ret) || texture == nullptr)
                {
                    continue;
                }

                const String texName = "_MainTex";
                material->setTexture(texName, texture->getUUID());
            }

            // mResources.emplace(materialName, material);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::createTexture(const FbxProperty &lFbxProperty, TexturePtr &texture)
    {
        TResult ret = T3D_OK;

        do
        {
            if (lFbxProperty.IsValid() )
            {
                const auto _createTexture = [this](FbxTexture *lFbxTexture, int lFbxBlendMode, TexturePtr &texture)
                {
                    TResult ret = T3D_OK;
                    
                    FbxFileTexture *lFbxFileTexture = FbxCast<FbxFileTexture>(lFbxTexture);
                    FbxProceduralTexture *lFbxProceduralTexture = FbxCast<FbxProceduralTexture>(lFbxTexture);

                    do
                    {
                        if (lFbxFileTexture == nullptr)
                        {
                            MCONV_LOG_WARNING("Invalid texture");
                            ret = T3D_ERR_INVALID_PARAM;
                            break;
                        }

                        String path = lFbxFileTexture->GetFileName();
                        String dir, title, ext;
                        Dir::parsePath(path, dir, title, ext);
                        
                        if (!Dir::exists(path))
                        {
                            path = mInputDir;
                        }
                        else
                        {
                            path = dir;
                        }

                        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kRead);
                        T3D_ASSERT(archive != nullptr);
                        String filename = title + "." + ext;
                        ImagePtr image = T3D_IMAGE_MGR.loadImage(archive, filename);
                        T3D_ASSERT(image != nullptr);
                        if (mIsOpenGLMetric)
                        {
                            // OpenGL 制式，纹理要翻转
                            image->flip();
                        }
                        texture = T3D_TEXTURE_MGR.createTexture2D(title, image);
                        T3D_ASSERT(texture != nullptr);

                        SamplerDesc samplerDesc;
                        if (lFbxTexture->GetWrapModeU() == FbxTexture::eRepeat)
                        {
                            samplerDesc.AddressU = TextureAddressMode::kWrap;
                        }
                        else if (lFbxTexture->GetWrapModeU() == FbxTexture::eClamp)
                        {
                            samplerDesc.AddressU = TextureAddressMode::kClamp;
                        }
                        
                        if (lFbxTexture->GetWrapModeV() == FbxTexture::eRepeat)
                        {
                            samplerDesc.AddressV = TextureAddressMode::kWrap;
                        }
                        else if (lFbxTexture->GetWrapModeV() == FbxTexture::eClamp)
                        {
                            samplerDesc.AddressV = TextureAddressMode::kClamp;
                        }
                        
                        texture->setSamplerDesc(samplerDesc);
                    } while (false);
                    
                    
                    // MCONV_LOG_INFO("            Name: \"%s\"", (const char *)lFbxTexture->GetName());
                    // if (lFbxFileTexture)
                    // {
                    //     MCONV_LOG_INFO("            Type: File Texture");
                    //     MCONV_LOG_INFO("            File Name: \"%s\"", (const char *)lFbxFileTexture->GetFileName());
                    // }
                    // else if (lFbxProceduralTexture)
                    // {
                    //     MCONV_LOG_INFO("            Type: Procedural Texture");
                    // }
                    // MCONV_LOG_INFO("            Scale U: %f", lFbxTexture->GetScaleU());
                    // MCONV_LOG_INFO("            Scale V: %f", lFbxTexture->GetScaleV());
                    // MCONV_LOG_INFO("            Translation U: %f", lFbxTexture->GetTranslationU());
                    // MCONV_LOG_INFO("            Translation V: %f", lFbxTexture->GetTranslationV());
                    // MCONV_LOG_INFO("            Swap UV: %d", lFbxTexture->GetSwapUV());
                    // MCONV_LOG_INFO("            Rotation U: %f", lFbxTexture->GetRotationU());
                    // MCONV_LOG_INFO("            Rotation V: %f", lFbxTexture->GetRotationV());
                    // MCONV_LOG_INFO("            Rotation W: %f", lFbxTexture->GetRotationW());
                    // const char* lAlphaSources[] = { "None", "RGB Intensity", "Black" };
                    // MCONV_LOG_INFO("            Alpha Source: %s", lAlphaSources[lFbxTexture->GetAlphaSource()]);
                    // MCONV_LOG_INFO("            Cropping Left: %d", lFbxTexture->GetCroppingLeft());
                    // MCONV_LOG_INFO("            Cropping Top: %d", lFbxTexture->GetCroppingTop());
                    // MCONV_LOG_INFO("            Cropping Right: %d", lFbxTexture->GetCroppingRight());
                    // MCONV_LOG_INFO("            Cropping Bottom: %d", lFbxTexture->GetCroppingBottom());
                    // const char* lMappingTypes[] = { "Null", "Planar", "Spherical", "Cylindrical", 
                    //     "Box", "Face", "UV", "Environment" };
                    // MCONV_LOG_INFO("            Mapping Type: %s", lMappingTypes[lFbxTexture->GetMappingType()]);
                    // if (lFbxTexture->GetMappingType() == FbxTexture::ePlanar)
                    // {
                    //     const char* lPlanarMappingNormals[] = { "X", "Y", "Z" };
                    //     MCONV_LOG_INFO("            Planar Mapping Normal: %s", lPlanarMappingNormals[lFbxTexture->GetPlanarMappingNormal()]);
                    // }
                    // const char* lBlendModes[]   = { "Translucent", "Additive", "Modulate", "Modulate2", "Over", "Normal", "Dissolve", "Darken", "ColorBurn", "LinearBurn",
                    //                                 "DarkerColor", "Lighten", "Screen", "ColorDodge", "LinearDodge", "LighterColor", "SoftLight", "HardLight", "VividLight",
                    //                                 "LinearLight", "PinLight", "HardMix", "Difference", "Exclusion", "Substract", "Divide", "Hue", "Saturation", "Color",
                    //                                 "Luminosity", "Overlay"};   
                    //
                    // if (lFbxBlendMode >= 0)
                    // {
                    //     MCONV_LOG_INFO("            Blend Mode: %s", lBlendModes[lFbxBlendMode]);
                    // }
                    // MCONV_LOG_INFO("            Alpha: %f", lFbxTexture->GetDefaultAlpha());
                    // if (lFbxFileTexture)
                    // {
                    //     const char* lMaterialUses[] = { "Model Material", "Default Material" };
                    //     MCONV_LOG_INFO("            Material Use: %s", lMaterialUses[lFbxFileTexture->GetMaterialUse()]);
                    // }
                    // const char* pTextureUses[] = { "Standard", "Shadow Map", "Light Map", 
                    //     "Spherical Reflexion Map", "Sphere Reflexion Map", "Bump Normal Map" };
                    // MCONV_LOG_INFO("            Texture Use: %s", pTextureUses[lFbxTexture->GetTextureUse()]);
                    // MCONV_LOG_INFO("");

                    return ret;
                };
                
                int lTextureCount = lFbxProperty.GetSrcObjectCount<FbxTexture>();
                for (int j = 0; j < lTextureCount; ++j)
                {
                    //Here we have to check if it's layeredtextures, or just textures:
                    FbxLayeredTexture *lFbxLayeredTexture = lFbxProperty.GetSrcObject<FbxLayeredTexture>(j);
                    if (lFbxLayeredTexture)
                    {                
                        int lNbTextures = lFbxLayeredTexture->GetSrcObjectCount<FbxTexture>();
                        for (int k =0; k<lNbTextures; ++k)
                        {
                            FbxTexture* lFbxTexture = lFbxLayeredTexture->GetSrcObject<FbxTexture>(k);
                            if (lFbxTexture)
                            {
                                //NOTE the blend mode is ALWAYS on the LayeredTexture and NOT the one on the texture.
                                //Why is that?  because one texture can be shared on different layered textures and might
                                //have different blend modes.
                                FbxLayeredTexture::EBlendMode lBlendMode;
                                lFbxLayeredTexture->GetTextureBlendMode(k, lBlendMode);
                                FbxFileTexture *lFbxFileTexture = FbxCast<FbxFileTexture>(lFbxTexture);
                                MCONV_LOG_INFO("Layered Textures [%d] - textures[%d] : %s", j, k, (const char *)lFbxProperty.GetName());
                                ret = _createTexture(lFbxTexture, lBlendMode, texture);
                                if (T3D_FAILED(ret))
                                {
                                    break;
                                }

                                String name = texture->getName() + "." + Resource::EXT_TEXTURE;
                                mResources.emplace(name, texture);
                            }
                        }
                    }
                    else
                    {
                        //no layered texture simply get on the property
                        FbxTexture* lFbxTexture = lFbxProperty.GetSrcObject<FbxTexture>(j);
                        if (lFbxTexture)
                        {
                            //display connected Material header only at the first time
                            FbxFileTexture *lFbxFileTexture = FbxCast<FbxFileTexture>(lFbxTexture);
                            MCONV_LOG_INFO("Textures[%d] : %s", j, (const char *)lFbxProperty.GetName());
                            ret = _createTexture(lFbxTexture, -1, texture);
                            if (T3D_FAILED(ret))
                            {
                                break;
                            }

                            String name = texture->getName() + "." + Resource::EXT_TEXTURE;
                            mResources.emplace(name, texture);
                        }
                    }

                    if (T3D_FAILED(ret))
                    {
                        break;
                    }
                }
            }//end if pProperty
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult FBXImporter::createSubMesh(const String &name, Material *material, SubMeshPtr &submesh)
    {
        TResult ret = T3D_OK;

        do
        {
            bool is16Bits = true;
            Buffer indices;
            if (mIndices.size() > std::numeric_limits<uint16_t>::max())
            {
                indices.DataSize = mIndices.size() * sizeof(uint32_t);
                indices.Data = T3D_POD_NEW_ARRAY(uint8_t, indices.DataSize);
                uint32_t *data = (uint32_t *)indices.Data;
                for (uint32_t idx = 0; idx < mIndices.size(); idx++)
                {
                    *data = mIndices[idx];
                    data++;
                }
                is16Bits = false;
            }
            else
            {
                indices.DataSize = mIndices.size() * sizeof(uint16_t);
                indices.Data = T3D_POD_NEW_ARRAY(uint8_t, indices.DataSize);
                uint16_t *data = (uint16_t *)indices.Data;
                for (uint32_t idx = 0; idx < mIndices.size(); idx++)
                {
                    *data = static_cast<uint16_t>(mIndices[idx]);
                    data++;
                }
                is16Bits = true;
            }

            submesh = SubMesh::create(name, material->getUUID(), PrimitiveType::kTriangleList, std::move(indices), is16Bits);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::createMesh()
    {
        TResult ret = T3D_OK;

        do
        {
            Vertices vertices;
            VertexAttributes attributes;
            VertexStrides strides;
            VertexOffsets offsets;
            
            uint32_t offset = 0;
            
            // position
            VertexAttribute attrPos(0, offset, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
            attributes.emplace_back(attrPos);
            offset += sizeof(Vector3);
            
            if (!mColors.empty())
            {
                // diffuse color
                T3D_ASSERT(mColors.size() == mVertices.size());
                VertexAttribute attrCol(0, offset, VertexAttribute::Type::E_VAT_FLOAT4, VertexAttribute::Semantic::E_VAS_DIFFUSE, 0);
                attributes.emplace_back(attrCol);
                offset += sizeof(ColorRGBA);
            }

            for (uint32_t i = 0; i < T3D_MAX_TEXTURE_LEVEL; i++)
            {
                // uv
                if (!mTexCoords[i].empty())
                {
                    T3D_ASSERT(mTexCoords[i].size() == mVertices.size());
                    VertexAttribute attrUV(0, offset, VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, i);
                    attributes.emplace_back(attrUV);
                    offset += sizeof(Vector2);
                }
            }

            if (!mNormals.empty())
            {
                // normal
                T3D_ASSERT(mNormals.size() == mVertices.size());
                VertexAttribute attrNor(0, offset, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_NORMAL, 0);
                attributes.emplace_back(attrNor);
                offset += sizeof(Vector3);
            }

            if (!mBinormals.empty())
            {
                // binormal
                T3D_ASSERT(mBinormals.size() == mVertices.size());
                VertexAttribute attrBin(0, offset, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_BINORMAL, 0);
                attributes.emplace_back(attrBin);
                offset += sizeof(Vector3);
            }

            if (!mTangents.empty())
            {
                // tangent
                T3D_ASSERT(mTangents.size() == mVertices.size());
                VertexAttribute attrTan(0, offset, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_TANGENT, 0);
                attributes.emplace_back(attrTan);
                offset += sizeof(Vector3);
            }

            bool hasSkin = false;

            if (!mBlendWeights.empty() && !mBlendIndices.empty())
            {
                // blend weights
                T3D_ASSERT(mBlendWeights.size() == mVertices.size());
                T3D_ASSERT(!mBlendIndices.empty());
                VertexAttribute attrWeight(0, offset, VertexAttribute::Type::E_VAT_FLOAT4, VertexAttribute::Semantic::E_VAS_BLENDWEIGHT, 0);
                attributes.emplace_back(attrWeight);
                offset += sizeof(Vector4);

                // blend indices
                T3D_ASSERT(mBlendIndices.size() == mVertices.size());
                T3D_ASSERT(!mBlendIndices.empty());
                VertexAttribute attrIndex(0, offset, VertexAttribute::Type::E_VAT_UBYTE4, VertexAttribute::Semantic::E_VAS_BLENDINDICES, 0);
                attributes.emplace_back(attrIndex);
                offset += sizeof(uint32_t);

                hasSkin = true;
            }
            else if (!mBlendWeights.empty() || !mBlendIndices.empty())
            {
                ret = T3D_ERR_FAIL;
                MCONV_LOG_ERROR("Blend weights and indices must be both present or both absent.");
                break;
            }
            else
            {
                hasSkin = false;
            }

            TArray<float32_t> data;

            for (size_t i = 0; i < mVertices.size(); i++)
            {
                // position
                const Vector3 &pos = mVertices[i];
                data.emplace_back(pos[0]);
                data.emplace_back(pos[1]);
                data.emplace_back(pos[2]);

                // diffuse color
                if (!mColors.empty())
                {
                    const ColorRGBA &color = mColors[i];
                    data.emplace_back(color.red());
                    data.emplace_back(color.green());
                    data.emplace_back(color.blue());
                    data.emplace_back(color.alpha());
                }

                // texture uv
                for (const auto &texCoord : mTexCoords)
                {
                    if (!texCoord.empty())
                    {
                        const Vector2 &uv = texCoord[i];
                        data.emplace_back(uv[0]);
                        data.emplace_back(uv[1]);
                    }
                }

                // normal
                if (!mNormals.empty())
                {
                    const Vector3 &nor = mNormals[i];
                    data.emplace_back(nor[0]);
                    data.emplace_back(nor[1]);
                    data.emplace_back(nor[2]);
                }

                // binormal
                if (!mBinormals.empty())
                {
                    const Vector3 &bin = mBinormals[i];
                    data.emplace_back(bin[0]);
                    data.emplace_back(bin[1]);
                    data.emplace_back(bin[2]);
                }
                
                // tangent
                if (!mTangents.empty())
                {
                    const Vector3 &tan = mTangents[i];
                    data.emplace_back(tan[0]);
                    data.emplace_back(tan[1]);
                    data.emplace_back(tan[2]);
                }
            }

            Buffer buffer;
            buffer.setData(data.data(), data.size() * sizeof(float32_t));
            
            vertices.emplace_back(buffer);
            strides.emplace_back(offset);
            offsets.emplace_back(0);

            String name = mOutputName;

            MeshPtr mesh;
            
            if (!hasSkin)
            {
                mesh = T3D_MESH_MGR.createMesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(mSubMeshes));
                if (mesh == nullptr)
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    MCONV_LOG_ERROR("Failed to create mesh %s", name.c_str());
                    break;
                }
            }
            else
            {
                mesh = T3D_MESH_MGR.createSkinnedMesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(mSubMeshes), mSkeleton, mAnimation);
                if (mesh == nullptr)
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    MCONV_LOG_ERROR("Failed to create skinned mesh %s", name.c_str());
                    break;
                }
            }

            name = name + "." + Resource::EXT_MESH;
            mResources.emplace(name, mesh);
        } while (false);

        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::processSkinnedMesh(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Starting processing skinned mesh ...")

            ret = getFbxMeshNode(lFbxScene->GetRootNode(), true);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to process skinned mesh ! ERROR [%d]", ret)
                break;
            }

            // 生成骨骼信息
            ret = generateBones();
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            // 处理完 mesh 后，提取 blend weights 和 blend indices
            // 使用 generateBones() 中建立的骨骼名称到索引的映射
            ret = extractBlendWeightsAndIndices(lFbxScene, lFbxScene->GetRootNode(), mBoneNameToIndexMap);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to extract blend weights and indices ! ERROR [%d]", ret)
                break;
            }

            MCONV_LOG_INFO("Completed processing skinned mesh !")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::extractBlendWeightsAndIndices(FbxScene *lFbxScene, FbxNode *lFbxNode, const TMap<String, uint8_t> &boneNameToIndexMap)
    {
        TResult ret = T3D_OK;

        do
        {
            if (lFbxNode->GetNodeAttribute() != nullptr)
            {
                if (lFbxNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
                {
                    FbxMesh *lFbxMesh = lFbxNode->GetMesh();
                    if (lFbxMesh != nullptr)
                    {
                        ret = processMeshBlendData(lFbxMesh, boneNameToIndexMap);
                        if (T3D_FAILED(ret))
                        {
                            break;
                        }
                    }
                }
            }

            // 递归处理子节点
            for (int32_t i = 0; i < lFbxNode->GetChildCount(); i++)
            {
                ret = extractBlendWeightsAndIndices(lFbxScene, lFbxNode->GetChild(i), boneNameToIndexMap);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::processMeshBlendData(FbxMesh *lFbxMesh, const TMap<String, uint8_t> &boneNameToIndexMap)
    {
        TResult ret = T3D_OK;

        do
        {
            // 检查是否有蒙皮数据
            int32_t deformerCount = lFbxMesh->GetDeformerCount(FbxDeformer::eSkin);
            if (deformerCount == 0)
            {
                // 没有蒙皮数据，跳过
                break;
            }

            // 获取控制点数量
            int32_t controlPointCount = lFbxMesh->GetControlPointsCount();

            // 为每个控制点创建权重和索引的列表
            // 结构：每个控制点可以有多个骨骼影响
            TArray<TArray<BoneWeightInfo>> controlPointWeightInfos(controlPointCount);
            
            // 遍历所有 Skin Deformer
            for (int32_t i = 0; i < deformerCount; i++)
            {
                FbxSkin *lFbxSkin = static_cast<FbxSkin *>(lFbxMesh->GetDeformer(i, FbxDeformer::eSkin));
                if (!lFbxSkin)
                {
                    continue;
                }

                // 遍历所有 Cluster（每个 Cluster 对应一个骨骼）
                int32_t clusterCount = lFbxSkin->GetClusterCount();
                for (int32_t c = 0; c < clusterCount; c++)
                {
                    FbxCluster *lFbxCluster = lFbxSkin->GetCluster(c);
                    if (!lFbxCluster)
                    {
                        continue;
                    }

                    // 获取骨骼节点
                    FbxNode *lFbxLink = lFbxCluster->GetLink();
                    if (!lFbxLink)
                    {
                        continue;
                    }

                    String boneName = lFbxLink->GetName();

                    // 查找骨骼在 mBones 中的索引
                    auto it = boneNameToIndexMap.find(boneName);
                    if (it == boneNameToIndexMap.end())
                    {
                        MCONV_LOG_WARNING("Bone [%s] not found in bone index map, skipping.", boneName.c_str());
                        continue;
                    }

                    uint8_t boneIndex = it->second;

                    // 获取该 Cluster 影响的控制点索引和权重
                    int32_t controlPointIndexCount = lFbxCluster->GetControlPointIndicesCount();
                    int *controlPointIndices = lFbxCluster->GetControlPointIndices();
                    double *fbxControlPointWeights = lFbxCluster->GetControlPointWeights();

                    if (controlPointIndices == nullptr || fbxControlPointWeights == nullptr)
                    {
                        continue;
                    }

                    // 为每个受影响的控制点添加权重信息
                    for (int32_t j = 0; j < controlPointIndexCount; j++)
                    {
                        int32_t ctrlPointIndex = controlPointIndices[j];
                        if (ctrlPointIndex < 0 || ctrlPointIndex >= controlPointCount)
                        {
                            continue;
                        }

                        double weight = fbxControlPointWeights[j];
                        if (weight > 0.0)
                        {
                            BoneWeightInfo info;
                            info.boneIndex = boneIndex;
                            info.weight = static_cast<float32_t>(weight);
                            controlPointWeightInfos[ctrlPointIndex].emplace_back(info);
                        }
                    }
                }
            }

            // 为每个控制点选择前 T3D_MAX_BLEND_BONES 个最大的权重并归一化
            // 然后基于实际顶点数量和控制点映射来创建权重和索引数组
            mBlendWeights.clear();
            mBlendIndices.clear();

            // 检查是否有顶点到控制点的映射
            if (mVertexToControlPointMap.empty())
            {
                MCONV_LOG_WARNING("Vertex to control point map is empty, cannot create blend weights/indices.");
                break;
            }

            // 为每个控制点预处理权重和索引
            TArray<TArray<float32_t>> controlPointWeights(controlPointCount);
            TArray<TArray<uint8_t>> controlPointIndices(controlPointCount);

            for (int32_t i = 0; i < controlPointCount; i++)
            {
                // 对每个控制点的权重进行排序，取前 T3D_MAX_BLEND_BONES 个最大的
                TArray<BoneWeightInfo> &weights = controlPointWeightInfos[i];
                
                // 按权重降序排序
                std::sort(weights.begin(), weights.end(), 
                    [](const BoneWeightInfo &a, const BoneWeightInfo &b) {
                        return a.weight > b.weight;
                    });

                // 创建权重和索引数组
                TArray<float32_t> vertexWeights(T3D_MAX_BLEND_BONES, 0.0f);
                TArray<uint8_t> vertexIndices(T3D_MAX_BLEND_BONES, 0xFF);

                // 取前 T3D_MAX_BLEND_BONES 个权重，并归一化
                size_t count = (weights.size() < T3D_MAX_BLEND_BONES) ? weights.size() : T3D_MAX_BLEND_BONES;
                float32_t totalWeight = 0.0f;

                for (size_t j = 0; j < count; j++)
                {
                    vertexWeights[j] = weights[j].weight;
                    vertexIndices[j] = weights[j].boneIndex;
                    totalWeight += weights[j].weight;
                }

                // 归一化权重（确保总和为 1.0）
                if (totalWeight > 0.0f)
                {
                    for (size_t j = 0; j < count; j++)
                    {
                        vertexWeights[j] /= totalWeight;
                    }
                }

                controlPointWeights[i] = vertexWeights;
                controlPointIndices[i] = vertexIndices;
            }

            // 基于实际顶点数量和控制点映射创建权重和索引数组
            size_t vertexCount = mVertexToControlPointMap.size();
            mBlendWeights.reserve(vertexCount);
            mBlendIndices.reserve(vertexCount);

            for (size_t i = 0; i < vertexCount; i++)
            {
                int32_t ctrlPointIndex = mVertexToControlPointMap[i];
                if (ctrlPointIndex >= 0 && ctrlPointIndex < controlPointCount)
                {
                    // 使用对应控制点的权重和索引
                    mBlendWeights.emplace_back(controlPointWeights[ctrlPointIndex]);
                    mBlendIndices.emplace_back(controlPointIndices[ctrlPointIndex]);
                }
                else
                {
                    // 无效的控制点索引，使用默认值
                    TArray<float32_t> defaultWeights(T3D_MAX_BLEND_BONES, 0.0f);
                    TArray<uint8_t> defaultIndices(T3D_MAX_BLEND_BONES, 0xFF);
                    mBlendWeights.emplace_back(defaultWeights);
                    mBlendIndices.emplace_back(defaultIndices);
                }
            }

            MCONV_LOG_INFO("Extracted blend weights and indices for %lld vertices (from %d control points)", 
                vertexCount, controlPointCount)

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::processSkeleton(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start collecting skeleton ...")
            ret = getSkeleton(lFbxScene, lFbxScene->GetRootNode(), nullptr);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to collect skeleton ! ERROR [%d]", ret)
                break;
            }
            MCONV_LOG_INFO("Completed collecting skeleton !")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::getSkeleton(FbxScene *lFbxScene, FbxNode *lFbxNode, GameObjectPtr parent)
    {
        TResult ret = T3D_OK;

        do
        {
            if (lFbxNode->GetNodeAttribute())
            {
                switch (lFbxNode->GetNodeAttribute()->GetAttributeType())
                {
                case FbxNodeAttribute::eSkeleton:
                    {
                        bool isRoot = false;
                    
                        FbxNode *lFbxParent = lFbxNode->GetParent();
                        if ((lFbxParent != nullptr
                            && (lFbxParent->GetNodeAttribute() != nullptr
                            && lFbxParent->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eSkeleton
                            || lFbxParent->GetNodeAttribute() == nullptr))
                            || lFbxParent == nullptr)
                        {
                            isRoot = true;
                        }
                    
                        const String name = lFbxNode->GetName();
                        GameObjectPtr go = GameObject::createWithTransform(name);

                        // FbxTimeSpan timeSpan;
                        // lFbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeSpan);
                        // FbxTime start = timeSpan.GetStart();
                        // FbxTime end = timeSpan.GetStop();

                        // FbxAMatrix lFbxLocalMat = lFbxNode->EvaluateLocalTransform(FBXSDK_TIME_ZERO);
                        // FbxVector4 lFbxLocalT = lFbxLocalMat.GetT();
                        // FbxQuaternion lFbxLocalQ = lFbxLocalMat.GetQ();
                        // FbxVector4 lFbxLocalS = lFbxLocalMat.GetS();

                        FbxAMatrix lFbxLocalMat = lFbxNode->EvaluateLocalTransform(FBXSDK_TIME_ZERO);
                        FbxVector4 lFbxLocalT = lFbxLocalMat.GetT();
                        FbxVector4 lFbxLocalS = lFbxLocalMat.GetS();
                        FbxVector4 lFbxLocalR = lFbxLocalMat.GetR();
                        FbxQuaternion lFbxLocalQ = lFbxLocalMat.GetQ();

                        MCONV_LOG_DEBUG("Fbx node: %s(%d), T: (%f, %f, %f, %f), Q: (%f, %f, %f, %f), E: (%f, %f, %f, %f), S: (%f, %f, %f, %f)",
                            lFbxNode->GetName(), isRoot,
                            lFbxLocalT[0], lFbxLocalT[1], lFbxLocalT[2], lFbxLocalT[3],
                            lFbxLocalQ[3], lFbxLocalQ[0], lFbxLocalQ[1], lFbxLocalQ[2],
                            lFbxLocalR[0], lFbxLocalR[1], lFbxLocalR[2], lFbxLocalR[3],
                            lFbxLocalS[0], lFbxLocalS[1], lFbxLocalS[2], lFbxLocalS[3])

                        // 转换成 tiny3d 的坐标系
                        Vector3 localT = FbxPointToTinyVector3Remap(lFbxLocalT);
                        Quaternion localQ = FbxEulerToTinyQuaternion(lFbxLocalR);
                        Vector3 localS = FbxPointToTinyVector3(lFbxLocalS);
                    
                        // FbxVector4 lFbxLocalT = evaluatePropertyInitialValue(lFbxNode->LclTranslation);
                        // FbxVector4 lFbxLocalS = evaluatePropertyInitialValue(lFbxNode->LclScaling);
                        // FbxVector4 lFbxLocalR = evaluatePropertyInitialValue(lFbxNode->LclRotation);

                        // FbxVector4 lFbxLocalT = lFbxNode->LclTranslation.Get();
                        // FbxVector4 lFbxLocalS = lFbxNode->LclScaling.Get();
                        // FbxVector4 lFbxLocalR = lFbxNode->LclRotation.Get();

                        // FbxAMatrix rotationMatrix;
                        // lFbxLocalR[1] = -lFbxLocalR[1];
                        // lFbxLocalR[2] = -lFbxLocalR[2];
                        // rotationMatrix.SetROnly(lFbxLocalR);
                        // FbxQuaternion lFbxLocalQ = rotationMatrix.GetQ();
                        // Quaternion q((float32_t)lFbxLocalQ[3], (float32_t)lFbxLocalQ[0], (float32_t)lFbxLocalQ[1], (float32_t)lFbxLocalQ[2]);
                        // q.normalize();
                        //
                        // Transform3D *n = static_cast<Transform3D *>(go->getTransformNode());
                        // n->setPosition((float32_t)-lFbxLocalT[0], (float32_t)lFbxLocalT[1], (float32_t)lFbxLocalT[2]);
                        // n->setOrientation(q);
                        // n->setScaling((float32_t)lFbxLocalS[0], (float32_t)lFbxLocalS[1], (float32_t)lFbxLocalS[2]);

                        Transform3D *n = static_cast<Transform3D *>(go->getTransformNode());
                        n->setPosition(localT);
                        n->setOrientation(localQ);
                        n->setScaling(localS);
                    
                        if (parent != nullptr)
                        {
                            TransformNode *p = parent->getTransformNode();
                            p->addChild(n);
                        }
                        else
                        {
                            T3D_ASSERT(mRoot == nullptr);
                            mRoot = go;
                            T3D_SCENE_MGR.getCurrentScene()->getRootTransform()->addChild(mRoot->getTransformNode());
                        }

                        BoneInfo info;
                        info.gameObject = go;
                        mBoneMap.emplace(name, info);

                        const Vector3 pos = n->getPosition();
                        const Quaternion ori = n->getOrientation();
                        const Vector3 scaling = n->getScaling();
                        // const Transform &xform = n->getLocalToWorldTransform();
                        // const Vector3 pos = xform.getTranslation();
                        // const Quaternion ori = xform.getOrientation();
                        // const Vector3 scaling = xform.getScaling();

                        // Matrix3 matR;
                        // ori.toRotationMatrix(matR);
                        // Radian xAngle, yAngle, zAngle;
                        // // matR.toEulerAnglesZXY(zAngle, xAngle, yAngle);
                        // matR.toEulerAnglesYXZ(yAngle, xAngle, zAngle);
                        // MCONV_LOG_DEBUG("Name : %s, Parant : %s, T : %s, Euler : (%f, %f, %f), Q : %s, S : %s",
                        //     name.c_str(),
                        //     parent != nullptr ? parent->getName().c_str() : "null",
                        //     pos.getDebugString().c_str(),
                        //     xAngle.valueDegrees(), yAngle.valueDegrees(), zAngle.valueDegrees(),
                        //     ori.getDebugString().c_str(),
                        //     scaling.getDebugString().c_str())

                        parent = go;
                    }
                    break;
                }
            }

            for (int32_t i = 0; i < lFbxNode->GetChildCount(); i++)
            {
                FbxNode *lFbxChild = lFbxNode->GetChild(i);
                getSkeleton(lFbxScene, lFbxChild, parent);
            }
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult FBXImporter::getOffsetMatrix(FbxMesh *lFbxMesh)
    {
        TResult ret = T3D_OK;
        
        do
        {
            int32_t deformerCount = lFbxMesh->GetDeformerCount(FbxDeformer::eSkin);
            for (int32_t i = 0; i < deformerCount; i++)
            {
                FbxSkin *lFbxSkin = static_cast<FbxSkin *>(lFbxMesh->GetDeformer(i, FbxDeformer::eSkin));
                if (!lFbxSkin)
                {
                    continue;
                }

                int32_t clusterCount = lFbxSkin->GetClusterCount();
                for (int32_t c = 0; c < clusterCount; c++)
                {
                    FbxCluster *lFbxCluster = lFbxSkin->GetCluster(c);
                    if (!lFbxCluster)
                    {
                        continue;
                    }

                    // 获取骨骼节点
                    FbxNode *lFbxLink = lFbxCluster->GetLink();
                    if (!lFbxLink)
                    {
                        continue;
                    }

                    String boneName = lFbxLink->GetName();
                    
                    // 查找对应的骨骼信息
                    auto it = mBoneMap.find(boneName);
                    if (it == mBoneMap.end())
                    {
                        MCONV_LOG_WARNING("Bone [%s] not found in bone map, skipping offset matrix.", boneName.c_str());
                        continue;
                    }

                    // 获取绑定姿态矩阵
                    // TransformMatrix: 网格节点在绑定姿态时的世界空间变换矩阵
                    // TransformLinkMatrix: 骨骼节点在绑定姿态时的世界空间变换矩阵
                    FbxAMatrix lFbxTransformMatrix;
                    FbxAMatrix lFbxTransformLinkMatrix;
                    lFbxCluster->GetTransformMatrix(lFbxTransformMatrix);
                    lFbxCluster->GetTransformLinkMatrix(lFbxTransformLinkMatrix);

                    // 计算 offset matrix
                    // offset matrix = TransformMatrix.Inverse() * TransformLinkMatrix
                    // 这个矩阵用于将顶点从模型空间转换到骨骼空间
                    FbxAMatrix lFbxOffsetMatrix = lFbxTransformMatrix.Inverse() * lFbxTransformLinkMatrix;

                    // 将 FbxAMatrix 转换为 Matrix4
                    // FBX 矩阵是列优先存储的，需要转换为行优先
                    Matrix4 offsetMatrix(
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(0, 0)),
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(1, 0)), 
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(2, 0)),
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(3, 0)),
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(0, 1)),
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(1, 1)), 
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(2, 1)),
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(3, 1)),
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(0, 2)),
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(1, 2)), 
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(2, 2)),
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(3, 2)),
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(0, 3)),
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(1, 3)), 
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(2, 3)),
                        static_cast<float32_t>(lFbxOffsetMatrix.Get(3, 3))
                    );

                    // 存储 offset matrix 到骨骼信息中
                    it->second.offsetMatrix = offsetMatrix;

                    MCONV_LOG_DEBUG("Bone [%s] offset matrix set.", boneName.c_str());
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::generateBones()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mBoneMap.empty())
            {
                // 没有骨骼数据，直接返回
                break;
            }

            // 建立 GameObject -> 骨骼名称的反向映射
            TMap<GameObject *, String> goToNameMap;
            for (const auto &item : mBoneMap)
            {
                goToNameMap.emplace(item.second.gameObject, item.first);
            }

            // 建立骨骼名称 -> 索引的映射（先按遍历顺序分配索引）
            // 为了保持顺序，我们可以从根节点开始递归遍历，或者简单地按 mBoneMap 的顺序
            TMap<String, uint16_t> nameToIndexMap;
            uint16_t currentIndex = 0;

            // 辅助函数：递归遍历骨架并分配索引
            std::function<void(GameObject *)> assignIndices = [&](GameObject *go)
            {
                if (go == nullptr)
                {
                    return;
                }

                auto it = goToNameMap.find(go);
                if (it != goToNameMap.end())
                {
                    const String &boneName = it->second;
                    nameToIndexMap.emplace(boneName, currentIndex++);

                    // 递归处理子节点
                    TransformNode *transformNode = go->getTransformNode();
                    if (transformNode != nullptr)
                    {
                        TransformNode::const_child_iterator itr = transformNode->child_begin();
                        while (itr != transformNode->child_end())
                        {
                            assignIndices((*itr)->getGameObject());
                            ++itr;
                        }
                    }
                }
            };

            // 从根节点开始分配索引（保持骨架层次顺序）
            if (mRoot != nullptr)
            {
                assignIndices(mRoot.get());
            }
            else
            {
                // 如果没有根节点，按 mBoneMap 的顺序分配索引
                for (const auto &item : mBoneMap)
                {
                    nameToIndexMap.emplace(item.first, currentIndex++);
                }
            }

            // 清空之前的骨骼数组
            // mBones 已删除，骨骼层级通过 mRoot 的 GameObject 树表达

            // 按照索引顺序在骨骼 GameObject 上挂载 Bone 组件（使用之前建立的索引顺序）
            TArray<String> boneNamesInOrder(nameToIndexMap.size());
            for (const auto &item : nameToIndexMap)
            {
                if (item.second < boneNamesInOrder.size())
                {
                    boneNamesInOrder[item.second] = item.first;
                }
            }

            for (const String &boneName : boneNamesInOrder)
            {
                auto it = mBoneMap.find(boneName);
                if (it == mBoneMap.end())
                {
                    continue;
                }

                const BoneInfo &boneInfo = it->second;
                GameObject *go = boneInfo.gameObject;
                
                if (go == nullptr)
                {
                    MCONV_LOG_WARNING("Bone [%s] has null GameObject, skipping.", boneName.c_str());
                    continue;
                }

                Transform3D *xform = static_cast<Transform3D *>(go->getTransformNode());
                if (xform == nullptr)
                {
                    MCONV_LOG_WARNING("Bone [%s] has null TransformNode, skipping.", boneName.c_str());
                    continue;
                }

                // 获取骨骼的变换信息（用于调试日志）
                const Vector3 &pos = xform->getPosition();
                const Quaternion &orientation = xform->getOrientation();
                const Vector3 &scaling = xform->getScaling();

                // 在骨骼 GameObject 上挂载 Bone 组件，存储 offsetMatrix
                BonePtr boneComp = go->getComponent<Bone>();
                if (boneComp == nullptr)
                {
                    boneComp = go->addComponent<Bone>();
                }
                if (boneComp != nullptr)
                {
                    boneComp->setOffsetMatrix(boneInfo.offsetMatrix);
                }
                else
                {
                    MCONV_LOG_ERROR("Failed to add Bone component for [%s]", boneName.c_str())
                    ret = T3D_ERR_FAIL;
                    break;
                }

                Matrix3 matR;
                orientation.toRotationMatrix(matR);
                Radian xAngle, yAngle, zAngle;
                // matR.toEulerAnglesZXY(zAngle, xAngle, yAngle);
                matR.toEulerAnglesYXZ(yAngle, xAngle, zAngle);

                MCONV_LOG_DEBUG("Bone name : %s, T %s, R (%f, %f, %f), S %s",
                    boneName.c_str(),
                    pos.getDebugString().c_str(),
                    xAngle.valueDegrees(), yAngle.valueDegrees(), zAngle.valueDegrees(),
                    scaling.getDebugString().c_str());
            }

            MCONV_LOG_INFO("Generated %lld bones.", nameToIndexMap.size());

            // 将骨骼名称到索引的映射保存到成员变量，供后续 processSkinnedMesh 使用
            mBoneNameToIndexMap.clear();
            for (const auto &item : nameToIndexMap)
            {
                mBoneNameToIndexMap.emplace(item.first, static_cast<uint8_t>(item.second));
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::createSkeleton()
    {
        TResult ret = T3D_OK;

        do
        {
            String name = mOutputName;
            mSkeleton = T3D_SKELETON_MGR.createSkeleton(name, mRoot);
            if (mSkeleton == nullptr)
            {
                MCONV_LOG_ERROR("Failed to create skeleton for [%s]", name.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            name = name + "." + Resource::EXT_SKELETON;
            mResources.emplace(name, mSkeleton);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::processAnimation(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Starting processing animation ...")

            // 获取场景中的所有动画栈
            int32_t animStackCount = lFbxScene->GetSrcObjectCount<FbxAnimStack>();
            if (animStackCount == 0)
            {
                MCONV_LOG_WARNING("No animation stack found in FBX file.")
                break;
            }

            // 获取全局时间设置
            FbxTime::EMode timeMode = lFbxScene->GetGlobalSettings().GetTimeMode();
            double frameRate = FbxTime::GetFrameRate(timeMode);
            MCONV_LOG_INFO("FBX time mode: %d, frame rate: %f", timeMode, frameRate)

            AnimationClips clips;

            // 遍历所有动画栈
            for (int32_t stackIndex = 0; stackIndex < animStackCount; stackIndex++)
            {
                FbxAnimStack *lAnimStack = lFbxScene->GetSrcObject<FbxAnimStack>(stackIndex);
                if (lAnimStack == nullptr)
                {
                    continue;
                }

                String animStackName = lAnimStack->GetName();
                MCONV_LOG_INFO("Processing animation stack: %s", animStackName.c_str())

                // 获取动画栈的时间范围
                FbxTimeSpan timeSpan = lAnimStack->GetLocalTimeSpan();
                FbxTime startTime = timeSpan.GetStart();
                FbxTime stopTime = timeSpan.GetStop();
                
                // 计算动画时长（转换为毫秒）
                double durationSeconds = (stopTime.GetSecondDouble() - startTime.GetSecondDouble());
                uint32_t durationMs = static_cast<uint32_t>(durationSeconds * 1000.0);
                
                MCONV_LOG_INFO("Animation duration: %.3f seconds (%u ms)", durationSeconds, durationMs)

                // 动画轨道映射：骨骼名称 -> AnimationTrack
                AnimationTracks tracks;

                // 遍历动画栈中的所有动画层
                int32_t animLayerCount = lAnimStack->GetMemberCount<FbxAnimLayer>();
                for (int32_t layerIndex = 0; layerIndex < animLayerCount; layerIndex++)
                {
                    FbxAnimLayer *lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(layerIndex);
                    if (lAnimLayer == nullptr)
                    {
                        continue;
                    }

                    // 遍历场景中的所有节点，查找有动画的骨骼节点
                    for (const auto &boneItem : mBoneMap)
                    {
                        const String &boneName = boneItem.first;
                        GameObject *go = boneItem.second.gameObject;
                        
                        if (go == nullptr)
                        {
                            continue;
                        }

                        // 通过骨骼名称在场景中查找对应的 FbxNode
                        // 注意：这里假设骨骼名称和 FbxNode 名称匹配
                        FbxNode *lFbxNode = lFbxScene->FindNodeByName(boneName.c_str());
                        if (lFbxNode == nullptr)
                        {
                            continue;
                        }

                        // 提取平移、旋转、缩放的关键帧
                        TranslationTrack translationTrack;
                        OrientationTrack orientationTrack;
                        ScalingTrack scalingTrack;

                        // 提取平移关键帧
                        FbxAnimCurveNode *lTAnimCurveNode = lFbxNode->LclTranslation.GetCurveNode(lAnimLayer);
                        if (lTAnimCurveNode != nullptr)
                        {
                            extractTranslationKeyframes(lTAnimCurveNode, startTime, stopTime, frameRate, translationTrack);
                        }

                        // 提取旋转关键帧（使用 Euler 角）
                        FbxAnimCurveNode *lRAnimCurveNode = lFbxNode->LclRotation.GetCurveNode(lAnimLayer);
                        if (lRAnimCurveNode != nullptr)
                        {
                            extractRotationKeyframes(lRAnimCurveNode, startTime, stopTime, frameRate, orientationTrack);
                        }

                        // 提取缩放关键帧
                        FbxAnimCurveNode *lSAnimCurveNode = lFbxNode->LclScaling.GetCurveNode(lAnimLayer);
                        if (lSAnimCurveNode != nullptr)
                        {
                            extractScalingKeyframes(lSAnimCurveNode, startTime, stopTime, frameRate, scalingTrack);
                        }

                        // 如果该骨骼有动画数据，创建 AnimationTrack
                        if (!translationTrack.empty() || !orientationTrack.empty() || !scalingTrack.empty())
                        {
                            AnimationTrackPtr track = AnimationTrack::create(translationTrack, orientationTrack, scalingTrack);
                            if (track != nullptr)
                            {
                                tracks.emplace(boneName, track);
                                MCONV_LOG_DEBUG("Bone [%s] animation: T=%lld, R=%lld, S=%lld", 
                                    boneName.c_str(), 
                                    translationTrack.size(), 
                                    orientationTrack.size(), 
                                    scalingTrack.size())
                            }
                        }
                    }
                }

                // 如果有动画轨道，创建 AnimationClip
                if (!tracks.empty())
                {
                    AnimationClipPtr clip = AnimationClip::create(animStackName, durationMs, std::move(tracks));
                    if (clip != nullptr)
                    {
                        clips.emplace(animStackName, clip);
                        MCONV_LOG_INFO("Created animation clip [%s] with %lld tracks, duration: %u ms", 
                            animStackName.c_str(), tracks.size(), durationMs)
                    }
                }
            }

            // 如果有动画片段，创建 SkeletalAnimation 并添加到资源中
            if (!clips.empty())
            {
                String animName = mOutputName;
                mAnimation = T3D_ANIMATION_MGR.createSkeletalAnimation(animName, std::move(clips));
                if (mAnimation != nullptr)
                {
                    animName = mOutputName  + "." + Resource::EXT_ANIMATION;
                    mResources.emplace(animName, mAnimation);
                    MCONV_LOG_INFO("Created skeletal animation [%s] with %lld clips", animName.c_str(), clips.size())
                }
                else
                {
                    MCONV_LOG_ERROR("Failed to create skeletal animation.")
                    ret = T3D_ERR_FAIL;
                    break;
                }
            }

            MCONV_LOG_INFO("Completed processing animation !")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    // 辅助函数：提取平移关键帧
    void FBXImporter::extractTranslationKeyframes(FbxAnimCurveNode *lCurveNode, 
        const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
        TranslationTrack &track)
    {
        track.clear();

        // 获取 X, Y, Z 轴的曲线
        FbxAnimCurve *lCurveX = lCurveNode->GetCurve(0); // X
        FbxAnimCurve *lCurveY = lCurveNode->GetCurve(1); // Y
        FbxAnimCurve *lCurveZ = lCurveNode->GetCurve(2); // Z

        // 收集所有关键帧时间点
        TSet<uint32_t> keyTimes;

        if (lCurveX != nullptr)
        {
            int32_t keyCount = lCurveX->KeyGetCount();
            for (int32_t i = 0; i < keyCount; i++)
            {
                FbxTime fbxTime = lCurveX->KeyGetTime(i);
                if (fbxTime >= startTime && fbxTime <= stopTime)
                {
                    double seconds = fbxTime.GetSecondDouble();
                    uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
                    keyTimes.insert(timeMs);
                }
            }
        }

        if (lCurveY != nullptr)
        {
            int32_t keyCount = lCurveY->KeyGetCount();
            for (int32_t i = 0; i < keyCount; i++)
            {
                FbxTime fbxTime = lCurveY->KeyGetTime(i);
                if (fbxTime >= startTime && fbxTime <= stopTime)
                {
                    double seconds = fbxTime.GetSecondDouble();
                    uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
                    keyTimes.insert(timeMs);
                }
            }
        }

        if (lCurveZ != nullptr)
        {
            int32_t keyCount = lCurveZ->KeyGetCount();
            for (int32_t i = 0; i < keyCount; i++)
            {
                FbxTime fbxTime = lCurveZ->KeyGetTime(i);
                if (fbxTime >= startTime && fbxTime <= stopTime)
                {
                    double seconds = fbxTime.GetSecondDouble();
                    uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
                    keyTimes.insert(timeMs);
                }
            }
        }

        // 为每个关键帧时间点创建关键帧
        for (uint32_t timeMs : keyTimes)
        {
            FbxTime fbxTime;
            fbxTime.SetSecondDouble(timeMs / 1000.0);

            float32_t x = 0.0f, y = 0.0f, z = 0.0f;

            if (lCurveX != nullptr)
            {
                x = static_cast<float32_t>(lCurveX->Evaluate(fbxTime));
            }
            if (lCurveY != nullptr)
            {
                y = static_cast<float32_t>(lCurveY->Evaluate(fbxTime));
            }
            if (lCurveZ != nullptr)
            {
                z = static_cast<float32_t>(lCurveZ->Evaluate(fbxTime));
            }

            FbxVector4 lFbxT(x, y, z, 1.0f);
            Vector3 translation = FbxPointToTinyVector3Remap(lFbxT);
            KfTranslationPtr keyframe = KfTranslation::create(timeMs, translation);
            if (keyframe != nullptr)
            {
                track.emplace_back(keyframe);
            }
        }

        // 按时间排序
        std::sort(track.begin(), track.end(), 
            [](const KfTranslationPtr &a, const KfTranslationPtr &b) {
                return a->getTime() < b->getTime();
            });
    }

    //--------------------------------------------------------------------------

    // 辅助函数：提取旋转关键帧
    void FBXImporter::extractRotationKeyframes(FbxAnimCurveNode *lCurveNode, 
        const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
        OrientationTrack &track)
    {
        track.clear();

        // 获取 X, Y, Z 轴的曲线（Euler 角）
        FbxAnimCurve *lCurveX = lCurveNode->GetCurve(0); // X (Pitch)
        FbxAnimCurve *lCurveY = lCurveNode->GetCurve(1); // Y (Yaw)
        FbxAnimCurve *lCurveZ = lCurveNode->GetCurve(2); // Z (Roll)

        // 收集所有关键帧时间点
        TSet<uint32_t> keyTimes;

        if (lCurveX != nullptr)
        {
            int32_t keyCount = lCurveX->KeyGetCount();
            for (int32_t i = 0; i < keyCount; i++)
            {
                FbxTime fbxTime = lCurveX->KeyGetTime(i);
                if (fbxTime >= startTime && fbxTime <= stopTime)
                {
                    double seconds = fbxTime.GetSecondDouble();
                    uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
                    keyTimes.insert(timeMs);
                }
            }
        }

        if (lCurveY != nullptr)
        {
            int32_t keyCount = lCurveY->KeyGetCount();
            for (int32_t i = 0; i < keyCount; i++)
            {
                FbxTime fbxTime = lCurveY->KeyGetTime(i);
                if (fbxTime >= startTime && fbxTime <= stopTime)
                {
                    double seconds = fbxTime.GetSecondDouble();
                    uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
                    keyTimes.insert(timeMs);
                }
            }
        }

        if (lCurveZ != nullptr)
        {
            int32_t keyCount = lCurveZ->KeyGetCount();
            for (int32_t i = 0; i < keyCount; i++)
            {
                FbxTime fbxTime = lCurveZ->KeyGetTime(i);
                if (fbxTime >= startTime && fbxTime <= stopTime)
                {
                    double seconds = fbxTime.GetSecondDouble();
                    uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
                    keyTimes.insert(timeMs);
                }
            }
        }

        // 为每个关键帧时间点创建关键帧
        for (uint32_t timeMs : keyTimes)
        {
            FbxTime fbxTime;
            fbxTime.SetSecondDouble(timeMs / 1000.0);

            float32_t x = 0.0f, y = 0.0f, z = 0.0f;

            if (lCurveX != nullptr)
            {
                x = static_cast<float32_t>(lCurveX->Evaluate(fbxTime));
            }
            if (lCurveY != nullptr)
            {
                y = static_cast<float32_t>(lCurveY->Evaluate(fbxTime));
            }
            if (lCurveZ != nullptr)
            {
                z = static_cast<float32_t>(lCurveZ->Evaluate(fbxTime));
            }

            // 将 Euler 角转换为四元数（度转弧度）
            // Radian rx(Degree(x).valueRadians());
            // Radian ry(Degree(y).valueRadians());
            // Radian rz(Degree(z).valueRadians());
            // Matrix3 rotM;
            // rotM.fromEulerAnglesYXZ(ry, rx, rz);
            // Quaternion orientation;
            // orientation.fromRotationMatrix(rotM);
            FbxVector4 lFbxEuler(x, y, z, 1.0f);
            Quaternion orientation = FbxEulerToTinyQuaternion(lFbxEuler);

            KfOrientationPtr keyframe = KfOrientation::create(timeMs, orientation);
            if (keyframe != nullptr)
            {
                track.emplace_back(keyframe);
            }
        }

        // 按时间排序
        std::sort(track.begin(), track.end(), 
            [](const KfOrientationPtr &a, const KfOrientationPtr &b)
            {
                return a->getTime() < b->getTime();
            });
    }

    //--------------------------------------------------------------------------

    // 辅助函数：提取缩放关键帧
    void FBXImporter::extractScalingKeyframes(FbxAnimCurveNode *lCurveNode, 
        const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
        ScalingTrack &track)
    {
        track.clear();

        // 获取 X, Y, Z 轴的曲线
        FbxAnimCurve *lCurveX = lCurveNode->GetCurve(0); // X
        FbxAnimCurve *lCurveY = lCurveNode->GetCurve(1); // Y
        FbxAnimCurve *lCurveZ = lCurveNode->GetCurve(2); // Z

        // 收集所有关键帧时间点
        TSet<uint32_t> keyTimes;

        if (lCurveX != nullptr)
        {
            int32_t keyCount = lCurveX->KeyGetCount();
            for (int32_t i = 0; i < keyCount; i++)
            {
                FbxTime fbxTime = lCurveX->KeyGetTime(i);
                if (fbxTime >= startTime && fbxTime <= stopTime)
                {
                    double seconds = fbxTime.GetSecondDouble();
                    uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
                    keyTimes.insert(timeMs);
                }
            }
        }

        if (lCurveY != nullptr)
        {
            int32_t keyCount = lCurveY->KeyGetCount();
            for (int32_t i = 0; i < keyCount; i++)
            {
                FbxTime fbxTime = lCurveY->KeyGetTime(i);
                if (fbxTime >= startTime && fbxTime <= stopTime)
                {
                    double seconds = fbxTime.GetSecondDouble();
                    uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
                    keyTimes.insert(timeMs);
                }
            }
        }

        if (lCurveZ != nullptr)
        {
            int32_t keyCount = lCurveZ->KeyGetCount();
            for (int32_t i = 0; i < keyCount; i++)
            {
                FbxTime fbxTime = lCurveZ->KeyGetTime(i);
                if (fbxTime >= startTime && fbxTime <= stopTime)
                {
                    double seconds = fbxTime.GetSecondDouble();
                    uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
                    keyTimes.insert(timeMs);
                }
            }
        }

        // 为每个关键帧时间点创建关键帧
        for (uint32_t timeMs : keyTimes)
        {
            FbxTime fbxTime;
            fbxTime.SetSecondDouble(timeMs / 1000.0);

            float32_t x = 1.0f, y = 1.0f, z = 1.0f; // 默认缩放为 1

            if (lCurveX != nullptr)
            {
                x = static_cast<float32_t>(lCurveX->Evaluate(fbxTime));
            }
            if (lCurveY != nullptr)
            {
                y = static_cast<float32_t>(lCurveY->Evaluate(fbxTime));
            }
            if (lCurveZ != nullptr)
            {
                z = static_cast<float32_t>(lCurveZ->Evaluate(fbxTime));
            }

            FbxVector4 lFbxS(x, y, z, 1.0f);
            Vector3 scaling = FbxPointToTinyVector3(lFbxS);
            KfScalingPtr keyframe = KfScaling::create(timeMs, scaling);
            if (keyframe != nullptr)
            {
                track.emplace_back(keyframe);
            }
        }

        // 按时间排序
        std::sort(track.begin(), track.end(), 
            [](const KfScalingPtr &a, const KfScalingPtr &b) {
                return a->getTime() < b->getTime();
            });
    }
}

