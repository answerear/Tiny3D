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

#ifndef __T3D_SERIALIZER_H__
#define __T3D_SERIALIZER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    #define T3D_FILE_MAGIC              "T3D"

    #define T3D_FILE_VERSION_00000100   0x00000100
    #define T3D_FILE_CURRENT_VERSION    T3D_FILE_VERSION_00000100


    struct FileObject
    {
    public:
        FileObject();

        char        magic[4];   /**< 文件魔数 */
        uint32_t    version;    /**< 文件版本号 */
        Object      *object;    /**< 要存储的对象 */
    };

    /**
     * @class Serializer
     * @brief 提供基于 object 子类对象的序列化和反序列化功能
     */
    class T3D_ENGINE_API Serializer : public Object, public Noncopyable
    {
        T3D_DECLARE_INTERFACE(Serializer);

    public:
        /**
         * @brief   序列化对象到数据流对象中。 该接口把类型一起序列化
         * @param [in,out] stream  : 数据流对象
         * @param [in] object       : 要序列化的对象
         * @return  序列化成功返回 T3D_OK .
         * @see     serializeWithoutType()
         */
        virtual TResult serialize(DataStream &stream, Object *object) = 0;

        /**
         * @brief   从数据流中反序列化生成对象. 
         *          该接口借助序列化时候保存了类型，能自动还原所有类型和数据.
         * @param [in,out] stream  : 数据流对象
         * @return 反序列化成功返回一个对象
         * @remarks 由于序列化的时候保存了类型信息，所以反序列化的时候能类型
         *          和数据还原。
         * @see     serializeWithType()
         * @see     deserializeWithoutType()
         */
        virtual Object* deserialize(DataStream& stream) = 0;
    };
}


#endif  /*__T3D_SERIALIZER_H__*/
