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


#include "T3DMeshConverterApp.h"
#include "T3DConverterOptions.h"
#include "T3DConverterCommand.h"
#include "T3DFBXImporter.h"
#include "T3DEngineImporter.h"
#include "T3DEngineExporter.h"


Tiny3D::MeshConverterApp theApp;


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MeshConverterApp::MeshConverterApp()
    {

    }

    //--------------------------------------------------------------------------

    MeshConverterApp::~MeshConverterApp()
    {

    }

    //--------------------------------------------------------------------------

    TResult MeshConverterApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
    {
        TResult ret = T3D_OK;

        do 
        {
            ConverterOptions opts;
            ConverterCommand cmd;

            // 解析命令行参数
            if (!cmd.parse(argc, argv, opts))
            {
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            // 创建导入器
            AssetImporterPtr importer = nullptr;

            switch (opts.srcFileType)
            {
            case MeshFileType::kFbx:
                importer = FBXImporter::create();
                break;
            case MeshFileType::kOgre:
                break;
            case MeshFileType::kTMesh:
            case MeshFileType::kTSkin:
            case MeshFileType::kTSkel:
            case MeshFileType::kTAni:
            case MeshFileType::kTiny3D:
                break;
            case MeshFileType::kAuto:
            default:
                ret = false;
                break;
            }

            if (!ret)
            {
                break;
            }
            
            Assets assets;

            // 导入资源
            ret = importer->run(opts, assets);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 创建导出器
            AssetExporterPtr exporter = nullptr;
            
            switch (opts.dstFileType)
            {
            case MeshFileType::kFbx:
            case MeshFileType::kOgre:
                break;
            case MeshFileType::kTMesh:
            case MeshFileType::kTSkin:
            case MeshFileType::kTSkel:
            case MeshFileType::kTAni:
            case MeshFileType::kTiny3D:
                exporter = EngineExporter::create();
                break;
            case MeshFileType::kAuto:
            default:
                ret = false;
                break;
            }

            // 导出资源
            ret = exporter->run(opts, assets);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void MeshConverterApp::applicationDidEnterBackground()
    {

    }
    
    //--------------------------------------------------------------------------

    void MeshConverterApp::applicationWillEnterForeground()
    {
        
    }

    //--------------------------------------------------------------------------

    void MeshConverterApp::applicationWillTerminate()
    {
        
    }
    
    //--------------------------------------------------------------------------

    void MeshConverterApp::applicationLowMemory()
    {
        // Handle low memory situation here
    }

    //--------------------------------------------------------------------------
}

