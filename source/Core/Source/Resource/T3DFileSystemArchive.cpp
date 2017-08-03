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

#include "Resource/T3DFileSystemArchive.h"
#include "Misc/T3DEntrance.h"


namespace Tiny3D
{
    const char * const FileSystemArchive::ARCHIVE_TYPE = "FileSystem";

    String FileSystemArchiveCreator::getType() const
    {
        return FileSystemArchive::ARCHIVE_TYPE;
    }

    ArchivePtr FileSystemArchiveCreator::createObject(int32_t argc, ...) const
    {
        va_list params;
        va_start(params, argc);
        String name = va_arg(params, char *);
        va_end(params);
        return FileSystemArchive::create(name);
    }

    FileSystemArchivePtr FileSystemArchive::create(const String &name)
    {
        FileSystemArchivePtr archive = new FileSystemArchive(name);
        archive->release();
        return archive;
    }

    FileSystemArchive::FileSystemArchive(const String &name)
        : Archive(name)
    {
        
    }

    FileSystemArchive::~FileSystemArchive()
    {

    }

    bool FileSystemArchive::load()
    {
        initFileStreamCache();
        return true;
    }

    void FileSystemArchive::unload()
    {
        cleanFileStreamCache();
        Archive::unload();
    }

    ResourcePtr FileSystemArchive::clone() const
    {
        ArchivePtr archive = create(mName);
        return archive;
    }

    String FileSystemArchive::getArchiveType() const
    {
        return ARCHIVE_TYPE;
    }

    String FileSystemArchive::getLocation() const
    {
        return getName();
    }

    bool FileSystemArchive::exists(const String &name) const
    {
        String path = getLocation() + Dir::NATIVE_SEPARATOR + name;
        return Dir::exists(path);
    }

    bool FileSystemArchive::read(const String &name, MemoryDataStream &stream)
    {
        String path = Entrance::getInstance().getAppPath() + getLocation() + Dir::NATIVE_SEPARATOR + name;
        FileDataStream *fs = nullptr;
        bool ret = false;

        if (getFileStreamFromCache(name, fs))
        {
            if (!fs->isOpened())
            {
                ret = fs->open(path.c_str(), FileDataStream::E_MODE_READ_WRITE);
            }
        }

        if (ret)
        {
            size_t size = fs->size();
            uint8_t *data = new uint8_t[size];
            if (fs->read(data, size) != size)
            {
                ret = false;
            }
            else
            {
                stream.setBuffer(data, size, false);
            }
        }

        return ret;
    }

    bool FileSystemArchive::write(const String &name, const MemoryDataStream &stream)
    {
        String path = Entrance::getInstance().getAppPath() + getLocation() + Dir::NATIVE_SEPARATOR + name;
        FileDataStream *fs = nullptr;
        bool ret = false;

        if (getFileStreamFromCache(name, fs))
        {
            if (!fs->isOpened())
            {
                ret = fs->open(path.c_str(), FileDataStream::E_MODE_READ_WRITE);
            }
        }

        if (ret)
        {
            uint8_t *data = nullptr;
            size_t size = 0;
            stream.getBuffer(data, size);
            if (fs->write(data, size) != size)
            {
                ret = false;
            }
        }

        return ret;
    }

    void FileSystemArchive::initFileStreamCache()
    {
        int32_t i = 0;
        for (i = 0; i < MAX_FILE_STREAM_CACHE; ++i)
        {
            FileDataStream *fs = new FileDataStream();
            mFileStreamPool.push_back(fs);
        }
    }

    bool FileSystemArchive::getFileStreamFromCache(const String &name, FileDataStream *&stream)
    {
        bool ret = false;
        auto itr = mFileStreamCache.find(name);

        if (itr != mFileStreamCache.end())
        {
            stream = itr->second;
            ret = true;
        }
        else
        {
            if (mFileStreamCache.size() >= MAX_FILE_STREAM_CACHE)
            {
                // 大于可缓存最大文件流数，需要删掉最老的一个，来存放最新打开的文件流
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
                    ret = true;
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
            ret = true;

        }

        return ret;
    }

    void FileSystemArchive::cleanFileStreamCache()
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
