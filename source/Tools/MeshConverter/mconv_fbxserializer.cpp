/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
 * Copyright (C) 2015-2017  Aaron Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#include "mconv_fbxserializer.h"
#include "mconv_log.h"


namespace mconv
{
    FbxSerializer::FbxSerializer()
        : mFbxManager(nullptr)
        , mFbxScene(nullptr)
    {
        initFBXObjects();
    }

    FbxSerializer::~FbxSerializer()
    {
        destroyFBXObjects();
    }

    bool FbxSerializer::load(const String &path, void *&pData)
    {
        if (mFbxManager == nullptr)
        {
            MCONV_LOG_ERROR("FBX Manager haven't been initialized !");
            return false;
        }

        // Create an importer
        FbxImporter *pFbxImporter = FbxImporter::Create(mFbxManager, "");

        // Initialize the importer by providing a filename.
        bool bImportStatus = pFbxImporter->Initialize(path.c_str(), -1, mFbxManager->GetIOSettings());

        if (!bImportStatus)
        {
            MCONV_LOG_ERROR("Unable to initialize FBX importer !");
            return false;
        }

        MCONV_LOG_INFO("Initialize FBX importer successfully !");

        // Import the scene
        bool bStatus = pFbxImporter->Import(mFbxScene);

        if (!bStatus)
        {
            MCONV_LOG_ERROR("Import FBX file failed !");
            return false;
        }

        MCONV_LOG_INFO("Import FBX file successfully !");

        // Destroy the importer
        pFbxImporter->Destroy();

        // 统一切换成OpenGL的右手坐标系和以米为单位的坐标系
        FbxAxisSystem SceneAxisSystem = mFbxScene->GetGlobalSettings().GetAxisSystem();
        FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
        if (SceneAxisSystem != OurAxisSystem)
        {
            MCONV_LOG_INFO("Start converting axis system to RIGHT HAND ......");
            OurAxisSystem.ConvertScene(mFbxScene);
            MCONV_LOG_INFO("Completed converting axis system !");
        }

        // Convert Unit System to what is used in this example, if needed
        FbxSystemUnit SceneSystemUnit = mFbxScene->GetGlobalSettings().GetSystemUnit();
        if (SceneSystemUnit.GetScaleFactor() != 1.0)
        {
            MCONV_LOG_INFO("Start converting unit to centimeter ......");
            //The unit in this example is centimeter.
            FbxSystemUnit::cm.ConvertScene(mFbxScene);
            MCONV_LOG_INFO("Completed converting unit !");
        }
        
//         FbxAxisSystem::OpenGL.ConvertScene(mFbxScene);
        // 统一以1米为单位
//         FbxSystemUnit::m.ConvertScene(mFbxScene);
        MCONV_LOG_INFO("Start converting face to triangles ......");
        // 不是三角形为面的mesh，统一转换成三角形为面的mesh
        FbxGeometryConverter converter(mFbxManager);
        converter.Triangulate(mFbxScene, true);
        MCONV_LOG_INFO("Completed converting face to triangles !");

        pData = mFbxScene;

        return bStatus;
    }

    bool FbxSerializer::save(const String &path, void *pData)
    {
        mFbxScene = static_cast<FbxScene*>(pData);

        return true;
    }

    bool FbxSerializer::initFBXObjects()
    {
        // 创建FBX SDK Manager
        destroyFBXObjects();

        mFbxManager = FbxManager::Create();
        if (mFbxManager == nullptr)
        {
            MCONV_LOG_ERROR("Unable to create FBX Manager !");
            return false;
        }

        // 创建IOSettings object
        FbxIOSettings *pIOSettings = FbxIOSettings::Create(mFbxManager, IOSROOT);
        mFbxManager->SetIOSettings(pIOSettings);

        // Load plugins from the executable directory (optional)
        FbxString path = FbxGetApplicationDirectory();
        mFbxManager->LoadPluginsDirectory(path.Buffer());

        // Create an FBX scene. This object holds most objects imported/exported from/to files.
        mFbxScene = FbxScene::Create(mFbxManager, "My Scene");
        if (mFbxScene == nullptr)
        {
            MCONV_LOG_ERROR("Unable to create FBX scene !");
            return false;
        }

        return true;
    }

    void FbxSerializer::destroyFBXObjects()
    {
        if (mFbxManager != nullptr)
        {
            mFbxManager->Destroy();
            mFbxManager = nullptr;
        }
    }
}
