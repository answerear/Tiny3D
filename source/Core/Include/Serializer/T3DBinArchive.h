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
    //   - readHeader 校验 magic 且版本须在 (0, kVersion] 范围内。按版本分叉解码时
    //     用 BinArchiveReader::getVersion() 取值。
    //
    //--------------------------------------------------------------------------

    /**
     * \brief T3DB 二进制格式的低层编解码原语与常量。
     */
    namespace BinArchiveIO
    {
        /// Magic 字节 'T'。
        constexpr uint8_t  kMagic0 = 'T';
        /// Magic 字节 '3'。
        constexpr uint8_t  kMagic1 = '3';
        /// Magic 字节 'D'。
        constexpr uint8_t  kMagic2 = 'D';
        /// Magic 字节 'B'。
        constexpr uint8_t  kMagic3 = 'B';
        /// 容器编码版本，语义见文件头注释中的「版本与兼容策略」。
        constexpr uint16_t kVersion = 1;

        /// 向流写入 n 字节；n 为 0 时不写。
        inline void writeBytes(DataStream &s, const void *p, size_t n)
        {
            if (n > 0)
            {
                s.write(const_cast<void *>(p), n);
            }
        }

        /// 向流写入 POD 值。
        template <typename T>
        inline void writePod(DataStream &s, T v)
        {
            s.write(&v, sizeof(T));
        }

        /// 向流写入单字节。
        inline void writeU8(DataStream &s, uint8_t v)
        {
            s.write(&v, sizeof(v));
        }

        /// 向流写入 ArchiveKind 标签字节。
        inline void writeKind(DataStream &s, ArchiveKind kind)
        {
            writeU8(s, static_cast<uint8_t>(kind));
        }

        /// 向流写入无符号 varint。
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

        /// 向流写入长度前缀字符串（varint 长度 + 裸字节）。
        inline void writeString(DataStream &s, const char *p, size_t len)
        {
            writeUVarint(s, static_cast<uint64_t>(len));
            writeBytes(s, p, len);
        }

        /// 向流写入 std::string。
        inline void writeString(DataStream &s, const std::string &str)
        {
            writeString(s, str.data(), str.size());
        }

        /// 向流写入 rttr::string_view。
        inline void writeString(DataStream &s, const rttr::string_view &str)
        {
            writeString(s, str.data(), str.length());
        }

        /// 从流读取单字节。
        inline uint8_t readU8(DataStream &s)
        {
            uint8_t v = 0;
            s.read(&v, sizeof(v));
            return v;
        }

        /// 从流读取 ArchiveKind 标签字节。
        inline ArchiveKind readKind(DataStream &s)
        {
            return static_cast<ArchiveKind>(readU8(s));
        }

        /// 从流读取 POD 值。
        template <typename T>
        inline T readPod(DataStream &s)
        {
            T v{};
            s.read(&v, sizeof(T));
            return v;
        }

        /// 从流读取无符号 varint。
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

        /// 从流读取长度前缀字符串。
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

        /// 相对 seek 跳过 n 字节。
        inline void skipBytes(DataStream &s, uint64_t n)
        {
            if (n > 0)
            {
                s.seek(static_cast<long_t>(n), true);
            }
        }

        /// 写出 T3DB 文件头（magic + version + flags + reserved）。
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
         * \param [in,out] s       : 输入数据流
         * \param [out] version    : 读出的格式版本号
         * \return 成功返回 T3D_OK；magic 错误返回 T3D_ERR_MISSING_MAGIC；
         *         版本为 0 或大于 kVersion 返回 T3D_ERR_MISSING_VERSION；
         *         读不足 4 字节 magic 返回 T3D_ERR_PARSE_STREAM
         */
        T3D_ENGINE_API TResult readHeader(DataStream &s, uint16_t &version);
    }

    //--------------------------------------------------------------------------

    /**
     * \brief T3DB 二进制写入归档后端。
     * \remarks 纯流式：结构事件即时落盘，无中间缓存。信封类事件
     *          （beginValue / beginMapEntry / MapTag / end*）多为空操作，
     *          值的类型信息由 kind 字节承载。
     */
    class T3D_ENGINE_API BinArchiveWriter : public IArchiveWriter
    {
    public:
        /**
         * \brief 构造写入后端。
         * \param [in,out] stream : 输出数据流，生命周期须覆盖本对象
         */
        explicit BinArchiveWriter(DataStream &stream)
            : mStream(stream)
        {
        }

        /// 写 T3DB 文件头。
        void beginDocument() override;
        /// 空操作。
        void endDocument() override {}

        /// 空操作（kind 字节在 write* 中写出）。
        void beginValue() override {}
        /// 空操作。
        void endValue() override {}

        /// 写 Null kind。
        void writeNull(const rttr::string_view &typeName) override;
        /// 写 Bool kind + 1 字节。
        void writeBool(const rttr::string_view &typeName, bool value) override;
        /// 写整数 kind + 对应宽度 POD。
        void writeInt(const rttr::string_view &typeName, ArchiveKind kind,
            int64_t value) override;
        /// 写无符号整数 kind + 对应宽度 POD。
        void writeUint(const rttr::string_view &typeName, ArchiveKind kind,
            uint64_t value) override;
        /// 写浮点 kind + float/double POD。
        void writeReal(const rttr::string_view &typeName, ArchiveKind kind,
            double value) override;
        /// 写 String kind + varint 长度前缀字符串。
        void writeString(const rttr::string_view &typeName,
            const String &value) override;
        /// 写 Enum kind + 名称或数值的字符串表示。
        void writeEnum(const rttr::string_view &typeName, const String &name,
            bool nameValid, uint64_t value, bool valueValid) override;

        /// 写 Buffer kind + varint 长度 + 裸字节。
        void writeBuffer(const uint8_t *data, size_t size) override;
        /// 写 Uuid kind + 16 字节。
        void writeUuid(const UUID &uuid) override;

        /// 写 Object kind + 类型名 + 属性计数 varint。
        void beginObject(const rttr::string_view &typeName,
            size_t propertyCount) override;
        /// 写属性名字符串。
        void writePropertyName(const rttr::string_view &name) override;
        /// 空操作。
        void endObject() override {}

        /// 写 Seq kind + 元素计数 varint。
        void beginArray(const rttr::string_view &typeName,
            size_t count) override;
        /// 空操作。
        void endArray() override {}

        /// 写 Map kind + keyOnly 标志 + 元素计数 varint。
        void beginMap(const rttr::string_view &typeName, bool keyOnly,
            size_t count) override;
        /// 空操作。
        void endMap() override {}

        /// 空操作。
        void beginMapEntry() override {}
        /// 空操作。
        void writeMapKeyTag() override {}
        /// 空操作。
        void writeMapValueTag() override {}
        /// 空操作。
        void endMapEntry() override {}

    private:
        /// 绑定的输出数据流。
        DataStream &mStream;
    };

    //--------------------------------------------------------------------------

    /**
     * \brief T3DB 二进制读取归档后端。
     * \remarks 纯流式、零物化：始终只前向消费字节。信封类事件多为空操作，
     *          值的类型信息由 kind 字节承载。对象属性剩余计数与容器元素剩余
     *          计数保存在栈中，endObject/endContainer 会补齐未读项以保持流对齐。
     */
    class T3D_ENGINE_API BinArchiveReader : public IArchiveReader
    {
    public:
        /**
         * \brief 构造读取后端。
         * \param [in,out] stream : 输入数据流，生命周期须覆盖本对象
         */
        explicit BinArchiveReader(DataStream &stream)
            : mStream(stream)
        {
        }

        /// 校验并消费 T3DB 文件头，结果写入 mVersion。
        TResult readHeader() override;

        /// 读取并返回下一个值的 kind 字节。
        ArchiveKind readKind() override;
        /// 空操作。
        void endValue() override {}

        /// 读 kind 后按 kind 递归跳过整个值。
        void skipValue() override;
        /// 按 kind 跳过载荷（kind 已由 readKind 消费）。
        void skipPayload(ArchiveKind kind) override;

        /// 读 1 字节布尔载荷。
        bool readBool() override;
        /// 按 kind 宽度读有符号整数载荷。
        int64_t readInt(ArchiveKind kind) override;
        /// 按 kind 宽度读无符号整数载荷。
        uint64_t readUint(ArchiveKind kind) override;
        /// 按 kind 读 float 或 double 载荷。
        double readReal(ArchiveKind kind) override;
        /// 读 varint 长度前缀字符串。
        String readString() override;

        /// 读 varint 长度 + 裸字节，重新分配 out.Data。
        void readBuffer(Buffer &out) override;
        /// 读 16 字节 UUID。
        void readUuid(UUID &out) override;

        /// 读类型名与属性计数，压入 mRemainProps。
        void beginObject(String &typeName) override;
        /// 递减剩余属性计数并读属性名。
        bool nextProperty(String &name) override;
        /// 补齐未读属性并保持流对齐。
        void endObject() override;

        /// 读元素计数 varint，压入 mRemainElems，返回计数作预分配提示。
        uint64_t beginArray() override;
        /// 补齐未读元素并保持流对齐。
        void endArray() override;

        /// 递减剩余元素计数。
        bool nextElement() override;

        /// 读 keyOnly 标志与元素计数，压入 mRemainElems。
        void beginMap() override;
        /// 补齐未读元素并保持流对齐。
        void endMap() override;

        /// 空操作。
        void beginMapEntry() override {}
        /// 空操作。
        void readMapKeyTag() override {}
        /// 空操作。
        void readMapValueTag() override {}
        /// 空操作。
        void endMapEntry() override {}

        /**
         * \brief 获取文件头中读出的格式版本号。
         * \return readHeader 成功后有效
         */
        uint16_t getVersion() const { return mVersion; }

    private:
        /**
         * \brief 一个容器层级的剩余元素状态。
         */
        struct ElemFrame
        {
            /// 该层尚未读取的元素个数。
            uint64_t remain {0};
            /// 每个元素占几个值：顺序容器与 set 为 1，键值对容器为 2。
            uint32_t valuesPerElem {1};
        };

        /**
         * \brief 离开容器：补齐未读元素以保持流对齐。
         */
        void endContainer();

        /// 绑定的输入数据流。
        DataStream &mStream;
        /// 各嵌套层级对象体中尚未读取的属性个数。
        TArray<uint64_t> mRemainProps;
        /// 各嵌套层级容器中尚未读取的元素状态。
        TArray<ElemFrame> mRemainElems;
        /// readHeader 读出的格式版本号。
        uint16_t mVersion{0};
    };
}


#endif    /*__T3D_BIN_ARCHIVE_H__*/
