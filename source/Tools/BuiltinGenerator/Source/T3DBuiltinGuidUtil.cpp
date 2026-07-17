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


#include "T3DBuiltinGuidUtil.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    UUID BuiltinGuidUtil::readExistingMetaUUID(const String &dir, const String &metaFilename)
    {
        String metaPath = dir + Dir::getNativeSeparator() + metaFilename;

        if (!Dir::exists(metaPath))
        {
            // 首次生成，没有历史 meta 可复用
            return UUID::INVALID;
        }

        FileDataStream fs;
        if (!fs.open(metaPath.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            BGEN_LOG_WARNING("Open existing meta file failed: %s", metaPath.c_str());
            return UUID::INVALID;
        }

        MetaPtr meta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
        fs.close();

        if (meta == nullptr)
        {
            BGEN_LOG_WARNING("Deserialize existing meta file failed: %s", metaPath.c_str());
            return UUID::INVALID;
        }

        return meta->getUUID();
    }

    //--------------------------------------------------------------------------

    bool BuiltinGuidUtil::readExistingShaderLabMeta(const String &dir, const String &metaFilename,
        UUID &outLabUUID, UUID &outShaderUUID)
    {
        outLabUUID = UUID::INVALID;
        outShaderUUID = UUID::INVALID;

        String metaPath = dir + Dir::getNativeSeparator() + metaFilename;

        if (!Dir::exists(metaPath))
        {
            // 首次生成，没有历史 meta 可复用
            return false;
        }

        FileDataStream fs;
        if (!fs.open(metaPath.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            BGEN_LOG_WARNING("Open existing shader lab meta file failed: %s", metaPath.c_str());
            return false;
        }

        MetaPtr meta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
        fs.close();

        if (meta == nullptr || meta->getType() != Meta::Type::kShaderLab)
        {
            BGEN_LOG_WARNING("Deserialize existing shader lab meta file failed: %s", metaPath.c_str());
            return false;
        }

        MetaShaderLab *metaShaderLab = static_cast<MetaShaderLab *>(meta.get());
        outLabUUID = metaShaderLab->getUUID();
        outShaderUUID = metaShaderLab->getShaderUUID();

        return true;
    }

    //--------------------------------------------------------------------------
}
