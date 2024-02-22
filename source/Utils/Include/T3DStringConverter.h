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
