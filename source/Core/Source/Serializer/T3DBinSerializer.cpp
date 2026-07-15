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

#include "Serializer/T3DBinSerializer.h"
#include "Resource/T3DResource.h"
#include "Component/T3DComponent.h"
#include "Object/T3DBuffer.h"
#include "UUID/T3DUUID.h"
#include "IO/T3DDataStream.h"
#include "T3DErrorDef.h"

#include <string>
#include <vector>


namespace Tiny3D
{
    using namespace rttr;

    //--------------------------------------------------------------------------
    //
    // T3DBin v1 二进制格式（内部实现，不导出）。
    //
    // [Header]
    //   magic    : 'T','3','D','B'
    //   version  : uint16 LE
    //   flags    : uint16 LE (预留)
    //   reserved : uint32 LE = 0
    // [RootValue]  等价 JSON 的 Object 节点，字符串内联（无 StringTable）。
    //
    // 每个 Value = kind:uint8 + payload。语义与 T3DJsonSerializer 保持行为等价：
    //   跳过 NO_SERIALIZE、空指针/空 SmartPtr 不写、Buffer/UUID 特殊处理、
    //   多态按派生类型名创建、onPreSave/onPostSave/onPostInit/onPostLoad、
    //   Resource 反序列化时 onAddComponentForLoadingResource。
    //
    //--------------------------------------------------------------------------

    namespace
    {
        constexpr uint8_t  kMagic0 = 'T';
        constexpr uint8_t  kMagic1 = '3';
        constexpr uint8_t  kMagic2 = 'D';
        constexpr uint8_t  kMagic3 = 'B';
        constexpr uint16_t kBinVersion = 1;

        // 自描述的值类型标签
        enum class BinKind : uint8_t
        {
            Null = 0,
            Bool,
            I8, U8,
            I16, U16,
            I32, U32,
            I64, U64,
            F32, F64,
            String,
            Enum,
            Object,
            Seq,
            Map,
            Buffer,
            Uuid,
        };

        //----------------------------------------------------------------------
        // 低层编解码原语（不复用 DataStream::operator<<(String)，因其读写不可往返）
        //----------------------------------------------------------------------

        inline void writeBytes(DataStream &s, const void *p, size_t n)
        {
            if (n > 0)
            {
                s.write(const_cast<void *>(p), n);
            }
        }

        template <typename T>
        inline void writePod(DataStream &s, T v)
        {
            s.write(&v, sizeof(T));
        }

        inline void writeU8(DataStream &s, uint8_t v)
        {
            s.write(&v, sizeof(v));
        }

        inline void writeKind(DataStream &s, BinKind kind)
        {
            writeU8(s, static_cast<uint8_t>(kind));
        }

        inline void writeUVarint(DataStream &s, uint64_t v)
        {
            while (v >= 0x80)
            {
                uint8_t b = static_cast<uint8_t>(v) | 0x80;
                s.write(&b, sizeof(b));
                v >>= 7;
            }
            uint8_t b = static_cast<uint8_t>(v);
            s.write(&b, sizeof(b));
        }

        inline void writeString(DataStream &s, const char *p, size_t len)
        {
            writeUVarint(s, static_cast<uint64_t>(len));
            writeBytes(s, p, len);
        }

        inline void writeString(DataStream &s, const std::string &str)
        {
            writeString(s, str.data(), str.size());
        }

        inline void writeString(DataStream &s, const rttr::string_view &str)
        {
            writeString(s, str.data(), str.length());
        }

        inline uint8_t readU8(DataStream &s)
        {
            uint8_t v = 0;
            s.read(&v, sizeof(v));
            return v;
        }

        inline BinKind readKind(DataStream &s)
        {
            return static_cast<BinKind>(readU8(s));
        }

        template <typename T>
        inline T readPod(DataStream &s)
        {
            T v{};
            s.read(&v, sizeof(T));
            return v;
        }

        inline uint64_t readUVarint(DataStream &s)
        {
            uint64_t result = 0;
            int shift = 0;
            while (true)
            {
                uint8_t b = readU8(s);
                result |= static_cast<uint64_t>(b & 0x7F) << shift;
                if ((b & 0x80) == 0)
                {
                    break;
                }
                shift += 7;
            }
            return result;
        }

