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

#ifndef __T3D_IMGCONV_ARGS_H__
#define __T3D_IMGCONV_ARGS_H__


#include <T3DPlatform.h>
#include <Meta/T3DMetaTexture.h>
#include <Resource/T3DResource.h>


namespace Tiny3D
{
    /**
     * \brief 强类型 imgconv 参数构造器
     * \remarks 将调用方语义化参数安全组装为与命令行等价的 TArray<String>（不含 argv[0]）
     */
    struct ImgconvArgs
    {
        StringArray args;

        /// 添加单个输入文件路径 (-i <path>)
        ImgconvArgs &input(const String &path);

        /// 添加多个输入文件路径 (-i <p1> <p2> ...)
        ImgconvArgs &inputs(const StringArray &paths);

        /// 指定单张输出路径 (-o <path>)
        ImgconvArgs &output(const String &path);

        /// 指定批量输出目录 (-O <dir>)
        ImgconvArgs &outputDir(const String &dir);

        /// 指定纹理导入类型 (-t default|ui|normal)
        ImgconvArgs &type(TextureImportType t);

        /// 是否写出配对 .meta (-a)
        ImgconvArgs &meta(bool enable = true);

        /// 指定单张 UUID (--uuid <hex32>)
        ImgconvArgs &uuid(const UUID &id);

        /// 指定单张 UUID 字符串 (--uuid <hex32>)
        ImgconvArgs &uuid(const String &hex32);

        /// 是否生成 নতুন UUID (-u new / -u reuse)
        ImgconvArgs &newUuid(bool forceNew = true);

        /// 覆盖预乘 alpha (--premultiply / --no-premultiply)
        ImgconvArgs &premultiply(bool enable);

        /// 覆盖 mipmap 生成 (--mips / --no-mips)
        ImgconvArgs &mips(bool enable);

        /// 覆盖垂直翻转 (--flip-y / --no-flip-y)
        ImgconvArgs &flipY(bool enable);

        /// 覆盖最大边长 (--max-size <n>)
        ImgconvArgs &maxSize(uint32_t size);

        /// 覆盖过滤模式 (--filter point|linear)
        ImgconvArgs &filter(FilterOptions f);

        /// 覆盖寻址模式 (--wrap repeat|clamp|mirror|border)
        ImgconvArgs &wrap(TextureAddressMode m);

        /// 强制重新生成 (-f)
        ImgconvArgs &force(bool enable = true);

        /// 输出详细日志 (-v)
        ImgconvArgs &verbose(bool enable = true);

        /// 导出为文本(JSON)格式 (--text，默认导出 T3DB 二进制)
        ImgconvArgs &textMode(bool enable = true);

        /// 从完整 TextureImportSettings 一键填充对应参数（供 TinyEditor Inspector Apply 使用）
        static ImgconvArgs fromImportSettings(
            const String &srcPath,
            const String &dstPath,
            const TextureImportSettings &settings,
            bool writeMeta = true,
            bool forceRebuild = true);
    };
}


#endif  /*__T3D_IMGCONV_ARGS_H__*/
