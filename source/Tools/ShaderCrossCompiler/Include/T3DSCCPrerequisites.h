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

    #define ARCHIVE_TYPE_FS     "FileSystem"
    
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
