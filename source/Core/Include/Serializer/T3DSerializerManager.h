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

#ifndef __T3D_SERIALIZER_MANAGER_H__
#define __T3D_SERIALIZER_MANAGER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Serializer/T3DSerializer.h"


namespace Tiny3D
{
    /**
     * \brief 序列化管理类，负责序列化对象和反序列化对象
     */
    class T3D_ENGINE_API SerializerManager
        : public Object
        , public Singleton<SerializerManager>
    {
    public:
        enum class FileMode : uint32_t
        {
            kBinary = 0,
            kText,
            kMax
        };

        static SerializerManagerPtr create();

        FileMode getFileMode() const { return mFileMode; }
        
        void setFileMode(FileMode mode)
        {
            mFileMode = mode;
            mSerializer = createSerializer();
        }

        /**
         * @brief 序列化对象到数据流对象中
         * @param [in,out] stream   : 数据流对象
         * @param [in] obj          : 要序列化的可反射对象
         * @return 序列化成功返回 T3D_OK .
         * @remarks 序列化的时候把数据和类型一起序列化保存起来，方便反序列化生成对象
         */
        template<typename T>
        TResult serialize(DataStream &stream, const T &obj)
        {
            return serializeObject(stream, obj);
        }

        /**
         * @brief 从数据流中反序列化生成对象. 
         *          该接口借助序列化时候保存了类型，能自动还原所有类型和数据.
         * @param [in,out] stream   : 数据流对象
         * @param [in,out] obj      : 返回的可序列化对象
         * @return 反序列化成功返回 T3D_OK .
         * @remarks 由于序列化的时候保存了类型信息，所以反序列化的时候能把类型
         *          和数据还原。
         */
        template<typename T>
        T *deserialize(DataStream &stream)
        {
            RTTRObject obj = deserializeObject(stream);
            return obj.try_convert<T>();
        }

        template<typename T>
        TResult deserialize(DataStream& stream, T& obj)
        {
            RTTRVariant var;
            TResult ret = deserializeObject(stream, var);
            obj = var.get_value<T>();
            return ret;
        }

        // template<typename T>
        // TResult serializeWithoutType(DataStream &stream, const T &obj)
        // {
        //     return serializeWithoutType(stream, obj);
        // }
        //
        // template<typename T>
        // TResult deserializeWithoutType(DataStream &stream, T &obj)
        // {
        //     return deserializeWithoutType(stream, obj);
        // }

        TResult serializeObject(DataStream &stream, const RTTRObject &obj);

        RTTRObject deserializeObject(DataStream &stream);

        TResult deserializeObject(DataStream &stream, RTTRVariant &obj);
        
    protected:
        SerializerPtr createSerializer();
        
        TResult serializeObjectWithoutType(DataStream &stream, const RTTRObject &obj);

        TResult deserializeObjectWithoutType(DataStream &stream, RTTRObject &obj);
        
    protected:
        FileMode        mFileMode = FileMode::kText;
        SerializerPtr   mSerializer;
    };

    #define T3D_SERIALIZER_MGR      (SerializerManager::getInstance())
}


#endif    /*__T3D_SERIALIZER_MANAGER_H__*/
