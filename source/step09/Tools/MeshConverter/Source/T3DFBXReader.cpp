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

            ret = parseFbxScene(pFbxScene, model);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (0);

        destroyFbxObjects();

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::parseFbxScene(FbxScene *pFbxScene, Model *model)
    {
        TResult ret = T3D_OK;

        do 
        {
            String name = pFbxScene->GetName();

            
        } while (0);

        return ret;
    }
}

