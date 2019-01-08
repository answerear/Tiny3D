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


#include "Resource/T3DZipArchieve.h"
#include "Support/minizip/unzip.h"
#include "Kernel/T3DEngine.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const ZipArchive::ARCHIVE_TYPE = "Zip";

    //--------------------------------------------------------------------------

    String ZipArchiveCreator::getType() const
    {
        return ZipArchive::ARCHIVE_TYPE;
    }

    ArchivePtr ZipArchiveCreator::createObject(int32_t argc, ...) const
    {
        va_list params;
        va_start(params, argc);
        String name = va_arg(params, char *);
        va_end(params);
        return ZipArchive::create(name);
    }

    //--------------------------------------------------------------------------

    ZipArchivePtr ZipArchive::create(const String &name)
    {
        ZipArchivePtr archive = new ZipArchive(name);
        archive->release();
        return archive;
    }

    //--------------------------------------------------------------------------

    ZipArchive::ZipArchive(const String &name)
        : Archive(name)
        , mZipFile(nullptr)
    {

    }

    ZipArchive::~ZipArchive()
    {

    }

    //--------------------------------------------------------------------------

    TResult ZipArchive::load()
    {
        TResult ret = T3D_ERR_OK;

        do 
        {
            String path = Engine::getInstance().getAppPath() + getLocation();

            // 打开 zip 文件
            mZipFile = unzOpen64(path.c_str());
            if (mZipFile == nullptr)
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                T3D_LOG_ERROR("Open zip file [%s] failed !", path.c_str());
                break;
            }
        } while (0);

        return ret;
    }

    TResult ZipArchive::unload()
    {
        if (mZipFile != nullptr)
        {
            unzClose(mZipFile);
            mZipFile = nullptr;
        }
        
        return T3D_ERR_OK;
    }

    ResourcePtr ZipArchive::clone() const
    {
        ArchivePtr archive = create(mName);
        return archive;
    }

    //--------------------------------------------------------------------------

    String ZipArchive::getArchiveType() const
    {
        return ARCHIVE_TYPE;
    }

    String ZipArchive::getLocation() const
    {
        return mName;
    }

    bool ZipArchive::exists(const String &name) const
    {
        bool ret = false;

        do 
        {
            if (mZipFile == nullptr)
            {
                ret = false;
                T3D_LOG_ERROR("Open zip file [%s] failed !", mName.c_str());
                break;
            }

            // 跳到指定文件，如果文件不存在，则失败了
            int zret = 0;
            zret = unzLocateFile(mZipFile, name.c_str(), 0);
            if (zret != UNZ_OK)
            {
                ret = false;
                T3D_LOG_ERROR("Locate file [%s] in zip file [%s] failed ! \
                    Error : %d", name.c_str(), mName.c_str(), zret);
                break;
            }

            ret = true;
        } while (0);

        return ret;
    }

    TResult ZipArchive::read(const String &name, MemoryDataStream &stream)
    {
        TResult ret = T3D_ERR_OK;

        do 
        {
            if (mZipFile == nullptr)
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                T3D_LOG_ERROR("Open zip file [%s] failed !", mName.c_str());
                break;
            }

            int zret = 0;

            // 跳到指定文件
            zret = unzLocateFile(mZipFile, name.c_str(), 0);
            if (zret != UNZ_OK)
            {
                ret = T3D_ERR_ZIP_FILE_LOCATE_FILE;
                T3D_LOG_ERROR("Locate file [%s] in zip file [%s] failed ! \
                    Error : %d", name.c_str(), mName.c_str(), zret);
                break;
            }

            // 获取压缩前的文件信息
            unz_file_info64 fileInfo;
            char filename[256];
            zret = unzGetCurrentFileInfo64(mZipFile, &fileInfo, filename,
                sizeof(filename), NULL, 0, NULL, 0);
            if (zret != UNZ_OK)
            {
                ret = T3D_ERR_ZIP_FILE_GET_FILE_INFO;
                T3D_LOG_ERROR("Get file [%s] info in zip file [%s] failed ! \
                    Error : %d", name.c_str(), mName.c_str(), zret);
                break;
            }

            // 读取文件内容
            uint64_t contentSize = fileInfo.compressed_size;
            uchar_t *content = new uchar_t[contentSize];

            zret = unzReadCurrentFile(mZipFile, content, contentSize);
            if (zret != UNZ_OK)
            {
                ret = T3D_ERR_ZIP_FILE_READ_DATA;
                T3D_LOG_ERROR("Get file [%s] data in zip file [%s] failed ! \
                    Error : %d", name.c_str(), mName.c_str(), zret);
                break;
            }

            stream.setBuffer(content, contentSize);

        } while (0);

        return ret;
    }

    TResult ZipArchive::write(const String &name, const MemoryDataStream &stream)
    {
        T3D_LOG_ERROR("Could not support append any file into current zip file [%s] !",
            mName.c_str());
        return T3D_ERR_ZIP_FILE_NOT_SUPPORT;
    }
}

