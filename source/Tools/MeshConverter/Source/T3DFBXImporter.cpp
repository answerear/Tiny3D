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
                MCONV_LOG_ERROR("Failed to load default material.")
                break;
            }

            if (opts.dstFileType & MeshFileType::kTMesh)
            {
                // 导出静态网格数据
                ret = processStaticMesh(lFbxScene);
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("Failed to process static mesh.")
                    break;
                }
            }
            
            if (opts.dstFileType & MeshFileType::kTSkin)
            {
                // 导出蒙皮数据
                ret = processSkinnedMesh(lFbxScene);
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("Failed to process skinned mesh.")
                    break;
                }
            }
            
            if (opts.dstFileType & MeshFileType::kTSkel)
            {
                // 导出骨架数据
                ret = processSkeleton(lFbxScene);
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("Failed to process skeleton.")
                    break;
                }
            }
            
            if (opts.dstFileType & MeshFileType::kTAni)
            {
                // 导出动画数据
                ret = processAnimation(lFbxScene);
                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("Failed to process animation.")
                    break;
                }
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
            // 统一转成 DirectX 坐标系
            FbxAxisSystem lSceneAxisSystem = lFbxScene->GetGlobalSettings().GetAxisSystem();
            if (lSceneAxisSystem != FbxAxisSystem::eDirectX)
            {
                MCONV_LOG_INFO("Start converting scene to DirectX coordinate system ...")
                FbxAxisSystem::DirectX.ConvertScene(lFbxScene);
                MCONV_LOG_INFO("Completed converting to DirectX coordinate system.")
                mIsOpenGLMetric = true;
            }

            // 统一转成米制单位
            FbxSystemUnit SceneSystemUnit = lFbxScene->GetGlobalSettings().GetSystemUnit();
            if (SceneSystemUnit.GetScaleFactor() != 1.0)
            {
                MCONV_LOG_INFO("Start converting unit to meter ......")
                FbxSystemUnit::m.ConvertScene(lFbxScene);
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
            MCONV_LOG_INFO("Begin compiling shader %s ...", inputPath.c_str());
            
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

    TResult FBXImporter::processStaticMesh(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Starting processing static mesh ...")

            ret = getFbxMeshNode(lFbxScene->GetRootNode());
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXImporter::getFbxMeshNode(FbxNode *lFbxNode)
    {
        TResult ret = T3D_OK;

        do
        {
            if (lFbxNode->GetNodeAttribute() != nullptr)
            {
                switch (lFbxNode->GetNodeAttribute()->GetAttributeType())
                {
                case FbxNodeAttribute::eMesh:
                    ret = processFbxMeshNode(lFbxNode);
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
                ret = getFbxMeshNode(lFbxNode->GetChild(i));
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

                mResources.emplace(material->getName(), material);

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

            ret = createMesh();
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to create mesh.")
                break;
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

                                mResources.emplace(texture->getName(), texture);
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

                            mResources.emplace(texture->getName(), texture);
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

            submesh = SubMesh::create(name, material->getUUID(), PrimitiveType::kTriangleList, indices, is16Bits);

            indices.release();
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
            MeshPtr mesh = T3D_MESH_MGR.createMesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(mSubMeshes));
            if (mesh == nullptr)
            {
                ret = T3D_ERR_RES_INVALID_OBJECT;
                MCONV_LOG_ERROR("Failed to create mesh %s", name.c_str());
                break;
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
            
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult FBXImporter::processSkeleton(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult FBXImporter::processAnimation(FbxScene *lFbxScene)
    {
        TResult ret = T3D_OK;

        do
        {
            
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------
}

