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


#include "T3DMetaFSArchive.h"
#include "T3DMetaFSMonitor.h"
#include "T3DMetaFSMonitorManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const MetaFSArchive::ARCHIVE_TYPE = "MetaFileSystem";

    //--------------------------------------------------------------------------

    MetaFSArchivePtr MetaFSArchive::create(const String &name, AccessMode mode, MetaFSMonitor *monitor)
    {
        MetaFSArchivePtr archive = new MetaFSArchive(name, mode, monitor);
        if (archive != nullptr && !archive->init())
        {
            archive = nullptr;
        }
        return archive;
    }

    //--------------------------------------------------------------------------

    MetaFSArchive::MetaFSArchive(const String &name, AccessMode mode, MetaFSMonitor *monitor)
        : Archive(name, mode)
        , mMonitor(monitor)
    {
        
    }

    //--------------------------------------------------------------------------

    MetaFSArchive::~MetaFSArchive()
    {
        
    }

    //--------------------------------------------------------------------------

    bool MetaFSArchive::init()
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool MetaFSArchive::canWrite() const
    {
        uint32_t mode = (uint32_t)getAccessMode();
        return archiveCanWrite(mode);
    }

    //--------------------------------------------------------------------------
    
    FileDataStream::EOpenMode MetaFSArchive::getFileOpenMode(AccessMode accMode) const
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

    ArchivePtr MetaFSArchive::clone() const
    {
        ArchivePtr archive = create(getName(), getAccessMode(), mMonitor);
        return archive;
    }

    //--------------------------------------------------------------------------

    String MetaFSArchive::getArchiveType() const
    {
        return ARCHIVE_TYPE;
    }

    //--------------------------------------------------------------------------

    String MetaFSArchive::getPath() const
    {
        return getName();
    }

    //--------------------------------------------------------------------------

    bool MetaFSArchive::exists(const String &name) const
    {
        String path = getPath() + Dir::getNativeSeparator() + name;
        return Dir::exists(path);
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchive::read(const String &name, const ArchiveReadCallback &callback, void *userData)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (callback == nullptr)
            {
                ret = T3D_ERR_INVALID_PARAM;
                MFS_LOG_ERROR("Read callback is nullptr when reading archive [%s] !", name.c_str());
                break;
            }

            if (name.empty())
            {
                ret = T3D_ERR_INVALID_PARAM;
                MFS_LOG_ERROR("Empty filename !");
                break;
            }

            // 打开文件
            FileDataStream fs;
            FileDataStream::EOpenMode mode = getFileOpenMode(getAccessMode());
            String path = getPath() + Dir::getNativeSeparator() + name;
            if (!fs.open(path.c_str(), mode))
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                MFS_LOG_ERROR("Failed to open file [%s] from meta file system !", path.c_str());
                break;
            }

            // 读数据
            ret = callback(fs, name, userData);

            // 关闭文件
            fs.close();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchive::write(const String &name, const ArchiveWriteCallback &callback, void *userData)
    {
        if (!canWrite())
        {
            T3D_LOG_ERROR(LOG_TAG_METAFS, "Access mode is not writable ! [%s]", name.c_str());
            return T3D_ERR_NOT_WRAITABLE_FILE;
        }

        TResult ret = T3D_OK;

        do 
        {
            if (callback == nullptr)
            {
                ret = T3D_ERR_INVALID_PARAM;
                MFS_LOG_ERROR("Write callback is nullptr when writing archive [%s] !", name.c_str());
                break;
            }

            if (name.empty())
            {
                ret = T3D_ERR_INVALID_PARAM;
                MFS_LOG_ERROR("Empty filename !");
                break;
            }

            // 打开文件
            FileDataStream fs;
            FileDataStream::EOpenMode mode = getFileOpenMode(getAccessMode());
            String path = getPath() + Dir::getNativeSeparator() + name;
            if (!fs.open(path.c_str(), mode))
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                MFS_LOG_ERROR("Failed to open file [%s] from meta file system !", path.c_str());
                break;
            }

            // 写数据
            ret = callback(fs, name, userData);

            // 关闭文件
            fs.close();
            
            if (T3D_SUCCEEDED(ret))
            {
                // 写数据成功，则同时生成 meta 文件
                mMonitor->generateMetaFile(name, userData);
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchive::read(const UUID &uuid, const ArchiveReadCallback &callback, void *userData)
    {
        TResult ret = T3D_OK;

        do
        {
            if (callback == nullptr)
            {
                ret = T3D_ERR_INVALID_PARAM;
                MFS_LOG_ERROR("Write callback is nullptr when reading archive [%s] !", uuid.toString().c_str());
                break;
            }

            // 找到对应文件名
            String name;
            MetaFSMonitor *monitor = nullptr;
            bool rval = T3D_MFS_MONITOR_MGR.getPathAndMonitor(uuid, mMonitor, name, monitor);
            if (!rval)
            {
                ret = T3D_ERR_INVALID_PARAM;
                MFS_LOG_ERROR("Invalid uuid : %s !", uuid.toString().c_str());
                break;
            }
            
            String path = monitor->getRootPath() + Dir::getNativeSeparator() + name;
            FileDataStream fs;
            FileDataStream::EOpenMode mode = getFileOpenMode(getAccessMode());
            if (!fs.open(path.c_str(), mode))
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                MFS_LOG_ERROR("Failed to open file [%s] (uuid:%s) for reading from meta file system !", path.c_str(), uuid.toString().c_str());
                break;
            }

            // 读数据
            ret = callback(fs, name, userData);

            // 关闭文件
            fs.close();
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchive::write(const UUID &uuid, const ArchiveWriteCallback &callback, void *userData)
    {
        if (!canWrite())
        {
            T3D_LOG_ERROR(LOG_TAG_METAFS, "Access mode is not writable ! (uuid: %s)", uuid.toString().c_str());
            return T3D_ERR_NOT_WRAITABLE_FILE;
        }
        
        TResult ret = T3D_OK;

        do
        {
            if (callback == nullptr)
            {
                ret = T3D_ERR_INVALID_PARAM;
                MFS_LOG_ERROR("Write callback is nullptr when writing archive (uuid: %s) !", uuid.toString().c_str());
                break;
            }

            // 打开文件
            String name;
            MetaFSMonitor *monitor = nullptr;
            bool rval = T3D_MFS_MONITOR_MGR.getPathAndMonitor(uuid, mMonitor, name, monitor);
            if (!rval)
            {
                ret = T3D_ERR_INVALID_PARAM;
                MFS_LOG_ERROR("Invalid uuid : %s !", uuid.toString().c_str());
                break;
            }
            
            String path = monitor->getRootPath() + Dir::getNativeSeparator() + name;
            FileDataStream fs;
            FileDataStream::EOpenMode mode = getFileOpenMode(getAccessMode());
            if (!fs.open(path.c_str(), mode))
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                MFS_LOG_ERROR("Failed to open file [%s] (uuid:%s) for writing to meta file system !", path.c_str(), uuid.toString().c_str());
                break;
            }

            // 写数据
            ret = callback(fs, name, userData);
            
            // 关闭文件
            fs.close();

            if (T3D_SUCCEEDED(ret))
            {
                // 写数据成功，则同时生成 meta 文件
                monitor->generateMetaFile(name, userData);
            }
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------
}

