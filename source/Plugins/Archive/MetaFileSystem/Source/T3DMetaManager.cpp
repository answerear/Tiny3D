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


#include "T3DMetaManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char *UUIDManager::MAPPINGS_FILE_NAME = "project.idx";
    
    //--------------------------------------------------------------------------

    TResult UUIDManager::startup(const String &path)
    {
        mProjectPath = path;

        

        T3D_TIMER_MGR.startTimer(5*1000, true, [](ID timerID, uint32_t dt)
        {
            
        });
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult UUIDManager::shutdown()
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult UUIDManager::buildMappings()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UUIDManager::readMappings()
    {
        TResult ret = T3D_OK;

        do
        {
            String filename = mProjectPath + Dir::getNativeSeparator() + MAPPINGS_FILE_NAME;
            FileDataStream fs;

            // 打開映射文件
            if (!fs.open(filename.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
            {
                T3D_LOG_ERROR(LOG_TAG_METAFS, "Failed to open file %s !", filename.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            // 从文件读取生成映射对象
            ret = T3D_SERIALIZER_MGR.deserialize(fs, mMappings);
            fs.close();

            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_METAFS, "Failed to deserialize object !");
                ret = T3D_ERR_DESERIALIZE_OBJECT;
                break;
            }

            // 从读取回来的映射文件建立 UUID => Path 的映射关系
            for (const auto &item : mMappings.paths)
            {
                mMappings.uuids.emplace(item.second, item.first);
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UUIDManager::writeMappings()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void UUIDManager::refresh(const UUID &uuid, const String &path)
    {
        if (path.empty())
            return;

        String oldPath;
        
        auto itr = mMappings.uuids.find(uuid);
        if (itr == mMappings.uuids.end())
        {
            // 没有保存过 UUID，可能 UUID 更新了
            mMappings.uuids.emplace(uuid, path);
        }
        else
        {
            if (itr->second != path)
            {
                // 路径更新了
                oldPath = itr->second;
                itr->second = path;
                mIsDirty = true;
            }
        }

        if (!oldPath.empty())
        {
            auto it = mMappings.paths.find(oldPath);
            if (it != mMappings.paths.end())
            {
                // 路径映射里面有老的路径，先删除
                mMappings.paths.erase(oldPath);
                mIsDirty = true;
            }
        }

        auto it = mMappings.paths.find(path);
        if (it != mMappings.paths.end())
        {
            if (uuid != it->second)
            {
                // 路径映射里面有路径，并且 UUID 更新了
                it->second = uuid;
                mIsDirty = true;
            }
        }
        else
        {
            // 路径映射里面没有路径，直接更新 UUID
            mMappings.paths.emplace(path, uuid);
            mIsDirty = true;
        }
    }

    //--------------------------------------------------------------------------

    const String &UUIDManager::getPath(const UUID &uuid) const
    {
        static String empty;

        auto itr = mMappings.uuids.find(uuid);
        if (itr != mMappings.uuids.end())
        {
            return itr->second;
        }

        return empty;
    }

    //--------------------------------------------------------------------------

    const UUID &UUIDManager::getUUID(const String &path) const
    {
        auto itr = mMappings.paths.find(path);
        if (itr != mMappings.paths.end())
        {
            return itr->second;
        }
        
        return UUID::INVALID;
    }
    //--------------------------------------------------------------------------
}

