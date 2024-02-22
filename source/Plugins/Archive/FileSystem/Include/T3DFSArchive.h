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
        static const char * const ARCHIVE_TYPE; /**< 档案类型 */

        /**
         * @brief 创建对象
         */
        static FileSystemArchivePtr create(const String &name, AccessMode mode);

        /**
         * @brief 析构函数
         */
        virtual ~FileSystemArchive() override;

        /**
         * @brief 获取档案类型
         */
        virtual String getArchiveType() const override;

    protected:
        /**
         * @brief 重写 Resource::clone() 接口
         */
        virtual ArchivePtr clone() const override;

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
        FileSystemArchive(const String &name, AccessMode mode);

        /**
         * @brief 从文件流对象缓存中获取文件流对象
         */
        TResult getFileStreamFromCache(const String &name, FileDataStream *&stream);

        /**
         * @brief 创建文件流缓存
         */
        void initFileStreamCache();

        /**
         * @brief 清除文件流对象缓存
         */
        void clearFileStreamCache();

        bool canWrite() const;

        FileDataStream::EOpenMode getFileOpenMode(AccessMode accMode) const;

    protected:
        enum
        {
            kMaxFileStreamCache = 10, /**< 文件流最大的缓存 */
        };

        typedef TMap<String, FileDataStream*>       FileStreamCache;
        typedef FileStreamCache::iterator           FileStreamCacheItr;
        typedef FileStreamCache::const_iterator     FileStreamCacheConstItr;
        typedef FileStreamCache::value_type         FileStreamCacheValue;

        typedef TMap<uint64_t, String>              FileIndexCache;
        typedef FileIndexCache::iterator            FileIndexCacheItr;
        typedef FileIndexCache::const_iterator      FileIndexCacheConstItr;
        typedef FileIndexCache::value_type          FileIndexCacheValue;

        typedef TList<FileDataStream*>              FileStreamPool;
        typedef FileStreamPool::iterator            FileStreamPoolItr;
        typedef FileStreamPool::const_iterator      FileStreamPoolConstItr;

        FileStreamPool  mFileStreamPool;    /**< 空闲文件流对象池 */
        FileIndexCache  mFileIndexCache;    /**< 文件流索引缓存，其中第一个是存在时间最长的对象 */
        FileStreamCache mFileStreamCache;   /**< 使用中的文件流缓存 */
    };
}


#endif  /*__T3D_FS_ARCHIEVE_H__*/
