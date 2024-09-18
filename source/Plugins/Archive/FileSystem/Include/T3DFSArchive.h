/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#ifndef __T3D_FS_ARCHIEVE_H__
#define __T3D_FS_ARCHIEVE_H__


#include "T3DFSArchivePrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief 文件系统档案结构类，用于访问文件系统里面的文件
     */
    class FileSystemArchive : public Archive
    {
    public:
        /// 档案类型
        static const char * const ARCHIVE_TYPE;

        /**
         * 创建对象
         */
        static FileSystemArchivePtr create(const String &name, AccessMode mode);

        /**
         * 析构函数
         */
        ~FileSystemArchive() override;

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
         * 重写 Archieve::read() 接口
         */
        TResult read(const UUID &uuid, const ArchiveReadCallback &callback) override;

        /**
         * 重写 Archieve::write() 接口
         */
        TResult write(const UUID &uuid, const ArchiveWriteCallback &callback) override;
        
        /**
         * 构造函数
         */
        FileSystemArchive(const String &name, AccessMode mode);

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


#endif  /*__T3D_FS_ARCHIEVE_H__*/
