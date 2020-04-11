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


#include "Serializer/T3DJSONModelReader.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_1(JSONModelReader, ModelReader);

    //--------------------------------------------------------------------------
    
    JSONModelReaderPtr JSONModelReader::create()
    {
        JSONModelReaderPtr reader = new JSONModelReader();
        reader->release();
        return reader;
    }

    //--------------------------------------------------------------------------
    
    JSONModelReader::JSONModelReader()
    {

    }

    //--------------------------------------------------------------------------
    
    JSONModelReader::~JSONModelReader()
    {

    }

    //--------------------------------------------------------------------------
    
    TResult JSONModelReader::parse(DataStream &stream, Model *model)
    {
        TResult ret = T3D_OK;

        rapidjson::Reader reader;

        uint8_t *data = nullptr;
        size_t size = stream.read(data);
        rapidjson::MemoryStream is((const char *)data, size);
        rapidjson::ParseResult result = reader.Parse(is, *this);
        if (result.Code() != rapidjson::ParseErrorCode::kParseErrorNone)
        {
            ret = T3D_ERR_FAIL;
            T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Parse model file failed from json !");
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::Null()
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::Bool(bool b)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::Int(int32_t i)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::Uint(uint32_t u)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::Int64(int64_t i)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::Uint64(uint64_t u)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::Double(float64_t f)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::String(const char *str, JsonSizeType length, bool copy)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::StartObject()
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::Key(const char *str, JsonSizeType length, bool copy)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::EndObject(JsonSizeType memberCount)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::StartArray()
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool JSONModelReader::EndArray(JsonSizeType elementCount)
    {
        return true;
    }
}
