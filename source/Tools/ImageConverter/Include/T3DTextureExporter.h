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

#ifndef __T3D_TEXTURE_EXPORTER_H__
#define __T3D_TEXTURE_EXPORTER_H__


#include "T3DImageConverterPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief 纹理导出器：负责将 Texture2D 序列化为 .ttex 并（可选）写出配对 .ttex.meta
     * \remarks 采用先写 .tmp 再 Dir::rename 原子替换的策略；带 -a 时严格先 rename .meta.tmp 再 rename .ttex.tmp
     */
    class TextureExporter : public Object
    {
    public:
        static TextureExporterPtr create();

        ~TextureExporter() override = default;

        /**
         * \brief 导出 .ttex 与可选的 .ttex.meta
         * \param [in] texture : 待导出的 Texture2D
         * \param [in] dstTtexPath : 目标 .ttex 路径
         * \param [in] generateMeta : 是否同时生成 .ttex.meta
         * \param [in] sourceHash : 源图 MD5 摘要字符串（写入 MetaTexture::SourceHash）
         * \param [in] settings : 本次生效的导入设置（写入 MetaTexture::ImportSettings）
         * \param [in] existingMeta : 已存在的 MetaTexture（若复用则更新其属性）
         * \param [out] outError : 失败时的错误描述
         * \return 成功返回 T3D_OK
         */
        TResult exportTexture(
            const Texture2DPtr &texture,
            const String &dstTtexPath,
            bool generateMeta,
            const String &sourceHash,
            const TextureImportSettings &settings,
            const MetaTexturePtr &existingMeta = nullptr,
            String *outError = nullptr,
            bool isTextMode = false);

    protected:
        TextureExporter() = default;
    };
}


#endif  /*__T3D_TEXTURE_EXPORTER_H__*/
