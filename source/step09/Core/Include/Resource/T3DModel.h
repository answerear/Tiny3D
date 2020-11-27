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


#ifndef __T3D_MODEL_H__
#define __T3D_MODEL_H__


#include "T3DResource.h"
#include "Kernel/T3DModelData.h"


namespace Tiny3D
{
    namespace Script
    {
        namespace FileFormat
        {
            class FileModel;
        }
    }

    using FileModel = Script::FileFormat::FileModel;

    /**
     * @class   Archive
     * @brief   档案类，用于档案文件的组织管理，隔离文件系统、zip压缩文件的实现细节
     */
    class T3D_ENGINE_API Model : public Resource
    {
        T3D_DECLARE_CLASS();

    public:
        static ModelPtr create(const String &name);

        /**
         * @fn  virtual MeshData::~Archive();
         * @brief   析构函数
         */
        virtual ~Model();

        /**
         * @fn  virtual Type MeshData::getType() const override;
         * @brief   获取资源类型，重写基类 Resource::getType() 接口
         * @return  The type.
         */
        virtual Type getType() const override;

        FileModel *getModelData() const 
        { 
            return mModelData;
        }

    protected:
        /**
         * @fn  MeshData::MeshData(const String &name);
         * @brief   构造函数
         * @param [in]  name    : 资源名称，一般使用档案路径作为名称.
         */
        Model(const String &name);

        virtual TResult load() override;

        virtual TResult unload() override;

        virtual ResourcePtr clone() const override;

    protected:
        FileModel   *mModelData;
    };
}


#endif  /*__T3D_MODEL_H__*/
