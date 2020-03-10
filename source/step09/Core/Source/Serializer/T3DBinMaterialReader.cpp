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


#include "Serializer/T3DBinMaterialReader.h"
#include "T3DErrorDef.h"
#include "Kernel/T3DCommon.h"
#include "protobuf/MaterialScriptObject.pb.h"


namespace Tiny3D
{
    using namespace Script;

    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_1(BinMaterialReader, MaterialReader);

    //--------------------------------------------------------------------------

    BinMaterialReaderPtr BinMaterialReader::create()
    {
        BinMaterialReaderPtr reader = new BinMaterialReader();
        reader->release();
        return reader;
    }

    //--------------------------------------------------------------------------

    BinMaterialReader::BinMaterialReader()
    {

    }

    //--------------------------------------------------------------------------

    BinMaterialReader::~BinMaterialReader()
    {

    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parse(DataStream &stream, Material *material)
    {
        TResult ret = T3D_OK;
        uint8_t *data = nullptr;

        do 
        {
            size_t size = stream.size();
            data = new uint8_t[size];

            size = stream.read(data);

            if (size == 0)
            {
                ret = T3D_ERR_INVALID_SIZE;
                break;
            }

            ret = parse(data, size, material);
        } while (0);

        T3D_SAFE_DELETE_ARRAY(data);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parse(
        const uint8_t *data, size_t size, Material *material)
    {
        TResult ret = T3D_OK;

        do 
        {
            // 读取文件头
            T3DFileHeader header;
            memcpy(&header, data, sizeof(header));

            if (stricmp(header.magic, T3D_MAGIC) != 0)
            {
                // 非法的文件类型
                ret = T3D_ERR_RES_INVALID_FILETYPE;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid file type !");
                break;
            }

            if (header.subtype != FileSubType::E_FST_SCRIPT)
            {
                // 非法材质脚本类型
                ret = T3D_ERR_RES_INVALID_FILETYPE;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid file subtype !");
                break;
            }

            // 从 proto buffer 解析出来 pb 对象
            MaterialSystem::Material object;

            if (!object.ParseFromArray(data, size))
            {
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid file content !");
                break;
            }


        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseMaterial(
        MaterialSystem::Material *src, Material *dst)
    {
        TResult ret = T3D_OK;

        do 
        {
        } while (0);

        return ret;
    }
}