        inline std::string readString(DataStream &s)
        {
            uint64_t n = readUVarint(s);
            std::string str;
            if (n > 0)
            {
                str.resize(static_cast<size_t>(n));
                s.read(&str[0], static_cast<size_t>(n));
            }
            return str;
        }

        inline void skipBytes(DataStream &s, uint64_t n)
        {
            if (n > 0)
            {
                s.seek(static_cast<long_t>(n), true);
            }
        }
    }

    //--------------------------------------------------------------------------
    // 写入器：遍历 RTTR 对象生成二进制流（对照 RTTRObjectJsonWriter）
    //--------------------------------------------------------------------------

    class RTTRObjectBinWriter
    {
    public:
        static void WriteValue(DataStream &s, const variant &var);
        static void WriteObject(DataStream &s, const instance &obj);

    private:
        static bool WriteAtomic(DataStream &s, const type &t, const variant &var);
        static void WriteSeq(DataStream &s, const variant_sequential_view &view);
        static void WriteMap(DataStream &s, const variant_associative_view &view);
    };

    //--------------------------------------------------------------------------

    bool RTTRObjectBinWriter::WriteAtomic(DataStream &s, const type &t,
        const variant &var)
    {
        if (t.is_arithmetic())
        {
            if (t == type::get<bool>())
            {
                writeKind(s, BinKind::Bool);
                writePod<uint8_t>(s, var.to_bool() ? 1 : 0);
            }
            else if (t == type::get<char>())
            {
                writeKind(s, BinKind::I8);
                writePod<int8_t>(s, static_cast<int8_t>(var.to_int8()));
            }
            else if (t == type::get<int8_t>())
            {
                writeKind(s, BinKind::I8);
                writePod<int8_t>(s, var.to_int8());
            }
            else if (t == type::get<uint8_t>())
            {
                writeKind(s, BinKind::U8);
                writePod<uint8_t>(s, var.to_uint8());
            }
            else if (t == type::get<int16_t>())
            {
                writeKind(s, BinKind::I16);
                writePod<int16_t>(s, var.to_int16());
            }
            else if (t == type::get<uint16_t>())
            {
                writeKind(s, BinKind::U16);
                writePod<uint16_t>(s, var.to_uint16());
            }
            else if (t == type::get<int32_t>())
            {
                writeKind(s, BinKind::I32);
                writePod<int32_t>(s, var.to_int32());
            }
            else if (t == type::get<uint32_t>())
            {
                writeKind(s, BinKind::U32);
                writePod<uint32_t>(s, var.to_uint32());
            }
            else if (t == type::get<int64_t>())
            {
                writeKind(s, BinKind::I64);
                writePod<int64_t>(s, var.to_int64());
            }
            else if (t == type::get<uint64_t>())
            {
                writeKind(s, BinKind::U64);
                writePod<uint64_t>(s, var.to_uint64());
            }
            else if (t == type::get<float>())
            {
                writeKind(s, BinKind::F32);
                writePod<float>(s, static_cast<float>(var.to_double()));
            }
            else if (t == type::get<double>())
            {
                writeKind(s, BinKind::F64);
                writePod<double>(s, var.to_double());
            }
            else
            {
                // 兜底：size_t / long 等平台相关整型，统一按 U64 存储
                writeKind(s, BinKind::U64);
                writePod<uint64_t>(s, var.to_uint64());
            }
            return true;
        }
        else if (t.is_enumeration())
        {
            writeKind(s, BinKind::Enum);
            bool ok = false;
            auto name = var.to_string(&ok);
            if (ok)
            {
                writeString(s, name);
            }
            else
            {
                // 名称不可用时退化为数值字符串
                writeString(s, std::to_string(var.to_uint64()));
            }
            return true;
        }
        else if (t == type::get<std::string>())
        {
            writeKind(s, BinKind::String);
            writeString(s, var.to_string());
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinWriter::WriteSeq(DataStream &s,
        const variant_sequential_view &view)
    {
        writeUVarint(s, static_cast<uint64_t>(view.get_size()));
        for (const auto &item : view)
        {
            // 与 JSON WriteSequentialContainer 对齐：先 extract_wrapped_value，
            // 再 WriteValue。容器 view 元素上的 SmartPtr wrapper 若直接
            // get_wrapped_instance 会失败，extract 后得到裸指针 variant 可正确写类型名。
            if (item.is_sequential_container())
            {
                writeKind(s, BinKind::Seq);
                WriteSeq(s, item.create_sequential_view());
            }
            else
            {
                WriteValue(s, item.extract_wrapped_value());
            }
        }
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinWriter::WriteMap(DataStream &s,
        const variant_associative_view &view)
    {
        const bool keyOnly = view.is_key_only_type();
        writeU8(s, keyOnly ? 1 : 0);
        writeUVarint(s, static_cast<uint64_t>(view.get_size()));

        // 与 JSON WriteAssociativeContainer 对齐：map 元素先 extract 再 WriteValue
        if (keyOnly)
        {
            for (const auto &item : view)
            {
                WriteValue(s, item.first.extract_wrapped_value());
            }
        }
        else
        {
            for (const auto &item : view)
            {
                WriteValue(s, item.first.extract_wrapped_value());
                WriteValue(s, item.second.extract_wrapped_value());
            }
        }
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinWriter::WriteObject(DataStream &s, const instance &obj)
    {
        const instance obj2 = obj.get_type().get_raw_type().is_wrapper()
            ? obj.get_wrapped_instance() : obj;

        const type derived = obj2.get_derived_type();
        writeString(s, derived.get_name());

        // 注意：Buffer / UUID 已在 WriteValue 中作为独立 kind 提前拦截，不会走到
        // 这里的属性遍历（否则会误把 Buffer::Data 裸指针当普通对象序列化）。

        std::vector<std::pair<std::string, variant>> entries;
        for (auto prop : derived.get_properties())
        {
            if (prop.get_metadata("NO_SERIALIZE"))
            {
                continue;
            }

            variant propValue = prop.get_value(obj2);
            if (!propValue)
            {
                continue;
            }

            const type pt = propValue.get_type();
            if (pt.is_pointer())
            {
                if (propValue.convert<void *>() == nullptr)
                {
                    continue;
                }
            }
            else if (pt.is_wrapper()
                && pt.get_wrapped_type().is_derived_from<Object>())
            {
                // 空的 SmartPtr<Object> 不写（对齐 JSON）。注意：对 null 智能指针
                // convert<SmartPtr<Object>> 可能返回 false，不能仅在 convert 成功时
                // 判空；这里直接解包裸指针判空，null 则跳过，避免写出空类型名对象。
                variant wv = propValue.extract_wrapped_value();
                if (!wv.is_valid() || wv.convert<void *>() == nullptr)
                {
                    continue;
                }
            }

            entries.emplace_back(prop.get_name().to_string(), propValue);
        }

        writeUVarint(s, static_cast<uint64_t>(entries.size()));
        for (auto &entry : entries)
        {
            writeString(s, entry.first);
            WriteValue(s, entry.second);
        }
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinWriter::WriteValue(DataStream &s, const variant &var)
    {
        const type value_type = var.get_type();
        const type wrapped_type = value_type.is_wrapper()
            ? value_type.get_wrapped_type() : value_type;
        const bool is_wrapper = wrapped_type != value_type;
        const type effType = is_wrapper ? wrapped_type : value_type;
        const variant eff = is_wrapper ? var.extract_wrapped_value() : var;

        const bool isObject = wrapped_type.is_derived_from<Object>();
        if (isObject)
        {
            auto preSave = wrapped_type.get_method("onPreSave");
            if (preSave)
            {
                preSave.invoke(is_wrapper ? var.extract_wrapped_value() : var);
            }
        }

        if (WriteAtomic(s, effType, eff))
        {
            // atomic / enum / string 已写
        }
        else if (var.is_sequential_container())
        {
            writeKind(s, BinKind::Seq);
            WriteSeq(s, var.create_sequential_view());
        }
        else if (var.is_associative_container())
        {
            writeKind(s, BinKind::Map);
            WriteMap(s, var.create_associative_view());
        }
        else if (effType == type::get<Buffer>())
        {
            writeKind(s, BinKind::Buffer);
            const Buffer &buffer = eff.get_value<Buffer>();
            writeUVarint(s, static_cast<uint64_t>(buffer.DataSize));
            writeBytes(s, buffer.Data, buffer.DataSize);
        }
        else if (effType == type::get<UUID>())
        {
            writeKind(s, BinKind::Uuid);
            const UUID &uuid = eff.get_value<UUID>();
            writeBytes(s, uuid.bytes, UUID::kDefaultLength);
        }
        else
        {
            if (effType.is_pointer() && eff.convert<void *>() == nullptr)
            {
                writeKind(s, BinKind::Null);
                return;
            }

            const instance childInst = var;
            const instance objInst = is_wrapper
                ? childInst.get_wrapped_instance() : childInst;

            if (!objInst.is_valid())
            {
                writeKind(s, BinKind::Null);
                return;
            }

            writeKind(s, BinKind::Object);
            // 与 JSON WriteVariant 对齐：wrapper 用 get_wrapped_instance() 取
            // 裸指针对象；容器元素已在 WriteSeq/WriteMap 中 extract 后再进入此路径。
            WriteObject(s, objInst);
        }

        if (isObject)
        {
            auto postSave = wrapped_type.get_method("onPostSave");
            if (postSave)
            {
                postSave.invoke(is_wrapper ? var.extract_wrapped_value() : var);
            }
        }
    }

    //--------------------------------------------------------------------------
    // 读取器：从二进制流还原 RTTR 对象（对照 RTTRObjectJsonReader）
    //--------------------------------------------------------------------------

    class RTTRObjectBinReader
    {
    public:
        variant ReadNew(DataStream &s, bool topLevel);

    private:
        void ReadInto(DataStream &s, const type &hint, variant &obj);
        void ReadIntoKind(DataStream &s, BinKind kind, const type &hint,
            variant &obj);
        variant ReadElement(DataStream &s, const type &elemType);
        void FillObjectMembers(DataStream &s, const type &klass, variant &obj);
        void ReadSeqInto(DataStream &s, variant &obj);
        void ReadMapInto(DataStream &s, variant &obj);

        void SkipValue(DataStream &s);
        void SkipPayload(DataStream &s, BinKind kind);

        static variant ReadAtomicByKind(DataStream &s, BinKind kind);
        static property FindProperty(const type &klass, const std::string &name);
        static void ConvertToSmartPtr(variant &var, const type &propType);

    private:
        variant *mObj{nullptr};
    };

    //--------------------------------------------------------------------------

    variant RTTRObjectBinReader::ReadAtomicByKind(DataStream &s, BinKind kind)
    {
        switch (kind)
        {
        case BinKind::Bool:   return variant(readPod<uint8_t>(s) != 0);
        case BinKind::I8:     return variant(readPod<int8_t>(s));
        case BinKind::U8:     return variant(readPod<uint8_t>(s));
        case BinKind::I16:    return variant(readPod<int16_t>(s));
        case BinKind::U16:    return variant(readPod<uint16_t>(s));
        case BinKind::I32:    return variant(readPod<int32_t>(s));
        case BinKind::U32:    return variant(readPod<uint32_t>(s));
        case BinKind::I64:    return variant(readPod<int64_t>(s));
        case BinKind::U64:    return variant(readPod<uint64_t>(s));
        case BinKind::F32:    return variant(readPod<float>(s));
        case BinKind::F64:    return variant(readPod<double>(s));
        case BinKind::String: return variant(String(readString(s)));
        case BinKind::Enum:   return variant(String(readString(s)));
        default:              return variant();
        }
    }

    //--------------------------------------------------------------------------

    property RTTRObjectBinReader::FindProperty(const type &klass,
        const std::string &name)
    {
        property prop = klass.get_property(name);
        if (prop.is_valid())
        {
            return prop;
        }

        // 改名兼容：回退查找 SERIALIZE_ALIAS 曾用名
        for (auto p : klass.get_properties())
        {
            variant md = p.get_metadata("SERIALIZE_ALIAS");
            if (md.is_valid())
            {
                bool ok = false;
                std::string alias = md.to_string(&ok);
                if (ok && alias == name)
                {
                    return p;
                }
            }
        }

        return prop;
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinReader::ConvertToSmartPtr(variant &var,
        const type &propType)
    {
        if (!propType.is_wrapper() || var.get_type() == propType)
        {
            return;
        }

        const type varType = var.get_type();
        if (varType.is_pointer()
            && (propType.get_wrapped_type() == varType.get_raw_type()
                || propType.get_wrapped_type().is_base_of(varType.get_raw_type()))
            && varType.get_raw_type().is_derived_from<Object>())
        {
            bool ok = false;
            Object *tempObj = var.convert<Object *>(&ok);
            T3D_ASSERT(ok);
            var = propType.create({tempObj});
            return;
        }

        // 兜底：RTTR 已注册 SmartPtr<T> 与 T* 的 convert
        if (!var.convert(propType))
        {
            T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                "Failed to convert value type [%s] to [%s] for SmartPtr property.",
                varType.get_name().data(), propType.get_name().data());
        }
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinReader::SkipPayload(DataStream &s, BinKind kind)
    {
        switch (kind)
        {
        case BinKind::Null:
            break;
        case BinKind::Bool:
        case BinKind::I8:
        case BinKind::U8:
            skipBytes(s, 1);
            break;
        case BinKind::I16:
        case BinKind::U16:
            skipBytes(s, 2);
            break;
        case BinKind::I32:
        case BinKind::U32:
        case BinKind::F32:
            skipBytes(s, 4);
            break;
        case BinKind::I64:
        case BinKind::U64:
        case BinKind::F64:
            skipBytes(s, 8);
            break;
        case BinKind::String:
        case BinKind::Enum:
        case BinKind::Buffer:
            skipBytes(s, readUVarint(s));
            break;
        case BinKind::Uuid:
            skipBytes(s, UUID::kDefaultLength);
            break;
        case BinKind::Object:
            {
                readString(s); // typename
                uint64_t count = readUVarint(s);
                for (uint64_t i = 0; i < count; ++i)
                {
                    readString(s); // property name
                    SkipValue(s);
                }
            }
            break;
        case BinKind::Seq:
            {
                uint64_t count = readUVarint(s);
                for (uint64_t i = 0; i < count; ++i)
                {
                    SkipValue(s);
                }
            }
            break;
        case BinKind::Map:
            {
                uint8_t keyOnly = readU8(s);
                uint64_t count = readUVarint(s);
                for (uint64_t i = 0; i < count; ++i)
                {
                    SkipValue(s);
                    if (keyOnly == 0)
                    {
                        SkipValue(s);
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinReader::SkipValue(DataStream &s)
    {
        BinKind kind = readKind(s);
        SkipPayload(s, kind);
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinReader::ReadSeqInto(DataStream &s, variant &obj)
    {
        auto view = obj.create_sequential_view();
        uint64_t count = readUVarint(s);
        view.set_size(static_cast<size_t>(count));
        const type itemType = view.get_value_type();
        for (uint64_t i = 0; i < count; ++i)
        {
            variant var = ReadElement(s, itemType);
            bool rval = view.set_value(static_cast<size_t>(i), var);
            if (!rval)
            {
                T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                    "Set element [%llu] value of sequential container failed !",
                    (unsigned long long)i);
            }
        }
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinReader::ReadMapInto(DataStream &s, variant &obj)
    {
        auto view = obj.create_associative_view();
        uint8_t keyOnly = readU8(s);
        (void)keyOnly;
        uint64_t count = readUVarint(s);

        if (view.is_key_only_type())
        {
            // set 容器：元素类型是 key_type。注意 rttr 对 set 的 get_value_type()
            // 返回无效类型，必须用 get_key_type()（否则无法按声明类型读元素，
            // 导致读入无效 variant、insert 失败、数据丢失）。
            type elemType = view.get_key_type();
            if (!elemType.is_valid())
            {
                elemType = view.get_value_type();
            }
            for (uint64_t i = 0; i < count; ++i)
            {
                variant key = ReadElement(s, elemType);
                auto rval = view.insert(key);
                if (!rval.second)
                {
                    T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                        "Failed to insert element [%llu] in set container !",
                        (unsigned long long)i);
                }
            }
        }
        else
        {
            const type keyType = view.get_key_type();
            const type valueType = view.get_value_type();
            for (uint64_t i = 0; i < count; ++i)
            {
                variant key = ReadElement(s, keyType);
                variant val = ReadElement(s, valueType);
                auto rval = view.insert(key, val);
                if (!rval.second)
                {
                    T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                        "Failed to insert element [%llu] in map container !",
                        (unsigned long long)i);
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    variant RTTRObjectBinReader::ReadElement(DataStream &s, const type &elemType)
    {
        const bool isPoly = elemType.is_pointer()
            || (elemType.is_wrapper()
                && elemType.get_wrapped_type().is_derived_from<Object>());

        if (isPoly)
        {
            variant var = ReadNew(s, false);
            if (var.is_valid())
            {
                ConvertToSmartPtr(var, elemType);
            }
            return var;
        }

        constructor ctor = elemType.get_constructor();
        variant var = ctor.is_valid() ? ctor.invoke() : variant();
        ReadInto(s, elemType, var);
        return var;
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinReader::ReadIntoKind(DataStream &s, BinKind kind,
        const type &hint, variant &obj)
    {
        if (kind == BinKind::Null)
        {
            return;
        }

        if (hint.is_arithmetic())
        {
            variant raw = ReadAtomicByKind(s, kind);
            if (!raw.is_valid())
            {
                SkipPayload(s, kind);
                T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                    "Incompatible value for arithmetic property [%s], skipped.",
                    hint.get_name().data());
                return;
            }
            if (raw.get_type() != hint)
            {
                raw.convert(hint);
            }
            obj = raw;
        }
        else if (hint.is_enumeration())
        {
            if (kind == BinKind::Enum || kind == BinKind::String)
            {
                std::string name = readString(s);
                obj = hint.get_enumeration().name_to_value(name);
            }
            else
            {
                variant raw = ReadAtomicByKind(s, kind);
                if (raw.is_valid() && raw.convert(hint))
                {
                    obj = raw;
                }
                else
                {
                    T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                        "Incompatible value for enum property [%s], skipped.",
                        hint.get_name().data());
                }
            }
        }
        else if (hint == type::get<std::string>())
        {
            if (kind == BinKind::String || kind == BinKind::Enum)
            {
                obj = String(readString(s));
            }
            else
            {
                variant raw = ReadAtomicByKind(s, kind);
                bool ok = false;
                String str = raw.is_valid() ? raw.to_string(&ok) : String();
                if (ok)
                {
                    obj = str;
                }
                else
                {
                    T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                        "Incompatible value for string property, skipped.");
                }
            }
        }
        else if (hint == type::get<Buffer>())
        {
            if (kind == BinKind::Buffer)
            {
                Buffer &buffer = obj.get_value<Buffer>();
                uint64_t n = readUVarint(s);
                T3D_POD_SAFE_DELETE_ARRAY(buffer.Data);
                buffer.Data = (n > 0)
                    ? T3D_POD_NEW_ARRAY(uint8_t, static_cast<size_t>(n)) : nullptr;
                buffer.DataSize = static_cast<size_t>(n);
                if (n > 0)
                {
                    s.read(buffer.Data, static_cast<size_t>(n));
                }
            }
            else
            {
                SkipPayload(s, kind);
                T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                    "Incompatible value for Buffer property, skipped.");
            }
        }
        else if (hint == type::get<UUID>())
        {
            if (kind == BinKind::Uuid)
            {
                UUID &uuid = obj.get_value<UUID>();
                s.read(uuid.bytes, UUID::kDefaultLength);
            }
            else if (kind == BinKind::String)
            {
                std::string str = readString(s);
                UUID &uuid = obj.get_value<UUID>();
                uuid.fromString(String(str));
            }
            else
            {
                SkipPayload(s, kind);
                T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                    "Incompatible value for UUID property, skipped.");
            }
        }
        else if (hint.is_sequential_container())
        {
            if (kind == BinKind::Seq)
            {
                ReadSeqInto(s, obj);
            }
            else
            {
                SkipPayload(s, kind);
                T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                    "Incompatible value for sequential container, skipped.");
            }
        }
        else if (hint.is_associative_container())
        {
            if (kind == BinKind::Map)
            {
                ReadMapInto(s, obj);
            }
            else
            {
                SkipPayload(s, kind);
                T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                    "Incompatible value for associative container, skipped.");
            }
        }
        else
        {
            // 值类型对象（TSTRUCT，非 Object 派生）
            if (kind == BinKind::Object)
            {
                readString(s); // typename（值类型用声明类型 hint，忽略之）
                FillObjectMembers(s, hint, obj);
            }
            else
            {
                SkipPayload(s, kind);
                T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                    "Incompatible value for object property [%s], skipped.",
                    hint.get_name().data());
            }
        }
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinReader::ReadInto(DataStream &s, const type &hint,
        variant &obj)
    {
        BinKind kind = readKind(s);
        ReadIntoKind(s, kind, hint, obj);
    }

    //--------------------------------------------------------------------------

    void RTTRObjectBinReader::FillObjectMembers(DataStream &s, const type &klass,
        variant &obj)
    {
        uint64_t count = readUVarint(s);
        for (uint64_t i = 0; i < count; ++i)
        {
            std::string pname = readString(s);
            property prop = FindProperty(klass, pname);
            if (!prop.is_valid())
            {
                T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                    "Property (%s) not found in class (%s), skipping.",
                    pname.c_str(), klass.get_name().data());
                SkipValue(s);
                continue;
            }

            const type propType = prop.get_type();
            const bool isPoly = propType.is_pointer()
                || (propType.is_wrapper()
                    && propType.get_wrapped_type().is_derived_from<Object>());

            variant val;
            if (isPoly)
            {
                val = ReadNew(s, false);
                if (val.is_valid())
                {
                    ConvertToSmartPtr(val, propType);
                }
                else
                {
                    continue;
                }
            }
            else if (propType.is_array())
            {
                val = prop.get_value(obj);
                ReadInto(s, propType, val);
            }
            else if (propType.is_arithmetic() || propType.is_enumeration()
                || propType == type::get<std::string>())
            {
                // 基础类型在 rttr 无注册构造器，无需预构造：ReadInto 直接给 val 赋值
                ReadInto(s, propType, val);
            }
            else
            {
                // 值类型对象 / 容器 / Buffer / UUID：需先按声明类型默认构造再原地填充
                constructor ctor = propType.get_constructor();
                if (!ctor.is_valid())
                {
                    T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                        "No default constructor for property (%s) type in "
                        "class (%s)!", pname.c_str(), klass.get_name().data());
                    SkipValue(s);
                    continue;
                }
                val = ctor.invoke();
                ReadInto(s, propType, val);
            }

            bool rval = prop.set_value(obj, val);
            if (!rval)
            {
                T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                    "Failed to set property (%s) in class (%s)!",
                    pname.c_str(), klass.get_name().data());
            }
        }

        if (klass.is_derived_from<Object>())
        {
            auto postLoad = klass.get_method("onPostLoad");
            if (postLoad)
            {
                postLoad.invoke(obj);
            }
        }

        if (mObj != nullptr && klass.is_derived_from<Component>())
        {
            auto addComponent =
                mObj->get_type().get_method("onAddComponentForLoadingResource");
            bool ok = false;
            Component *component = obj.convert<Component *>(&ok);
            if (addComponent.is_valid() && ok)
            {
                Resource *resource = mObj->convert<Resource *>(&ok);
                if (ok)
                {
                    addComponent.invoke(resource, component);
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    variant RTTRObjectBinReader::ReadNew(DataStream &s, bool topLevel)
    {
        BinKind kind = readKind(s);

        switch (kind)
        {
        case BinKind::Null:
            return variant();
        case BinKind::Object:
            {
                std::string typeName = readString(s);
                type klass = type::get_by_name(typeName);
                if (!klass.is_valid())
                {
                    T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                        "RTTR type [%s] is not registered !", typeName.c_str());
                    // 消费对象体以维持流对齐
                    uint64_t count = readUVarint(s);
                    for (uint64_t i = 0; i < count; ++i)
                    {
                        readString(s);
                        SkipValue(s);
                    }
                    return variant();
                }

                constructor ctor = klass.get_constructor();
                variant obj = ctor.invoke();

                if (topLevel && klass.is_derived_from<Resource>())
                {
                    mObj = &obj;
                }

                if (klass.is_derived_from<Object>())
                {
                    auto onPostInit = klass.get_method("onPostInit");
                    if (onPostInit)
                    {
                        onPostInit.invoke(obj);
                    }
                }

                FillObjectMembers(s, klass, obj);
                return obj;
            }
        case BinKind::Buffer:
            {
                variant obj = type::get<Buffer>().get_constructor().invoke();
                Buffer &buffer = obj.get_value<Buffer>();
                uint64_t n = readUVarint(s);
                buffer.Data = (n > 0)
                    ? T3D_POD_NEW_ARRAY(uint8_t, static_cast<size_t>(n)) : nullptr;
                buffer.DataSize = static_cast<size_t>(n);
                if (n > 0)
                {
                    s.read(buffer.Data, static_cast<size_t>(n));
                }
                return obj;
            }
        case BinKind::Uuid:
            {
                variant obj = type::get<UUID>().get_constructor().invoke();
                UUID &uuid = obj.get_value<UUID>();
                s.read(uuid.bytes, UUID::kDefaultLength);
                return obj;
            }
        case BinKind::Seq:
        case BinKind::Map:
            T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                "Unexpected self-describing container without type hint, "
                "skipping.");
            SkipPayload(s, kind);
            return variant();
        default:
            return ReadAtomicByKind(s, kind);
        }
    }

    //--------------------------------------------------------------------------
    // 文件头
    //--------------------------------------------------------------------------

    namespace
    {
        void writeHeader(DataStream &s)
        {
            uint8_t magic[4] = { kMagic0, kMagic1, kMagic2, kMagic3 };
            s.write(magic, sizeof(magic));
            writePod<uint16_t>(s, kBinVersion);
            writePod<uint16_t>(s, 0);   // flags
            writePod<uint32_t>(s, 0);   // reserved
        }

        TResult readHeader(DataStream &s, uint16_t &version)
        {
            uint8_t magic[4] = { 0, 0, 0, 0 };
            if (s.read(magic, sizeof(magic)) != sizeof(magic))
            {
                T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                    "Failed to read binary header !");
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
            if (version == 0 || version > kBinVersion)
            {
                T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                    "Unsupported binary version [%u] !", (uint32_t)version);
                return T3D_ERR_MISSING_VERSION;
            }
            return T3D_OK;
        }
    }

    //--------------------------------------------------------------------------

    BinSerializerPtr BinSerializer::create()
    {
        return T3D_NEW BinSerializer();
    }

    //--------------------------------------------------------------------------

    TResult BinSerializer::serialize(DataStream &stream, const RTTRObject &obj)
    {
        writeHeader(stream);

        const instance topInst(obj);
        const instance topInst2 = topInst.get_type().get_raw_type().is_wrapper()
            ? topInst.get_wrapped_instance() : topInst;
        const type topType = topInst2.get_derived_type();

        if (topType.is_derived_from<Object>())
        {
            auto preSave = topType.get_method("onPreSave");
            if (preSave)
            {
                preSave.invoke(topInst2);
            }
        }

        writeKind(stream, BinKind::Object);
        RTTRObjectBinWriter::WriteObject(stream, obj);

        if (topType.is_derived_from<Object>())
        {
            auto postSave = topType.get_method("onPostSave");
            if (postSave)
            {
                postSave.invoke(topInst2);
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RTTRObject BinSerializer::deserialize(DataStream &stream)
    {
        uint16_t version = 0;
        if (T3D_FAILED(readHeader(stream, version)))
        {
            return RTTRObject();
        }

        RTTRObjectBinReader reader;
        return reader.ReadNew(stream, true);
    }

    //--------------------------------------------------------------------------

    TResult BinSerializer::deserialize(DataStream &stream, RTTRVariant &obj)
    {
        uint16_t version = 0;
        TResult ret = readHeader(stream, version);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        RTTRObjectBinReader reader;
        obj = reader.ReadNew(stream, true);
        if (!obj.is_valid())
        {
            return T3D_ERR_DESERIALIZE_OBJECT;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
