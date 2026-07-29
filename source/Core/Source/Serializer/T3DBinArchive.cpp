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

#include "Serializer/T3DBinArchive.h"
#include "Object/T3DBuffer.h"
#include "UUID/T3DUUID.h"
#include "T3DErrorDef.h"

#include <string>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TResult BinArchiveIO::readHeader(DataStream &s, uint16_t &version)
    {
        uint8_t magic[4] = { 0, 0, 0, 0 };
        if (s.read(magic, sizeof(magic)) != sizeof(magic))
        {
            T3D_LOG_ERROR(LOG_TAG_SERIALIZE, "Failed to read binary header !");
            return T3D_ERR_PARSE_STREAM;
        }

        if (magic[0] != kMagic0 || magic[1] != kMagic1
            || magic[2] != kMagic2 || magic[3] != kMagic3)
        {
            T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                "Invalid binary magic (expect T3DB) !");
            return T3D_ERR_MISSING_MAGIC;
        }

        version = readPod<uint16_t>(s);
        (void)readPod<uint16_t>(s);   // flags
        (void)readPod<uint32_t>(s);   // reserved

        if (version == 0 || version > kVersion)
        {
            T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                "Unsupported binary version [%u] !", (uint32_t)version);
            return T3D_ERR_MISSING_VERSION;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::beginDocument()
    {
        BinArchiveIO::writeHeader(mStream);
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::writeNull(const rttr::string_view &typeName)
    {
        (void)typeName;
        BinArchiveIO::writeKind(mStream, ArchiveKind::Null);
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::writeBool(const rttr::string_view &typeName,
        bool value)
    {
        (void)typeName;
        BinArchiveIO::writeKind(mStream, ArchiveKind::Bool);
        BinArchiveIO::writePod<uint8_t>(mStream, value ? 1 : 0);
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::writeInt(const rttr::string_view &typeName,
        ArchiveKind kind, int64_t value)
    {
        (void)typeName;
        BinArchiveIO::writeKind(mStream, kind);

        switch (kind)
        {
        case ArchiveKind::I8:
            BinArchiveIO::writePod<int8_t>(mStream,
                static_cast<int8_t>(value));
            break;
        case ArchiveKind::I16:
            BinArchiveIO::writePod<int16_t>(mStream,
                static_cast<int16_t>(value));
            break;
        case ArchiveKind::I32:
            BinArchiveIO::writePod<int32_t>(mStream,
                static_cast<int32_t>(value));
            break;
        default:
            BinArchiveIO::writePod<int64_t>(mStream, value);
            break;
        }
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::writeUint(const rttr::string_view &typeName,
        ArchiveKind kind, uint64_t value)
    {
        (void)typeName;
        BinArchiveIO::writeKind(mStream, kind);

        switch (kind)
        {
        case ArchiveKind::U8:
            BinArchiveIO::writePod<uint8_t>(mStream,
                static_cast<uint8_t>(value));
            break;
        case ArchiveKind::U16:
            BinArchiveIO::writePod<uint16_t>(mStream,
                static_cast<uint16_t>(value));
            break;
        case ArchiveKind::U32:
            BinArchiveIO::writePod<uint32_t>(mStream,
                static_cast<uint32_t>(value));
            break;
        default:
            BinArchiveIO::writePod<uint64_t>(mStream, value);
            break;
        }
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::writeReal(const rttr::string_view &typeName,
        ArchiveKind kind, double value)
    {
        (void)typeName;
        BinArchiveIO::writeKind(mStream, kind);

        if (kind == ArchiveKind::F32)
        {
            BinArchiveIO::writePod<float>(mStream, static_cast<float>(value));
        }
        else
        {
            BinArchiveIO::writePod<double>(mStream, value);
        }
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::writeString(const rttr::string_view &typeName,
        const String &value)
    {
        (void)typeName;
        BinArchiveIO::writeKind(mStream, ArchiveKind::String);
        BinArchiveIO::writeString(mStream, value);
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::writeEnum(const rttr::string_view &typeName,
        const String &name, bool nameValid, uint64_t value, bool valueValid)
    {
        (void)typeName;
        (void)valueValid;
        BinArchiveIO::writeKind(mStream, ArchiveKind::Enum);
        // 名称优先；不可用时退化为数值的十进制字符串表示
        BinArchiveIO::writeString(mStream,
            nameValid ? name : String(std::to_string(value)));
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::writeBuffer(const uint8_t *data, size_t size)
    {
        BinArchiveIO::writeKind(mStream, ArchiveKind::Buffer);
        BinArchiveIO::writeUVarint(mStream, static_cast<uint64_t>(size));
        BinArchiveIO::writeBytes(mStream, data, size);
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::writeUuid(const UUID &uuid)
    {
        BinArchiveIO::writeKind(mStream, ArchiveKind::Uuid);
        BinArchiveIO::writeBytes(mStream, uuid.bytes, UUID::kDefaultLength);
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::beginObject(const rttr::string_view &typeName,
        size_t propertyCount)
    {
        BinArchiveIO::writeKind(mStream, ArchiveKind::Object);
        BinArchiveIO::writeString(mStream, typeName);
        BinArchiveIO::writeUVarint(mStream,
            static_cast<uint64_t>(propertyCount));
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::writePropertyName(const rttr::string_view &name)
    {
        BinArchiveIO::writeString(mStream, name);
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::beginArray(const rttr::string_view &typeName,
        size_t count)
    {
        (void)typeName;
        BinArchiveIO::writeKind(mStream, ArchiveKind::Seq);
        BinArchiveIO::writeUVarint(mStream, static_cast<uint64_t>(count));
    }

    //--------------------------------------------------------------------------

    void BinArchiveWriter::beginMap(const rttr::string_view &typeName,
        bool keyOnly, size_t count)
    {
        (void)typeName;
        BinArchiveIO::writeKind(mStream, ArchiveKind::Map);
        BinArchiveIO::writeU8(mStream, keyOnly ? 1 : 0);
        BinArchiveIO::writeUVarint(mStream, static_cast<uint64_t>(count));
    }

    //--------------------------------------------------------------------------

    TResult BinArchiveReader::readHeader()
    {
        return BinArchiveIO::readHeader(mStream, mVersion);
    }

    //--------------------------------------------------------------------------

    ArchiveKind BinArchiveReader::readKind()
    {
        return BinArchiveIO::readKind(mStream);
    }

    //--------------------------------------------------------------------------

    void BinArchiveReader::skipValue()
    {
        skipPayload(BinArchiveIO::readKind(mStream));
    }

    //--------------------------------------------------------------------------

    void BinArchiveReader::skipPayload(ArchiveKind kind)
    {
        // 全部显式限定：成员 readString 会遮蔽同名的 BinArchiveIO 原语。
        using BinArchiveIO::readU8;
        using BinArchiveIO::readUVarint;
        using BinArchiveIO::skipBytes;

        switch (kind)
        {
        case ArchiveKind::Null:
            break;
        case ArchiveKind::Bool:
        case ArchiveKind::I8:
        case ArchiveKind::U8:
            skipBytes(mStream, 1);
            break;
        case ArchiveKind::I16:
        case ArchiveKind::U16:
            skipBytes(mStream, 2);
            break;
        case ArchiveKind::I32:
        case ArchiveKind::U32:
        case ArchiveKind::F32:
            skipBytes(mStream, 4);
            break;
        case ArchiveKind::I64:
        case ArchiveKind::U64:
        case ArchiveKind::F64:
            skipBytes(mStream, 8);
            break;
        case ArchiveKind::String:
        case ArchiveKind::Enum:
        case ArchiveKind::Buffer:
            skipBytes(mStream, readUVarint(mStream));
            break;
        case ArchiveKind::Uuid:
            skipBytes(mStream, UUID::kDefaultLength);
            break;
        case ArchiveKind::Object:
            {
                BinArchiveIO::readString(mStream);      // typename
                uint64_t count = readUVarint(mStream);
                for (uint64_t i = 0; i < count; ++i)
                {
                    BinArchiveIO::readString(mStream);  // property name
                    skipValue();
                }
            }
            break;
        case ArchiveKind::Seq:
            {
                uint64_t count = readUVarint(mStream);
                for (uint64_t i = 0; i < count; ++i)
                {
                    skipValue();
                }
            }
            break;
        case ArchiveKind::Map:
            {
                uint8_t keyOnly = readU8(mStream);
                uint64_t count = readUVarint(mStream);
                for (uint64_t i = 0; i < count; ++i)
                {
                    skipValue();
                    if (keyOnly == 0)
                    {
                        skipValue();
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    bool BinArchiveReader::readBool()
    {
        return BinArchiveIO::readPod<uint8_t>(mStream) != 0;
    }

    //--------------------------------------------------------------------------

    int64_t BinArchiveReader::readInt(ArchiveKind kind)
    {
        switch (kind)
        {
        case ArchiveKind::I8:
            return BinArchiveIO::readPod<int8_t>(mStream);
        case ArchiveKind::I16:
            return BinArchiveIO::readPod<int16_t>(mStream);
        case ArchiveKind::I32:
            return BinArchiveIO::readPod<int32_t>(mStream);
        default:
            return BinArchiveIO::readPod<int64_t>(mStream);
        }
    }

    //--------------------------------------------------------------------------

    uint64_t BinArchiveReader::readUint(ArchiveKind kind)
    {
        switch (kind)
        {
        case ArchiveKind::U8:
            return BinArchiveIO::readPod<uint8_t>(mStream);
        case ArchiveKind::U16:
            return BinArchiveIO::readPod<uint16_t>(mStream);
        case ArchiveKind::U32:
            return BinArchiveIO::readPod<uint32_t>(mStream);
        default:
            return BinArchiveIO::readPod<uint64_t>(mStream);
        }
    }

    //--------------------------------------------------------------------------

    double BinArchiveReader::readReal(ArchiveKind kind)
    {
        if (kind == ArchiveKind::F32)
        {
            return BinArchiveIO::readPod<float>(mStream);
        }
        return BinArchiveIO::readPod<double>(mStream);
    }

    //--------------------------------------------------------------------------

    String BinArchiveReader::readString()
    {
        return String(BinArchiveIO::readString(mStream));
    }

    //--------------------------------------------------------------------------

    void BinArchiveReader::readBuffer(Buffer &out)
    {
        uint64_t n = BinArchiveIO::readUVarint(mStream);

        T3D_POD_SAFE_DELETE_ARRAY(out.Data);
        out.Data = (n > 0)
            ? T3D_POD_NEW_ARRAY(uint8_t, static_cast<size_t>(n)) : nullptr;
        out.DataSize = static_cast<size_t>(n);

        if (n > 0)
        {
            mStream.read(out.Data, static_cast<size_t>(n));
        }
    }

    //--------------------------------------------------------------------------

    void BinArchiveReader::readUuid(UUID &out)
    {
        mStream.read(out.bytes, UUID::kDefaultLength);
    }

    //--------------------------------------------------------------------------

    void BinArchiveReader::beginObject(String &typeName)
    {
        typeName = String(BinArchiveIO::readString(mStream));
        mRemainProps.push_back(BinArchiveIO::readUVarint(mStream));
    }

    //--------------------------------------------------------------------------

    bool BinArchiveReader::nextProperty(String &name)
    {
        if (mRemainProps.empty() || mRemainProps.back() == 0)
        {
            return false;
        }

        --mRemainProps.back();
        name = String(BinArchiveIO::readString(mStream));
        return true;
    }

    //--------------------------------------------------------------------------

    void BinArchiveReader::endObject()
    {
        if (mRemainProps.empty())
        {
            return;
        }

        // 遍历层可能提前停止拉取（例如出错跳出），此处补齐剩余属性以保持流对齐。
        uint64_t remain = mRemainProps.back();
        mRemainProps.pop_back();
        for (uint64_t i = 0; i < remain; ++i)
        {
            BinArchiveIO::readString(mStream);
            skipValue();
        }
    }

    //--------------------------------------------------------------------------

    uint64_t BinArchiveReader::beginArray()
    {
        const uint64_t count = BinArchiveIO::readUVarint(mStream);
        mRemainElems.push_back(ElemFrame{count, 1});
        return count;
    }

    //--------------------------------------------------------------------------

    void BinArchiveReader::beginMap()
    {
        // keyOnly 落盘标记只用于确定每个元素占几个值，不外报：容器结构一律由
        // 遍历层按声明类型驱动。
        const bool keyOnly = (BinArchiveIO::readU8(mStream) != 0);
        const uint64_t count = BinArchiveIO::readUVarint(mStream);
        mRemainElems.push_back(ElemFrame{count, keyOnly ? 1u : 2u});
    }

    //--------------------------------------------------------------------------

    bool BinArchiveReader::nextElement()
    {
        if (mRemainElems.empty() || mRemainElems.back().remain == 0)
        {
            return false;
        }

        --mRemainElems.back().remain;
        return true;
    }

    //--------------------------------------------------------------------------

    void BinArchiveReader::endArray()
    {
        endContainer();
    }

    //--------------------------------------------------------------------------

    void BinArchiveReader::endMap()
    {
        endContainer();
    }

    //--------------------------------------------------------------------------

    void BinArchiveReader::endContainer()
    {
        if (mRemainElems.empty())
        {
            return;
        }

        // 与 endObject 同理：遍历层可能提前停止拉取，此处补齐剩余元素以保持流对齐。
        const ElemFrame frame = mRemainElems.back();
        mRemainElems.pop_back();

        for (uint64_t i = 0; i < frame.remain; ++i)
        {
            for (uint32_t v = 0; v < frame.valuesPerElem; ++v)
            {
                skipValue();
            }
        }
    }

    //--------------------------------------------------------------------------
}
