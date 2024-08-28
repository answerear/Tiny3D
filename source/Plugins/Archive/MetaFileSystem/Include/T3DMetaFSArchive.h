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


#include "T3DMetaFSArchivePrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief Meta 文件系统档案结构类，用于访问 Meta 文件系统里面的文件
     */
    class MetaFSArchive : public Archive
    {
    public:
        /// 档案类型
        static const char * const ARCHIVE_TYPE;

        /**
         * 创建对象
         */
        static MetaFSArchivePtr create(const String &name, AccessMode mode);

        /**
         * 析构函数
         */
        ~MetaFSArchive() override;

        /**
         * 重写 Archive::getArchiveType() 接口
         */
        String getArchiveType() const override;

    protected:
        /**
         * 重写 Archive::clone() 接口
         */
        ArchivePtr clone() const override;

        /**
         * 重写 Archieve::getPath() 接口
         */
        String getPath() const override;

        /**
         * 重写 Archieve::exists() 接口
         */
        bool exists(const String &name) const override;

        /**
         * 重写 Archieve::read() 接口
         */
        TResult read(const String &name, const ArchiveReadCallback &callback) override;

        /**
         * 重写 Archieve::write() 接口
         */
        TResult write(const String &name, const ArchiveWriteCallback &callback) override;

        /**
         * 构造函数
         */
        MetaFSArchive(const String &name, AccessMode mode);

        /**
         * 是否可写
         */
        bool canWrite() const;

        /**
         * 根据档案访问模式获取文件流打开模式 
         */
        FileDataStream::EOpenMode getFileOpenMode(AccessMode accMode) const;
    };
}