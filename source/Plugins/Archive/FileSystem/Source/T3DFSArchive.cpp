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


#include "T3DFSArchive.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const FileSystemArchive::ARCHIVE_TYPE = "FileSystem";

    //--------------------------------------------------------------------------

    FileSystemArchivePtr FileSystemArchive::create(const String &name, AccessMode mode)
    {
        FileSystemArchivePtr archive = new FileSystemArchive(name, mode);
        // archive->release();
        return archive;
    }

    //--------------------------------------------------------------------------

    FileSystemArchive::FileSystemArchive(const String &name, AccessMode mode)
        : Archive(name, mode)
    {
        initFileStreamCache();
    }

    //--------------------------------------------------------------------------

    FileSystemArchive::~FileSystemArchive()
    {
        clearFileStreamCache();
    }

    //--------------------------------------------------------------------------

    bool FileSystemArchive::canWrite() const
    {
        return ((uint32_t)getAccessMode() & (uint32_t)AccessMode::kRead) == (uint32_t)AccessMode::kRead;
    }

    //--------------------------------------------------------------------------
    
    FileDataStream::EOpenMode FileSystemArchive::getFileOpenMode(AccessMode accMode) const
    {
        uint32_t outMode = FileDataStream::E_MODE_NOT_OPEN;

        uint32_t inMode = (uint32_t)accMode;

        if (inMode == (uint32_t)AccessMode::kReadOnly || inMode == (uint32_t)AccessMode::kReadTxtOnly)
        {
            outMode |= FileDataStream::E_MODE_READ_ONLY;
        }
        else
        {
            if (inMode & (uint32_t)AccessMode::kAppend)
            {
                outMode |= FileDataStream::E_MODE_APPEND;
            }
            else if (inMode & (uint32_t)AccessMode::kTruncate)
            {
                outMode |= FileDataStream::E_MODE_TRUNCATE;
            }

            outMode |= (FileDataStream::E_MODE_READ_ONLY | FileDataStream::E_MODE_WRITE_ONLY);
        }

        if (inMode & (uint32_t)AccessMode::kText)
        {
            outMode |= FileDataStream::E_MODE_TEXT;
        }

        return (FileDataStream::EOpenMode)outMode;
    }
    
    //--------------------------------------------------------------------------

    ArchivePtr FileSystemArchive::clone() const
    {
        ArchivePtr archive = create(getName(), getAccessMode());
        return archive;
    }

    //--------------------------------------------------------------------------

    String FileSystemArchive::getArchiveType() const
    {
        return ARCHIVE_TYPE;
    }

    //--------------------------------------------------------------------------

    String FileSystemArchive::getLocation() const
    {
        return getName();
    }

    //--------------------------------------------------------------------------

    bool FileSystemArchive::exists(const String &name) const
    {
        String path = getLocation() + Dir::getNativeSeparator() + name;
        return Dir::exists(path);
    }

    //--------------------------------------------------------------------------

    TResult FileSystemArchive::read(const String &name, MemoryDataStream &stream)
    {
        String path = getLocation() + Dir::getNativeSeparator() + name;
        FileDataStream *fs = nullptr;
        TResult ret = T3D_OK;

        do 
        {
            ret = getFileStreamFromCache(name, fs);
            if (T3D_FAILED(ret))
            {
                break;
            }

            if (!fs->isOpened())
            {
                // 文件没有打开，先打开文件
                FileDataStream::EOpenMode mode = getFileOpenMode(getAccessMode());
                if (!fs->open(path.c_str(), mode))
                {
                    ret = T3D_ERR_FILE_NOT_EXIST;
                    T3D_LOG_ERROR(LOG_TAG_FILESYSTEM, "Open file [%s] from file system failed !", name.c_str());
                    break;
                }
            }

            // 跳到文件头
            fs->seek(0, false);

            // 读数据
            size_t size = fs->size();
            uint8_t *data = new uint8_t[size];
            if (fs->read(data, size) != size)
            {
                ret = T3D_ERR_FILE_DATA_MISSING;
                T3D_LOG_ERROR(LOG_TAG_FILESYSTEM, "Read file [%s] from file system failed !", name.c_str());
                break;
            }

            stream.setBuffer(data, size, false);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FileSystemArchive::write(const String &name, const MemoryDataStream &stream)
    {
        if (!canWrite())
        {
            T3D_LOG_ERROR(LOG_TAG_FILESYSTEM, "Access mode is not writable ! [%s]", name.c_str());
            return T3D_ERR_NOT_WRAITABLE_FILE;
        }

        String path = getLocation() + Dir::getNativeSeparator() + name;
        FileDataStream *fs = nullptr;
        TResult ret = T3D_OK;

        do 
        {
            ret = getFileStreamFromCache(name, fs);
            if (T3D_FAILED(ret))
            {
                break;
            }

            if (!fs->isOpened())
            {
                // 文件没有打开，先打开文件
                FileDataStream::EOpenMode mode = getFileOpenMode(getAccessMode());
                if (!fs->open(path.c_str(), mode))
                {
                    ret = T3D_ERR_FILE_NOT_EXIST;
                    T3D_LOG_ERROR(LOG_TAG_FILESYSTEM, "Open file [%s] from file system failed !", name.c_str());
                    break;
                }
            }

            // 跳到文件头
            fs->seek(0, false);

            // 写数据
            uint8_t *data = nullptr;
            size_t size = 0;
            stream.getBuffer(data, size);
            if (fs->write(data, size) != size)
            {
                ret = T3D_ERR_FILE_DATA_MISSING;
                T3D_LOG_ERROR(LOG_TAG_FILESYSTEM, "Write file [%s] from file system failed !", name.c_str());
                break;
            }

            fs->flush();
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FileSystemArchive::getFileStreamFromCache(const String &name, FileDataStream *&stream)
    {
        TResult ret = T3D_OK;

        do 
        {
            auto itr = mFileStreamCache.find(name);

            if (itr != mFileStreamCache.end())
            {
                stream = itr->second;
            }
            else
            {
                if (mFileStreamCache.size() >= kMaxFileStreamCache)
                {
                    // 大于可缓存最大文件流数，需要删掉最老的一个，
                    // 来存放最新打开的文件流
                    auto itr = mFileIndexCache.begin();

                    auto i = mFileStreamCache.find(itr->second);

                    if (i != mFileStreamCache.end())
                    {
                        FileDataStream *fs = i->second;

                        if (fs->isOpened())
                        {
                            fs->close();
                        }

                        mFileStreamCache.erase(i);
                        mFileIndexCache.erase(itr);
                        stream = fs;
                    }
                }
                else
                {
                    // 没有大于缓存，从空闲缓存链表获取空闲对象
                    FileDataStream *fs = mFileStreamPool.front();
                    mFileStreamPool.pop_front();
                    stream = fs;
                }

                uint64_t timestamp = DateTime::currentMSecsSinceEpoch();
                mFileIndexCache.insert(FileIndexCacheValue(timestamp, name));
                mFileStreamCache.insert(FileStreamCacheValue(name, stream));
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    void FileSystemArchive::initFileStreamCache()
    {
        size_t i = 0;
        for (i = 0; i < kMaxFileStreamCache; ++i)
        {
            FileDataStream *fs = new FileDataStream();
            mFileStreamPool.push_back(fs);
        }
    }

    //--------------------------------------------------------------------------

    void FileSystemArchive::clearFileStreamCache()
    {
        auto itr = mFileStreamPool.begin();

        while (itr != mFileStreamPool.end())
        {
            FileDataStream *fs = *itr;
            T3D_SAFE_DELETE(fs);
            ++itr;
        }

        mFileStreamPool.clear();

        auto i = mFileStreamCache.begin();

        while (i != mFileStreamCache.end())
        {
            FileDataStream *fs = i->second;
            T3D_SAFE_DELETE(fs);
            ++i;
        }

        mFileStreamCache.clear();
    }
}

