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

#ifndef __T3D_BIN_SERIALIZER_H__
#define __T3D_BIN_SERIALIZER_H__


#include "Serializer/T3DSerializer.h"


namespace Tiny3D
{
    /**
     * @brief   Serializer 子类，在二进制流和对象之间序列化和反序列化
     */
    class T3D_ENGINE_API BinSerializer : public Serializer
    {
    public:
        /**
         * @brief   创建对象
         */
        static BinSerializerPtr create();

        /**
         * @brief   析构函数.
         */
        ~BinSerializer() override;

        /**
         * @brief   重写 Serializer::serialize() 接口
         */
        TResult serialize(DataStream &stream, const RTTRObject &obj) override;

        /**
         * @brief   重写 Serializer::deserialize() 接口
         */
        RTTRObject deserialize(DataStream &stream) override;

    protected:
        /**
         * @brief   构造函数
         */
        BinSerializer();
    };
}


#endif  /*__T3D_BIN_SERIALIZER_H__*/
