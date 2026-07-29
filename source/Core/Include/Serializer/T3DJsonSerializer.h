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

#ifndef __T3D_JSON_SERIALIZER_H__
#define __T3D_JSON_SERIALIZER_H__


#include "Serializer/T3DSerializer.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API JsonSerializer : public Serializer
    {
    public:
        static JsonSerializerPtr create();

        TResult serialize(DataStream &stream, const RTTRObject &obj) override;

        TResult deserialize(DataStream& stream, RTTRVariant& obj) override;

        template<typename T>
        TResult serializeObject(DataStream &stream, const T &obj)
        {
            return serialize(stream, obj);
        }

        template<typename T>
        T *deserializeObject(DataStream &stream)
        {
            RTTRVariant var;
            if (T3D_FAILED(deserialize(stream, var)))
            {
                return nullptr;
            }

            // 所有权说明同 SerializerManager::deserialize<T>：instance 只做指针校正，
            // 对象所有权随裸指针交给调用方，且 var 必须活到取指针之后。
            T *obj = RTTRObject(var).try_convert<T>();

            if (obj == nullptr)
            {
                discardUnclaimed(var, rttr::type::get<T>());
            }

            return obj;
        }

        template<typename T>
        TResult deserializeObject(DataStream& stream, T& obj)
        {
            RTTRVariant var;
            TResult ret = deserialize(stream, var);
            obj = var.get_value<T>();
            return ret;
        }
    };
}


#endif    /*__T3D_JSON_SERIALIZER_H__*/
