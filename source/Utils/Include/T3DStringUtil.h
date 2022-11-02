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

#ifndef __T3D_STRING_H__
#define __T3D_STRING_H__


#include "T3DUtilsPrerequisites.h"


namespace Tiny3D
{
    typedef TArray<String>  StringArray;
    typedef TList<String> StringList;

    class T3D_UTILS_API StringUtil
    {
    public:
        /**
         * @brief 合并字符串
         * @param [in] strings : 要合并的字符串数组
         * @param [in] delims : 分隔字符，默认是空格字符
         * @return 返回合并后的一个字符串
         */
        static String merge(const StringArray &strings, const String &delims = " ");

        static StringList split2(const String &str, const String &delims = " ");
     
        /**
         * @brief 分离字符串
         * @param [in] str : 待分离的字符串
         * @param [in] delims : 分隔字符，默认是空格字符
         * @param [in] maxSplits : 分离出来的最大字符串数，默认就是实际分离数量
         * @return 返回分离出来的字符串数组
         */
        static StringArray split(const String &str, const String &delims = " ", 
            size_t maxSplits = 0);

        /**
         * @brief 提取路径和文件名
         * @param [in] fullpath : 完整路径名
         * @param [out] path : 提取出来的路径名
         * @param [out] name : 提取出来的文件名
         */
        static void split(const String &fullpath, String &path, String &name);

        /**
         * @brief 去掉字符串前后的制表键(\t)、换行符(\n)、回车符(\r)
         * @param [in,out] str : 需要trim的字符串，trim是直接对该字符串操作
         * @param [in] left : 是否去掉字符串前面的多余字符，默认去掉
         * @param [in] right : 是否去掉字符串后面多余字符，默认去掉
         */
        static void trim(String &str, bool left = true, bool right = true);

        /**
         * @brief 把字符串转换成小写并返回
         * @param [in,out] str : 要转换的字符串，并且在这里返回
         */
        static void toLowerCase(String &str);

        /**
         * @brief 把字符串转换成大小并返回
         * @param [in,out] str : 要转换的字符串，并且在这里返回
         */
        static void toUpperCase(String& str);

        /**
         * @brief 判断是否包含以 pattern 表示开始的字符串
         * @param [in] str : 要判断的字符串
         * @param [in] pattern : 要判断是否包含开头的模式字符串
         * @param [in] lowerCase : 是否全小写比较
         * @return 符合pattern形式字符串开始返回true。
         */
        static bool startsWith(const String& str, const String& pattern, 
            bool lowerCase = true);

        /**
         * @brief 判断是否包含以 pattern 表示结束的字符串
         * @param [in] str : 要判断的字符串
         * @parma [in] pattern : 要判断是否包含结尾的模式字符串
         * @param [in] lowerCase : 是否全小写比较
         * @return 符合pattern形式字符串结尾返回true。
         */
        static bool endsWith(const String& str, const String& pattern, 
            bool lowerCase = true);

        /**
         * @brief 替换所有指定的字符串为新的字符串
         * @param [in,out] str : 要替换的字符串，并且在这个参数返回替换后结果
         * @param [in] replaceWhat : 被替换的字符串
         * @param [in] replaceWithWhat : 替换进去的新字符串
         */
        static void replaceAll(String &str, const String &replaceWhat, 
            const String &replaceWithWhat);

        /**
         * @brief 简单的字符串匹配查找
         * @param [in] str : 输入字符串
         * @param [in] pattern : 需要匹配的字符串，支持通配符 *
         * @param [in] caseSensitive : 是否区分大小写，默认是区分大小写
         * @return 找到匹配串，则返回true
         */
        static bool match(const String& str, const String& pattern, 
            bool caseSensitive = true);

        /**
         * @brief 读取一行字符串，返回该行字符串的结束位置
         * @param [in] str : 输入多行字符串
         * @param [in] startPos : 要读取的起始位置
         * @return 返回该行字符串的结束位置
         */
        static size_t findLine(const String& str, size_t startPos);
    };
}


#endif    /*__T3D_STRING_H__*/
