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

#ifndef __T3D_IMAGE_CONVERTER_COMMAND_H__
#define __T3D_IMAGE_CONVERTER_COMMAND_H__


#include "T3DImageConverterPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief ImageConverter 参数解析器（命令行与 IPC ReqImport.args 共用）
     */
    class ImageConverterCommand : public Allocator
    {
    public:
        /**
         * \brief 从命令行 argc/argv 解析参数（跳过 argv[0]）
         * \param [in] argc : 参数个数
         * \param [in] argv : 参数数组
         * \param [out] options : 解析结果
         * \param [out] outError : 解析失败时的可读错误描述
         * \return 解析成功返回 true
         */
        bool parse(int32_t argc, char *argv[], ImageConverterOptions &options, String *outError = nullptr) const;

        /**
         * \brief 从字符串数组（不含程序名 argv[0]）解析参数，供命令行与 IPC ReqImport 共用
         * \param [in] args : 参数列表
         * \param [out] options : 解析结果
         * \param [out] outError : 解析失败时的可读错误描述
         * \return 解析成功返回 true
         */
        bool parse(const StringArray &args, ImageConverterOptions &options, String *outError = nullptr) const;

        /// 打印帮助信息到 stdout
        void printHelp() const;
    };
}


#endif  /*__T3D_IMAGE_CONVERTER_COMMAND_H__*/
