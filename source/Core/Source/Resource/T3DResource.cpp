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


#include "Resource/T3DResource.h"
#include "Kernel/T3DArchive.h"
#include "Serializer/T3DSerializerManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Resource::Resource(const String &strName)
        : mMeta(nullptr)
        , mState(State::kUnloaded)
        , mName(strName)
        , mCompletedCB(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    Resource::~Resource()
    {
        T3D_ASSERT(isUnloaded(), "Resource has not unloaded !");
    }

    //--------------------------------------------------------------------------

    TResult Resource::load(ArchivePtr archive)
    {
        TResult ret = T3D_OK;

        do
        {
            mState = State::kLoading;
            
            MemoryDataStream stream;
            TResult ret = archive->read(getName(), stream);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read reousrce data [%s] from archive failed !",
                    getName().c_str());
                break;
            }

            uint8_t *buffer = nullptr;
            size_t bufSize = 0;
            stream.getBuffer(buffer, bufSize);
            
            // 读取 meta 信息
            uint32_t metaSize = 0;
            if (bufSize < sizeof(metaSize))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid data size for resource [%s] !", getName().c_str());
                break;
            }
            // meta 数据大小
            memcpy(&metaSize, buffer, sizeof(metaSize));
            if (bufSize < metaSize)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid meta size for resource [%s] !", getName().c_str());
                break;
            }
            // meta 数据
            uint8_t *metaData = buffer + sizeof(metaSize);
            MemoryDataStream ms(metaData, metaSize, false);
            // 反序列化出 meta 对象
            mMeta = T3D_SERIALIZER_MGR.deserialize<Meta>(ms);
            if (mMeta == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Deserialize meta for resource [%s] failed !", getName().c_str());
                break;
            }

            // 读取资源数据
            if (bufSize < metaSize + sizeof(uint32_t) * 2)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid data size for resource [%s] !", getName().c_str());
                break;
            }
            
            uint8_t *data = buffer + metaSize;
            uint32_t dataSize = 0;
            // 资源数据大小
            memcpy(&dataSize, data, sizeof(dataSize));
            
            // 资源数据
            data += sizeof(dataSize);
            MemoryDataStream ds(data, dataSize, false);
            ret = loadData(ds);
            
            if (T3D_FAILED(ret))
            {
                mMeta = nullptr;
                break;
            }

            mState = State::kLoaded;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Resource::unload()
    {
        mState = State::kUnloaded;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
