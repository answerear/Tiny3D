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


#include "T3DFBXReader.h"
#include "T3DFBXDataStream.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    FBXReaderPtr FBXReader::create()
    {
        FBXReaderPtr reader = new FBXReader();
        reader->release();
        return reader;
    }

    //--------------------------------------------------------------------------

    FBXReader::FBXReader()
        : mFbxManager(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    FBXReader::~FBXReader()
    {

    }

    //--------------------------------------------------------------------------

    TResult FBXReader::initFbxObjects()
    {
        TResult ret = T3D_OK;

        do 
        {
            ret = destroyFbxObjects();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 创建FBX SDK Manager
            mFbxManager = FbxManager::Create();
            if (mFbxManager == nullptr)
            {
                MCONV_LOG_ERROR("Unable to create FBX Manager !");
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            // 创建IOSettings object
            FbxIOSettings *pIOSettings = FbxIOSettings::Create(mFbxManager, IOSROOT);
            mFbxManager->SetIOSettings(pIOSettings);

            // Load plugins from the executable directory (optional)
            FbxString path = FbxGetApplicationDirectory();
            mFbxManager->LoadPluginsDirectory(path.Buffer());

        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::destroyFbxObjects()
    {
        if (mFbxManager != nullptr)
        {
            mFbxManager->Destroy();
            mFbxManager = nullptr;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::importFbxScene(DataStream &stream, FbxScene *pFbxScene)
    {
        TResult ret = T3D_OK;

        do 
        {
            // Create an fbx importer object.
            FbxImporter *importer = FbxImporter::Create(mFbxManager, "");
            if (importer == nullptr)
            {
                MCONV_LOG_ERROR("Unable to create FBX importer object !");
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            // Initialize the importer by providing a filename.
            FbxDataStream fbxStream(stream, mFbxManager, true);
            bool bImportStatus = importer->Initialize(&fbxStream, nullptr, -1, mFbxManager->GetIOSettings());

            if (!bImportStatus)
            {
                MCONV_LOG_ERROR("Unable to initialize FBX importer !");
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            MCONV_LOG_INFO("Initialize FBX importer successfully !");

            // Import the scene
            bool bStatus = importer->Import(pFbxScene);

            if (!bStatus)
            {
                MCONV_LOG_ERROR("Import FBX file failed !");
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            MCONV_LOG_INFO("Import FBX file successfully !");

            // Destroy the importer
            importer->Destroy();
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::setupMetricSystem(FbxScene *pFbxScene)
    {
        TResult ret = T3D_OK;

        do 
        {
            // 统一切换成OpenGL的右手坐标系和以米为单位的坐标系
            FbxAxisSystem SceneAxisSystem = pFbxScene->GetGlobalSettings().GetAxisSystem();
            FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
            if (SceneAxisSystem != OurAxisSystem)
            {
                MCONV_LOG_INFO("Start converting axis system to RIGHT HAND ......");
                OurAxisSystem.ConvertScene(pFbxScene);
                MCONV_LOG_INFO("Completed converting axis system !");
            }

            // Convert Unit System to what is used in this example, if needed
            FbxSystemUnit SceneSystemUnit = pFbxScene->GetGlobalSettings().GetSystemUnit();
            if (SceneSystemUnit.GetScaleFactor() != 1.0)
            {
                MCONV_LOG_INFO("Start converting unit to centimeter ......");
                //The unit in this example is centimeter.
                FbxSystemUnit::cm.ConvertScene(pFbxScene);
                MCONV_LOG_INFO("Completed converting unit !");
            }

            // 不是三角形为面的mesh，统一转换成三角形为面的mesh
            MCONV_LOG_INFO("Start converting face to triangles ......");
            FbxGeometryConverter converter(mFbxManager);
            converter.Triangulate(pFbxScene, true);
            MCONV_LOG_INFO("Completed converting face to triangles !");
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::parse(DataStream &stream, Model *model)
    {
        TResult ret = T3D_OK;

        FbxScene *pFbxScene = nullptr;

        do 
        {
            // Initialize fbx objects.
            ret = initFbxObjects();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // Create an FBX scene. 
            // This object holds most objects imported/exported from/to files.
            pFbxScene = FbxScene::Create(mFbxManager, "My Scene");
            if (pFbxScene == nullptr)
            {
                MCONV_LOG_ERROR("Unable to create FBX scene !");
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            // Import data from fbx file.
            ret = importFbxScene(stream, pFbxScene);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // Setup metric system
            ret = setupMetricSystem(pFbxScene);
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = processFbxScene(pFbxScene, model);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (0);

        destroyFbxObjects();

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxScene(FbxScene *pFbxScene, Model *model)
    {
        TResult ret = T3D_OK;

        do 
        {
            String name = pFbxScene->GetName();

            FbxNode *pFbxRoot = pFbxScene->GetRootNode();

            mTabCount = 0;
            processFbxNode(pFbxRoot);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxNode(FbxNode *pFbxNode)
    {
        TResult ret = T3D_OK;

        do 
        {
            std::stringstream ss;
            for (size_t i = 0; i < mTabCount; ++i)
            {
                ss << "\t";
            }

            MCONV_LOG_INFO("%s%s(Node)", ss.str().c_str(), pFbxNode->GetName());
            MCONV_LOG_INFO("%s{", ss.str().c_str());
            FbxDouble3 pos = pFbxNode->LclTranslation.Get();
            MCONV_LOG_INFO("%s\tposition (%f, %f, %f)", ss.str().c_str(), pos[0], pos[1], pos[2]);
            FbxDouble3 rotation = pFbxNode->LclRotation.Get();
            MCONV_LOG_INFO("%s\trotation (%f, %f, %f)", ss.str().c_str(), rotation[0], rotation[1], rotation[2]);
            FbxDouble3 scale = pFbxNode->LclScaling.Get();
            MCONV_LOG_INFO("%s\tscaling (%f, %f, %f)", ss.str().c_str(), scale[0], scale[1], scale[2]);

            for (size_t i = 0; i < pFbxNode->GetNodeAttributeCount(); ++i)
            {
                FbxNodeAttribute *pFbxAttrib = pFbxNode->GetNodeAttributeByIndex(i);

                FbxNodeAttribute::EType type = pFbxAttrib->GetAttributeType();

                switch (type)
                {
                case FbxNodeAttribute::EType::eNull:
                    MCONV_LOG_INFO("%s\tAttribute : eNull", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eMarker:
                    MCONV_LOG_INFO("%s\tAttribute : eMarker", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eSkeleton:
                    MCONV_LOG_INFO("%s\tAttribute : eSkeleton", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eMesh:
                    MCONV_LOG_INFO("%s\tAttribute : eMesh", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eNurbs:
                    MCONV_LOG_INFO("%s\tAttribute : eNurbs", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::ePatch:
                    MCONV_LOG_INFO("%s\tAttribute : ePatch", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eCamera:
                    MCONV_LOG_INFO("%s\tAttribute : eCamera", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eCameraStereo:
                    MCONV_LOG_INFO("%s\tAttribute : eCameraStereo", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eCameraSwitcher:
                    MCONV_LOG_INFO("%s\tAttribute : eCameraSwitcher", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eLight:
                    MCONV_LOG_INFO("%s\tAttribute : eLight", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eOpticalReference:
                    MCONV_LOG_INFO("%s\tAttribute : eOpticalReference", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eOpticalMarker:
                    MCONV_LOG_INFO("%s\tAttribute : eOpticalMarker", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eNurbsCurve:
                    MCONV_LOG_INFO("%s\tAttribute : eNurbsCurve", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eTrimNurbsSurface:
                    MCONV_LOG_INFO("%s\tAttribute : eTrimNurbsSurface", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eBoundary:
                    MCONV_LOG_INFO("%s\tAttribute : eBoundary", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eNurbsSurface:
                    MCONV_LOG_INFO("%s\tAttribute : eNurbsSurface", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eShape:
                    MCONV_LOG_INFO("%s\tAttribute : eShape", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eLODGroup:
                    MCONV_LOG_INFO("%s\tAttribute : eLODGroup", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eSubDiv:
                    MCONV_LOG_INFO("%s\tAttribute : eSubDiv", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eCachedEffect:
                    MCONV_LOG_INFO("%s\tAttribute : eCachedEffect", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eLine:
                    MCONV_LOG_INFO("%s\tAttribute : eLine", ss.str().c_str());
                    break;
                }
            }

            MCONV_LOG_INFO("%s}", ss.str().c_str());

            mTabCount++;

            for (int32_t i = 0; i < pFbxNode->GetChildCount(); ++i)
            {
                FbxNode *pFbxChild = pFbxNode->GetChild(i);
                processFbxNode(pFbxChild);
            }

            mTabCount--;
        } while (0);

        return ret;
    }
}

