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


#include "T3DConverterGuidUtil.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    UUID ConverterGuidUtil::readExistingMetaUUID(const String &dir, const String &metaFilename)
    {
        String metaPath = dir + Dir::getNativeSeparator() + metaFilename;

        if (!Dir::exists(metaPath))
        {
            // 首次转换，没有历史 meta 可复用
            return UUID::INVALID;
        }

        FileDataStream fs;
        if (!fs.open(metaPath.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            MCONV_LOG_WARNING("Open existing meta file failed: %s", metaPath.c_str())
            return UUID::INVALID;
        }

        MetaPtr meta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
        fs.close();

        if (meta == nullptr)
        {
            MCONV_LOG_WARNING("Deserialize existing meta file failed: %s", metaPath.c_str())
            return UUID::INVALID;
        }

        return meta->getUUID();
    }

    //--------------------------------------------------------------------------
}
