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

#ifndef __T3D_SERIALIZE_ARCHIVE_H__
#define __T3D_SERIALIZE_ARCHIVE_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief 归档值的自描述类型标签。
     * \remarks 枚举值即 T3DB 二进制格式落盘的 kind 字节，**不可重排或插入**，
     *          新增标签只能追加在末尾，否则破坏已烘焙 bundle 的兼容性。
     */
    enum class ArchiveKind : uint8_t
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

    /**
     * \brief 序列化写入归档后端接口（前向单遍）。
     *
     * \remarks 共享遍历层 RttrArchive 负责所有 RTTR 语义（属性过滤、多态、
     *          生命周期回调等），本接口只负责把「结构事件 + 叶子值」落成具体
     *          格式的字节。调用序列严格深度优先、与落盘顺序一致，实现方无需
     *          回退定位，因此天然支持流式写出。
     *
     *          调用约定（对齐 JSON 的 {RTTI_Type, RTTI_Value} 信封与 T3DB 的
     *          kind 字节两种表达）：
     *
     *          文档       ：beginDocument [值] endDocument
     *          值         ：beginValue <单个叶子/对象/容器> endValue
     *          对象       ：beginObject (writePropertyName [值])* endObject
     *          顺序容器   ：beginArray [值]* endArray
     *          关联容器   ：beginMap ( keyOnly ? [值]*
     *                                : (beginMapEntry writeMapKeyTag [值]
     *                                   writeMapValueTag [值] endMapEntry)* )
     *                       endMap
     *
     *          typeName 参数供文本格式写出 RTTI 类型名；二进制格式除对象外可忽略。
     */
    class T3D_ENGINE_API IArchiveWriter
    {
    public:
        /// 虚析构。
        virtual ~IArchiveWriter() = default;

        /**
         * \brief 写文件头，并进入根值位置。
         */
        virtual void beginDocument() = 0;

        /**
         * \brief 收尾文档结构。
         */
        virtual void endDocument() = 0;

        /**
         * \brief 进入一个值的信封。
         * \remarks 文本格式据此产生 `{}` 包裹；二进制格式为空操作，kind 字节已自带类型信息。
         */
        virtual void beginValue() = 0;

        /**
         * \brief 离开当前值的信封。
         */
        virtual void endValue() = 0;

        /**
         * \brief 写 null 值。
         * \param [in] typeName : RTTI 类型名，文本格式写入 RTTI_Type
         */
        virtual void writeNull(const rttr::string_view &typeName) = 0;

        /**
         * \brief 写布尔值。
         * \param [in] typeName : RTTI 类型名
         * \param [in] value    : 布尔值
         */
        virtual void writeBool(const rttr::string_view &typeName, bool value) = 0;

        /**
         * \brief 写有符号整数。
         * \param [in] typeName : RTTI 类型名
         * \param [in] kind     : 确切宽度（I8/I16/I32/I64）
         * \param [in] value    : 整数值
         */
        virtual void writeInt(const rttr::string_view &typeName, ArchiveKind kind,
            int64_t value) = 0;

        /**
         * \brief 写无符号整数。
         * \param [in] typeName : RTTI 类型名
         * \param [in] kind     : 确切宽度（U8/U16/U32/U64）
         * \param [in] value    : 整数值
         */
        virtual void writeUint(const rttr::string_view &typeName, ArchiveKind kind,
            uint64_t value) = 0;

        /**
         * \brief 写浮点数。
         * \param [in] typeName : RTTI 类型名
         * \param [in] kind     : 确切宽度（F32/F64）
         * \param [in] value    : 浮点值
         */
        virtual void writeReal(const rttr::string_view &typeName, ArchiveKind kind,
            double value) = 0;

        /**
         * \brief 写字符串。
         * \param [in] typeName : RTTI 类型名
         * \param [in] value    : 字符串内容
         */
        virtual void writeString(const rttr::string_view &typeName,
            const String &value) = 0;

        /**
         * \brief 写枚举值。
         * \param [in] typeName  : RTTI 类型名
         * \param [in] name      : 枚举项名称，nameValid 为 true 时有效
         * \param [in] nameValid : name 是否可用
         * \param [in] value     : 枚举底层数值，valueValid 为 true 时有效
         * \param [in] valueValid: value 是否可用
         * \remarks 名称优先；名称不可用时各后端按自身策略退化为数值表达。
         */
        virtual void writeEnum(const rttr::string_view &typeName,
            const String &name, bool nameValid, uint64_t value,
            bool valueValid) = 0;

        /**
         * \brief 写二进制块。
         * \param [in] data : 原始字节指针，可为 nullptr（size 为 0 时）
         * \param [in] size : 字节数
         * \remarks 文本格式转 base64 内嵌字符串信封，二进制格式写 varint 长度 + 裸字节。
         */
        virtual void writeBuffer(const uint8_t *data, size_t size) = 0;

        /**
         * \brief 写 UUID。
         * \param [in] uuid : 待写 UUID
         * \remarks 文本格式转字符串内嵌信封，二进制格式写 16 字节裸数据。
         */
        virtual void writeUuid(const UUID &uuid) = 0;

        /**
         * \brief 进入对象体。
         * \param [in] typeName       : 运行时派生类型名
         * \param [in] propertyCount  : 已过滤后的实际待写属性个数；二进制格式据此写计数前缀
         */
        virtual void beginObject(const rttr::string_view &typeName,
            size_t propertyCount) = 0;

        /**
         * \brief 写对象属性名。
         * \param [in] name : 属性名
         */
        virtual void writePropertyName(const rttr::string_view &name) = 0;

        /**
         * \brief 离开当前对象体。
         */
        virtual void endObject() = 0;

        /**
         * \brief 进入顺序容器。
         * \param [in] typeName : 容器 RTTI 类型名
         * \param [in] count      : 元素个数；二进制格式据此写计数前缀
         */
        virtual void beginArray(const rttr::string_view &typeName,
            size_t count) = 0;

        /**
         * \brief 离开当前顺序容器。
         */
        virtual void endArray() = 0;

        /**
         * \brief 进入关联容器。
         * \param [in] typeName : 容器 RTTI 类型名
         * \param [in] keyOnly  : true 表示 set 类容器（仅 key，无 value 标签）
         * \param [in] count    : 元素个数；二进制格式据此写计数前缀
         */
        virtual void beginMap(const rttr::string_view &typeName, bool keyOnly,
            size_t count) = 0;

        /**
         * \brief 离开当前关联容器。
         */
        virtual void endMap() = 0;

        /**
         * \brief 进入一个键值对条目（仅非 keyOnly 容器调用）。
         */
        virtual void beginMapEntry() = 0;

        /**
         * \brief 写 map 条目的 key 侧标签（文本格式写 RTTI_Map_Key）。
         */
        virtual void writeMapKeyTag() = 0;

        /**
         * \brief 写 map 条目的 value 侧标签（文本格式写 RTTI_Map_Value）。
         */
        virtual void writeMapValueTag() = 0;

        /**
         * \brief 离开当前键值对条目。
         */
        virtual void endMapEntry() = 0;
    };

    /**
     * \brief 序列化读取归档后端接口（前向单遍 pull）。
     *
     * \remarks 与 IArchiveWriter 严格镜像：共享遍历层 RttrArchive 承载所有 RTTR
     *          语义（按声明类型驱动、多态建对象、alias 回退、生命周期回调等），
     *          本接口只负责按落盘顺序把字节解成「结构事件 + 叶子值」。
     *
     *          调用序列与写入端一一对应，只允许前向推进 + 1-token 前瞻
     *          （readKind 本身即前瞻并消费），不需要任意回退定位，因此两个后端
     *          都可以纯流式实现、不物化中间树：
     *
     *          文档       ：readHeader [值]
     *          值         ：readKind <单个叶子/对象/容器> endValue
     *          对象       ：beginObject (nextProperty [值])* endObject
     *          顺序容器   ：beginArray (nextElement [值])* endArray
     *          关联容器   ：beginMap ( keyOnly ? (nextElement [值])*
     *                                : (nextElement beginMapEntry
     *                                   readMapKeyTag [值]
     *                                   readMapValueTag [值] endMapEntry)* )
     *                       endMap
     *
     *          成员与元素一律**拉取**而非「先取个数再循环」：个数只有二进制后端
     *          能预知，流式文本后端在读到结束标记前无从得知。beginArray 仍返回
     *          个数，但仅作为预分配提示（未知时给 0），循环由 nextElement 驱动。
     *
     *          关联容器是否 keyOnly 不由本接口报告：结构一律按声明类型驱动，
     *          落盘标记只在二进制后端内部用于对齐字节。
     *
     *          结构类方法均在 readKind 返回对应标签后调用；end* 与 *Tag 供文本
     *          后端推进/校验节点位置，二进制后端多为空操作。
     */
    class T3D_ENGINE_API IArchiveReader
    {
    public:
        /// 虚析构。
        virtual ~IArchiveReader() = default;

        /**
         * \brief 读并校验文件头。
         * \return 成功返回 T3D_OK；magic/版本/结构缺失时返回 T3D_ERR_MISSING_MAGIC /
         *         T3D_ERR_MISSING_VERSION / T3D_ERR_PARSE_STREAM 等
         */
        virtual TResult readHeader() = 0;

        /**
         * \brief 进入下一个值的信封并返回其类型标签。
         * \return 值的 ArchiveKind；须与一次 endValue() 配对
         * \remarks 每次调用消费类型标签（及 JSON 信封结构）；载荷由后续 read* 读取。
         */
        virtual ArchiveKind readKind() = 0;

        /**
         * \brief 离开当前值的信封。
         */
        virtual void endValue() = 0;

        /**
         * \brief 跳过一个完整值（含其类型标签与载荷）。
         * \remarks 用于未注册类型、已删除属性等前向兼容场景。
         */
        virtual void skipValue() = 0;

        /**
         * \brief 跳过当前值的载荷（类型标签已由 readKind 消费）。
         * \param [in] kind : 已由 readKind 返回的标签，用于确定跳过宽度
         * \remarks 供遍历层在「标签与声明类型不匹配」时丢弃载荷、保持流对齐。
         */
        virtual void skipPayload(ArchiveKind kind) = 0;

        /**
         * \brief 读布尔载荷。
         * \return 布尔值
         */
        virtual bool readBool() = 0;

        /**
         * \brief 读有符号整数载荷。
         * \param [in] kind : 落盘的确切宽度（I8/I16/I32/I64）
         * \return 整数值
         */
        virtual int64_t readInt(ArchiveKind kind) = 0;

        /**
         * \brief 读无符号整数载荷。
         * \param [in] kind : 落盘的确切宽度（U8/U16/U32/U64）
         * \return 整数值
         */
        virtual uint64_t readUint(ArchiveKind kind) = 0;

        /**
         * \brief 读浮点数载荷。
         * \param [in] kind : 落盘的确切宽度（F32/F64）
         * \return 浮点值
         */
        virtual double readReal(ArchiveKind kind) = 0;

        /**
         * \brief 读字符串载荷。
         * \return 字符串内容；亦用于读枚举项名称
         */
        virtual String readString() = 0;

        /**
         * \brief 读二进制块载荷。
         * \param [out] out : 输出 Buffer；实现会先释放 out 已有 Data 再重新分配
         * \remarks 调用方接管 out.Data 的所有权。
         */
        virtual void readBuffer(Buffer &out) = 0;

        /**
         * \brief 读 UUID 载荷。
         * \param [out] out : 输出 UUID
         */
        virtual void readUuid(UUID &out) = 0;

        /**
         * \brief 进入对象体。
         * \param [out] typeName : 落盘的 RTTI 类型名，多态建对象前需要
         * \remarks 须在 readKind 返回 ArchiveKind::Object 后调用。
         */
        virtual void beginObject(String &typeName) = 0;

        /**
         * \brief 拉取下一个属性名。
         * \param [out] name : 属性名
         * \return 还有属性返回 true；已读完返回 false
         * \remarks 用拉取而非「先取个数再循环」，使流式文本后端也能实现。
         */
        virtual bool nextProperty(String &name) = 0;

        /**
         * \brief 离开当前对象体。
         * \remarks 二进制后端会补齐未读属性以保持流对齐。
         */
        virtual void endObject() = 0;

        /**
         * \brief 进入顺序容器。
         * \return 元素个数，**仅作预分配提示**；流式文本后端返回 0
         * \remarks 实际循环由 nextElement 驱动。
         */
        virtual uint64_t beginArray() = 0;

        /**
         * \brief 离开当前顺序容器。
         * \remarks 二进制后端会补齐未读元素以保持流对齐。
         */
        virtual void endArray() = 0;

        /**
         * \brief 拉取容器的下一个元素。
         * \return 还有元素返回 true；已读完返回 false
         * \remarks 顺序容器与关联容器共用。返回 true 后须读出一个完整元素
         *          （keyOnly 关联容器读一个值，否则读一个键值对条目）。
         */
        virtual bool nextElement() = 0;

        /**
         * \brief 进入关联容器。
         * \remarks JSON 后端与 beginArray 等价（同为数组载荷）。
         */
        virtual void beginMap() = 0;

        /**
         * \brief 离开当前关联容器。
         */
        virtual void endMap() = 0;

        /**
         * \brief 进入一个键值对条目（仅非 keyOnly 容器调用）。
         */
        virtual void beginMapEntry() = 0;

        /**
         * \brief 读取并校验 map 条目的 key 侧标签。
         */
        virtual void readMapKeyTag() = 0;

        /**
         * \brief 读取并校验 map 条目的 value 侧标签。
         */
        virtual void readMapValueTag() = 0;

        /**
         * \brief 离开当前键值对条目。
         */
        virtual void endMapEntry() = 0;
    };
}


#endif    /*__T3D_SERIALIZE_ARCHIVE_H__*/
