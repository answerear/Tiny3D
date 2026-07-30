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


#include "T3DAssetExporter.h"


namespace Tiny3D
{
    class EngineExporter;

    T3D_DECLARE_SMART_PTR(EngineExporter);
    
    class EngineExporter : public AssetExporter
    {
    public:
        static EngineExporterPtr create();

        ~EngineExporter() override;
        
    protected:
        TResult run(const ConverterOptions &opts, const Assets &resources) override;

        TResult exportResource(const ConverterOptions &opts, Resource *resource);

        TResult exportMaterial(const ConverterOptions &opts, Material *material);

        TResult exportTexture(const ConverterOptions &opts, Texture *texture);

        TResult exportMesh(const ConverterOptions &opts, Mesh *mesh);

        TResult exportSkinnedMesh(const ConverterOptions &opts, SkinnedMesh *mesh);

        TResult exportSkeleton(const ConverterOptions &opts, Skeleton *skeleton);

        TResult exportSkeletalAnimation(const ConverterOptions &opts, SkeletalAnimation *anim);

        /**
         * \brief kNew 策略下删除输出文件已有的 meta，让其按资源新的 uuid 重新生成
         * \param [in] opts : 转换选项
         * \param [in] filename : 输出文件名，不含 ".meta" 后缀
         */
        void removeStaleMeta(const ConverterOptions &opts, const String &filename) const;
    };
}

