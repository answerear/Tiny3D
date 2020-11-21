﻿/*******************************************************************************
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

#ifndef __T3D_JSON_MODEL_WRITER_H__
#define __T3D_JSON_MODEL_WRITER_H__


#include "Serializer/T3DModelWriter.h"
#include "prettywriter.h"


namespace Tiny3D
{
    using JsonStringBuffer = rapidjson::StringBuffer;
    using JsonPrettyWriter = rapidjson::PrettyWriter<JsonStringBuffer>;

    class JsonModelWriter : public ModelWriter
    {
        T3D_DECLARE_CLASS();
        T3D_DISABLE_COPY(JsonModelWriter);

    public:
        static JsonModelWriterPtr create();

        virtual ~JsonModelWriter();

    protected:
        JsonModelWriter();

        virtual TResult serialize(DataStream &stream, Model *model) override;

        TResult serializeHeader(JsonPrettyWriter &writer, Model *model);
        TResult serializeModel(JsonPrettyWriter &writer, Model *model);
        TResult serializeNodes(JsonPrettyWriter &writer, Model *model);
    };
}


#endif  /*__T3D_JSON_MODEL_WRITER_H__*/
