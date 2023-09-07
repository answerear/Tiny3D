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


#include "Serializer/T3DSerializerManager.h"
#include "Serializer/T3DBinSerializer.h"
#include "Serializer/T3DJsonSerializer.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SerializerManagerPtr SerializerManager::create()
    {
        SerializerManagerPtr mgr = new SerializerManager();
        // mgr->release();
        return mgr;
    }
    
    //--------------------------------------------------------------------------

    SerializerPtr SerializerManager::createSerializer()
    {
        SerializerPtr serializer;

        switch (mFileMode)
        {
        case FileMode::kBinary:
            serializer = BinSerializer::create();
            break;
        case FileMode::kText:
            serializer = JsonSerializer::create();
            break;
        default:
            break;
        }

        return serializer;
    }
    
    //--------------------------------------------------------------------------

    TResult SerializerManager::serializeObject(DataStream &stream, const RTTRObject &obj)
    {
        TResult ret = T3D_OK;
        
        if (mSerializer != nullptr)
        {
            ret = mSerializer->serialize(stream, obj);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    RTTRObject SerializerManager::deserializeObject(DataStream &stream)
    {
        if (mSerializer != nullptr)
        {
            return mSerializer->deserialize(stream);
        }

        return RTTRObject{};
    }

    //--------------------------------------------------------------------------
    
    TResult SerializerManager::deserializeObject(DataStream &stream, RTTRVariant &obj)
    {
        TResult ret = T3D_OK;

        if (mSerializer != nullptr)
        {
            ret = mSerializer->deserialize(stream, obj);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult SerializerManager::serializeObjectWithoutType(DataStream &stream, const RTTRObject &obj)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult SerializerManager::deserializeObjectWithoutType(DataStream &stream, RTTRObject &obj)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------
}
