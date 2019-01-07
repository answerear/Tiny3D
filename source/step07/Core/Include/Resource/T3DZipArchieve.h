/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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


#ifndef __T3D_ZIP_ARCHIVE_H__
#define __T3D_ZIP_ARCHIVE_H__


#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveCreator.h"


namespace Tiny3D
{
    /**
     * @brief zip 压缩档案结构类，用于访问 zip 压缩包里面的文件
     */
    class T3D_ENGINE_API ZipArchive : public Archive
    {
    public:
        static const char * const ARCHIVE_TYPE; /**< 档案类型 */

        /**
         * @brief 创建对象
         */
        static ZipArchivePtr create(const String &name);

        /**
         * @brief 析构函数
         */
        virtual ~ZipArchive();

        /**
         * @brief 获取档案类型
         */
        virtual String getArchiveType() const override;

    protected:
        /**
         * @brief 重写 Resource::load() 接口
         */
        virtual TResult load() override;

        /**
         * @brief 重写 Resource::unload() 接口
         */
        virtual TResult unload() override;

        /**
         * @brief 重写 Resource::clone() 接口
         */
        virtual ResourcePtr clone() const override;

        /**
         * @brief 重写 Archieve::getLocation() 接口
         */
        virtual String getLocation() const override;

        /**
         * @brief 重写 Archieve::exists() 接口
         */
        virtual bool exists(const String &name) const override;

        /**
         * @brief 重写 Archieve::read() 接口
         */
        virtual TResult read(const String &name, MemoryDataStream &stream) override;

        /**
         * @brief 重写 Archieve::write() 接口
         */
        virtual TResult write(const String &name, const MemoryDataStream &stream) override;

        /**
         * @brief 构造函数
         */
        ZipArchive(const String &name);
    };


    /**
     * @brief zip 压缩文件系统档案结构构建器类，用于构建 zip 档案结构对象
     */
    class T3D_ENGINE_API ZipArchiveCreator : public ArchiveCreator
    {
    public:
        /**
         * @brief 重写 ArchieveCreator::getType() 接口
         */
        virtual String getType() const override;

        /**
         * @brief 重写 ArchieveCreator::createObject() 接口
         */
        virtual ArchivePtr createObject(int32_t argc, ...) const override;
    };
}


#endif  /*__T3D_ZIP_ARCHIVE_H__*/
