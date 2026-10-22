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

#ifndef __T3D_IMAGE_IMPORTER_H__
#define __T3D_IMAGE_IMPORTER_H__


#include "T3DImageConverterPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief 图像导入器：负责图像解码、格式统一、FlipY、MaxSize 缩放、Premultiply Alpha、Mipmap 生成及构建 Texture2D
     */
    class ImageImporter : public Object
    {
    public:
        static ImageImporterPtr create();

        ~ImageImporter() override = default;

        /**
         * \brief 导入单张图片并生成 Texture2D 对象
         * \param [in] srcPath : 源图片路径
         * \param [in] texName : 写入 Texture2D 的名称（通常为不含后缀的文件名）
         * \param [in] settings : 已合并的纹理导入设置
         * \param [in] uuid : 分配给该 Texture2D 的 UUID
         * \param [out] outTexture : 生成的 Texture2D
         * \param [out] outError : 失败时的错误信息
         * \return 成功返回 T3D_OK
         */
        TResult importTexture(
            const String &srcPath,
            const String &texName,
            const TextureImportSettings &settings,
            const UUID &uuid,
            Texture2DPtr &outTexture,
            String *outError = nullptr);

    protected:
        ImageImporter() = default;
    };
}


#endif  /*__T3D_IMAGE_IMPORTER_H__*/
