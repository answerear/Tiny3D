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


#include "Serializer/T3DJsonArchive.h"
#define RAPIDJSON_HAS_STDSTRING 1
#include <prettywriter.h>
#include <reader.h>
#include "Object/T3DBuffer.h"
#include "UUID/T3DUUID.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    using namespace JsonArchiveIO;

    //--------------------------------------------------------------------------

    /**
     * \brief 把 rapidjson 的字节流接口桥接到引擎 DataStream。
     */
    class JsonOutStream
    {
    public:
        typedef signed char Ch;

        explicit JsonOutStream(DataStream &stream)
            : mStream(stream)
        {
        }

        Ch Peek() const { return '\0'; }
        Ch Take() const { return '\0'; }
        size_t Tell() const { return (size_t)mStream.tell(); }

        Ch *PutBegin() { return nullptr; }
        size_t PutEnd(Ch *) { return 0; }

        void Put(Ch c) { mStream << c; }
        void Flush() {}

    private:
        DataStream &mStream;
    };

    //--------------------------------------------------------------------------

    struct JsonArchiveWriter::Impl
    {
        explicit Impl(DataStream &stream)
            : os(stream)
            , writer(os)
        {
            writer.SetIndent(' ', 2);
        }

        /** \brief 写出 RTTI_Type 标签，并把游标停在 RTTI_Value 的值位置。 */
        void writeTypeTag(const rttr::string_view &typeName)
        {
            writer.Key(kTypeKey);
            writer.String(typeName.data(),
                static_cast<rapidjson::SizeType>(typeName.length()), false);
            writer.Key(kValueKey);
        }

        JsonOutStream os;
        rapidjson::PrettyWriter<JsonOutStream> writer;
    };

    //--------------------------------------------------------------------------

    JsonArchiveWriter::JsonArchiveWriter(DataStream &stream)
        : mImpl(T3D_NEW Impl(stream))
    {
    }

    //--------------------------------------------------------------------------

    JsonArchiveWriter::~JsonArchiveWriter()
    {
        T3D_DELETE mImpl;
        mImpl = nullptr;
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::beginDocument()
    {
        auto &writer = mImpl->writer;
        writer.StartObject();
        writer.Key(kMagicKey);
        writer.String(kMagicValue);
        writer.Key(kVersionKey);
        writer.String(kVersionStr);
        writer.Key(kObjectKey);
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::endDocument()
    {
        mImpl->writer.EndObject();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::beginValue()
    {
        mImpl->writer.StartObject();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::endValue()
    {
        mImpl->writer.EndObject();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writeNull(const rttr::string_view &typeName)
    {
        mImpl->writeTypeTag(typeName);
        mImpl->writer.Null();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writeBool(const rttr::string_view &typeName,
        bool value)
    {
        mImpl->writeTypeTag(typeName);
        mImpl->writer.Bool(value);
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writeInt(const rttr::string_view &typeName,
        ArchiveKind kind, int64_t value)
    {
        mImpl->writeTypeTag(typeName);

        if (kind == ArchiveKind::I64)
        {
            mImpl->writer.Int64(value);
        }
        else
        {
            mImpl->writer.Int(static_cast<int32_t>(value));
        }
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writeUint(const rttr::string_view &typeName,
        ArchiveKind kind, uint64_t value)
    {
        mImpl->writeTypeTag(typeName);

        if (kind == ArchiveKind::U64)
        {
            mImpl->writer.Uint64(value);
        }
        else
        {
            mImpl->writer.Uint(static_cast<uint32_t>(value));
        }
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writeReal(const rttr::string_view &typeName,
        ArchiveKind kind, double value)
    {
        (void)kind;
        mImpl->writeTypeTag(typeName);
        mImpl->writer.Double(value);
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writeString(const rttr::string_view &typeName,
        const String &value)
    {
        mImpl->writeTypeTag(typeName);
        mImpl->writer.String(value);
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writeEnum(const rttr::string_view &typeName,
        const String &name, bool nameValid, uint64_t value, bool valueValid)
    {
        mImpl->writeTypeTag(typeName);

        if (nameValid)
        {
            mImpl->writer.String(name);
        }
        else if (valueValid)
        {
            mImpl->writer.Uint64(value);
        }
        else
        {
            mImpl->writer.Null();
        }
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writeBuffer(const uint8_t *data, size_t size)
    {
        // Buffer 在文本格式里转 base64，内嵌成一个 std::string 值信封
        mImpl->writeTypeTag(rttr::type::get<Buffer>().get_name());
        mImpl->writer.StartObject();
        mImpl->writeTypeTag(rttr::type::get<String>().get_name());
        mImpl->writer.String(base64_encode(data, size));
        mImpl->writer.EndObject();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writeUuid(const UUID &uuid)
    {
        mImpl->writeTypeTag(rttr::type::get<UUID>().get_name());
        mImpl->writer.StartObject();
        mImpl->writeTypeTag(rttr::type::get<String>().get_name());
        mImpl->writer.String(uuid.toString());
        mImpl->writer.EndObject();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::beginObject(const rttr::string_view &typeName,
        size_t propertyCount)
    {
        (void)propertyCount;
        mImpl->writeTypeTag(typeName);
        mImpl->writer.StartObject();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writePropertyName(const rttr::string_view &name)
    {
        mImpl->writer.String(name.data(),
            static_cast<rapidjson::SizeType>(name.length()), false);
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::endObject()
    {
        mImpl->writer.EndObject();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::beginArray(const rttr::string_view &typeName,
        size_t count)
    {
        (void)count;
        mImpl->writeTypeTag(typeName);
        mImpl->writer.StartArray();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::endArray()
    {
        mImpl->writer.EndArray();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::beginMap(const rttr::string_view &typeName,
        bool keyOnly, size_t count)
    {
        (void)keyOnly;
        (void)count;
        mImpl->writeTypeTag(typeName);
        mImpl->writer.StartArray();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::endMap()
    {
        mImpl->writer.EndArray();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::beginMapEntry()
    {
        mImpl->writer.StartObject();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writeMapKeyTag()
    {
        mImpl->writer.Key(kMapKeyKey);
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::writeMapValueTag()
    {
        mImpl->writer.Key(kMapValueKey);
    }

    //--------------------------------------------------------------------------

    void JsonArchiveWriter::endMapEntry()
    {
        mImpl->writer.EndObject();
    }

    //--------------------------------------------------------------------------

    namespace
    {
        /** \brief 逐 token 拉取解析用的 rapidjson 解析选项。 */
        const unsigned kSaxParseFlags =
            rapidjson::kParseDefaultFlags | rapidjson::kParseIterativeFlag;

        //----------------------------------------------------------------------

        /**
         * \brief 单个 JSON token（SAX 事件）的快照。
         * \remarks 数字同时保存三种视图：落盘类型名给出的宽度未必与 JSON 里的实际
         *          数字形态一致（手改过的资产），取值时按需选用，避免读成 0。
         */
        struct JsonToken
        {
            enum class Type : uint8_t
            {
                None,
                Null,
                Bool,
                Int, Uint, Int64, Uint64, Double,
                String,
                Key,
                StartObject, EndObject,
                StartArray, EndArray,
            };

            Type type {Type::None};
            bool boolean {false};
            int64_t i64 {0};
            uint64_t u64 {0};
            double dbl {0.0};
            String str;
        };

        //----------------------------------------------------------------------

        /**
         * \brief rapidjson SAX 处理器：把单个事件抓存进 JsonToken。
         * \remarks 配合 `IterativeParseNext` 使用——它每次调用恰好触发一次回调，
         *          由此把 push 式的 SAX 反转成 pull 式的 token 流。
         */
        class JsonTokenSink
        {
        public:
            explicit JsonTokenSink(JsonToken &token)
                : mToken(token)
            {
            }

            bool Null() { return set(JsonToken::Type::Null); }

            bool Bool(bool value)
            {
                mToken.boolean = value;
                return set(JsonToken::Type::Bool);
            }

            bool Int(int value)
            {
                return setSigned(JsonToken::Type::Int, value);
            }

            bool Uint(unsigned value)
            {
                return setUnsigned(JsonToken::Type::Uint, value);
            }

            bool Int64(int64_t value)
            {
                return setSigned(JsonToken::Type::Int64, value);
            }

            bool Uint64(uint64_t value)
            {
                return setUnsigned(JsonToken::Type::Uint64, value);
            }

            bool Double(double value)
            {
                mToken.dbl = value;
                // 越界的 double 转整型是未定义行为，只在可表示时填充整型视图
                mToken.i64 = (value >= -9.2233720368547758e18
                    && value <= 9.2233720368547758e18)
                    ? static_cast<int64_t>(value) : 0;
                mToken.u64 = (value >= 0.0 && value <= 1.8446744073709552e19)
                    ? static_cast<uint64_t>(value) : 0;
                return set(JsonToken::Type::Double);
            }

            bool RawNumber(const char *, rapidjson::SizeType, bool)
            {
                // 仅 kParseNumbersAsStringsFlag 下才会触发，本处未启用
                return false;
            }

            bool String(const char *str, rapidjson::SizeType length, bool)
            {
                return setStr(JsonToken::Type::String, str, length);
            }

            bool Key(const char *str, rapidjson::SizeType length, bool)
            {
                return setStr(JsonToken::Type::Key, str, length);
            }

            bool StartObject() { return set(JsonToken::Type::StartObject); }
            bool EndObject(rapidjson::SizeType) { return set(JsonToken::Type::EndObject); }
            bool StartArray() { return set(JsonToken::Type::StartArray); }
            bool EndArray(rapidjson::SizeType) { return set(JsonToken::Type::EndArray); }

        private:
            bool set(JsonToken::Type type)
            {
                mToken.type = type;
                return true;
            }

            bool setSigned(JsonToken::Type type, int64_t value)
            {
                mToken.i64 = value;
                mToken.u64 = static_cast<uint64_t>(value);
                mToken.dbl = static_cast<double>(value);
                return set(type);
            }

            bool setUnsigned(JsonToken::Type type, uint64_t value)
            {
                mToken.u64 = value;
                mToken.i64 = static_cast<int64_t>(value);
                mToken.dbl = static_cast<double>(value);
                return set(type);
            }

            bool setStr(JsonToken::Type type, const char *str,
                rapidjson::SizeType length)
            {
                // 指针只在本次回调内有效，必须立刻复制
                mToken.str.assign(str, length);
                return set(type);
            }

            JsonToken &mToken;
        };

        //----------------------------------------------------------------------

        /**
         * \brief 把引擎 DataStream 桥接成 rapidjson 的只读字节流。
         * \remarks 带一段内部读缓冲：SAX 是逐字节 Peek/Take 的，直接透到
         *          DataStream 会退化成每字节一次虚函数调用。
         */
        class JsonInStream
        {
        public:
            typedef char Ch;

            explicit JsonInStream(DataStream &stream)
                : mStream(stream)
            {
            }

            Ch Peek() const { return fill() ? mBuf[mPos] : '\0'; }

            Ch Take() const
            {
                if (!fill())
                {
                    return '\0';
                }
                ++mCount;
                return mBuf[mPos++];
            }

            size_t Tell() const { return mCount; }

            Ch *PutBegin() { return nullptr; }
            size_t PutEnd(Ch *) { return 0; }

            void Put(Ch) {}
            void Flush() {}

        private:
            /** \brief 确保缓冲里至少还有一个可读字节。 */
            bool fill() const
            {
                if (mPos < mEnd)
                {
                    return true;
                }
                if (mEof)
                {
                    return false;
                }

                mPos = 0;
                mEnd = mStream.read(mBuf, kBufSize);
                if (mEnd == 0)
                {
                    mEof = true;
                    return false;
                }
                return true;
            }

            static const size_t kBufSize = 4096;

            DataStream &mStream;
            mutable char mBuf[kBufSize];
            mutable size_t mPos {0};
            mutable size_t mEnd {0};
            mutable size_t mCount {0};
            mutable bool mEof {false};
        };

        //----------------------------------------------------------------------

        /**
         * \brief RTTR 算术类型 → 落盘标签，与写入端 writeAtomic 的选择严格对应。
         * \return 非算术类型返回 false。
         */
        bool arithmeticKind(const rttr::type &t, ArchiveKind &kind)
        {
            using rttr::type;

            if (!t.is_arithmetic())
            {
                return false;
            }

            if (t == type::get<bool>())
            {
                kind = ArchiveKind::Bool;
            }
            else if (t == type::get<char>() || t == type::get<int8_t>())
            {
                kind = ArchiveKind::I8;
            }
            else if (t == type::get<int16_t>())
            {
                kind = ArchiveKind::I16;
            }
            else if (t == type::get<int32_t>())
            {
                kind = ArchiveKind::I32;
            }
            else if (t == type::get<int64_t>())
            {
                kind = ArchiveKind::I64;
            }
            else if (t == type::get<uint8_t>())
            {
                kind = ArchiveKind::U8;
            }
            else if (t == type::get<uint16_t>())
            {
                kind = ArchiveKind::U16;
            }
            else if (t == type::get<uint32_t>())
            {
                kind = ArchiveKind::U32;
            }
            else if (t == type::get<uint64_t>())
            {
                kind = ArchiveKind::U64;
            }
            else if (t == type::get<float>())
            {
                kind = ArchiveKind::F32;
            }
            else if (t == type::get<double>())
            {
                kind = ArchiveKind::F64;
            }
            else
            {
                // 兜底同写入端：long / size_t 等平台相关整型按 U64
                kind = ArchiveKind::U64;
            }

            return true;
        }

        //----------------------------------------------------------------------

        /**
         * \brief 判定数字载荷的确切宽度。
         * \remarks 优先用落盘的 RTTI 类型名。JSON 的数字本身不带宽度信息，只按
         *          JSON 形态推断会把 float 一律报成 F64，从而逼出一次 double→
         *          float 的收窄转换；而 rttr 的收窄带范围校验，FLT_MAX 这类边界
         *          值会被拒绝，属性就读不进去（SamplerDesc::MaxLOD 曾因此失败）。
         *          用类型名后标签即声明类型，遍历层无需任何转换，与自带 kind 字节
         *          的二进制后端行为完全一致。
         */
        ArchiveKind numberKind(const String &typeName, const JsonToken &token)
        {
            if (!typeName.empty())
            {
                const rttr::type t = rttr::type::get_by_name(typeName);
                ArchiveKind kind = ArchiveKind::Null;
                if (t.is_valid() && arithmeticKind(t, kind))
                {
                    return kind;
                }
            }

            // 类型名不可用（如手写 JSON），或类型是枚举而值写成了数字：退回按
            // JSON 数字形态取能无损表达的最小档，交由遍历层按声明类型转换。
            switch (token.type)
            {
            case JsonToken::Type::Int:    return ArchiveKind::I32;
            case JsonToken::Type::Uint:   return ArchiveKind::U32;
            case JsonToken::Type::Int64:  return ArchiveKind::I64;
            case JsonToken::Type::Uint64: return ArchiveKind::U64;
            default:                      return ArchiveKind::F64;
            }
        }

        //----------------------------------------------------------------------

        /**
         * \brief 判定数组载荷是顺序容器还是关联容器。
         * \remarks JSON 把两者都写成 `[...]`，流式下又无法前瞻首元素形状，只能靠
         *          落盘的 RTTI 类型名判定。判不出按顺序容器处理；若声明类型实际是
         *          关联容器，遍历层会因标签不匹配而告警，不会静默丢数据。
         */
        ArchiveKind containerKind(const String &typeName)
        {
            if (!typeName.empty())
            {
                const rttr::type t = rttr::type::get_by_name(typeName);
                if (t.is_valid() && t.is_associative_container())
                {
                    return ArchiveKind::Map;
                }
            }

            return ArchiveKind::Seq;
        }

        //----------------------------------------------------------------------

        /**
         * \brief 判定对象载荷是普通对象还是 Buffer / UUID。
         * \remarks 后两者的载荷同样是对象（内嵌一层字符串信封），只能靠类型名区分。
         */
        ArchiveKind objectKind(const String &typeName)
        {
            if (!typeName.empty())
            {
                const rttr::type t = rttr::type::get_by_name(typeName);
                if (t == rttr::type::get<Buffer>())
                {
                    return ArchiveKind::Buffer;
                }
                if (t == rttr::type::get<UUID>())
                {
                    return ArchiveKind::Uuid;
                }
            }

            return ArchiveKind::Object;
        }

        //----------------------------------------------------------------------

        /**
         * \brief 由信封的类型名与载荷 token 推断值的类型标签。
         * \remarks token 形态是主判据（决定结构走向），类型名只用于补齐 JSON 表达
         *          不出的信息：数字宽度、Buffer/UUID、关联容器与顺序容器之分。
         */
        ArchiveKind kindOf(const String &typeName, const JsonToken &token)
        {
            switch (token.type)
            {
            case JsonToken::Type::Null:
                return ArchiveKind::Null;
            case JsonToken::Type::Bool:
                return ArchiveKind::Bool;
            case JsonToken::Type::String:
                // 字符串既可能是 std::string 也可能是枚举项名，遍历层对两者等价
                return ArchiveKind::String;
            case JsonToken::Type::Int:
            case JsonToken::Type::Uint:
            case JsonToken::Type::Int64:
            case JsonToken::Type::Uint64:
            case JsonToken::Type::Double:
                return numberKind(typeName, token);
            case JsonToken::Type::StartArray:
                return containerKind(typeName);
            case JsonToken::Type::StartObject:
                return objectKind(typeName);
            default:
                return ArchiveKind::Null;
            }
        }
    }

    //--------------------------------------------------------------------------

    struct JsonArchiveReader::Impl
    {
        explicit Impl(DataStream &stream)
            : is(stream)
        {
        }

        /**
         * \brief 确保前瞻槽里有一个 token。
         * \return 取不到（流已尽或解析出错）返回 false。
         */
        bool ensure()
        {
            if (failed)
            {
                return false;
            }
            if (has)
            {
                return true;
            }

            token.type = JsonToken::Type::None;
            JsonTokenSink sink(token);

            if (!reader.IterativeParseNext<kSaxParseFlags>(is, sink))
            {
                failed = true;
                T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                    "Failed to parse json stream (pos:%zu) ! ERROR [%d]",
                    reader.GetErrorOffset(), (int32_t)reader.GetParseErrorCode());
                return false;
            }

            // 解析成功但没产出 token，说明文档已收尾，视为读到尽头
            if (token.type == JsonToken::Type::None)
            {
                return false;
            }

            has = true;
            return true;
        }

        /**
         * \brief 前瞻当前 token（不消费）。
         * \remarks 判定标签必须经由本函数取 token：token 成员只有在前瞻槽已装填时
         *          才有效，直接读会拿到上一个（已消费的）token。
         * \return 取不到时返回 type 为 None 的空 token，调用方无需额外判空。
         */
        const JsonToken &peekToken()
        {
            if (!ensure())
            {
                token.type = JsonToken::Type::None;
            }
            return token;
        }

        /** \brief 前瞻当前 token 的类型（不消费）。 */
        JsonToken::Type peek() { return peekToken().type; }

        /** \brief 消费当前 token。 */
        void take()
        {
            if (ensure())
            {
                has = false;
            }
        }

        /** \brief 消费一个应为指定类型的 token。类型不符时不消费。 */
        bool take(JsonToken::Type expect)
        {
            if (peek() != expect)
            {
                return false;
            }
            has = false;
            return true;
        }

        /** \brief 消费一个 Key token 并取出键名；当前不是 Key 时返回 false。 */
        bool takeKey(String &out)
        {
            if (peek() != JsonToken::Type::Key)
            {
                return false;
            }
            out = token.str;
            has = false;
            return true;
        }

        /** \brief 消费一个 String token 并取出内容。 */
        bool takeString(String &out)
        {
            if (peek() != JsonToken::Type::String)
            {
                return false;
            }
            out = token.str;
            has = false;
            return true;
        }

        /** \brief 当前 token 是否为数字。 */
        bool isNumber()
        {
            switch (peek())
            {
            case JsonToken::Type::Int:
            case JsonToken::Type::Uint:
            case JsonToken::Type::Int64:
            case JsonToken::Type::Uint64:
            case JsonToken::Type::Double:
                return true;
            default:
                return false;
            }
        }

        /** \brief 深度配平地跳过一个完整 JSON 值（含其全部子结构）。 */
        void skipOne()
        {
            const JsonToken::Type first = peek();
            if (first == JsonToken::Type::None)
            {
                return;
            }

            if (first != JsonToken::Type::StartObject
                && first != JsonToken::Type::StartArray)
            {
                has = false;
                return;
            }

            size_t depth = 0;
            for (;;)
            {
                const JsonToken::Type cur = peek();
                if (cur == JsonToken::Type::None)
                {
                    return;
                }

                if (cur == JsonToken::Type::StartObject
                    || cur == JsonToken::Type::StartArray)
                {
                    ++depth;
                }
                else if (cur == JsonToken::Type::EndObject
                    || cur == JsonToken::Type::EndArray)
                {
                    --depth;
                }

                has = false;

                if (depth == 0)
                {
                    return;
                }
            }
        }

        /**
         * \brief 消费一个应为指定名字的键。
         * \remarks 名字不符只告警不中断：流式下值的位置已由键序确定，继续读还能
         *          拿到数据，硬失败反而会丢掉整个对象。
         */
        void expectKey(const char *expect)
        {
            String key;
            if (!takeKey(key))
            {
                fail("Expect a key in json map entry");
                return;
            }

            if (key != expect)
            {
                T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                    "Unexpected key [%s] in json map entry, expect [%s].",
                    key.c_str(), expect);
            }
        }

        /**
         * \brief 读出内嵌在 Buffer / UUID 载荷里的字符串。
         * \remarks 两者的载荷都是一层 {RTTI_Type:"std::string", RTTI_Value:"..."}
         *          信封，与写入端的 writeBuffer / writeUuid 对应。
         */
        String innerString()
        {
            String value;

            if (!take(JsonToken::Type::StartObject))
            {
                fail("Expect an object payload for Buffer/UUID value");
                return value;
            }

            String key;
            while (takeKey(key))
            {
                if (key == kValueKey && takeString(value))
                {
                    continue;
                }
                skipOne();
            }

            if (!take(JsonToken::Type::EndObject))
            {
                fail("Unbalanced Buffer/UUID payload");
            }

            return value;
        }

        /**
         * \brief 标记读取失败。
         * \remarks 流式解析一旦与预期结构错位就无法回退重试，因此置位后所有后续
         *          操作都退化为空操作，让遍历层尽快收敛，而不是继续啃错位的字节。
         */
        void fail(const char *what)
        {
            if (!failed)
            {
                T3D_LOG_ERROR(LOG_TAG_SERIALIZE, "%s (pos:%zu) !", what,
                    is.Tell());
                failed = true;
            }
        }

        JsonInStream is;
        rapidjson::Reader reader;
        /** 1 个 token 的前瞻槽，has 为 true 时有效。 */
        JsonToken token;
        bool has {false};
        bool failed {false};
        /** 当前值信封的 RTTI 类型名：readKind 写入，紧随其后的 beginObject 取用。 */
        String typeName;
    };

    //--------------------------------------------------------------------------

    JsonArchiveReader::JsonArchiveReader(DataStream &stream)
        : mImpl(T3D_NEW Impl(stream))
    {
    }

    //--------------------------------------------------------------------------

    JsonArchiveReader::~JsonArchiveReader()
    {
        T3D_DELETE mImpl;
        mImpl = nullptr;
    }

    //--------------------------------------------------------------------------

    TResult JsonArchiveReader::readHeader()
    {
        mImpl->reader.IterativeParseInit();

        if (!mImpl->take(JsonToken::Type::StartObject))
        {
            T3D_LOG_ERROR(LOG_TAG_SERIALIZE, "Json root is not an object !");
            return T3D_ERR_PARSE_STREAM;
        }

        bool magic = false;
        bool version = false;
        bool object = false;

        // 按存储顺序扫描顶层键，遇到 Object 即停（其载荷交给遍历层读）。对这几个
        // 键的先后顺序不敏感，未识别的键连值一起跳过。
        String key;
        while (mImpl->takeKey(key))
        {
            if (key == kObjectKey)
            {
                object = true;
                break;
            }

            if (key == kMagicKey)
            {
                magic = true;
            }
            else if (key == kVersionKey)
            {
                version = true;
            }

            // 只校验存在性、不比对取值，与改造前一致：避免因版本号字面量变化拒读
            // 既有资产。
            mImpl->skipOne();
        }

        if (!magic)
        {
            T3D_LOG_ERROR(LOG_TAG_SERIALIZE, "Missing magic field !");
            return T3D_ERR_MISSING_MAGIC;
        }
        if (!version)
        {
            T3D_LOG_ERROR(LOG_TAG_SERIALIZE, "Missing version field !");
            return T3D_ERR_MISSING_VERSION;
        }
        if (!object)
        {
            T3D_LOG_ERROR(LOG_TAG_SERIALIZE, "Missing object contain !");
            return T3D_ERR_MISSING_OBJECT;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ArchiveKind JsonArchiveReader::readKind()
    {
        mImpl->typeName.clear();

        if (!mImpl->take(JsonToken::Type::StartObject))
        {
            mImpl->fail("Malformed json value envelope (expect an object)");
            return ArchiveKind::Null;
        }

        // 信封形如 {RTTI_Type, RTTI_Value}。流式下只能按序读：扫描键直到
        // RTTI_Value，其载荷留给随后的叶子/结构读取去消费。
        bool located = false;
        String key;
        while (mImpl->takeKey(key))
        {
            if (key == kValueKey)
            {
                located = true;
                break;
            }

            if (key == kTypeKey && mImpl->takeString(mImpl->typeName))
            {
                continue;
            }

            mImpl->skipOne();
        }

        if (!located)
        {
            // 此刻停在信封的收尾 token 上，endValue 仍能正常配平
            T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                "Missing RTTI_Value in json value envelope !");
            return ArchiveKind::Null;
        }

        // 载荷 token 到这一步才被读入前瞻槽，标签由它与类型名共同判定
        const ArchiveKind kind = kindOf(mImpl->typeName, mImpl->peekToken());

        if (kind == ArchiveKind::Null)
        {
            // 接口没有单独的"读 null"调用，空值载荷在此就地消费，使 endValue
            // 总能停在信封的收尾 token 上。
            mImpl->take();
        }

        return kind;
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::endValue()
    {
        if (!mImpl->take(JsonToken::Type::EndObject))
        {
            mImpl->fail("Unbalanced json value envelope");
        }
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::skipValue()
    {
        mImpl->skipOne();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::skipPayload(ArchiveKind kind)
    {
        // Null 的载荷已在 readKind 里就地消费，其余载荷此刻仍停在前瞻槽上
        if (kind != ArchiveKind::Null)
        {
            mImpl->skipOne();
        }
    }

    //--------------------------------------------------------------------------

    bool JsonArchiveReader::readBool()
    {
        const JsonToken &token = mImpl->peekToken();
        const bool value = (token.type == JsonToken::Type::Bool)
            ? token.boolean : false;
        mImpl->take();
        return value;
    }

    //--------------------------------------------------------------------------

    int64_t JsonArchiveReader::readInt(ArchiveKind kind)
    {
        // kind 由声明类型推得，JSON 里的实际数字形态未必与之相符（手改过的资产），
        // 因此按实际形态取值再交给遍历层收窄，避免静默读成 0。
        (void)kind;
        const JsonToken &token = mImpl->peekToken();
        const int64_t value = mImpl->isNumber() ? token.i64 : 0;
        mImpl->take();
        return value;
    }

    //--------------------------------------------------------------------------

    uint64_t JsonArchiveReader::readUint(ArchiveKind kind)
    {
        (void)kind;
        const JsonToken &token = mImpl->peekToken();
        const uint64_t value = mImpl->isNumber() ? token.u64 : 0;
        mImpl->take();
        return value;
    }

    //--------------------------------------------------------------------------

    double JsonArchiveReader::readReal(ArchiveKind kind)
    {
        (void)kind;
        const JsonToken &token = mImpl->peekToken();
        const double value = mImpl->isNumber() ? token.dbl : 0.0;
        mImpl->take();
        return value;
    }

    //--------------------------------------------------------------------------

    String JsonArchiveReader::readString()
    {
        const JsonToken &token = mImpl->peekToken();
        String value;
        if (token.type == JsonToken::Type::String)
        {
            value = token.str;
        }
        mImpl->take();
        return value;
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::readBuffer(Buffer &out)
    {
        const String decoded = base64_decode(mImpl->innerString());

        T3D_POD_SAFE_DELETE_ARRAY(out.Data);
        out.Data = !decoded.empty()
            ? T3D_POD_NEW_ARRAY(uint8_t, decoded.size()) : nullptr;
        out.DataSize = decoded.size();

        if (!decoded.empty())
        {
            memcpy(out.Data, decoded.data(), decoded.size());
        }
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::readUuid(UUID &out)
    {
        out.fromString(mImpl->innerString());
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::beginObject(String &typeName)
    {
        typeName = mImpl->typeName;

        if (!mImpl->take(JsonToken::Type::StartObject))
        {
            mImpl->fail("Expect an object payload");
        }
    }

    //--------------------------------------------------------------------------

    bool JsonArchiveReader::nextProperty(String &name)
    {
        // 读到对象收尾 token 时 takeKey 返回 false，天然终止属性拉取
        return mImpl->takeKey(name);
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::endObject()
    {
        if (!mImpl->take(JsonToken::Type::EndObject))
        {
            mImpl->fail("Unbalanced json object payload");
        }
    }

    //--------------------------------------------------------------------------

    uint64_t JsonArchiveReader::beginArray()
    {
        if (!mImpl->take(JsonToken::Type::StartArray))
        {
            mImpl->fail("Expect an array payload");
        }

        // 流式下元素个数不可预知，遍历层按需增长容器
        return 0;
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::endArray()
    {
        if (!mImpl->take(JsonToken::Type::EndArray))
        {
            mImpl->fail("Unbalanced json array payload");
        }
    }

    //--------------------------------------------------------------------------

    bool JsonArchiveReader::nextElement()
    {
        const JsonToken::Type type = mImpl->peek();
        return type != JsonToken::Type::EndArray
            && type != JsonToken::Type::None;
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::beginMap()
    {
        // 关联容器与顺序容器在 JSON 里同为数组
        beginArray();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::endMap()
    {
        endArray();
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::beginMapEntry()
    {
        if (!mImpl->take(JsonToken::Type::StartObject))
        {
            mImpl->fail("Expect an object for json map entry");
        }
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::readMapKeyTag()
    {
        mImpl->expectKey(kMapKeyKey);
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::readMapValueTag()
    {
        mImpl->expectKey(kMapValueKey);
    }

    //--------------------------------------------------------------------------

    void JsonArchiveReader::endMapEntry()
    {
        if (!mImpl->take(JsonToken::Type::EndObject))
        {
            mImpl->fail("Unbalanced json map entry");
        }
    }

    //--------------------------------------------------------------------------
}
