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

        do
        {
            // 初始化 FBX 对象
            ret = initFbxObjects();
            if (T3D_FAILED(ret))
            {
                break;
            }

            FbxScene *lFbxScene = FbxScene::Create(mFbxManager, "My Scene");
            if (lFbxScene == nullptr)
            {
                MCONV_LOG_ERROR("Failed to create FbxScene.")
                ret = T3D_ERR_FBX_SCENE_CREATE_FAILED;
                break;
            }

            // 导入 FBX 文件
        } while (false);

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
            // 创建 FBX IO
            FbxImporter *lImporter = FbxImporter::Create(mFbxManager, "");
            FBXDataStream lStream(stream, mFbxManager, true);
            if (!lImporter->Initialize(&lStream, mFbxManager->GetIOSettings()))
            {
                MCONV_LOG_ERROR("Failed to initialize FBX importer.")
                ret = T3D_ERR_FBX_IMPORTER_INIT_FAILED;
                break;
            }

            // 导入 FBX 场景
            if (!lImporter->Import(lFbxScene))
            {
                MCONV_LOG_ERROR("Failed to import FBX scene.")
                ret = T3D_ERR_FBX_SCENE_IMPORT_FAILED;
                break;
            }

            // 销毁 FBX IO
            lImporter->Destroy();
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
}

