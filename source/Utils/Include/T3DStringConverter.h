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

#ifndef __T3D_STRING_CONVERTER_H__
#define __T3D_STRING_CONVERTER_H__


#include "T3DUtilsPrerequisites.h"


namespace Tiny3D
{
    class T3D_UTILS_API StringConverter
    {
    public:
        /**
         * @brief 把对应类型数值转换成字符串
         * @param [in] val : 数值
         * @param [in] precision : 精度
         * @param [in] width : 最大宽度
         * @param [in] fill : 不足宽度时填充的字符
         * @param [in] flags : 标记，请参考std::ios::fmtflags
         * @return 返回字符串对象
         */
        template<typename T>
        static String toString(T val, uint16_t precision = 6,
            uint16_t width = 0, char fill = ' ',
            std::ios::fmtflags flags = std::ios::fmtflags(0));

        template<typename T>
        static T parseValue(const String &val, T defaultValue = T{});

        /**
         * @brief 是否数字
         * @param [in] val : 数字字符串
         * @return 是数字返回true
         */
        static bool isNumber(const String& val);
    };
}

#include "T3DStringConverter.inl"

#endif  /*__T3D_STRING_CONVERTER_H__*/
