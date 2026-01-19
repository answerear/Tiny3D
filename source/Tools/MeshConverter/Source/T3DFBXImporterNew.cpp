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


#include "T3DFBXImporterNew.h"
#include "T3DFBXDataStream.h"
#include "T3DMeshConverterError.h"

#define T3D_FBX_LOADER_RH   // FBX 文件加载后使用右手系，然后人工转换为左手系

namespace Tiny3D
{
    //--------------------------------------------------------------------------
    // 骨骼权重信息结构体（用于提取蒙皮网格的混合权重）
    //--------------------------------------------------------------------------
    struct BoneWeightInfo
    {
        uint16_t boneIndex {0xFF};
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
    
    struct CoordinateSystemInfo {
        char upAxis;      // 'X', 'Y', 'Z'
        int upSign;       // +1 or -1
        char frontAxis;   // 'X', 'Y', 'Z'
        int frontSign;    // +1 or -1
        char rightAxis;   // 'X', 'Y', 'Z'
        int rightSign;    // +1 or -1
        bool isLeftHanded;
    };

    //--------------------------------------------------------------------------
    
    CoordinateSystemInfo GetCoordinateSystemInfo(const FbxAxisSystem& axisSystem) {
        CoordinateSystemInfo info;
        
        // 获取 Up
        FbxAxisSystem::EUpVector upVector = axisSystem.GetUpVector(info.upSign);
        info.upAxis = (upVector == FbxAxisSystem::eXAxis) ? 'X' :
                      (upVector == FbxAxisSystem::eYAxis) ? 'Y' : 'Z';
        
        // 获取 Front Parity
        int frontSign;
        FbxAxisSystem::EFrontVector frontParity = axisSystem.GetFrontVector(frontSign);
        
        // 推导 Front 轴
        if (upVector == FbxAxisSystem::eYAxis) 
        {
            if (frontParity == FbxAxisSystem::eParityOdd) 
            {
                info.frontAxis = 'Z';
                info.frontSign = frontSign;
            }
            else 
            {
                info.frontAxis = 'X';
                info.frontSign = frontSign;
            }
        } 
        else if (upVector == FbxAxisSystem::eZAxis) 
        {
            if (frontParity == FbxAxisSystem::eParityOdd) 
            {
                info.frontAxis = 'Y';
                info.frontSign = frontSign;
            } 
            else 
            {
                info.frontAxis = 'X';
                info.frontSign = frontSign;
            }
        } 
        else 
        { // X-Up
            if (frontParity == FbxAxisSystem::eParityOdd)
            {
                info.frontAxis = 'Z';
                info.frontSign = frontSign;
            } 
            else 
            {
                info.frontAxis = 'Y';
                info.frontSign = frontSign;
            }
        }
        
        // 推导 Right 轴（剩余的轴）
        if ((info.upAxis == 'Y' && info.frontAxis == 'Z') ||
            (info.upAxis == 'Z' && info.frontAxis == 'Y')) 
        {
            info.rightAxis = 'X';
        } else if ((info.upAxis == 'X' && info.frontAxis == 'Z') ||
                   (info.upAxis == 'Z' && info.frontAxis == 'X')) 
        {
            info.rightAxis = 'Y';
        } 
        else 
        {
            info.rightAxis = 'Z';
        }
        
        // 手性
        info.isLeftHanded = (axisSystem.GetCoorSystem() == FbxAxisSystem::eLeftHanded);
        
        // 根据手性和叉乘规则确定 Right 符号
        // 简化处理（实际需要向量叉乘计算）
        info.rightSign = 1;
        
        return info;
    }

    //--------------------------------------------------------------------------
    
    // 使用示例
    void PrintCoordInfo(const FbxAxisSystem& axisSystem, const char* name) 
    {
        CoordinateSystemInfo info = GetCoordinateSystemInfo(axisSystem);
        
        std::stringstream ss;
        ss << name << ": "
                  << (info.rightSign > 0 ? "+" : "-") << info.rightAxis << " Right, "
                  << (info.upSign > 0 ? "+" : "-") << info.upAxis << " Up, "
                  << (info.frontSign > 0 ? "+" : "-") << info.frontAxis << " Front, "
                  << (info.isLeftHanded ? "Left-Handed" : "Right-Handed");
        
        MCONV_LOG_INFO("%s", ss.str().c_str());
    }
    
    //--------------------------------------------------------------------------

    FBXImporterNewPtr FBXImporterNew::create()
    {
        return T3D_NEW FBXImporterNew();
    }

    //--------------------------------------------------------------------------

    FBXImporterNew::FBXImporterNew()
    {
        
    }

    //--------------------------------------------------------------------------

    FBXImporterNew::~FBXImporterNew()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::run(const ConverterOptions &opts, Assets &resources)
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

