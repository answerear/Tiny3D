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


#include "Serializer/T3DSerializerManager.h"
#include "Serializer/T3DBinSerializer.h"
#include "Serializer/T3DJsonSerializer.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SerializerManagerPtr SerializerManager::create()
    {
        SerializerManagerPtr mgr = T3D_NEW SerializerManager();
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
