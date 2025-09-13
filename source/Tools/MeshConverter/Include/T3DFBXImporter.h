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


#pragma once


#include "T3DAssetImporter.h"


namespace Tiny3D
{
    class FBXImporter;

    T3D_DECLARE_SMART_PTR(FBXImporter);
    
    class FBXImporter : public AssetImporter
    {
    public:
        static FBXImporterPtr create();

        ~FBXImporter() override;

    protected:
        FBXImporter();

        TResult run(const ConverterOptions &opts, Assets &resources) override;

        /**
         * 初始化 FBX SDK 对象
         * @return T3D_OK 成功
         */
        TResult initFbxObjects();

        /**
         * 释放 FBX SDK 对象
         * @return T3D_OK 成功
         */
        TResult destroyFbxObjects();

        TResult importScene(DataStream &stream, FbxScene *lFbxScene);
        
    protected:
        FbxManager *mFbxManager {nullptr};
    };
}