            // 先处理 FBX 场景，收集场景结点和 mesh 对象
            ret = processFbxScene(lFbxScene);
            if (T3D_FAILED(ret))
            {
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

        // 释放所有中间数据
        releaseAllObjects();
        
        // 释放 FBX 对象
        destroyFbxObjects();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::initFbxObjects()
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Initializing FBX objects...")
            
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

    TResult FBXImporterNew::destroyFbxObjects()
    {
        if (mFbxManager != nullptr)
        {
            mFbxManager->Destroy();
            mFbxManager = nullptr;
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::releaseAllObjects()
    {
        for (auto meshData : mFbxMeshData)
        {
            T3D_SAFE_DELETE(meshData.second);
        }

        for (auto skelAniData : mFbxSkeletons)
        {
            for (auto aniData : skelAniData.second->animationClips)
            {
                T3D_SAFE_DELETE(aniData.second);
            }
            
            T3D_SAFE_DELETE(skelAniData.second);
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::importScene(DataStream &stream, FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start importing FBX scene ...")
            
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

            mFbxScene = lFbxScene;
            
            MCONV_LOG_INFO("Completed importing FBX scene.")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::setupMetricSystem(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            FbxGlobalSettings &lFbxGlobalSettings = lFbxScene->GetGlobalSettings();

            FbxTime::EMode timeMode = lFbxScene->GetGlobalSettings().GetTimeMode();
            if (timeMode != FbxTime::eCustom)
                mFbxFrameRate = FbxTime::GetFrameRate(timeMode);
            else
                mFbxFrameRate = lFbxGlobalSettings.GetCustomFrameRate();
            mFbxSampleRate = 1.0 / mFbxFrameRate;
            
            // 统一转成 DirectX 坐标系
            FbxAxisSystem lSceneAxisSystem = lFbxGlobalSettings.GetAxisSystem();
#if defined (T3D_FBX_LOADER_RH)
            // FbxAxisSystem  tinyAxisSystem(FbxAxisSystem::eYAxis, (FbxAxisSystem::EFrontVector)-FbxAxisSystem::eParityOdd, FbxAxisSystem::eLeftHanded); // Unity
            FbxAxisSystem tinyAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
#else
            // FbxAxisSystem tinyAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eLeftHanded); // LH
            FbxAxisSystem tinyAxisSystem = FbxAxisSystem::eDirectX;
            // FbxAxisSystem tinyAxisSystem = FbxAxisSystem::eOpenGL;
            // FbxAxisSystem tinyAxisSystem = FbxAxisSystem::eMayaYUp;
#endif
            
            PrintCoordInfo(lSceneAxisSystem, "FBX Scene");
            PrintCoordInfo(tinyAxisSystem, "Tiny3D");
            
            if (lSceneAxisSystem != tinyAxisSystem)
            {
                MCONV_LOG_INFO("Start converting scene to Tiny3D coordinate system ...")
                tinyAxisSystem.ConvertScene(lFbxScene);
                MCONV_LOG_INFO("Completed converting to Tiny3D coordinate system.")
            }
            
            if (lSceneAxisSystem == FbxAxisSystem::eOpenGL)
            {
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

    TResult FBXImporterNew::compileAllShaders(const String &tempPath, const String &assetsPath)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start compiling shaders ......")
            
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

            MCONV_LOG_INFO("Completed compiling shaders !")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::compileShaders(const String &inputPath, const String &outputPath)
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

    TResult FBXImporterNew::compileShader(const String &inputPath, const String &outputPath)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start compiling shader %s ...", inputPath.c_str())
            
            // 从 meta 中读取出 UUID
            String metaPath = inputPath + ".meta";
            FileDataStream fs;
            if (!fs.open(metaPath.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
            {
                MCONV_LOG_ERROR("Failed to open meta file [%s] !", metaPath.c_str())
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
                MCONV_LOG_ERROR("Failed to start scc.exe from source file (%s) ! ERROR [%d]", inputPath.c_str(), ret)
                break;
            }

            // 等待编译结束
            ret = process.wait();
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to wait process exiting from source file (%s) ! ERROR [%d]", inputPath.c_str(), ret)
                break;
            }

            uint32_t exitCode = process.getExitCode();
            if (exitCode != 0)
            {
                // 编译出错了，只能退出
                MCONV_LOG_ERROR("Failed to compile shader (%s) ! ERROR [%d]", inputPath.c_str(), ret)
                break;
            }

            MCONV_LOG_INFO("Completed compiling shader !")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult FBXImporterNew::loadDefaultMaterial(const String &rootPath, const String &relativePath)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start loading default material [%s/%s] ...", rootPath.c_str(), relativePath.c_str())
            
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(rootPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kRead);
            T3D_ASSERT(archive != nullptr);
            mDefaultMaterial = T3D_MATERIAL_MGR.loadMaterial(archive, relativePath);

            if (mDefaultMaterial == nullptr)
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                MCONV_LOG_ERROR("Failed to load default material !")
                break;
            }

            MCONV_LOG_INFO("Completed loading default material !")

            //String path = rootPath + "/" + relativePath;
            //MCONV_LOG_INFO("Start loading default material [%s] ...", path.c_str())
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

    TResult FBXImporterNew::processFbxScene(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start processing fbx scene ...")
            
            // 收集 FBX 中所有的 mesh 信息
            ret = preprocessFbxScene(lFbxScene);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 生成骨骼数据和动画数据
            ret = generateSkeletonsAndAnimations(lFbxScene);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 生成 mesh 和骨架关联数据，以及骨骼的 invert bind pose matrices 数据
            ret = generateMeshesLinkData();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 生成 mesh 数据（包括蒙皮权重和索引）
            ret = generateMeshes(true);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            // 创建 skeletons and animations 资源对象
            ret = createSkeletonsAndAnimations();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 创建 meshes 资源对象
            ret = createMeshes();
            if (T3D_FAILED(ret))
            {
                break;
            }
            MCONV_LOG_INFO("Completed processing fbx scene !")
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::preprocessFbxScene(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start preprocessing fbx meshes ...")

            // 收集 FbxMesh 对象
            for (int32_t i = 0; i < lFbxScene->GetRootNode()->GetChildCount(); i++)
            {
                FbxNode *lFbxNode = lFbxScene->GetRootNode()->GetChild(i);
                ret = collectFbxMesh(lFbxNode, lFbxNode);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                break;
            }
            
            MCONV_LOG_INFO("Completed preprocessing fbx meshes !")
        } while (false);

        return ret; 
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::recursiveCollectFbxMesh(FbxNode *lFbxRoot, FbxNode *lFbxNode)
    {
        TResult ret = T3D_OK;

        do
        {
            ret = collectFbxMesh(lFbxRoot, lFbxNode);
            if (T3D_FAILED(ret))
            {
                break;
            }

            for (int32_t i = 0; i < lFbxNode->GetChildCount(); i++)
            {
                FbxNode *lFbxChildNode = lFbxNode->GetChild(i);
                ret = recursiveCollectFbxMesh(lFbxRoot, lFbxChildNode);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::collectFbxMesh(FbxNode *lFbxRoot, FbxNode *lFbxNode)
    {
        TResult ret = T3D_OK;

        do
        {
            FbxNodeAttribute* lFbxAttribute = lFbxNode->GetNodeAttribute();
            if (lFbxAttribute != nullptr)
            {
                if (lFbxAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
                {
                    MCONV_LOG_INFO("Collect fbx mesh: %s", lFbxNode->GetName())
                    
                    // 建立 FbxMeshNodeLUT
                    FbxMesh *lFbxMesh = lFbxNode->GetMesh();
                    mFbxMeshes.emplace(lFbxRoot, lFbxMesh);

                    // 建立 FbxMeshLUT
                    MeshData *meshData = T3D_NEW MeshData;
                    mFbxMeshData.emplace(lFbxMesh, meshData);

                    // 建立 FbxMeshRootLUT
                    mFbxMeshRoots.emplace(lFbxMesh, lFbxRoot);

                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::generateSkeletonsAndAnimations(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start generating skeletons and animations data ...")
            
            for (int32_t i = 0; i < lFbxScene->GetRootNode()->GetChildCount(); i++)
            {
                FbxNode *lFbxNode = lFbxScene->GetRootNode()->GetChild(i);
                uint16_t parentIdx = BoneNode::kInvalidIndex;
                ret = recursiveGenerateSkeletonsAndAnimations(lFbxScene, lFbxNode, lFbxNode, parentIdx);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                break;
            }
            
            MCONV_LOG_INFO("Completed generating skeletons and animations data.")
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::recursiveGenerateSkeletonsAndAnimations(FbxScene *lFbxScene, FbxNode *lFbxRoot, FbxNode *lFbxNode, uint16_t parentIdx)
    {
        TResult ret = T3D_OK;

        do
        {
            uint16_t boneIdx = BoneNode::kInvalidIndex;
            ret = generateSkeletonAndAnimation(lFbxScene, lFbxRoot, lFbxNode, parentIdx, boneIdx);
            if (T3D_FAILED(ret))
            {
                break;
            }

            if (boneIdx != BoneNode::kInvalidIndex)
            {
                parentIdx = boneIdx;
            }
            
            for (int32_t i = 0; i < lFbxNode->GetChildCount(); i++)
            {
                FbxNode *lFbxChildNode = lFbxNode->GetChild(i);
                ret = recursiveGenerateSkeletonsAndAnimations(lFbxScene, lFbxRoot, lFbxChildNode, parentIdx);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::generateSkeletonAndAnimation(FbxScene *lFbxScene, FbxNode *lFbxRoot, FbxNode *lFbxNode, uint16_t parentIdx, uint16_t &boneIdx)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start generating skeleton and animation data for node [%s] ...", lFbxNode->GetName())
            
            FbxNodeAttribute* lFbxAttribute = lFbxNode->GetNodeAttribute();
            if (lFbxAttribute != nullptr)
            {
                if (lFbxAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
                {
                    SkeletalAnimationData *skelAniData = getSkeletalAnimationData(lFbxRoot);
                    if (skelAniData != nullptr)
                    {
                        // 骨骼
                        ret = generateBone(lFbxNode, skelAniData, parentIdx, boneIdx);
                        if (T3D_FAILED(ret))
                        {
                            break;
                        }

                        // 跟骨骼相关的动画数据
                        ret = generateBoneAnimationTrack(lFbxScene, lFbxNode, skelAniData);
                        if (T3D_FAILED(ret))
                        {
                            break;
                        }
                    }
                }
            }

            if (T3D_FAILED(ret))
            {
                break;
            }

            MCONV_LOG_INFO("Completed generating skeleton and animation data !")
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::generateBone(FbxNode *lFbxNode, SkeletalAnimationData *skelAniData, uint16_t parentIdx, uint16_t &boneIdx)
    {
        TResult ret = T3D_OK;

        do
        {
            String boneName = lFbxNode->GetName();
            // FbxVector4 lFbxLocalT = lFbxNode->EvaluateLocalTranslation(FBXSDK_TIME_ZERO);
            // FbxVector4 lFbxLocalR = lFbxNode->EvaluateLocalRotation(FBXSDK_TIME_ZERO);
            // FbxVector4 lFbxLocalS = lFbxNode->EvaluateLocalScaling(FBXSDK_TIME_ZERO);
            // FbxVector4 lFbxLocalT = evaluatePropertyInitialValue(lFbxNode->LclTranslation);
            // FbxVector4 lFbxLocalR = evaluatePropertyInitialValue(lFbxNode->LclRotation);
            // FbxVector4 lFbxLocalS = evaluatePropertyInitialValue(lFbxNode->LclScaling);
            FbxAMatrix lFbxLocalMat = lFbxNode->EvaluateLocalTransform(FBXSDK_TIME_ZERO);
            FbxVector4 lFbxLocalT = lFbxLocalMat.GetT();
            FbxVector4 lFbxLocalR = lFbxLocalMat.GetR();
            FbxVector4 lFbxLocalS = lFbxLocalMat.GetS();

            Vector3 localT = FbxPointToTinyVector3Remap(lFbxLocalT);
            Quaternion localQ = FbxEulerToTinyQuaternion(lFbxLocalR);
            Vector3 localS = FbxPointToTinyVector3(lFbxLocalS);

            Matrix4 offsetM(false);
            BoneNodePtr bone = BoneNode::create(boneName, parentIdx, localT, localQ, localS, offsetM);
            skelAniData->bones.emplace_back(bone);
            boneIdx = static_cast<uint16_t>(skelAniData->bones.size() - 1);

            skelAniData->boneMap[lFbxNode] = boneIdx;

            Radian rx, ry, rz;
            localQ.toEulerAnglesYXZ(ry, rx, rz);
            MCONV_LOG_INFO("Generated Bone: %s, parent: %u, idx: %u, T: %s, Q: %s, E: (%f, %f, %f), S: %s", 
                boneName.c_str(), parentIdx, boneIdx,
                localT.getDebugString().c_str(),
                localQ.getDebugString().c_str(),
                rx.valueDegrees(), ry.valueDegrees(), rz.valueDegrees(),
                localS.getDebugString().c_str())
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::generateBoneAnimationTrack(FbxScene *lFbxScene, FbxNode *lFbxNode, SkeletalAnimationData *skelAniData)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start generating animation track ...")
            
            int32_t aniStackCount = lFbxScene->GetSrcObjectCount<FbxAnimStack>();
            if (aniStackCount == 0)
            {
                MCONV_LOG_WARNING("No animation stack found in FBX file.")
                break;
            }

            String boneName = lFbxNode->GetName();

            // 获取全局时间设置
            FbxTime::EMode timeMode = lFbxScene->GetGlobalSettings().GetTimeMode();
            double frameRate = mFbxFrameRate;
            double sampleRate = mFbxSampleRate;
            MCONV_LOG_INFO("Bone : %s, FBX time mode: %d, frame rate: %f, sample rate : %f", boneName.c_str(), timeMode, frameRate, sampleRate)

            // 遍历所有动画栈
            for (int32_t stackIdx = 0; stackIdx < aniStackCount; stackIdx++)
            {
                FbxAnimStack *lFbxAniStack = lFbxScene->GetSrcObject<FbxAnimStack>(stackIdx);
                if (lFbxAniStack == nullptr)
                {
                    continue;
                }

                String aniStackName = lFbxAniStack->GetName();
                MCONV_LOG_INFO("Processing animation stack: %s", aniStackName.c_str())

                // 获取动画栈的时间范围
                FbxTimeSpan timeSpan = lFbxAniStack->GetLocalTimeSpan();
                FbxTime startTime = timeSpan.GetStart();
                FbxTime stopTime = timeSpan.GetStop();
                
                // 计算动画时长（转换为毫秒）
                AnimationTimeRange range;
                range.timeRangeStart = static_cast<float>(startTime.GetSecondDouble());
                range.timeRangeEnd = static_cast<float>(stopTime.GetSecondDouble());
                float durationSeconds = range.timeRangeEnd - range.timeRangeStart;
                uint32_t durationMs = static_cast<uint32_t>(durationSeconds * 1000.0);

                MCONV_LOG_INFO("Animation duration: %.3f seconds (%u ms)", durationSeconds, durationMs)
                
                const auto itrClip = skelAniData->animationClips.find(aniStackName);
                if (itrClip == skelAniData->animationClips.end())
                {
                    AnimationClipData *clipData = T3D_NEW AnimationClipData();
                    clipData->duration = durationMs;
                    skelAniData->animationClips.emplace(aniStackName, clipData);
                }

                AnimationTracks &tracks = skelAniData->animationClips[aniStackName]->tracks;
                
                

                // 遍历动画栈中的所有动画层
                int32_t aniLayerCount = lFbxAniStack->GetMemberCount<FbxAnimLayer>();
                for (int32_t layerIdx = 0; layerIdx < aniLayerCount; layerIdx++)
                {
                    FbxAnimLayer *lFbxAniLayer = lFbxAniStack->GetMember<FbxAnimLayer>(layerIdx);
                    if (lFbxAniLayer == nullptr)
                    {
                        continue;
                    }

                    MCONV_LOG_INFO("Processing animation layer: %d", layerIdx)

                    // 提取平移、旋转、缩放的关键帧
                    TranslationTrack translationTrack;
                    OrientationTrack orientationTrack;
                    ScalingTrack scalingTrack;
                    
                    FbxAnimCurve *lFbxTransCurve = lFbxNode->LclTranslation.GetCurve(lFbxAniLayer);
                    if (lFbxTransCurve != nullptr)
                    {
                        int nKeyframeCount = lFbxTransCurve->KeyGetCount();
                    
                        int k = 0;
                        for (k = 0; k < nKeyframeCount; ++k)
                        {
                            FbxTime frameTime = lFbxTransCurve->KeyGetTime(k);
                            if (frameTime >= FBXSDK_TIME_ZERO)
                            {
                                FbxAMatrix lFbxM = lFbxNode->EvaluateLocalTransform(frameTime);
                                FbxVector4 lFbxT = lFbxM.GetT();
                                uint32_t time = static_cast<uint32_t>(frameTime.GetSecondDouble() * 1000.0);
                                Vector3 T = FbxPointToTinyVector3Remap(lFbxT);
                                KfTranslationPtr keyframe = KfTranslation::create(time, T);
                                if (keyframe != nullptr)
                                {
                                    translationTrack.emplace_back(keyframe);
                                }
                            }
                        }
                        
                        // 按时间排序
                        std::sort(translationTrack.begin(), translationTrack.end(), 
                            [](const KfTranslationPtr &a, const KfTranslationPtr &b) {
                                return a->getTime() < b->getTime();
                            });
                    }
                    
                    FbxAnimCurve *lFbxRotationCurve = lFbxNode->LclRotation.GetCurve(lFbxAniLayer);
                    if (lFbxRotationCurve != nullptr)
                    {
                        float minimum, maximum;
                        evaluateTimeRange(lFbxRotationCurve, range, minimum, maximum);
                        int firstFrame = static_cast<int>(std::floorf(minimum / static_cast<float>(sampleRate)) + 0.5f);
                        int lastFrame = static_cast<int>(std::floorf(maximum / static_cast<float>(sampleRate)) + 0.5f);
                        lastFrame = std::max(firstFrame + 1, lastFrame);   // generate at least two keys at a minimum
                        
                        for (int32_t f = firstFrame; f <= lastFrame; f++)
                        {
                            float time = static_cast<float>(f) * static_cast<float>(sampleRate);
                            FbxTime lFbxTime;
                            lFbxTime.SetSecondDouble(time);
                            FbxAMatrix lFbxM = lFbxNode->EvaluateLocalTransform(lFbxTime);
                            FbxVector4 lFbxR = lFbxM.GetR();
                            Quaternion q = FbxEulerToTinyQuaternion(lFbxR);
                            uint32_t timeMs = static_cast<uint32_t>(time * 1000.0);
                            KfOrientationPtr keyframe = KfOrientation::create(timeMs, q);
                            if (keyframe != nullptr)
                            {
                                orientationTrack.emplace_back(keyframe);
                            }
                        }
                        // int nKeyframeCount = lFbxRotationCurve->KeyGetCount();
                        //
                        // int k = 0;
                        // for (k = 0; k < nKeyframeCount; ++k)
                        // {
                        //     FbxTime frameTime = lFbxRotationCurve->KeyGetTime(k);
                        //     if (frameTime >= FBXSDK_TIME_ZERO)
                        //     {
                        //         FbxAMatrix lFbxM = lFbxNode->EvaluateLocalTransform(frameTime);
                        //         FbxVector4 lFbxR = lFbxM.GetR();
                        //         Quaternion q = FbxEulerToTinyQuaternion(lFbxR);
                        //         uint32_t time = static_cast<uint32_t>(frameTime.GetSecondDouble() * 1000.0);
                        //         KfOrientationPtr keyframe = KfOrientation::create(time, q);
                        //         if (keyframe != nullptr)
                        //         {
                        //             orientationTrack.emplace_back(keyframe);
                        //         }
                        //     }
                        // }
                        //
                        // 按时间排序
                        std::sort(orientationTrack.begin(), orientationTrack.end(), 
                            [](const KfOrientationPtr &a, const KfOrientationPtr &b) {
                                return a->getTime() < b->getTime();
                            });
                    }
                    
                    FbxAnimCurve *lFbxScaleCurve = lFbxNode->LclScaling.GetCurve(lFbxAniLayer);
                    if (lFbxScaleCurve != nullptr)
                    {
                        int nKeyframeCount = lFbxScaleCurve->KeyGetCount();
                    
                        int k = 0;
                        for (k = 0; k < nKeyframeCount; ++k)
                        {
                            FbxTime frameTime = lFbxScaleCurve->KeyGetTime(k);
                            if (frameTime >= FBXSDK_TIME_ZERO)
                            {
                                FbxAMatrix lFbxM = lFbxNode->EvaluateLocalTransform(frameTime);
                                FbxVector4 lFbxS = lFbxM.GetS();
                                uint32_t time = static_cast<uint32_t>(frameTime.GetSecondDouble() * 1000.0);
                                Vector3 S = FbxPointToTinyVector3(lFbxS);
                                KfScalingPtr keyframe = KfScaling::create(time, S);
                                if (keyframe != nullptr)
                                {
                                    scalingTrack.emplace_back(keyframe);
                                }
                            }
                        }
                        
                        // 按时间排序
                        std::sort(scalingTrack.begin(), scalingTrack.end(), 
                            [](const KfScalingPtr &a, const KfScalingPtr &b) {
                                return a->getTime() < b->getTime();
                            });
                    }
                    
                    // // 提取平移关键帧
                    // FbxAnimCurveNode *lTAnimCurveNode = lFbxNode->LclTranslation.GetCurveNode(lFbxAniLayer);
                    // if (lTAnimCurveNode != nullptr)
                    // {
                    //     extractTranslationKeyframes(lTAnimCurveNode, startTime, stopTime, frameRate, translationTrack);
                    // }
                    //
                    // // 提取旋转关键帧（使用 Euler 角）
                    // FbxAnimCurveNode *lRAnimCurveNode = lFbxNode->LclRotation.GetCurveNode(lFbxAniLayer);
                    // if (lRAnimCurveNode != nullptr)
                    // {
                    //     extractRotationKeyframes(lRAnimCurveNode, startTime, stopTime, frameRate, orientationTrack);
                    // }
                    //
                    // // 提取缩放关键帧
                    // FbxAnimCurveNode *lSAnimCurveNode = lFbxNode->LclScaling.GetCurveNode(lFbxAniLayer);
                    // if (lSAnimCurveNode != nullptr)
                    // {
                    //     extractScalingKeyframes(lSAnimCurveNode, startTime, stopTime, frameRate, scalingTrack);
                    // }

                    // 如果该骨骼有动画数据，创建 AnimationTrack
                    if (!translationTrack.empty() || !orientationTrack.empty() || !scalingTrack.empty())
                    {
                        AnimationTrackPtr track = AnimationTrack::create(translationTrack, orientationTrack, scalingTrack);
                        if (track != nullptr)
                        {
                            tracks.emplace(boneName, track);
                            MCONV_LOG_DEBUG("Bone [%s] animation: T=%llu, R=%llu, S=%llu", 
                                boneName.c_str(), 
                                translationTrack.size(), 
                                orientationTrack.size(), 
                                scalingTrack.size())
                        }
                    }
                }
            }

            MCONV_LOG_INFO("Completed generating animation track !")
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void FBXImporterNew::evaluateTimeRange(FbxAnimCurve *lFbxCurve, const AnimationTimeRange &range, float &minimum, float &maximum) const
    {
        minimum = std::numeric_limits<float>::infinity();
        maximum = -std::numeric_limits<float>::infinity();

        // for (int i = 0; i < 3; ++i)
        // {
        //     if (curve[i])
        //     {
        //         if (curve[i]->KeyGetCount())
        //         {
        //             float curMin = static_cast<float>(curve[i]->KeyGet(0).GetTime().GetSecondDouble());
        //             float curMax = static_cast<float>(curve[i]->KeyGet(curve[i]->KeyGetCount() - 1).GetTime().GetSecondDouble());
        //             if (curMin > range.timeRangeEnd || curMax < range.timeRangeStart)
        //                 continue;
        //
        //             minimum = std::min<float>(curMin, minimum);
        //             maximum = std::max<float>(curMax, maximum);
        //         }
        //     }
        // }
        
        float curMin = static_cast<float>(lFbxCurve->KeyGet(0).GetTime().GetSecondDouble());
        float curMax = static_cast<float>(lFbxCurve->KeyGet(lFbxCurve->KeyGetCount() - 1).GetTime().GetSecondDouble());
        if (curMin >= range.timeRangeStart && curMax <= range.timeRangeEnd)
        {
            minimum = std::max<float>(curMin, range.timeRangeStart);
            maximum = std::min<float>(curMax, range.timeRangeEnd);
        }
    }
    
    //--------------------------------------------------------------------------
    
    void FBXImporterNew::setupTimeRange(FbxScene &fbxScene, FbxAnimStack &animStack, AnimationTimeRange &output) const
    {
        FbxTimeSpan bakeTimeSpan = animStack.GetLocalTimeSpan();
        // This is a special workaround for a change in FBXSDK 2012.2
        // For some COLLADA files animStack->GetLocalTimeSpan() is [0 0]
        // So we get animation range from keyframes themselves using GetAnimationInterval
        if (bakeTimeSpan.GetDuration().GetSecondDouble() < std::numeric_limits<float>::epsilon())
        {
            FbxTimeSpan tempbakeTimeSpan;
            const int animLayerCount = animStack.GetSrcObjectCount<FbxAnimLayer>();
            bool isRangeValid = false;
            for (int i = 0; i < animLayerCount; ++i)
                isRangeValid = fbxScene.GetRootNode()->GetAnimationInterval(tempbakeTimeSpan, &animStack, i);
            if (isRangeValid)
            {
                bakeTimeSpan = tempbakeTimeSpan;
            }
        }

        output.timeRangeStart = static_cast<float>(bakeTimeSpan.GetStart().GetSecondDouble());
        output.timeRangeEnd = static_cast<float>(bakeTimeSpan.GetStop().GetSecondDouble());
        output.timeRangeOffset = -output.timeRangeStart;
    }
    
    //--------------------------------------------------------------------------
    
    // void FBXImporterNew::extractTranslationKeyframes(FbxAnimCurveNode *lCurveNode, 
    //     const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
    //     TranslationTrack &track)
    // {
    //     track.clear();
    //
    //     // 获取 X, Y, Z 轴的曲线
    //     FbxAnimCurve *lCurveX = lCurveNode->GetCurve(0); // X
    //     FbxAnimCurve *lCurveY = lCurveNode->GetCurve(1); // Y
    //     FbxAnimCurve *lCurveZ = lCurveNode->GetCurve(2); // Z
    //
    //     // 收集所有关键帧时间点
    //     TSet<uint32_t> keyTimes;
    //
    //     if (lCurveX != nullptr)
    //     {
    //         int32_t keyCount = lCurveX->KeyGetCount();
    //         for (int32_t i = 0; i < keyCount; i++)
    //         {
    //             FbxTime fbxTime = lCurveX->KeyGetTime(i);
    //             if (fbxTime >= startTime && fbxTime <= stopTime)
    //             {
    //                 double seconds = fbxTime.GetSecondDouble();
    //                 uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
    //                 keyTimes.insert(timeMs);
    //             }
    //         }
    //     }
    //
    //     if (lCurveY != nullptr)
    //     {
    //         int32_t keyCount = lCurveY->KeyGetCount();
    //         for (int32_t i = 0; i < keyCount; i++)
    //         {
    //             FbxTime fbxTime = lCurveY->KeyGetTime(i);
    //             if (fbxTime >= startTime && fbxTime <= stopTime)
    //             {
    //                 double seconds = fbxTime.GetSecondDouble();
    //                 uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
    //                 keyTimes.insert(timeMs);
    //             }
    //         }
    //     }
    //
    //     if (lCurveZ != nullptr)
    //     {
    //         int32_t keyCount = lCurveZ->KeyGetCount();
    //         for (int32_t i = 0; i < keyCount; i++)
    //         {
    //             FbxTime fbxTime = lCurveZ->KeyGetTime(i);
    //             if (fbxTime >= startTime && fbxTime <= stopTime)
    //             {
    //                 double seconds = fbxTime.GetSecondDouble();
    //                 uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
    //                 keyTimes.insert(timeMs);
    //             }
    //         }
    //     }
    //
    //     // 为每个关键帧时间点创建关键帧
    //     for (uint32_t timeMs : keyTimes)
    //     {
    //         FbxTime fbxTime;
    //         fbxTime.SetSecondDouble(timeMs / 1000.0);
    //
    //         float32_t x = 0.0f, y = 0.0f, z = 0.0f;
    //
    //         if (lCurveX != nullptr)
    //         {
    //             x = static_cast<float32_t>(lCurveX->Evaluate(fbxTime));
    //         }
    //         if (lCurveY != nullptr)
    //         {
    //             y = static_cast<float32_t>(lCurveY->Evaluate(fbxTime));
    //         }
    //         if (lCurveZ != nullptr)
    //         {
    //             z = static_cast<float32_t>(lCurveZ->Evaluate(fbxTime));
    //         }
    //
    //         FbxVector4 lFbxT(x, y, z, 1.0);
    //         Vector3 translation = FbxPointToTinyVector3Remap(lFbxT);
    //         KfTranslationPtr keyframe = KfTranslation::create(timeMs, translation);
    //         if (keyframe != nullptr)
    //         {
    //             track.emplace_back(keyframe);
    //         }
    //     }
    //
    //     // 按时间排序
    //     std::sort(track.begin(), track.end(), 
    //         [](const KfTranslationPtr &a, const KfTranslationPtr &b) {
    //             return a->getTime() < b->getTime();
    //         });
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void FBXImporterNew::extractRotationKeyframes(FbxAnimCurveNode *lCurveNode, 
    //     const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
    //     OrientationTrack &track)
    // {
    //     track.clear();
    //
    //     const char* rotationName = "Lcl Rotation";
    //     
    //     // 获取 X, Y, Z 轴的曲线（Euler 角）
    //     int32_t chnlIndex = lCurveNode->GetChannelIndex(FBXSDK_CURVENODE_COMPONENT_X);
    //     FbxAnimCurve *lCurveX = lCurveNode->GetCurve(chnlIndex, 0, rotationName); // X (Pitch)
    //     chnlIndex = lCurveNode->GetChannelIndex(FBXSDK_CURVENODE_COMPONENT_Y);
    //     FbxAnimCurve *lCurveY = lCurveNode->GetCurve(chnlIndex, 0, rotationName); // Y (Yaw)
    //     chnlIndex = lCurveNode->GetChannelIndex(FBXSDK_CURVENODE_COMPONENT_Z);
    //     FbxAnimCurve *lCurveZ = lCurveNode->GetCurve(chnlIndex, 0, rotationName); // Z (Roll)
    //
    //     // 收集所有关键帧时间点
    //     TSet<uint32_t> keyTimes;
    //
    //     if (lCurveX != nullptr)
    //     {
    //         int32_t keyCount = lCurveX->KeyGetCount();
    //         for (int32_t i = 0; i < keyCount; i++)
    //         {
    //             FbxTime fbxTime = lCurveX->KeyGetTime(i);
    //             if (fbxTime >= startTime && fbxTime <= stopTime)
    //             {
    //                 double seconds = fbxTime.GetSecondDouble();
    //                 uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
    //                 keyTimes.insert(timeMs);
    //             }
    //         }
    //     }
    //
    //     if (lCurveY != nullptr)
    //     {
    //         int32_t keyCount = lCurveY->KeyGetCount();
    //         for (int32_t i = 0; i < keyCount; i++)
    //         {
    //             FbxTime fbxTime = lCurveY->KeyGetTime(i);
    //             if (fbxTime >= startTime && fbxTime <= stopTime)
    //             {
    //                 double seconds = fbxTime.GetSecondDouble();
    //                 uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
    //                 keyTimes.insert(timeMs);
    //             }
    //         }
    //     }
    //
    //     if (lCurveZ != nullptr)
    //     {
    //         int32_t keyCount = lCurveZ->KeyGetCount();
    //         for (int32_t i = 0; i < keyCount; i++)
    //         {
    //             FbxTime fbxTime = lCurveZ->KeyGetTime(i);
    //             if (fbxTime >= startTime && fbxTime <= stopTime)
    //             {
    //                 double seconds = fbxTime.GetSecondDouble();
    //                 uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
    //                 keyTimes.insert(timeMs);
    //             }
    //         }
    //     }
    //
    //     // 为每个关键帧时间点创建关键帧
    //     for (uint32_t timeMs : keyTimes)
    //     {
    //         FbxTime fbxTime;
    //         fbxTime.SetSecondDouble(timeMs / 1000.0);
    //
    //         float32_t x = 0.0f, y = 0.0f, z = 0.0f;
    //
    //         if (lCurveX != nullptr)
    //         {
    //             x = static_cast<float32_t>(lCurveX->Evaluate(fbxTime));
    //         }
    //         if (lCurveY != nullptr)
    //         {
    //             y = static_cast<float32_t>(lCurveY->Evaluate(fbxTime));
    //         }
    //         if (lCurveZ != nullptr)
    //         {
    //             z = static_cast<float32_t>(lCurveZ->Evaluate(fbxTime));
    //         }
    //
    //         FbxVector4 lFbxR(x, y, z, 1.0f);
    //
    //         Quaternion orientation = FbxEulerToTinyQuaternion(lFbxR);
    //
    //         KfOrientationPtr keyframe = KfOrientation::create(timeMs, orientation);
    //         if (keyframe != nullptr)
    //         {
    //             track.emplace_back(keyframe);
    //         }
    //     }
    //
    //     // 按时间排序
    //     std::sort(track.begin(), track.end(), 
    //         [](const KfOrientationPtr &a, const KfOrientationPtr &b)
    //         {
    //             return a->getTime() < b->getTime();
    //         });
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void FBXImporterNew::extractScalingKeyframes(FbxAnimCurveNode *lCurveNode, 
    //     const FbxTime &startTime, const FbxTime &stopTime, double frameRate, 
    //     ScalingTrack &track)
    // {
    //     track.clear();
    //
    //     // 获取 X, Y, Z 轴的曲线
    //     FbxAnimCurve *lCurveX = lCurveNode->GetCurve(0); // X
    //     FbxAnimCurve *lCurveY = lCurveNode->GetCurve(1); // Y
    //     FbxAnimCurve *lCurveZ = lCurveNode->GetCurve(2); // Z
    //
    //     // 收集所有关键帧时间点
    //     TSet<uint32_t> keyTimes;
    //
    //     if (lCurveX != nullptr)
    //     {
    //         int32_t keyCount = lCurveX->KeyGetCount();
    //         for (int32_t i = 0; i < keyCount; i++)
    //         {
    //             FbxTime fbxTime = lCurveX->KeyGetTime(i);
    //             if (fbxTime >= startTime && fbxTime <= stopTime)
    //             {
    //                 double seconds = fbxTime.GetSecondDouble();
    //                 uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
    //                 keyTimes.insert(timeMs);
    //             }
    //         }
    //     }
    //
    //     if (lCurveY != nullptr)
    //     {
    //         int32_t keyCount = lCurveY->KeyGetCount();
    //         for (int32_t i = 0; i < keyCount; i++)
    //         {
    //             FbxTime fbxTime = lCurveY->KeyGetTime(i);
    //             if (fbxTime >= startTime && fbxTime <= stopTime)
    //             {
    //                 double seconds = fbxTime.GetSecondDouble();
    //                 uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
    //                 keyTimes.insert(timeMs);
    //             }
    //         }
    //     }
    //
    //     if (lCurveZ != nullptr)
    //     {
    //         int32_t keyCount = lCurveZ->KeyGetCount();
    //         for (int32_t i = 0; i < keyCount; i++)
    //         {
    //             FbxTime fbxTime = lCurveZ->KeyGetTime(i);
    //             if (fbxTime >= startTime && fbxTime <= stopTime)
    //             {
    //                 double seconds = fbxTime.GetSecondDouble();
    //                 uint32_t timeMs = static_cast<uint32_t>(seconds * 1000.0);
    //                 keyTimes.insert(timeMs);
    //             }
    //         }
    //     }
    //
    //     // 为每个关键帧时间点创建关键帧
    //     for (uint32_t timeMs : keyTimes)
    //     {
    //         FbxTime fbxTime;
    //         fbxTime.SetSecondDouble(timeMs / 1000.0);
    //
    //         float32_t x = 1.0f, y = 1.0f, z = 1.0f; // 默认缩放为 1
    //
    //         if (lCurveX != nullptr)
    //         {
    //             x = static_cast<float32_t>(lCurveX->Evaluate(fbxTime));
    //         }
    //         if (lCurveY != nullptr)
    //         {
    //             y = static_cast<float32_t>(lCurveY->Evaluate(fbxTime));
    //         }
    //         if (lCurveZ != nullptr)
    //         {
    //             z = static_cast<float32_t>(lCurveZ->Evaluate(fbxTime));
    //         }
    //
    //         FbxVector4 lFbxS(x, y, z, 1.0f);
    //         Vector3 scaling = FbxPointToTinyVector3(lFbxS);
    //         KfScalingPtr keyframe = KfScaling::create(timeMs, scaling);
    //         if (keyframe != nullptr)
    //         {
    //             track.emplace_back(keyframe);
    //         }
    //     }
    //
    //     // 按时间排序
    //     std::sort(track.begin(), track.end(), 
    //         [](const KfScalingPtr &a, const KfScalingPtr &b) {
    //             return a->getTime() < b->getTime();
    //         });
    // }
    
    //--------------------------------------------------------------------------

    TResult FBXImporterNew::generateMeshesLinkData()
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start generating meshes link data...")
            
            for (const auto mesh : mFbxMeshes)
            {
                ret = generateMeshLinkData(mesh.first, mesh.second);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                break;
            }
            
            MCONV_LOG_INFO("Completed generating meshes link data !")
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::generateMeshLinkData(FbxNode *lFbxMeshRoot, FbxMesh *lFbxMesh)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start generating mesh [%s] link data...", lFbxMesh->GetNode()->GetName())
            
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
                    if (lFbxCluster == nullptr)
                    {
                        continue;
                    }

                    // 获取骨骼节点
                    FbxNode *lFbxLink = lFbxCluster->GetLink();
                    if (lFbxLink == nullptr)
                    {
                        continue;
                    }

                    // 获取骨骼根节点
                    FbxNode *lFbxSkeletalRoot = getFbxSkeletalRoot(lFbxLink);
                    if (lFbxSkeletalRoot == nullptr)
                    {
                        MCONV_LOG_WARNING("Failed to get skeletal root !")
                        continue;
                    }

                    // 通过骨骼根节点获取网格根节点
                    FbxNode *lFbxNode = nullptr;
                    if (!lookupFbxMeshRoot(lFbxSkeletalRoot, lFbxNode))
                    {
                        // 没有映射关系，添加映射关系
                        mFbxBoneRootToMeshRoot.emplace(lFbxSkeletalRoot, lFbxMeshRoot);
                        mFbxMeshRootToBoneRoot.emplace(lFbxMeshRoot, lFbxSkeletalRoot);
                    }

                    SkeletalAnimationData *skelAniData = getSkeletalAnimationData(lFbxSkeletalRoot);
                    if (skelAniData == nullptr)
                    {
                        MCONV_LOG_WARNING("Failed to get skeletal animation data !")
                        continue;
                    }
                    
                    String boneName = lFbxLink->GetName();
                    
                    // 查找对应的骨骼信息
                    auto it = skelAniData->boneMap.find(lFbxLink);
                    if (it == skelAniData->boneMap.end())
                    {
                        MCONV_LOG_WARNING("Bone [%s] not found in bone map, skipping offset matrix.", boneName.c_str());
                        continue;
                    }

                    uint16_t boneIdx = it->second;
                    BoneNodePtr boneNode = skelAniData->bones[boneIdx];

                    // 获取绑定姿态矩阵
                    // TransformMatrix: 网格节点在绑定姿态时的世界空间变换矩阵
                    // TransformLinkMatrix: 骨骼节点在绑定姿态时的世界空间变换矩阵
                    FbxAMatrix lFbxTransformMatrix;
                    FbxAMatrix lFbxTransformLinkMatrix;
                    lFbxCluster->GetTransformMatrix(lFbxTransformMatrix);
                    lFbxCluster->GetTransformLinkMatrix(lFbxTransformLinkMatrix);

                    // 计算 offset matrix
                    FbxAMatrix lFbxOffsetM = lFbxTransformLinkMatrix.Inverse() * lFbxTransformMatrix;
                    // FbxAMatrix lFbxOffsetM = lFbxTransformLinkMatrix.Inverse();
                    
                    // 存储 offset matrix 到骨骼信息中
                    // FbxVector4 lFbxT = lFbxTransformLinkMatrix.GetT();
                    // Vector3 T = FbxPointToTinyVector3Remap(lFbxT);
                    // FbxVector4 lFbxR = lFbxTransformLinkMatrix.GetR();
                    // Quaternion Q = FbxEulerToTinyQuaternion(lFbxR);
                    // FbxVector4 lFbxS = lFbxTransformLinkMatrix.GetS();
                    // Vector3 S = FbxPointToTinyVector3(lFbxS);
                    // Transform transform(T, S, Q);
                    // Matrix4 mat = transform.getAffineMatrix();
                    // boneNode->setOffsetMatrix(mat.inverse());

                    Matrix4 mat = FbxAMatrixToMatrix4(lFbxOffsetM);
                    boneNode->setOffsetMatrix(mat);
                    
                    MCONV_LOG_DEBUG("Bone [%s] offset matrix set. M: %s", boneName.c_str(), mat.getDebugString(true).c_str())
                }
            }

            MCONV_LOG_INFO("Completed generating mesh link data.")
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::generateMeshes(bool hasSkin)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start generating meshes data ...")
            
            for (const auto mesh : mFbxMeshes)
            {
                MeshData *meshData = nullptr;
                if (lookupMeshData(mesh.second, meshData))
                {
                    // 1. 生成网格数据（顶点、法线、UV等）
                    ret = generateMesh(mesh.second, meshData);
                    if (T3D_FAILED(ret))
                    {
                        break;
                    }

                    if (hasSkin)
                    {
                        // 2. 生成蒙皮数据（如果有骨骼动画）
                        ret = generateMeshSkinData(mesh.second, meshData);
                        if (T3D_FAILED(ret))
                        {
                            break;
                        }
                    }
                    
                    // // 3. 最后进行顶点去重和索引生成
                    // ret = generateMeshIndices(meshData);
                    // if (T3D_FAILED(ret))
                    // {
                    //     break;
                    // }
                }
                else
                {
                    MCONV_LOG_ERROR("Failed to lookup mesh data.")
                    ret = T3D_ERR_NOT_FOUND;
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to generate meshes data.")
                break;
            }

            MCONV_LOG_INFO("Completed generating meshes data.")
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::generateMeshSkinData(FbxMesh *lFbxMesh, MeshData *meshData)
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
                    if (lFbxCluster == nullptr)
                    {
                        continue;
                    }

                    // 获取骨骼节点
                    FbxNode *lFbxLink = lFbxCluster->GetLink();
                    if (lFbxLink == nullptr)
                    {
                        continue;
                    }

                    String boneName = lFbxLink->GetName();
                    FbxNode *lFbxSkeletalRoot = getFbxSkeletalRoot(lFbxLink);
                    if (lFbxSkeletalRoot == nullptr)
                    {
                        continue;
                    }

                    // 查找骨骼在 mBones 中的索引
                    SkeletalAnimationData *skelAniData = getSkeletalAnimationData(lFbxSkeletalRoot);
                    if (skelAniData == nullptr)
                    {
                        continue;
                    }

                    const auto it = skelAniData->boneMap.find(lFbxLink);
                    if (it == skelAniData->boneMap.end())
                    {
                        continue;
                    }
                    
                    // 获取骨骼索引 (在 mBones 中)
                    uint16_t boneIndex = it->second;

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
            meshData->blendWeights.clear();
            meshData->blendIndices.clear();

            // 检查是否有顶点到控制点的映射
            if (meshData->vertexToControlPointMap.empty())
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
            size_t vertexCount = meshData->vertexToControlPointMap.size();
            meshData->blendWeights.resize(vertexCount);
            meshData->blendIndices.resize(vertexCount);
            
            for (size_t i = 0; i < vertexCount; i++)
            {
                int32_t ctrlPointIndex = meshData->vertexToControlPointMap[i];
                if (ctrlPointIndex >= 0 && ctrlPointIndex < controlPointCount)
                {
                    // 使用对应控制点的权重和索引
                    TArray<float_t> &weights = meshData->blendWeights[i]; 
                    for (const auto &weight : controlPointWeights[ctrlPointIndex])
                    {
                        weights.emplace_back(weight);
                    }
                    T3D_ASSERT(weights.size() <= 4);
                    
                    TArray<uint8_t> &indices = meshData->blendIndices[i];
                    for (const auto &index : controlPointIndices[ctrlPointIndex])
                    {
                        indices.emplace_back(index);
                    }
                    T3D_ASSERT(indices.size() <= 4);
                }
                else
                {
                    // 无效的控制点索引，使用默认值
                    T3D_ASSERT(false);
                    MCONV_LOG_ERROR("Invalid control point index: %d", ctrlPointIndex)
                    ret = T3D_ERR_OUT_OF_BOUND;
                    break;
                }
            }
            
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            MCONV_LOG_INFO("Extracted blend weights and indices for %lld vertices (from %d control points)", 
                vertexCount, controlPointCount)

        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult FBXImporterNew::generateMesh(FbxMesh *lFbxMesh, MeshData *meshData)
    {
        TResult ret = T3D_OK;

        do
        {
            FbxNode *lFbxNode = lFbxMesh->GetNode();
            String name = lFbxNode->GetName();
            
            MCONV_LOG_INFO("Starting generating mesh [%s] ...", name.c_str())

            // 三角形数量
            int32_t triangleCount = lFbxMesh->GetPolygonCount();
            int32_t ctrlPointCount = lFbxMesh->GetControlPointsCount();
            MCONV_LOG_INFO("FBX mesh triangle count: %d, control point count: %d", triangleCount, ctrlPointCount)

            // 获取三角形材质索引，让相同材质的三角形在一起，形成一个 submesh
            TArray<int32_t> triangleMaterialIndices(triangleCount, -1);
            getFbxTriangleMaterialIndices(lFbxMesh, triangleCount, triangleMaterialIndices);

            // 获取三角形平滑组索引，相同平滑组的顶点合并在一起，使用面法线的平均值作为法线
            TArray<int32_t> triangleSmGroupIndices(triangleCount, -1);
            getFbxTriangleSmoothGroupIndices(lFbxMesh, triangleCount, triangleSmGroupIndices);

            int32_t texUVCount = lFbxMesh->GetElementUVCount();

            int32_t materialCount = lFbxNode->GetMaterialCount();

            int32_t vertexCount = triangleCount * 3;
            meshData->vertices.clear();
            meshData->vertices.reserve(vertexCount);
            meshData->normals.clear();
            meshData->normals.reserve(vertexCount);
            meshData->smoothingGroups.clear();
            meshData->smoothingGroups.reserve(vertexCount);
            meshData->tangents.clear();
            meshData->tangents.reserve(vertexCount);
            meshData->binormals.clear();
            meshData->binormals.reserve(vertexCount);
            meshData->vertexToControlPointMap.clear();  // 清空控制点索引映射
            meshData->vertexToControlPointMap.reserve(vertexCount);
            for (int32_t l = 0; l < T3D_MAX_TEXTURE_LEVEL; l++)
            {
                meshData->texCoords[l].clear();
                meshData->texCoords[l].reserve(vertexCount);
            }
            
            for (int32_t k = 0; k < materialCount; k++)
            {
                FbxSurfaceMaterial *lMaterial = lFbxNode->GetMaterial(k);
                
                MCONV_LOG_INFO("Material [%d]: %s", k, lMaterial->GetName())
                
                name += String("_") + lFbxNode->GetMaterial(k)->GetName();
                FbxSurfaceMaterial *lFbxMaterial = lFbxNode->GetMaterial(k);
                SubMeshData *subMeshData = T3D_NEW SubMeshData();
                subMeshData->fbxMaterial = lFbxMaterial;
                meshData->materials.emplace(name, subMeshData);

                for (int32_t i = 0; i < triangleCount; i++)
                {
                    if (triangleMaterialIndices[i] == k)
                    {
                        // 处理三角形
                        // 由于X轴镜像会导致三角形绕序反转，需要调整顶点顺序
#if defined (T3D_FBX_LOADER_RH)
                        const int32_t vertexOrder[3] = {0, 2, 1};  // 反转绕序：从 (0,1,2) 变为 (0,2,1)
#else
                        const int32_t vertexOrder[3] = {0, 1, 2};  // 保持原顺序
#endif

                        for (int32_t j = 0; j < 3; j++)
                        {
                            // 获取顶点
                            Vector3 V;
                            int32_t vertexIndex = vertexOrder[j];
                            int32_t ctrlPointIndex = lFbxMesh->GetPolygonVertex(i, vertexIndex);
                            readVertex(lFbxMesh, ctrlPointIndex, V);

                            // 获取 uv
                            Vector2Array texCoords;
                            texCoords.reserve(texUVCount);
                            for (int32_t l = 0; l < texUVCount; l++)
                            {
                                Vector2 UV;
                                readUV(lFbxMesh, ctrlPointIndex, vertexIndex + 3 * i, l, UV);
                                texCoords.emplace_back(UV);
                            }

                            // 获取顶点颜色
                            ColorRGBA C;
                            readColor(lFbxMesh, ctrlPointIndex, vertexIndex + 3 * i, C);

                            // 获取法线
                            Vector3 N, T, B;
                            readNormal(lFbxMesh, ctrlPointIndex, vertexIndex + 3 * i, N);
                            if (texUVCount > 0)
                            {
                                readTangent(lFbxMesh, ctrlPointIndex, vertexIndex + 3 * i, T);
                                readBinormal(lFbxMesh, ctrlPointIndex, vertexIndex + 3 * i, B);
                            }

                            // 查找已经收集的顶点中，是否有和当前顶点属于一个光滑组的点
                            // 如果同一个光滑组且控制点相同，则累计光滑组的法线、副法线、切线，用于计算平均值
                            // 注意：必须比较控制点索引，因为不同控制点的顶点不应该共享法线
                            uint32_t v = 0;
                            // for (v = 0; v < meshData->vertices.size(); v++)
                            // {
                            //     if (meshData->vertices[v] == V && 
                            //         meshData->smoothingGroups[v] == triangleSmGroupIndices[i] &&
                            //         meshData->vertexToControlPointMap[v] == ctrlPointIndex)
                            //     {
                            //         N += meshData->normals[v];
                            //         meshData->normals[v] = N;
                            //
                            //         if (texUVCount > 0)
                            //         {
                            //             T += meshData->tangents[v];
                            //             meshData->tangents[v] = T;
                            //
                            //             B += meshData->binormals[v];
                            //             meshData->binormals[v] = B;
                            //         }
                            //     }
                            // }
                            //
                            // // 位置
                            // meshData->vertices.emplace_back(V);
                            // // 颜色
                            // meshData->colors.emplace_back(C);
                            // // 光滑组
                            // meshData->smoothingGroups.emplace_back(triangleSmGroupIndices[i]);
                            // // 纹理
                            // for (int32_t layer = 0; layer < texUVCount; layer++)
                            // {
                            //     meshData->texCoords[layer].emplace_back(texCoords[layer]);
                            // }
                            // // 法线
                            // meshData->normals.emplace_back(N);
                            // // 切线
                            // meshData->tangents.emplace_back(T);
                            // // 副法线
                            // meshData->binormals.emplace_back(B);
                            // // 保存该顶点对应的控制点索引
                            // meshData->vertexToControlPointMap.emplace_back(ctrlPointIndex);
                            
                            // 查看这个顶点是否已经存在（顶点位置、光滑组、UV、控制点索引都相同则为相同）
                            // 注意：必须比较控制点索引，因为不同控制点的蒙皮数据不同
                            for (v = 0; v < meshData->vertices.size(); v++)
                            {
                                if (meshData->vertices[v] == V && 
                                    meshData->smoothingGroups[v] == triangleSmGroupIndices[i] &&
                                    meshData->vertexToControlPointMap[v] == ctrlPointIndex)
                                {
                                    // 如果已经存在，则不添加
                                    int32_t layer = 0;
                                    for (layer = 0; layer < texUVCount; layer++)
                                    {
                                        if (meshData->texCoords[layer][v] != texCoords[layer])
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
                            
                            if (v == meshData->vertices.size())
                            {
                                // 跑到这里表示没有找到相同的顶点，添加
                            
                                // 位置
                                meshData->vertices.emplace_back(V);
                                // 颜色
                                meshData->colors.emplace_back(C);
                                // 光滑组
                                meshData->smoothingGroups.emplace_back(triangleSmGroupIndices[i]);
                                // 纹理
                                for (int32_t layer = 0; layer < texUVCount; layer++)
                                {
                                    meshData->texCoords[layer].emplace_back(texCoords[layer]);
                                }
                                // 法线
                                meshData->normals.emplace_back(N);
                                // 切线
                                meshData->tangents.emplace_back(T);
                                // 副法线
                                meshData->binormals.emplace_back(B);
                                // 保存该顶点对应的控制点索引
                                meshData->vertexToControlPointMap.emplace_back(ctrlPointIndex);
                            }
                            
                            // 索引
                            subMeshData->indices.emplace_back(v);
                        }
                    }
                }
            }
            
            MCONV_LOG_INFO("Completed generating mesh !")
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void FBXImporterNew::getFbxTriangleSmoothGroupIndices(FbxGeometryBase *lFbxGeometry, int32_t triangleCount, TArray<int32_t> &triangleSmGroupIndices) const
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

    void FBXImporterNew::getFbxTriangleMaterialIndices(FbxGeometryBase *lFbxGeometry, int32_t triangleCount, TArray<int32_t> &triangleMaterialIndices) const
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

    void FBXImporterNew::readVertex(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, Vector3 &vertex)
    {
        FbxVector4 *lFbxCtrlPoints = lFbxGeometry->GetControlPoints();

        vertex = FbxPointToTinyVector3Remap(lFbxCtrlPoints[ctrlPointIndex]);
        // vertex[0] = static_cast<float32_t>(lFbxCtrlPoints[ctrlPointIndex][0]);
        // vertex[1] = static_cast<float32_t>(lFbxCtrlPoints[ctrlPointIndex][1]);
        // vertex[2] = static_cast<float32_t>(lFbxCtrlPoints[ctrlPointIndex][2]);
    }

    //--------------------------------------------------------------------------

    void FBXImporterNew::readNormal(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &normal)
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
                        // normal[0] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(ctrlPointIndex)[0]);
                        // normal[1] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(ctrlPointIndex)[1]);
                        // normal[2] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(ctrlPointIndex)[2]);
                        normal = FbxPointToTinyVector3Remap(lFbxNormal->GetDirectArray().GetAt(ctrlPointIndex));
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxNormal->GetIndexArray().GetAt(ctrlPointIndex);
                        // normal[0] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[0]);
                        // normal[1] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[1]);
                        // normal[2] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[2]);
                        normal = FbxPointToTinyVector3Remap(lFbxNormal->GetDirectArray().GetAt(id));
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
                        // normal[0] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(vertexCounter)[0]);
                        // normal[1] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(vertexCounter)[1]);
                        // normal[2] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(vertexCounter)[2]);
                        normal = FbxPointToTinyVector3Remap(lFbxNormal->GetDirectArray().GetAt(vertexCounter));
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxNormal->GetIndexArray().GetAt(vertexCounter);
                        // normal[0] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[0]);
                        // normal[1] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[1]);
                        // normal[2] = static_cast<float32_t>(lFbxNormal->GetDirectArray().GetAt(id)[2]);
                        normal = FbxPointToTinyVector3Remap(lFbxNormal->GetDirectArray().GetAt(id));
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

    void FBXImporterNew::readTangent(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &tangent)
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
                        // tangent[0] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(ctrlPointIndex)[0]);
                        // tangent[1] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(ctrlPointIndex)[1]);
                        // tangent[2] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(ctrlPointIndex)[2]);
                        tangent = FbxPointToTinyVector3Remap(lFbxTangent->GetDirectArray().GetAt(ctrlPointIndex));
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxTangent->GetIndexArray().GetAt(ctrlPointIndex);
                        // tangent[0] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[0]);
                        // tangent[1] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[1]);
                        // tangent[2] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[2]);
                        tangent = FbxPointToTinyVector3Remap(lFbxTangent->GetDirectArray().GetAt(id));
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
                        // tangent[0] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(vertexCounter)[0]);
                        // tangent[1] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(vertexCounter)[1]);
                        // tangent[2] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(vertexCounter)[2]);
                        tangent = FbxPointToTinyVector3Remap(lFbxTangent->GetDirectArray().GetAt(vertexCounter));
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxTangent->GetIndexArray().GetAt(vertexCounter);
                        // tangent[0] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[0]);
                        // tangent[1] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[1]);
                        // tangent[2] = static_cast<float32_t>(lFbxTangent->GetDirectArray().GetAt(id)[2]);
                        tangent = FbxPointToTinyVector3Remap(lFbxTangent->GetDirectArray().GetAt(id));
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

    void FBXImporterNew::readBinormal(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, Vector3 &binormal)
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
                        // binormal[0] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(ctrlPointIndex)[0]);
                        // binormal[1] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(ctrlPointIndex)[1]);
                        // binormal[2] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(ctrlPointIndex)[2]);
                        binormal = FbxPointToTinyVector3Remap(lFbxBinormal->GetDirectArray().GetAt(ctrlPointIndex));
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxBinormal->GetIndexArray().GetAt(ctrlPointIndex);
                        // binormal[0] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[0]);
                        // binormal[1] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[1]);
                        // binormal[2] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[2]);
                        binormal = FbxPointToTinyVector3Remap(lFbxBinormal->GetDirectArray().GetAt(id));
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
                        // binormal[0] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(vertexCounter)[0]);
                        // binormal[1] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(vertexCounter)[1]);
                        // binormal[2] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(vertexCounter)[2]);
                        binormal = FbxPointToTinyVector3Remap(lFbxBinormal->GetDirectArray().GetAt(vertexCounter));
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int32_t id = lFbxBinormal->GetIndexArray().GetAt(vertexCounter);
                        // binormal[0] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[0]);
                        // binormal[1] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[1]);
                        // binormal[2] = static_cast<float32_t>(lFbxBinormal->GetDirectArray().GetAt(id)[2]);
                        binormal = FbxPointToTinyVector3Remap(lFbxBinormal->GetDirectArray().GetAt(id));
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

    void FBXImporterNew::readColor(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t vertexCounter, ColorRGBA &color)
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

    void FBXImporterNew::readUV(FbxGeometryBase *lFbxGeometry, int32_t ctrlPointIndex, int32_t texUVIndex, int32_t uvLayer, Vector2 &uv)
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
                        int32_t id = lFbxUV->GetIndexArray().GetAt(texUVIndex);
                        uv[0] = static_cast<float32_t>(lFbxUV->GetDirectArray().GetAt(id)[0]);
                        uv[1] = static_cast<float32_t>(lFbxUV->GetDirectArray().GetAt(id)[1]);
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

    // TResult FBXImporterNew::generateMeshIndices(MeshData *meshData)
    // {
    //     TResult ret = T3D_OK;
    //     do
    //     {
    //         // 如果顶点数据为空，直接返回
    //         if (meshData->vertices.empty())
    //         {
    //             break;
    //         }
    //
    //         // 保存原始数据
    //         Vector3Array originalVertices = meshData->vertices;
    //         Vector3Array originalNormals = meshData->normals;
    //         ColorArray originalColors = meshData->colors;
    //         Vector3Array originalTangents = meshData->tangents;
    //         Vector3Array originalBinormals = meshData->binormals;
    //         TArray<TArray<float32_t>> originalBlendWeights = meshData->blendWeights;
    //         TArray<TArray<uint8_t>> originalBlendIndices = meshData->blendIndices;
    //         
    //         // 保存原始纹理坐标（支持多层）
    //         Vector2Array originalTexCoords[T3D_MAX_TEXTURE_LEVEL];
    //         for (int32_t i = 0; i < T3D_MAX_TEXTURE_LEVEL; ++i)
    //         {
    //             originalTexCoords[i] = meshData->texCoords[i];
    //         }
    //
    //         // 清空原始数据，准备存放去重后的数据
    //         meshData->vertices.clear();
    //         meshData->normals.clear();
    //         meshData->colors.clear();
    //         meshData->tangents.clear();
    //         meshData->binormals.clear();
    //         meshData->blendWeights.clear();
    //         meshData->blendIndices.clear();
    //         meshData->indices.clear();
    //         
    //         for (int32_t i = 0; i < T3D_MAX_TEXTURE_LEVEL; ++i)
    //         {
    //             meshData->texCoords[i].clear();
    //         }
    //
    //         // 用于存储顶点到新索引的映射
    //         // key: 原始顶点索引, value: 去重后的新索引
    //         TMap<uint32_t, uint32_t> vertexIndexMap;
    //
    //         // 遍历所有原始顶点
    //         uint32_t originalVertexCount = originalVertices.size();
    //         for (uint32_t i = 0; i < originalVertexCount; ++i)
    //         {
    //             // 检查是否已经存在相同的顶点
    //             bool found = false;
    //             uint32_t newIndex = 0;
    //
    //             // 遍历已经去重的顶点，查找是否有完全相同的顶点
    //             for (uint32_t j = 0; j < meshData->vertices.size(); ++j)
    //             {
    //                 bool isSame = true;
    //
    //                 // 比较顶点位置
    //                 if (meshData->vertices[j] != originalVertices[i])
    //                 {
    //                     isSame = false;
    //                 }
    //
    //                 // 比较法线（如果存在）
    //                 if (isSame && !originalNormals.empty())
    //                 {
    //                     if (meshData->normals[j] != originalNormals[i])
    //                     {
    //                         isSame = false;
    //                     }
    //                 }
    //
    //                 // 比较颜色（如果存在）
    //                 if (isSame && !originalColors.empty())
    //                 {
    //                     if (meshData->colors[j] != originalColors[i])
    //                     {
    //                         isSame = false;
    //                     }
    //                 }
    //
    //                 // 比较纹理坐标（所有层）
    //                 if (isSame)
    //                 {
    //                     for (int32_t k = 0; k < T3D_MAX_TEXTURE_LEVEL; ++k)
    //                     {
    //                         if (!originalTexCoords[k].empty())
    //                         {
    //                             if (meshData->texCoords[k][j] != originalTexCoords[k][i])
    //                             {
    //                                 isSame = false;
    //                                 break;
    //                             }
    //                         }
    //                     }
    //                 }
    //
    //                 // 比较切线（如果存在）
    //                 if (isSame && !originalTangents.empty())
    //                 {
    //                     if (meshData->tangents[j] != originalTangents[i])
    //                     {
    //                         isSame = false;
    //                     }
    //                 }
    //
    //                 // 比较副法线（如果存在）
    //                 if (isSame && !originalBinormals.empty())
    //                 {
    //                     if (meshData->binormals[j] != originalBinormals[i])
    //                     {
    //                         isSame = false;
    //                     }
    //                 }
    //
    //                 // 比较蒙皮权重（如果存在）
    //                 if (isSame && !originalBlendWeights.empty())
    //                 {
    //                     const auto &weights1 = meshData->blendWeights[j];
    //                     const auto &weights2 = originalBlendWeights[i];
    //                     
    //                     if (weights1.size() != weights2.size())
    //                     {
    //                         isSame = false;
    //                     }
    //                     else
    //                     {
    //                         for (size_t w = 0; w < weights1.size(); ++w)
    //                         {
    //                             if (fabs(weights1[w] - weights2[w]) > 0.0001f)
    //                             {
    //                                 isSame = false;
    //                                 break;
    //                             }
    //                         }
    //                     }
    //                 }
    //
    //                 // 比较蒙皮索引（如果存在）
    //                 if (isSame && !originalBlendIndices.empty())
    //                 {
    //                     const auto &indices1 = meshData->blendIndices[j];
    //                     const auto &indices2 = originalBlendIndices[i];
    //                     
    //                     if (indices1.size() != indices2.size())
    //                     {
    //                         isSame = false;
    //                     }
    //                     else
    //                     {
    //                         for (size_t b = 0; b < indices1.size(); ++b)
    //                         {
    //                             if (indices1[b] != indices2[b])
    //                             {
    //                                 isSame = false;
    //                                 break;
    //                             }
    //                         }
    //                     }
    //                 }
    //
    //                 // 如果找到完全相同的顶点
    //                 if (isSame)
    //                 {
    //                     found = true;
    //                     newIndex = j;
    //                     break;
    //                 }
    //             }
    //
    //             // 如果没有找到相同的顶点，添加新顶点
    //             if (!found)
    //             {
    //                 newIndex = meshData->vertices.size();
    //
    //                 // 添加顶点位置
    //                 meshData->vertices.push_back(originalVertices[i]);
    //
    //                 // 添加法线
    //                 if (!originalNormals.empty())
    //                 {
    //                     meshData->normals.emplace_back(originalNormals[i]);
    //                 }
    //
    //                 // 添加颜色
    //                 if (!originalColors.empty())
    //                 {
    //                     meshData->colors.emplace_back(originalColors[i]);
    //                 }
    //
    //                 // 添加纹理坐标
    //                 for (int32_t k = 0; k < T3D_MAX_TEXTURE_LEVEL; ++k)
    //                 {
    //                     if (!originalTexCoords[k].empty())
    //                     {
    //                         meshData->texCoords[k].emplace_back(originalTexCoords[k][i]);
    //                     }
    //                 }
    //
    //                 // 添加切线
    //                 if (!originalTangents.empty())
    //                 {
    //                     meshData->tangents.emplace_back(originalTangents[i]);
    //                 }
    //
    //                 // 添加副法线
    //                 if (!originalBinormals.empty())
    //                 {
    //                     meshData->binormals.emplace_back(originalBinormals[i]);
    //                 }
    //
    //                 // 添加蒙皮权重
    //                 if (!originalBlendWeights.empty())
    //                 {
    //                     meshData->blendWeights.emplace_back(TArray<float_t>(originalBlendWeights[i].size()));
    //                     auto &weights = meshData->blendWeights.back();
    //                     int32_t j = 0;
    //                     for (const auto &weight : originalBlendWeights[i])
    //                     {
    //                         weights[j] = weight;
    //                         j++;
    //                     }
    //                     T3D_ASSERT(meshData->blendWeights.back().size() == originalBlendWeights[i].size());
    //                 }
    //
    //                 // 添加蒙皮索引
    //                 if (!originalBlendIndices.empty())
    //                 {
    //                     meshData->blendIndices.emplace_back(TArray<uint8_t>(originalBlendIndices[i].size()));
    //                     auto &indices = meshData->blendIndices.back();
    //                     int32_t j = 0;
    //                     for (const auto &index : originalBlendIndices[i])
    //                     {
    //                         indices[j] = index;
    //                         j++;
    //                     }
    //                     T3D_ASSERT(meshData->blendIndices.back().size() == originalBlendIndices[i].size());
    //                 }
    //             }
    //
    //             // 添加索引
    //             meshData->indices.push_back(newIndex);
    //         }
    //
    //         MCONV_LOG_INFO("Mesh indices generated: original vertices = %u, unique vertices = %llu, indices = %llu",
    //             originalVertexCount, meshData->vertices.size(), meshData->indices.size())
    //
    //     } while (false);
    //     
    //     return ret;
    // }
    
    //--------------------------------------------------------------------------

    TResult FBXImporterNew::createMaterialsAndSubMeshes(MeshData *meshData)
    {
        TResult ret = T3D_OK;

        do
        {
            for (const auto &materialData : meshData->materials)
            {
                const auto &name = materialData.first;
                SubMeshData *subMeshData = materialData.second;
                // 创建对应的材质
                MaterialPtr material;
                ret = createMaterial(subMeshData->fbxMaterial, material);
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("Failed to create material.")
                    break;
                }

                String materialName = material->getName() + "." + Resource::EXT_MATERIAL;
                mResources.emplace(materialName, material);

                // 创建子网格
                SubMeshPtr submesh;
                ret = createSubMesh(name, subMeshData, material, submesh);
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("failed to create submesh.")
                    break;
                }

                meshData->subMeshes.emplace(name, submesh);
            }
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult FBXImporterNew::createMaterial(FbxSurfaceMaterial *lFbxMaterial, MaterialPtr &material)
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

    TResult FBXImporterNew::createTexture(const FbxProperty &lFbxProperty, TexturePtr &texture)
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

    TResult FBXImporterNew::createSubMesh(const String &name, SubMeshData *subMeshData, Material *material, SubMeshPtr &submesh)
    {
        TResult ret = T3D_OK;

        do
        {
            bool is16Bits = true;
            Buffer indices;
            if (subMeshData->indices.size() > std::numeric_limits<uint16_t>::max())
            {
                indices.DataSize = subMeshData->indices.size() * sizeof(uint32_t);
                indices.Data = T3D_POD_NEW_ARRAY(uint8_t, indices.DataSize);
                uint32_t *data = (uint32_t *)indices.Data;
                for (uint32_t idx = 0; idx < subMeshData->indices.size(); idx++)
                {
                    *data = subMeshData->indices[idx];
                    data++;
                }
                is16Bits = false;
            }
            else
            {
                indices.DataSize = subMeshData->indices.size() * sizeof(uint16_t);
                indices.Data = T3D_POD_NEW_ARRAY(uint8_t, indices.DataSize);
                uint16_t *data = (uint16_t *)indices.Data;
                for (uint32_t idx = 0; idx < subMeshData->indices.size(); idx++)
                {
                    *data = static_cast<uint16_t>(subMeshData->indices[idx]);
                    data++;
                }
                is16Bits = true;
            }

            submesh = SubMesh::create(name, material->getUUID(), PrimitiveType::kTriangleList, indices, is16Bits);

            indices.release();
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult FBXImporterNew::createMesh(FbxNode *lFbxMeshRoot, MeshData *meshData)
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
            
            if (!meshData->colors.empty())
            {
                // diffuse color
                T3D_ASSERT(meshData->colors.size() == meshData->vertices.size());
                VertexAttribute attrCol(0, offset, VertexAttribute::Type::E_VAT_FLOAT4, VertexAttribute::Semantic::E_VAS_DIFFUSE, 0);
                attributes.emplace_back(attrCol);
                offset += sizeof(ColorRGBA);
            }

            for (uint32_t i = 0; i < T3D_MAX_TEXTURE_LEVEL; i++)
            {
                // uv
                if (!meshData->texCoords[i].empty())
                {
                    T3D_ASSERT(meshData->texCoords[i].size() == meshData->vertices.size());
                    VertexAttribute attrUV(0, offset, VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, i);
                    attributes.emplace_back(attrUV);
                    offset += sizeof(Vector2);
                }
            }

            if (!meshData->normals.empty())
            {
                // normal
                T3D_ASSERT(meshData->normals.size() == meshData->vertices.size());
                VertexAttribute attrNor(0, offset, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_NORMAL, 0);
                attributes.emplace_back(attrNor);
                offset += sizeof(Vector3);
            }

            if (!meshData->binormals.empty())
            {
                // binormal
                T3D_ASSERT(meshData->binormals.size() == meshData->vertices.size());
                VertexAttribute attrBin(0, offset, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_BINORMAL, 0);
                attributes.emplace_back(attrBin);
                offset += sizeof(Vector3);
            }

            if (!meshData->tangents.empty())
            {
                // tangent
                T3D_ASSERT(meshData->tangents.size() == meshData->vertices.size());
                VertexAttribute attrTan(0, offset, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_TANGENT, 0);
                attributes.emplace_back(attrTan);
                offset += sizeof(Vector3);
            }

            bool hasSkin = false;

            if (!meshData->blendWeights.empty() && !meshData->blendIndices.empty())
            {
                // blend weights
                T3D_ASSERT(meshData->blendWeights.size() == meshData->vertices.size());
                T3D_ASSERT(!meshData->blendIndices.empty());
                VertexAttribute attrWeight(0, offset, VertexAttribute::Type::E_VAT_FLOAT4, VertexAttribute::Semantic::E_VAS_BLENDWEIGHT, 0);
                attributes.emplace_back(attrWeight);
                offset += sizeof(Vector4);

                // blend indices
                T3D_ASSERT(meshData->blendIndices.size() == meshData->vertices.size());
                T3D_ASSERT(!meshData->blendIndices.empty());
                VertexAttribute attrIndex(0, offset, VertexAttribute::Type::E_VAT_UBYTE4, VertexAttribute::Semantic::E_VAS_BLENDINDICES, 0);
                attributes.emplace_back(attrIndex);
                offset += sizeof(uint32_t);

                hasSkin = true;
            }
            else if (!meshData->blendWeights.empty() || !meshData->blendIndices.empty())
            {
                ret = T3D_ERR_FAIL;
                MCONV_LOG_ERROR("Blend weights and indices must be both present or both absent.")
                break;
            }
            else
            {
                hasSkin = false;
            }

            TArray<float32_t> data;

            for (size_t i = 0; i < meshData->vertices.size(); i++)
            {
                // position
                const Vector3 &pos = meshData->vertices[i];
                data.emplace_back(pos[0]);
                data.emplace_back(pos[1]);
                data.emplace_back(pos[2]);

                // diffuse color
                if (!meshData->colors.empty())
                {
                    const ColorRGBA &color = meshData->colors[i];
                    data.emplace_back(color.red());
                    data.emplace_back(color.green());
                    data.emplace_back(color.blue());
                    data.emplace_back(color.alpha());
                }

                // texture uv
                for (const auto &texCoord : meshData->texCoords)
                {
                    if (!texCoord.empty())
                    {
                        const Vector2 &uv = texCoord[i];
                        data.emplace_back(uv[0]);
                        data.emplace_back(uv[1]);
                    }
                }

                // normal
                if (!meshData->normals.empty())
                {
                    const Vector3 &nor = meshData->normals[i];
                    data.emplace_back(nor[0]);
                    data.emplace_back(nor[1]);
                    data.emplace_back(nor[2]);
                }

                // binormal
                if (!meshData->binormals.empty())
                {
                    const Vector3 &bin = meshData->binormals[i];
                    data.emplace_back(bin[0]);
                    data.emplace_back(bin[1]);
                    data.emplace_back(bin[2]);
                }
                
                // tangent
                if (!meshData->tangents.empty())
                {
                    const Vector3 &tan = meshData->tangents[i];
                    data.emplace_back(tan[0]);
                    data.emplace_back(tan[1]);
                    data.emplace_back(tan[2]);
                }

                // blend weights (最多4个权重)
                if (!meshData->blendWeights.empty())
                {
                    const TArray<float32_t> &weights = meshData->blendWeights[i];
                    for (size_t j = 0; j < T3D_MAX_BLEND_BONES; j++)
                    {
                        data.emplace_back(weights[j]);
                    }
                }

                // blend indices (最多4个索引)
                if (!meshData->blendIndices.empty())
                {
                    const TArray<uint8_t> &indices = meshData->blendIndices[i];
                    // 将4个uint8_t打包成一个float32_t
                    uint32_t packedIndices = 0;
                    for (size_t j = 0; j < T3D_MAX_BLEND_BONES; j++)
                    {
                        packedIndices |= (static_cast<uint32_t>(indices[j]) << (j * 8));
                    }
                    // 将uint32_t重新解释为float32_t
                    float32_t packedFloat;
                    memcpy(&packedFloat, &packedIndices, sizeof(float32_t));
                    data.emplace_back(packedFloat);
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
                mesh = T3D_MESH_MGR.createMesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(meshData->subMeshes));
                if (mesh == nullptr)
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    MCONV_LOG_ERROR("Failed to create mesh %s", name.c_str())
                    break;
                }
            }
            else
            {
                FbxNode *lFbxBoneRoot = nullptr;
                if (lookupFbxBoneRoot(lFbxMeshRoot, lFbxBoneRoot))
                {
                    SkeletalAnimationData *skelAniData = getSkeletalAnimationData(lFbxBoneRoot);
                    if (skelAniData != nullptr)
                    {
                        mesh = T3D_MESH_MGR.createSkinnedMesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(meshData->subMeshes), skelAniData->skeleton, skelAniData->animation);
                        if (mesh == nullptr)
                        {
                            ret = T3D_ERR_RES_INVALID_OBJECT;
                            MCONV_LOG_ERROR("Failed to create skinned mesh %s", name.c_str())
                            break;
                        }
                    }
                    else
                    {
                        ret = T3D_ERR_RES_INVALID_OBJECT;
                        MCONV_LOG_ERROR("Failed to get skeletal animation data when create skinned mesh %s", name.c_str())
                        break;
                    }
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    MCONV_LOG_ERROR("Failed to lookup fbx bone root when create skinned mesh %s", name.c_str())
                    break;
                }
            }

            name = name + "." + Resource::EXT_MESH;
            mResources.emplace(name, mesh);
        } while (false);

        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult FBXImporterNew::createMeshes()
    {
        TResult ret = T3D_OK;

        do
        {
            for (const auto mesh : mFbxMeshData)
            {
                FbxNode *lFbxMeshRoot = nullptr;
                if (lookupFbxMeshRoot(mesh.first, lFbxMeshRoot))
                {
                    ret = createMaterialsAndSubMeshes(mesh.second);
                    if (T3D_FAILED(ret))
                    {
                        break;
                    }
                    
                    ret = createMesh(lFbxMeshRoot, mesh.second);
                    if (T3D_FAILED(ret))
                    {
                        break;
                    }
                }
                else
                {
                    MCONV_LOG_ERROR("Failed to lookup fbx mesh root.")
                    ret = T3D_ERR_NOT_FOUND;
                    break;
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::createSkeletonsAndAnimations()
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start creating skeleton and animation resources ...")
            
            for (const auto skelAniData : mFbxSkeletons)
            {
                MCONV_LOG_INFO("Start creating skeleton and animation resources for [%s] ...", skelAniData.first->GetName())
                
                ret = createSkeleton(skelAniData.second);
                if (T3D_FAILED(ret))
                {
                    break;
                }

                ret = createAnimation(skelAniData.second);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to create skeleton and animation resources.")
                break;
                
            }

            MCONV_LOG_INFO("Completed creating skeleton and animation resources.")
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::createSkeleton(SkeletalAnimationData *skelAniData)
    {
        TResult ret = T3D_OK;

        do
        {
            String name = mOutputName;
            skelAniData->skeleton = T3D_SKELETON_MGR.createSkeleton(name, std::move(skelAniData->bones));
            if (skelAniData->skeleton == nullptr)
            {
                MCONV_LOG_ERROR("Failed to create skeleton for [%s]", name.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            name = name + "." + Resource::EXT_SKELETON;
            mResources.emplace(name, skelAniData->skeleton);
            MCONV_LOG_INFO("Created skeleton [%s] with %lld bones", name.c_str(), skelAniData->bones.size())
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporterNew::createAnimation(SkeletalAnimationData *skelAniData)
    {
        TResult ret = T3D_OK;

        do
        {
            AnimationClips clips;
            for (const auto aniClip : skelAniData->animationClips)
            {
                AnimationClipData *clipData = aniClip.second; 
                AnimationClipPtr clip = AnimationClip::create(aniClip.first, clipData->duration, std::move(clipData->tracks));
                if (clip != nullptr)
                {
                    clips.emplace(aniClip.first, clip);
                    MCONV_LOG_INFO("Created animation clip [%s] with %lld tracks, duration: %u ms", 
                            aniClip.first.c_str(), clipData->tracks.size(), clipData->duration)
                    
                    for (const auto &track : clip->getTracks())
                    {
                        MCONV_LOG_DEBUG("\tTrack: %s", track.first.c_str())
                        
                        MCONV_LOG_DEBUG("\t\tTrack Translation: ")
                        for (const auto &trackT : track.second->getTranslationTrack())
                        {
                            MCONV_LOG_DEBUG("\t\tTime: %u, T: %s", trackT->getTime(), trackT->getTranslation().getDebugString().c_str())
                        }
                        
                        MCONV_LOG_DEBUG("\t\tTrack Orientation: ")
                        for (const auto &trackO : track.second->getOrientationTrack())
                        {
                            Radian pitch, yaw, roll;
                            trackO->getOrientation().toEulerAnglesYXZ(yaw, pitch, roll);
                            MCONV_LOG_DEBUG("\t\tTime: %u, O: %s, E: (%f, %f, %f)", 
                                trackO->getTime(), trackO->getOrientation().getDebugString().c_str(), pitch.valueDegrees(), yaw.valueDegrees(), roll.valueDegrees())
                        }
                        
                        MCONV_LOG_DEBUG("\t\tTrack Scaling: ")
                        for (const auto &trackS : track.second->getScalingTrack())
                        {
                            MCONV_LOG_DEBUG("\t\tTime: %u, S: %s", trackS->getTime(), trackS->getScaling().getDebugString().c_str())
                        }
                    }
                }
            }

            if (!clips.empty())
            {
                String animName = mOutputName;
                skelAniData->animation = T3D_ANIMATION_MGR.createSkeletalAnimation(animName, std::move(clips));
                if (skelAniData->animation != nullptr)
                {
                    animName = mOutputName  + "." + Resource::EXT_ANIMATION;
                    mResources.emplace(animName, skelAniData->animation);
                    MCONV_LOG_INFO("Created skeletal animation [%s] with %lld clips", animName.c_str(), clips.size())
                }
                else
                {
                    MCONV_LOG_ERROR("Failed to create skeletal animation.")
                    ret = T3D_ERR_FAIL;
                    break;
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    Vector3 FBXImporterNew::FbxPointToTinyVector3Remap(const FbxVector4 &lFbxPos) const
    {
#if defined (T3D_FBX_LOADER_RH)
        return Vector3(static_cast<float>(-lFbxPos[0]), static_cast<float>(lFbxPos[1]), static_cast<float>(lFbxPos[2]));
#else
        return Vector3(static_cast<float>(lFbxPos[0]), static_cast<float>(lFbxPos[1]), static_cast<float>(lFbxPos[2]));
#endif
    }

    //--------------------------------------------------------------------------
    
    Quaternion FBXImporterNew::FbxEulerToTinyQuaternion(const FbxVector4 &lFbxEuler) const
    {
#if defined (T3D_FBX_LOADER_RH)
        FbxVector4 lFbxFixedEuler(lFbxEuler[0], -lFbxEuler[1], -lFbxEuler[2], lFbxEuler[3]);
#else
        FbxVector4 lFbxFixedEuler(lFbxEuler[0], lFbxEuler[1], lFbxEuler[2], lFbxEuler[3]);
#endif

        FbxAMatrix lFbxRotationM;
        lFbxRotationM.SetROnly(lFbxFixedEuler);

        FbxQuaternion lFbxQ = lFbxRotationM.GetQ();
        Quaternion q(static_cast<float>(lFbxQ[3]), static_cast<float>(lFbxQ[0]), static_cast<float>(lFbxQ[1]), static_cast<float>(lFbxQ[2]));
        q.normalize();
        return q;
    }
    
    //--------------------------------------------------------------------------

    Vector3 FBXImporterNew::FbxPointToTinyVector3(const FbxVector4 &lFbxPos) const
    {
        return Vector3(static_cast<float>(lFbxPos[0]), static_cast<float>(lFbxPos[1]), static_cast<float>(lFbxPos[2]));
    }

    //--------------------------------------------------------------------------

    Matrix4 FBXImporterNew::FbxAMatrixToMatrix4(const FbxAMatrix lFbxM) const
    {
        Matrix4 mat;

#if defined (T3D_FBX_LOADER_RH)
        mat[0][0] = static_cast<float>(lFbxM[0][0]);
        mat[1][0]  = static_cast<float>(-lFbxM[0][1]);
        mat[2][0]  = static_cast<float>(-lFbxM[0][2]);
        mat[3][0]  = static_cast<float>(lFbxM[0][3]);
        
        mat[0][1] = static_cast<float>(-lFbxM[1][0]);
        mat[1][1]  = static_cast<float>(lFbxM[1][1]);
        mat[2][1]  = static_cast<float>(lFbxM[1][2]);
        mat[3][1]  = static_cast<float>(lFbxM[1][3]);
        
        mat[0][2]  = static_cast<float>(-lFbxM[2][0]);
        mat[1][2]  = static_cast<float>(lFbxM[2][1]);
        mat[2][2] = static_cast<float>(lFbxM[2][2]);
        mat[3][2] = static_cast<float>(lFbxM[2][3]);
        
        mat[0][3] = static_cast<float>(-lFbxM[3][0]);
        mat[1][3] = static_cast<float>(lFbxM[3][1]);
        mat[2][3] = static_cast<float>(lFbxM[3][2]);
        mat[3][3] = static_cast<float>(lFbxM[3][3]);
#else
        mat[0][0] = static_cast<float>(lFbxM[0][0]);
        mat[1][0]  = static_cast<float>(lFbxM[0][1]);
        mat[2][0]  = static_cast<float>(lFbxM[0][2]);
        mat[3][0]  = static_cast<float>(lFbxM[0][3]);

        mat[0][1] = static_cast<float>(lFbxM[1][0]);
        mat[1][1]  = static_cast<float>(lFbxM[1][1]);
        mat[2][1]  = static_cast<float>(lFbxM[1][2]);
        mat[3][1]  = static_cast<float>(lFbxM[1][3]);

        mat[0][2]  = static_cast<float>(lFbxM[2][0]);
        mat[1][2]  = static_cast<float>(lFbxM[2][1]);
        mat[2][2] = static_cast<float>(lFbxM[2][2]);
        mat[3][2] = static_cast<float>(lFbxM[2][3]);

        mat[0][3] = static_cast<float>(lFbxM[3][0]);
        mat[1][3] = static_cast<float>(lFbxM[3][1]);
        mat[2][3] = static_cast<float>(lFbxM[3][2]);
        mat[3][3] = static_cast<float>(lFbxM[3][3]);
#endif
        
        return mat;
    }

    //--------------------------------------------------------------------------

    bool FBXImporterNew::lookupFbxMeshRoot(FbxNode *lFbxSkeletalRoot, FbxNode *&lFbxMeshRoot) const
    {
        bool ret = false;
        
        const auto itr = mFbxBoneRootToMeshRoot.find(lFbxSkeletalRoot);
        if (itr != mFbxBoneRootToMeshRoot.end())
        {
            lFbxMeshRoot = itr->second;
            ret = true;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool FBXImporterNew::lookupFbxBoneRoot(FbxNode *lFbxMeshRoot, FbxNode *&lFbxBoneRoot) const
    {
        bool ret = false;
        
        const auto itr = mFbxMeshRootToBoneRoot.find(lFbxMeshRoot);
        if (itr != mFbxMeshRootToBoneRoot.end())
        {
            lFbxBoneRoot = itr->second;
            ret = true;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool FBXImporterNew::lookupMeshData(FbxMesh *lFbxMesh, MeshData *&meshData) const
    {
        bool ret = false;

        const auto itr = mFbxMeshData.find(lFbxMesh);
        if (itr != mFbxMeshData.end())
        {
            meshData = itr->second;
            ret = true;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool FBXImporterNew::lookupFbxMeshRoot(FbxMesh *lFbxMesh, FbxNode *&lFbxMeshRoot) const
    {
        bool ret = false;
        
        const auto itr = mFbxMeshRoots.find(lFbxMesh);
        if (itr != mFbxMeshRoots.end())
        {
            lFbxMeshRoot = itr->second;
            ret = true;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    FbxNode *FBXImporterNew::getFbxSkeletalRoot(FbxNode *lFbxNode) const
    {
        FbxNode *lFbxParent = lFbxNode;
        FbxNode *lFbxRoot = lFbxNode;

        while (lFbxParent != mFbxScene->GetRootNode())
        {
            lFbxRoot = lFbxParent;
            lFbxParent = lFbxRoot->GetParent();
        }

        return lFbxRoot;
    }

    //--------------------------------------------------------------------------

    FBXImporterNew::SkeletalAnimationData *FBXImporterNew::getSkeletalAnimationData(FbxNode *lFbxRoot)
    {
        SkeletalAnimationData *skelAniData = nullptr;
        const auto itr = mFbxSkeletons.find(lFbxRoot);
        if (itr != mFbxSkeletons.end())
        {
            skelAniData = itr->second;
        }
        else
        {
            SkeletalAnimationData *skeletalAnimationData = T3D_NEW SkeletalAnimationData();
            mFbxSkeletons.emplace(lFbxRoot, skeletalAnimationData);
            skelAniData = skeletalAnimationData;
        }

        return skelAniData;
    }
    
    //--------------------------------------------------------------------------
}

