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

#ifndef __T3D_JSON_ARCHIVE_H__
#define __T3D_JSON_ARCHIVE_H__


#include "Serializer/T3DSerializeArchive.h"
#include "IO/T3DDataStream.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    //
    // T3D JSON 文本格式：
    //
    // { "Magic":"T3D", "Version":"0.0.1", "Object": <Value> }
    //
    // 每个 <Value> 都被包进一层 {RTTI_Type, RTTI_Value} 信封：
    //   标量     : { "RTTI_Type":"int32_t",  "RTTI_Value": 5 }
    //   对象     : { "RTTI_Type":"Tiny3D::X","RTTI_Value": { "prop": <Value> } }
    //   顺序容器 : { "RTTI_Type":"...",      "RTTI_Value": [ <Value>, ... ] }
    //   关联容器 : { "RTTI_Type":"...",      "RTTI_Value":
    //                [ { "RTTI_Map_Key": <Value>,
    //                    "RTTI_Map_Value": <Value> }, ... ] }
    //   set 容器 : RTTI_Value 直接是 [ <Value>, ... ]
    //
    // 版本与兼容策略（与 T3DBinArchive.h 一致）：
    //   - kVersionStr 描述的是**容器编码**（头部三键、信封两键、键值对条目的键名
    //     与嵌套形状），与资产内容的 schema 无关。
    //   - 属性增删改名等 schema 变化**不需要**升版本，由共享遍历层的前向兼容规则
    //     承担（见 T3DRttrArchive.h 的行为基线）。
    //   - readHeader 只校验 Magic/Version/Object 三个键**存在**，不比对版本字面量，
    //     以免旧资产因版本号变化被拒读。真要按版本分叉解码时才需要改这里。
    //   - JSON 表达不出的信息靠 RTTI_Type 名补：数字宽度、顺序容器与关联容器之分。
    //     因此**类型名不是装饰，改动 RTTR 注册名会影响可读性**。
    //
    //--------------------------------------------------------------------------

    namespace JsonArchiveIO
    {
        constexpr const char *kMagicKey    = "Magic";
        constexpr const char *kMagicValue  = "T3D";
        constexpr const char *kVersionKey  = "Version";
        /// 容器编码版本，语义见上方"版本与兼容策略"
        constexpr const char *kVersionStr  = "0.0.1";
        constexpr const char *kObjectKey   = "Object";

        constexpr const char *kTypeKey     = "RTTI_Type";
        constexpr const char *kValueKey    = "RTTI_Value";
        constexpr const char *kMapKeyKey   = "RTTI_Map_Key";
        constexpr const char *kMapValueKey = "RTTI_Map_Value";
    }

    //--------------------------------------------------------------------------

    /**
     * \brief JSON 文本写入归档后端。
     * \remarks rapidjson 依赖完全隐藏在实现文件里，避免污染引擎公共头。
     */
    class T3D_ENGINE_API JsonArchiveWriter : public IArchiveWriter
    {
    public:
        explicit JsonArchiveWriter(DataStream &stream);
        ~JsonArchiveWriter() override;

        void beginDocument() override;
        void endDocument() override;

        void beginValue() override;
        void endValue() override;

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
        void endObject() override;

        void beginArray(const rttr::string_view &typeName,
            size_t count) override;
        void endArray() override;

        void beginMap(const rttr::string_view &typeName, bool keyOnly,
            size_t count) override;
        void endMap() override;

        void beginMapEntry() override;
        void writeMapKeyTag() override;
        void writeMapValueTag() override;
        void endMapEntry() override;

    private:
        struct Impl;
        Impl *mImpl {nullptr};
    };

    //--------------------------------------------------------------------------

    /**
     * \brief JSON 文本读取归档后端（真流式，不物化 DOM）。
     *
     * \remarks 用 `rapidjson::Reader` 的 `IterativeParseNext` 逐 token 拉取——它
     *          每次调用恰好触发一次 SAX 回调，由此把 push 式的 SAX 反转成 pull
     *          式的 token 流。内部只保留 **1 个 token 的前瞻槽**，配合按存储顺序
     *          拉取成员/元素，即可满足 IArchiveReader 的前向单遍语义。
     *
     *          峰值内存由此从「整棵 DOM + 字符串副本」降到「一段读缓冲 + 一个
     *          token + RTTR 对象图本身」。
     *
     *          代价是对手改过的 JSON 更严格：信封内必须 `RTTI_Type` 先于
     *          `RTTI_Value`，键值对条目内必须键先于值（流式无法回退定位）。顶层
     *          的 Magic/Version/Object 与信封内的多余键仍可乱序/跳过。
     *
     *          rapidjson 依赖完全隐藏在实现文件里，避免污染引擎公共头。
     */
    class T3D_ENGINE_API JsonArchiveReader : public IArchiveReader
    {
    public:
        explicit JsonArchiveReader(DataStream &stream);
        ~JsonArchiveReader() override;

        TResult readHeader() override;

        ArchiveKind readKind() override;
        void endValue() override;

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

        void beginMapEntry() override;
        void readMapKeyTag() override;
        void readMapValueTag() override;
        void endMapEntry() override;

    private:
        struct Impl;
        Impl *mImpl {nullptr};
    };
}


#endif    /*__T3D_JSON_ARCHIVE_H__*/
