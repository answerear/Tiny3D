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

    /**
     * \brief JSON 文本格式的键名与版本常量。
     */
    namespace JsonArchiveIO
    {
        /// 顶层 Magic 键名。
        constexpr const char *kMagicKey    = "Magic";
        /// 顶层 Magic 取值。
        constexpr const char *kMagicValue  = "T3D";
        /// 顶层 Version 键名。
        constexpr const char *kVersionKey  = "Version";
        /// 容器编码版本字面量，语义见文件头注释中的「版本与兼容策略」。
        constexpr const char *kVersionStr  = "0.0.1";
        /// 顶层根对象键名。
        constexpr const char *kObjectKey   = "Object";

        /// 值信封中的 RTTI 类型键名。
        constexpr const char *kTypeKey     = "RTTI_Type";
        /// 值信封中的 RTTI 值键名。
        constexpr const char *kValueKey    = "RTTI_Value";
        /// 键值对条目中的 key 侧键名。
        constexpr const char *kMapKeyKey   = "RTTI_Map_Key";
        /// 键值对条目中的 value 侧键名。
        constexpr const char *kMapValueKey = "RTTI_Map_Value";
    }

    //--------------------------------------------------------------------------

    /**
     * \brief JSON 文本写入归档后端。
     * \remarks rapidjson 依赖完全隐藏在实现文件的 Impl 中，避免污染引擎公共头。
     */
    class T3D_ENGINE_API JsonArchiveWriter : public IArchiveWriter
    {
    public:
        /**
         * \brief 构造写入后端。
         * \param [in,out] stream : 输出数据流，生命周期须覆盖本对象
         */
        explicit JsonArchiveWriter(DataStream &stream);

        /// 释放 Impl。
        ~JsonArchiveWriter() override;

        /// 写顶层 {Magic, Version, Object} 并进入 Object 值位置。
        void beginDocument() override;
        /// 收尾根对象。
        void endDocument() override;

        /// 进入 {RTTI_Type, RTTI_Value} 值信封。
        void beginValue() override;
        /// 离开值信封。
        void endValue() override;

        /// 写 null 值信封。
        void writeNull(const rttr::string_view &typeName) override;
        /// 写布尔值信封。
        void writeBool(const rttr::string_view &typeName, bool value) override;
        /// 写整数信封（I64 用 Int64，其余用 Int）。
        void writeInt(const rttr::string_view &typeName, ArchiveKind kind,
            int64_t value) override;
        /// 写无符号整数信封（U64 用 Uint64，其余用 Uint）。
        void writeUint(const rttr::string_view &typeName, ArchiveKind kind,
            uint64_t value) override;
        /// 写浮点数信封。
        void writeReal(const rttr::string_view &typeName, ArchiveKind kind,
            double value) override;
        /// 写字符串信封。
        void writeString(const rttr::string_view &typeName,
            const String &value) override;
        /// 写枚举信封（名称优先，否则数值或 null）。
        void writeEnum(const rttr::string_view &typeName, const String &name,
            bool nameValid, uint64_t value, bool valueValid) override;

        /// 写 Buffer 信封（base64 内嵌字符串子信封）。
        void writeBuffer(const uint8_t *data, size_t size) override;
        /// 写 UUID 信封（字符串子信封）。
        void writeUuid(const UUID &uuid) override;

        /// 写对象信封并进入 RTTI_Value 对象体。
        void beginObject(const rttr::string_view &typeName,
            size_t propertyCount) override;
        /// 写属性名键。
        void writePropertyName(const rttr::string_view &name) override;
        /// 离开对象体。
        void endObject() override;

        /// 写顺序容器信封并进入数组。
        void beginArray(const rttr::string_view &typeName,
            size_t count) override;
        /// 离开数组。
        void endArray() override;

        /// 写关联/set 容器信封并进入数组（propertyCount/keyOnly 被忽略）。
        void beginMap(const rttr::string_view &typeName, bool keyOnly,
            size_t count) override;
        /// 离开数组。
        void endMap() override;

        /// 进入键值对条目对象。
        void beginMapEntry() override;
        /// 写 RTTI_Map_Key 键。
        void writeMapKeyTag() override;
        /// 写 RTTI_Map_Value 键。
        void writeMapValueTag() override;
        /// 离开键值对条目对象。
        void endMapEntry() override;

    private:
        /// Pimpl：持有 rapidjson PrettyWriter 与输出流桥接。
        struct Impl;
        /// 写入实现，构造时分配、析构时释放。
        Impl *mImpl {nullptr};
    };

    //--------------------------------------------------------------------------

    /**
     * \brief JSON 文本读取归档后端（真流式，不物化 DOM）。
     *
     * \remarks 用 rapidjson::Reader 的 IterativeParseNext 逐 token 拉取，
     *          内部只保留 1 个 token 的前瞻槽。峰值内存为「读缓冲 + 一个 token +
     *          RTTR 对象图本身」。信封内须 RTTI_Type 先于 RTTI_Value，键值对条目
     *          内须键先于值。rapidjson 依赖隐藏在 Impl 中。
     */
    class T3D_ENGINE_API JsonArchiveReader : public IArchiveReader
    {
    public:
        /**
         * \brief 构造读取后端。
         * \param [in,out] stream : 输入数据流，生命周期须覆盖本对象
         */
        explicit JsonArchiveReader(DataStream &stream);

        /// 释放 Impl。
        ~JsonArchiveReader() override;

        /**
         * \brief 读并校验顶层 Magic/Version/Object 键存在性。
         * \return 成功返回 T3D_OK；根非对象或缺键返回对应错误码
         * \remarks 不比对 Version 字面量取值。
         */
        TResult readHeader() override;

        /// 进入值信封，读 RTTI_Type，返回由类型名与载荷 token 推断的 ArchiveKind。
        ArchiveKind readKind() override;
        /// 消费值信封的 EndObject。
        void endValue() override;

        /// 深度跳过当前 JSON 值（含子结构）。
        void skipValue() override;
        /// 跳过载荷 token（Null 已在 readKind 消费）。
        void skipPayload(ArchiveKind kind) override;

        /// 读布尔 token。
        bool readBool() override;
        /// 读整数 token（按实际 JSON 数字形态取 i64）。
        int64_t readInt(ArchiveKind kind) override;
        /// 读无符号整数 token（按实际 JSON 数字形态取 u64）。
        uint64_t readUint(ArchiveKind kind) override;
        /// 读浮点 token。
        double readReal(ArchiveKind kind) override;
        /// 读字符串 token。
        String readString() override;

        /// 读 base64 内嵌字符串并解码到 out。
        void readBuffer(Buffer &out) override;
        /// 读内嵌字符串并解析为 UUID。
        void readUuid(UUID &out) override;

        /// 取 readKind 缓存的类型名并进入对象载荷。
        void beginObject(String &typeName) override;
        /// 拉取下一个 Key token 作为属性名。
        bool nextProperty(String &name) override;
        /// 消费对象载荷的 EndObject。
        void endObject() override;

        /// 进入数组载荷；返回 0（元素个数不可预知）。
        uint64_t beginArray() override;
        /// 消费数组的 EndArray。
        void endArray() override;

        /// 前瞻非 EndArray/None 时返回 true。
        bool nextElement() override;

        /// 与 beginArray 等价（关联容器同为数组载荷）。
        void beginMap() override;
        /// 与 endArray 等价。
        void endMap() override;

        /// 进入键值对条目对象。
        void beginMapEntry() override;
        /// 消费并校验 RTTI_Map_Key 键。
        void readMapKeyTag() override;
        /// 消费并校验 RTTI_Map_Value 键。
        void readMapValueTag() override;
        /// 消费条目对象的 EndObject。
        void endMapEntry() override;

    private:
        /// Pimpl：持有 rapidjson Reader、1-token 前瞻槽与读流桥接。
        struct Impl;
        /// 读取实现，构造时分配、析构时释放。
        Impl *mImpl {nullptr};
    };
}


#endif    /*__T3D_JSON_ARCHIVE_H__*/
