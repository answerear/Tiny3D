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


#include "T3DTextureExporter.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TextureExporterPtr TextureExporter::create()
    {
        return T3D_NEW TextureExporter();
    }

    //--------------------------------------------------------------------------

    TResult TextureExporter::exportTexture(
        const Texture2DPtr &texture,
        const String &dstTtexPath,
        bool generateMeta,
        const String &sourceHash,
        const TextureImportSettings &settings,
        const MetaTexturePtr &existingMeta,
        String *outError,
        bool isTextMode)
    {
        auto reportErr = [&](const String &msg)
        {
            if (outError != nullptr)
            {
                *outError = msg;
            }
            IMGCONV_LOG_ERROR("%s", msg.c_str());
        };

        if (texture == nullptr)
        {
            reportErr("Cannot export null Texture2D.");
            return T3D_ERR_INVALID_POINTER;
        }

        // 确保输出目录存在
        String outDir;
        String outName;
        StringUtil::split(dstTtexPath, outDir, outName);
        if (!outDir.empty() && !Dir::exists(outDir))
        {
            if (!Dir::makeDir(outDir))
            {
                reportErr("Failed to create output directory: " + outDir);
                return T3D_ERR_FAIL;
            }
        }

        const String tmpTtexPath = dstTtexPath + ".tmp";
        const String dstMetaPath = dstTtexPath + ".meta";
        const String tmpMetaPath = dstMetaPath + ".tmp";

        // 1. 先写出 .ttex.tmp（默认二进制 T3DB 格式；开启 isTextMode 时为 JSON 文本格式）
        {
            FileDataStream fs;
            if (!fs.open(tmpTtexPath.c_str(), FileDataStream::E_MODE_WRITE_ONLY))
            {
                reportErr("Failed to open temporary ttex file for writing: " + tmpTtexPath);
                return T3D_ERR_FAIL;
            }
            T3D_SERIALIZER_MGR.setFileMode(isTextMode
                ? SerializerManager::FileMode::kText
                : SerializerManager::FileMode::kBinary);
            TResult ret = T3D_SERIALIZER_MGR.serialize(fs, texture.get());
            fs.close();

            if (T3D_FAILED(ret))
            {
                Dir::remove(tmpTtexPath);
                reportErr("Failed to serialize .ttex temporary file: " + tmpTtexPath);
                return ret;
            }
        }

        // 2. 若启用 -a，先写出 .ttex.meta.tmp（JSON 文本格式）
        if (generateMeta)
        {
            MetaTexturePtr meta = existingMeta;
            if (meta == nullptr)
            {
                meta = MetaTexture::create(texture->getUUID());
            }
            else
            {
                meta->setUUID(texture->getUUID());
            }

            meta->setSourceHash(sourceHash);
            meta->setImportSettings(settings);

            FileDataStream fs;
            if (!fs.open(tmpMetaPath.c_str(), FileDataStream::E_MODE_WRITE_ONLY))
            {
                Dir::remove(tmpTtexPath);
                reportErr("Failed to open temporary meta file for writing: " + tmpMetaPath);
                return T3D_ERR_FAIL;
            }
            T3D_SERIALIZER_MGR.setFileMode(SerializerManager::FileMode::kText);
            TResult ret = T3D_SERIALIZER_MGR.serialize(fs, meta.get());
            fs.close();

            if (T3D_FAILED(ret))
            {
                Dir::remove(tmpTtexPath);
                Dir::remove(tmpMetaPath);
                reportErr("Failed to serialize .ttex.meta temporary file: " + tmpMetaPath);
                return ret;
            }

            // 严格遵守顺序：先 rename .ttex.meta.tmp -> .ttex.meta，再 rename .ttex.tmp -> .ttex
            // 避免 Editor 文件监控在看到 .ttex 落地瞬间找不到配对 .meta 而自动创建空 GUID 的新 .meta
            if (!Dir::rename(tmpMetaPath, dstMetaPath, true))
            {
                Dir::remove(tmpTtexPath);
                Dir::remove(tmpMetaPath);
                reportErr("Failed to rename temporary meta file to: " + dstMetaPath);
                return T3D_ERR_FAIL;
            }
        }

        // 3. rename .ttex.tmp -> .ttex
        if (!Dir::rename(tmpTtexPath, dstTtexPath, true))
        {
            Dir::remove(tmpTtexPath);
            reportErr("Failed to rename temporary ttex file to: " + dstTtexPath);
            return T3D_ERR_FAIL;
        }

        return T3D_OK;
    }
}
