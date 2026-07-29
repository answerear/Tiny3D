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


#include "Serializer/T3DJsonSerializer.h"
#include "Serializer/T3DJsonArchive.h"
#include "Serializer/T3DRttrArchive.h"
#include "IO/T3DDataStream.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    //
    // JSON 文本格式定义、键名常量与读写后端见 T3DJsonArchive.h。
    // RTTR 遍历语义（读与写）已统一由 RttrArchive 承载，本文件只做装配。
    //
    //--------------------------------------------------------------------------

    JsonSerializerPtr JsonSerializer::create()
    {
        return T3D_NEW JsonSerializer();
    }

    //--------------------------------------------------------------------------

    TResult JsonSerializer::serialize(DataStream &stream, const RTTRObject &obj)
    {
        JsonArchiveWriter writer(stream);
        return RttrArchive::write(writer, obj);
    }

    //--------------------------------------------------------------------------

    TResult JsonSerializer::deserialize(DataStream &stream, RTTRVariant &obj)
    {
        JsonArchiveReader reader(stream);

        TResult ret = RttrArchive::read(reader, obj);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        if (!obj.is_valid())
        {
            return T3D_ERR_DESERIALIZE_OBJECT;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
