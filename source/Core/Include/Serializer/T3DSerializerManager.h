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

#ifndef __T3D_SERIALIZER_MANAGER_H__
#define __T3D_SERIALIZER_MANAGER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


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
        
    protected:
        SerializerPtr createSerializer();
        
        TResult serializeObject(DataStream &stream, const RTTRObject &obj);

        RTTRObject deserializeObject(DataStream &stream);

    protected:
        FileMode        mFileMode = FileMode::kText;
        SerializerPtr   mSerializer = nullptr;
    };

    #define T3D_SERIALIZER_MGR      (SerializerManager::getInstance())
}


#endif    /*__T3D_SERIALIZER_MANAGER_H__*/
