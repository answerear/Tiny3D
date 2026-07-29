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

#ifndef __T3D_BIN_ARCHIVE_H__
#define __T3D_BIN_ARCHIVE_H__


#include "Serializer/T3DSerializeArchive.h"
#include "IO/T3DDataStream.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    //
    // T3DBin v1 二进制格式：
    //
    // [Header]
    //   magic    : 'T','3','D','B'
    //   version  : uint16 LE
    //   flags    : uint16 LE (预留)
    //   reserved : uint32 LE = 0
    // [RootValue]  等价 JSON 的 Object 节点，字符串内联（无 StringTable）。
    //
    // 每个 Value = kind:uint8 (ArchiveKind) + payload，完全自描述且自定界，
    // 因此可前向单遍读取与跳过，无需回退定位。
    //
    // 版本与兼容策略：
    //   - kVersion 描述的是**容器编码**（头部布局、kind 字节编号、varint/字符串
    //     等原语的编码方式），与资产内容的 schema 无关。
    //   - 属性增删改名、类型替换等 schema 变化**不需要**升版本：读取由声明类型
    //     驱动，未知属性前向跳过、改名走 SERIALIZE_ALIAS 回退、标签与声明类型
    //     不符则丢弃载荷并告警（见 T3DRttrArchive.h 的行为基线）。
    //   - 只有改动上述容器编码本身才升 kVersion；ArchiveKind 的枚举值即落盘字节，
    //     新增标签只能追加在末尾。
    //   - readHeader 目前只校验 magic 与"版本可读出"，不比对具体版本号。真正需要
    //     按版本分叉解码时，用 BinArchiveReader::getVersion() 取值。
    //
    //--------------------------------------------------------------------------

    namespace BinArchiveIO
    {
        constexpr uint8_t  kMagic0 = 'T';
        constexpr uint8_t  kMagic1 = '3';
        constexpr uint8_t  kMagic2 = 'D';
        constexpr uint8_t  kMagic3 = 'B';
        /// 容器编码版本，语义见上方"版本与兼容策略"
        constexpr uint16_t kVersion = 1;

        //----------------------------------------------------------------------
        // 低层编解码原语
        // 不复用 DataStream::operator<<(String)，因其读写不可往返。
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

        inline void writeKind(DataStream &s, ArchiveKind kind)
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

        inline ArchiveKind readKind(DataStream &s)
        {
            return static_cast<ArchiveKind>(readU8(s));
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

        inline void writeHeader(DataStream &s)
        {
            uint8_t magic[4] = { kMagic0, kMagic1, kMagic2, kMagic3 };
            s.write(magic, sizeof(magic));
            writePod<uint16_t>(s, kVersion);
            writePod<uint16_t>(s, 0);   // flags
            writePod<uint32_t>(s, 0);   // reserved
        }

        /**
         * \brief 校验并消费二进制文件头。
         * \param [out] version : 读出的格式版本号。
         * \return 成功返回 T3D_OK。
         */
        T3D_ENGINE_API TResult readHeader(DataStream &s, uint16_t &version);
    }

    //--------------------------------------------------------------------------

    /**
     * \brief T3DB 二进制写入归档后端。
     * \remarks 纯流式：所有结构事件即时落盘，无中间缓存。信封类事件
     *          （beginValue / beginMapEntry / MapTag）为空操作，值的类型信息
     *          由 kind 字节承载。
     */
    class T3D_ENGINE_API BinArchiveWriter : public IArchiveWriter
    {
    public:
        explicit BinArchiveWriter(DataStream &stream)
            : mStream(stream)
        {
        }

        void beginDocument() override;
        void endDocument() override {}

        void beginValue() override {}
        void endValue() override {}

        void writeNull(const rttr::string_view &typeName) override;
        void writeBool(const rttr::string_view &typeName, bool value) override;
        void writeInt(const rttr::string_view &typeName, ArchiveKind kind,
            int64_t value) override;
        void writeUint(const rttr::string_view &typeName, ArchiveKind kind,
            uint64_t value) override;
        void writeReal(const rttr::string_view &typeName, ArchiveKind kind,
            double value) override;
        void writeString(const rttr::string_view &typeName,
            const String &value) override;
        void writeEnum(const rttr::string_view &typeName, const String &name,
            bool nameValid, uint64_t value, bool valueValid) override;

        void writeBuffer(const uint8_t *data, size_t size) override;
        void writeUuid(const UUID &uuid) override;

        void beginObject(const rttr::string_view &typeName,
            size_t propertyCount) override;
        void writePropertyName(const rttr::string_view &name) override;
        void endObject() override {}

        void beginArray(const rttr::string_view &typeName,
            size_t count) override;
        void endArray() override {}

        void beginMap(const rttr::string_view &typeName, bool keyOnly,
            size_t count) override;
        void endMap() override {}

        void beginMapEntry() override {}
        void writeMapKeyTag() override {}
        void writeMapValueTag() override {}
        void endMapEntry() override {}

    private:
        DataStream &mStream;
    };

    //--------------------------------------------------------------------------

    /**
     * \brief T3DB 二进制读取归档后端。
     * \remarks 纯流式、零物化：始终只前向消费字节，不构建任何中间树，因此内存
     *          占用与文件大小无关。信封类事件（endValue / beginMapEntry /
     *          MapTag / end*）为空操作，值的类型信息由 kind 字节承载。
     *
     *          唯一的状态是对象属性剩余计数栈：磁盘上对象体以「属性个数 + 若干
     *          (名字, 值)」编码，而接口按 nextProperty 拉取，故需按嵌套深度记录
     *          各层还剩多少属性。
     */
    class T3D_ENGINE_API BinArchiveReader : public IArchiveReader
    {
    public:
        explicit BinArchiveReader(DataStream &stream)
            : mStream(stream)
        {
        }

        TResult readHeader() override;

        ArchiveKind readKind() override;
        void endValue() override {}

        void skipValue() override;
        void skipPayload(ArchiveKind kind) override;

        bool readBool() override;
        int64_t readInt(ArchiveKind kind) override;
        uint64_t readUint(ArchiveKind kind) override;
        double readReal(ArchiveKind kind) override;
        String readString() override;

        void readBuffer(Buffer &out) override;
        void readUuid(UUID &out) override;

        void beginObject(String &typeName) override;
        bool nextProperty(String &name) override;
        void endObject() override;

        uint64_t beginArray() override;
        void endArray() override;

        bool nextElement() override;

        void beginMap() override;
        void endMap() override;

        void beginMapEntry() override {}
        void readMapKeyTag() override {}
        void readMapValueTag() override {}
        void endMapEntry() override {}

        /** \brief 文件头中读出的格式版本号，readHeader 成功后有效。 */
        uint16_t getVersion() const { return mVersion; }

    private:
        /** \brief 一个容器层级的剩余元素状态。 */
        struct ElemFrame
        {
            uint64_t remain {0};
            /** 每个元素占几个值：顺序容器与 set 为 1，键值对容器为 2。 */
            uint32_t valuesPerElem {1};
        };

        /** \brief 离开容器：补齐未读元素以保持流对齐。 */
        void endContainer();

        DataStream &mStream;
        /** 各嵌套层级对象体中尚未读取的属性个数。 */
        TArray<uint64_t> mRemainProps;
        /** 各嵌套层级容器中尚未读取的元素状态。 */
        TArray<ElemFrame> mRemainElems;
        uint16_t mVersion{0};
    };
}


#endif    /*__T3D_BIN_ARCHIVE_H__*/
