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

#ifndef __T3D_JSON_MODEL_READER_H__
#define __T3D_JSON_MODEL_READER_H__


#include "Serializer/T3DModelReader.h"
#include "reader.h"


namespace Tiny3D
{
    class JSONModelReader 
        : public ModelReader
        , public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, JSONModelReader>
    {
        T3D_DECLARE_CLASS();
        T3D_DISABLE_COPY(JSONModelReader);

    public:
        static JSONModelReaderPtr create();

        virtual ~JSONModelReader();

        bool Null();
        bool Bool(bool b);
        bool Int(int32_t i);
        bool Uint(uint32_t u);
        bool Int64(int64_t i);
        bool Uint64(uint64_t u);
        bool Double(float64_t f);
        bool String(const char *str, rapidjson::SizeType length, bool copy);

        bool StartObject();
        bool Key(const char *str, rapidjson::SizeType length, bool copy);
        bool EndObject(rapidjson::SizeType memberCount);
        bool StartArray();
        bool EndArray(rapidjson::SizeType elementCount);

    protected:
        JSONModelReader();

        virtual TResult parse(DataStream &stream, Model *model) override;
    };
}


#endif  /*__T3D_JSON_MODEL_READER_H__*/
