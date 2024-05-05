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
        
    }

    //--------------------------------------------------------------------------

    FileSystemArchive::~FileSystemArchive()
    {
        
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

    String FileSystemArchive::getPath() const
    {
        return getName();
    }

    //--------------------------------------------------------------------------

    bool FileSystemArchive::exists(const String &name) const
    {
        String path = getPath() + Dir::getNativeSeparator() + name;
        return Dir::exists(path);
    }

    //--------------------------------------------------------------------------

    TResult FileSystemArchive::read(const String &name, const ArchiveReadCallback &callback)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (callback == nullptr)
            {
                ret = T3D_ERR_INVALID_PARAM;
                T3D_LOG_ERROR(LOG_TAG_FILESYSTEM, "Read callback is nullptr when reading archive [%s] !", name.c_str());
                break;
            }

            // 打开文件
            FileDataStream fs;
            FileDataStream::EOpenMode mode = getFileOpenMode(getAccessMode());
            String path = getPath() + Dir::getNativeSeparator() + name;
            if (!fs.open(path.c_str(), mode))
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                T3D_LOG_ERROR(LOG_TAG_FILESYSTEM, "Open file [%s] from file system failed !", path.c_str());
                break;
            }

            // 读数据
            ret = callback(fs);

            // 关闭文件
            fs.close();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FileSystemArchive::write(const String &name, const ArchiveWriteCallback &callback)
    {
        if (!canWrite())
        {
            T3D_LOG_ERROR(LOG_TAG_FILESYSTEM, "Access mode is not writable ! [%s]", name.c_str());
            return T3D_ERR_NOT_WRAITABLE_FILE;
        }

        TResult ret = T3D_OK;

        do 
        {
            if (callback == nullptr)
            {
                ret = T3D_ERR_INVALID_PARAM;
                T3D_LOG_ERROR(LOG_TAG_FILESYSTEM, "Write callback is nullptr when writing archive [%s] !", name.c_str());
                break;
            }

            // 打开文件
            FileDataStream fs;
            FileDataStream::EOpenMode mode = getFileOpenMode(getAccessMode());
            String path = getPath() + Dir::getNativeSeparator() + name;
            if (!fs.open(path.c_str(), mode))
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                T3D_LOG_ERROR(LOG_TAG_FILESYSTEM, "Open file [%s] from file system failed !", path.c_str());
                break;
            }

            // 写数据
            ret = callback(fs);

            // 关闭文件
            fs.close();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
}

