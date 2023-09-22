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

#ifndef __T3D_SCC_PREREQUISITES_H__
#define __T3D_SCC_PREREQUISITES_H__


#include <Tiny3D.h>
#include <memory>
#include <unordered_map>
#include <ShaderConductor.hpp>


namespace Tiny3D
{
    #define LOG_TAG     "ShaderCrossCompiler"
    #define CURRENT_VERSION_STR "0.0.1"

    #define SCC_LOG_ERROR(fmt, ...)   \
        T3D_LOG_ERROR(LOG_TAG, fmt, ##__VA_ARGS__);  \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");

    #define SCC_LOG_WARNING(fmt, ...) \
        T3D_LOG_WARNING(LOG_TAG, fmt, ##__VA_ARGS__);    \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");

    #define SCC_LOG_INFO(fmt, ...)    \
        T3D_LOG_INFO(LOG_TAG, fmt, ##__VA_ARGS__);   \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");

    #define SCC_LOG_DEBUG(fmt, ...)   \
        T3D_LOG_DEBUG(LOG_TAG, fmt, ##__VA_ARGS__);  \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");

    struct Args
    {
        enum Options
        {
            OPT_PRINT_VERSION = (1 << 0),       /// 输出版本号
            OPT_PRINT_HELP = (1 << 1),          /// 输出帮助信息
            OPT_ENABLE_DEBUG_INFO = (1 << 2),   /// 转换到目标平台着色器语言时是否输出调试信息
            OPT_BINARY_FILE = (1 << 3),         /// 是否输出二进制文件，还是 json 文件
        };

        bool hasOptions(uint32_t flags) const
        {
            return (flags & options) == flags;
        }

        uint32_t    options {0};        /// 选项
        uint32_t    optimizeLevel {3};  /// 优化级别，0 to 3
        String      include {};        /// 包含的头文件目录路径
        String      target {};         /// 输出目标平台
        String      baseName {};       /// 输出文件的基本名称
    };


}


#endif  /*__T3D_SCC_PREREQUISITES_H__*/
