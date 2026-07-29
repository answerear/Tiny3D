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

#include "Serializer/T3DRttrArchive.h"
#include "Serializer/T3DSerializerManager.h"
#include "Resource/T3DResource.h"
#include "Component/T3DComponent.h"
#include "Object/T3DBuffer.h"
#include "UUID/T3DUUID.h"
#include "T3DErrorDef.h"

#include <utility>
#include <vector>


namespace Tiny3D
{
    using namespace rttr;

    namespace
    {
        void writeValue(IArchiveWriter &w, const variant &var);
        void writeObjectBody(IArchiveWriter &w, const instance &obj);

        //----------------------------------------------------------------------

        /**
         * \brief 判断可空值（裸指针 / SmartPtr<Object>）是否为空，为空则不落盘。
         * \remarks SmartPtr 的 wrapper_mapper::convert 只在源非空且类型匹配时置
         *          成功位，所以"能转成非空 SmartPtr<Object>"即代表非空，其余按空
         *          处理。不能改用 extract_wrapped_value() 再转 void* 判空：非空
         *          SmartPtr 解包出的裸指针并不总能转成 void*，会把非空属性误判为
         *          空而静默丢数据（Pass 的 RenderState 曾因此在二进制里整棵丢失）。
         *          也不能直接 get_value<SmartPtr<Object>>()，convert 失败时变体里
         *          仍是原类型，按别的类型取值是未定义行为。
         */
        bool isNullValue(const variant &var)
        {
            if (var.get_type().is_wrapper())
            {
                variant probe = var;
                if (!probe.convert(type::get<SmartPtr<Object>>()))
                {
                    return true;
                }
                return probe.get_value<SmartPtr<Object>>() == nullptr;
            }

            bool ok = false;
            void *raw = var.convert<void *>(&ok);
            return !ok || raw == nullptr;
        }

        //----------------------------------------------------------------------

        /**
         * \brief 写出可直接落成单个叶子值的类型（算术 / 枚举 / 字符串）。
         * \return 已处理返回 true；否则返回 false，交由调用方按容器或对象处理。
         */
        bool writeAtomic(IArchiveWriter &w, const type &t, const variant &var)
        {
            if (t.is_arithmetic())
            {
                const string_view name = t.get_name();

                if (t == type::get<bool>())
                {
                    w.writeBool(name, var.to_bool());
                }
                else if (t == type::get<char>() || t == type::get<int8_t>())
                {
                    w.writeInt(name, ArchiveKind::I8, var.to_int8());
                }
                else if (t == type::get<int16_t>())
                {
                    w.writeInt(name, ArchiveKind::I16, var.to_int16());
                }
                else if (t == type::get<int32_t>())
                {
                    w.writeInt(name, ArchiveKind::I32, var.to_int32());
                }
                else if (t == type::get<int64_t>())
                {
                    w.writeInt(name, ArchiveKind::I64, var.to_int64());
                }
                else if (t == type::get<uint8_t>())
                {
                    w.writeUint(name, ArchiveKind::U8, var.to_uint8());
                }
                else if (t == type::get<uint16_t>())
                {
                    w.writeUint(name, ArchiveKind::U16, var.to_uint16());
                }
                else if (t == type::get<uint32_t>())
                {
                    w.writeUint(name, ArchiveKind::U32, var.to_uint32());
                }
                else if (t == type::get<uint64_t>())
                {
                    w.writeUint(name, ArchiveKind::U64, var.to_uint64());
                }
                else if (t == type::get<float>())
                {
                    w.writeReal(name, ArchiveKind::F32, var.to_double());
                }
                else if (t == type::get<double>())
                {
                    w.writeReal(name, ArchiveKind::F64, var.to_double());
                }
                else
                {
                    // 兜底：long / size_t 等平台相关整型统一按 U64 落盘
                    w.writeUint(name, ArchiveKind::U64, var.to_uint64());
                }

                return true;
            }

            if (t.is_enumeration())
            {
                bool nameOk = false;
                const String enumName = var.to_string(&nameOk);
                bool valueOk = false;
                const uint64_t enumValue = var.to_uint64(&valueOk);
                w.writeEnum(t.get_name(), enumName, nameOk, enumValue, valueOk);
                return true;
            }

            if (t == type::get<std::string>())
            {
                w.writeString(t.get_name(), var.to_string());
                return true;
            }

            return false;
        }

        //----------------------------------------------------------------------

        /**
         * \brief 写出容器内的单个元素（含值信封）。
         * \remarks 元素本身是容器时直接透传，不能先 extract_wrapped_value，
         *          否则会丢失容器语义；其余情况解包后再写，因为容器 view 的
         *          元素可能是 SmartPtr wrapper，解包成裸指针才能取到派生类型名。
         */
        void writeElement(IArchiveWriter &w, const variant &item)
        {
            w.beginValue();

            if (item.is_sequential_container() || item.is_associative_container())
            {
                writeValue(w, item);
            }
            else
            {
                writeValue(w, item.extract_wrapped_value());
            }

            w.endValue();
        }

        //----------------------------------------------------------------------

        void writeSeq(IArchiveWriter &w, const type &t,
            const variant_sequential_view &view)
        {
            w.beginArray(t.get_name(), view.get_size());

            for (const auto &item : view)
            {
                writeElement(w, item);
            }

            w.endArray();
        }

        //----------------------------------------------------------------------

        void writeMap(IArchiveWriter &w, const type &t,
            const variant_associative_view &view)
        {
            const bool keyOnly = view.is_key_only_type();
            w.beginMap(t.get_name(), keyOnly, view.get_size());

            if (keyOnly)
            {
                // set 容器：只有 key，元素直接作为值写出
                for (const auto &item : view)
                {
                    writeElement(w, item.first);
                }
            }
            else
            {
                for (const auto &item : view)
                {
                    w.beginMapEntry();
                    w.writeMapKeyTag();
                    writeElement(w, item.first);
                    w.writeMapValueTag();
                    writeElement(w, item.second);
                    w.endMapEntry();
                }
            }

            w.endMap();
        }

        //----------------------------------------------------------------------

        /**
         * \brief 写出对象内容：Buffer / UUID 走专用表达，其余遍历属性。
         */
        void writeObjectBody(IArchiveWriter &w, const instance &obj)
        {
            const instance obj2 = obj.get_type().get_raw_type().is_wrapper()
                ? obj.get_wrapped_instance() : obj;
            const type derived = obj2.get_derived_type();

            // Buffer / UUID 必须提前拦截，否则会把 Buffer::Data 这样的裸指针
            // 当成普通属性序列化。
            if (derived == type::get<Buffer>())
            {
                Buffer *buffer = obj2.try_convert<Buffer>();
                if (buffer != nullptr)
                {
                    w.writeBuffer(buffer->Data, buffer->DataSize);
                    return;
                }
            }
            else if (derived == type::get<UUID>())
            {
                UUID *uuid = obj2.try_convert<UUID>();
                if (uuid != nullptr)
                {
                    w.writeUuid(*uuid);
                    return;
                }
            }

            // 先收集实际待写属性：二进制格式需要属性个数作为前缀
            std::vector<std::pair<string_view, variant>> entries;

            for (auto prop : derived.get_properties())
            {
                if (prop.get_metadata("NO_SERIALIZE"))
                {
                    continue;
                }

                variant propValue = prop.get_value(obj2);
                if (!propValue)
                {
                    // 取不到值，无法序列化
                    continue;
                }

                // 空裸指针 / 空 SmartPtr<Object> 不写
                const type pt = propValue.get_type();
                const bool nullable = pt.is_pointer()
                    || (pt.is_wrapper()
                        && pt.get_wrapped_type().is_derived_from<Object>());
                if (nullable && isNullValue(propValue))
                {
                    continue;
                }

                // 必须 move 而非拷贝：变体内若是 unordered 容器，多一次拷贝就会
                // 改变桶布局进而改变迭代顺序，写出的字节序列随之不同。move 与统一
                // 前 JSON 写出器（直接使用属性变体、不额外拷贝）的顺序一致，因此
                // 纳入版本管理的 JSON 资产保持字节稳定；二进制打包产物是可重新
                // 生成的构建产物，其无序容器顺序变化不影响语义。
                entries.emplace_back(prop.get_name(), std::move(propValue));
            }

            w.beginObject(derived.get_name(), entries.size());

            for (auto &entry : entries)
            {
                w.writePropertyName(entry.first);
                w.beginValue();
                writeValue(w, entry.second);
                w.endValue();
            }

            w.endObject();
        }

        //----------------------------------------------------------------------

        void writeValue(IArchiveWriter &w, const variant &var)
        {
            const type valueType = var.get_type();
            const type wrappedType = valueType.is_wrapper()
                ? valueType.get_wrapped_type() : valueType;
            const bool isWrapper = wrappedType != valueType;
            const type effType = isWrapper ? wrappedType : valueType;
            const variant eff = isWrapper ? var.extract_wrapped_value() : var;

            // 生命周期回调按 wrapped 类型查找；var 可能是 SmartPtr<T>（wrapper），
            // 必须先解包成裸指针再 invoke，否则 RTTR 因类型不匹配会静默失败。
            const bool isObject = wrappedType.is_derived_from<Object>();
            const bool invokeLifecycle =
                T3D_SERIALIZER_MGR.isInvokeLifecycleCallbacks();

            if (isObject && invokeLifecycle)
            {
                auto preSave = wrappedType.get_method("onPreSave");
                if (preSave)
                {
                    preSave.invoke(eff);
                }
            }

            if (writeAtomic(w, effType, eff))
            {
                // 算术 / 枚举 / 字符串已落盘
            }
            else if (var.is_sequential_container())
            {
                writeSeq(w, valueType, var.create_sequential_view());
            }
            else if (var.is_associative_container())
            {
                writeMap(w, valueType, var.create_associative_view());
            }
            else if (effType.is_pointer() && isNullValue(eff))
            {
                w.writeNull(effType.get_name());
            }
            else
            {
                const instance childInst = var;
                const instance objInst = isWrapper
                    ? childInst.get_wrapped_instance() : childInst;

                if (objInst.is_valid())
                {
                    writeObjectBody(w, objInst);
                }
                else
                {
                    w.writeNull(effType.get_name());
                }
            }

            if (isObject && invokeLifecycle)
            {
                auto postSave = wrappedType.get_method("onPostSave");
                if (postSave)
                {
                    postSave.invoke(eff);
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    TResult RttrArchive::write(IArchiveWriter &writer, const RTTRObject &obj)
    {
        writer.beginDocument();

        const instance topInst(obj);
        const instance topInst2 = topInst.get_type().get_raw_type().is_wrapper()
            ? topInst.get_wrapped_instance() : topInst;
        const type topType = topInst2.get_derived_type();

        // 见 SerializerManager：离线纯数据直通写出时关闭生命周期回调，避免
        // onPreSave 对扁平层级表的重采集破坏字节往返一致性。
        const bool invokeLifecycle =
            T3D_SERIALIZER_MGR.isInvokeLifecycleCallbacks();
        const bool isObject = topType.is_derived_from<Object>();

        // 顶层对象不经 writeValue，生命周期回调需在此处显式触发
        if (isObject && invokeLifecycle)
        {
            auto preSave = topType.get_method("onPreSave");
            if (preSave)
            {
                preSave.invoke(topInst2);
            }
        }

        writer.beginValue();
        writeObjectBody(writer, topInst2);
        writer.endValue();

        if (isObject && invokeLifecycle)
        {
            auto postSave = topType.get_method("onPostSave");
            if (postSave)
            {
                postSave.invoke(topInst2);
            }
        }

        writer.endDocument();

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    namespace
    {
        /**
         * \brief 读取遍历过程中的跨层状态。
         */
        struct ReadContext
        {
            /**
             * 顶层 Resource 对象，供其内的 Component 回调 
             * onAddComponentForLoadingResource 时定位宿主资源。
             */
            variant *mTopResource{nullptr};
        };

        variant readNew(IArchiveReader &r, ReadContext &ctx, bool topLevel);
        void readInto(IArchiveReader &r, ReadContext &ctx, const type &hint,
            variant &obj);
        void fillObjectMembers(IArchiveReader &r, ReadContext &ctx,
            const type &klass, variant &obj);

        //----------------------------------------------------------------------

        /**
         * \brief 按落盘标签读出一个叶子值，变体类型与标签的确切宽度一致。
         * \remarks 保持确切宽度而非一律用最宽类型，是为了后续 convert 到声明类型
         *          时语义与写出端对称（例如 U64 极大值不会先被压成 int64）。
         * \return 标签不是叶子类型时返回无效变体，载荷未被消费。
         */
        variant readAtomicByKind(IArchiveReader &r, ArchiveKind kind)
        {
            switch (kind)
            {
            case ArchiveKind::Bool:
                return variant(r.readBool());
            case ArchiveKind::I8:
                return variant(static_cast<int8_t>(r.readInt(kind)));
            case ArchiveKind::U8:
                return variant(static_cast<uint8_t>(r.readUint(kind)));
            case ArchiveKind::I16:
                return variant(static_cast<int16_t>(r.readInt(kind)));
            case ArchiveKind::U16:
                return variant(static_cast<uint16_t>(r.readUint(kind)));
            case ArchiveKind::I32:
                return variant(static_cast<int32_t>(r.readInt(kind)));
            case ArchiveKind::U32:
                return variant(static_cast<uint32_t>(r.readUint(kind)));
            case ArchiveKind::I64:
                return variant(r.readInt(kind));
            case ArchiveKind::U64:
                return variant(r.readUint(kind));
            case ArchiveKind::F32:
                return variant(static_cast<float>(r.readReal(kind)));
            case ArchiveKind::F64:
                return variant(r.readReal(kind));
            case ArchiveKind::String:
            case ArchiveKind::Enum:
                return variant(r.readString());
            default:
                return variant();
            }
        }

        //----------------------------------------------------------------------

        /**
         * \brief 按落盘属性名在类里查找 RTTR 属性。
         * \remarks 找不到时回退匹配 SERIALIZE_ALIAS 元数据记录的曾用名，使属性
         *          改名后仍能读入历史资产。
         */
        property findProperty(const type &klass, const String &name)
        {
            property prop = klass.get_property(name);
            if (prop.is_valid())
            {
                return prop;
            }

            for (auto p : klass.get_properties())
            {
                variant md = p.get_metadata("SERIALIZE_ALIAS");
                if (md.is_valid())
                {
                    bool ok = false;
                    String alias = md.to_string(&ok);
                    if (ok && alias == name)
                    {
                        return p;
                    }
                }
            }

            return prop;
        }

        //----------------------------------------------------------------------

        /**
         * \brief 把读出的裸指针变体转成属性声明的 SmartPtr 类型。
         */
        void convertToSmartPtr(variant &var, const type &propType)
        {
            if (!propType.is_wrapper() || var.get_type() == propType)
            {
                return;
            }

            const type varType = var.get_type();
            if (varType.is_pointer()
                && (propType.get_wrapped_type() == varType.get_raw_type()
                    || propType.get_wrapped_type().is_base_of(
                        varType.get_raw_type()))
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

        //----------------------------------------------------------------------

        /**
         * \brief 判断声明类型是否需要按多态方式新建对象。
         */
        bool isPolymorphic(const type &t)
        {
            return t.is_pointer()
                || (t.is_wrapper()
                    && t.get_wrapped_type().is_derived_from<Object>());
        }

        //----------------------------------------------------------------------

        /**
         * \brief 读出容器内的单个元素。
         */
        variant readElement(IArchiveReader &r, ReadContext &ctx,
            const type &elemType)
        {
            if (isPolymorphic(elemType))
            {
                variant var = readNew(r, ctx, false);
                if (var.is_valid())
                {
                    convertToSmartPtr(var, elemType);
                }
                return var;
            }

            constructor ctor = elemType.get_constructor();
            variant var = ctor.is_valid() ? ctor.invoke() : variant();
            readInto(r, ctx, elemType, var);
            return var;
        }

        //----------------------------------------------------------------------

        void readSeqInto(IArchiveReader &r, ReadContext &ctx, variant &obj)
        {
            auto view = obj.create_sequential_view();
            // 个数只是预分配提示：二进制后端能给出确切值，流式文本后端给 0。
            const uint64_t hint = r.beginArray();
            if (hint > 0)
            {
                view.set_size(static_cast<size_t>(hint));
            }
            const type itemType = view.get_value_type();

            size_t i = 0;
            while (r.nextElement())
            {
                // 未预分配（或落盘个数多于提示）时按需增长。定长原生数组的
                // set_size 会失败，但只要下标在界内 set_value 仍然有效。
                if (i >= view.get_size())
                {
                    view.set_size(i + 1);
                }

                variant var = readElement(r, ctx, itemType);
                if (!view.set_value(i, var))
                {
                    T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                        "Set element [%zu] value of sequential container failed !",
                        i);
                }
                ++i;
            }

            // 落盘个数少于预分配提示（如流被截断）时收缩，避免留下默认构造的尾巴
            if (i < view.get_size())
            {
                view.set_size(i);
            }

            r.endArray();
        }

        //----------------------------------------------------------------------

        void readMapInto(IArchiveReader &r, ReadContext &ctx, variant &obj)
        {
            auto view = obj.create_associative_view();

            // 结构按声明类型驱动而非落盘标记：容器在 set 与 map 之间改过声明时，
            // 数据本身已无法解释，此处不做额外兼容。
            r.beginMap();

            if (view.is_key_only_type())
            {
                // set 容器：元素类型是 key_type。注意 rttr 对 set 的
                // get_value_type() 返回无效类型，必须用 get_key_type()（否则无法
                // 按声明类型读元素，导致读入无效 variant、insert 失败、数据丢失）。
                type elemType = view.get_key_type();
                if (!elemType.is_valid())
                {
                    elemType = view.get_value_type();
                }

                size_t i = 0;
                while (r.nextElement())
                {
                    variant key = readElement(r, ctx, elemType);
                    if (!view.insert(key).second)
                    {
                        T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                            "Failed to insert element [%zu] in set container !", i);
                    }
                    ++i;
                }
            }
            else
            {
                const type keyType = view.get_key_type();
                const type valueType = view.get_value_type();

                size_t i = 0;
                while (r.nextElement())
                {
                    r.beginMapEntry();
                    r.readMapKeyTag();
                    variant key = readElement(r, ctx, keyType);
                    r.readMapValueTag();
                    variant val = readElement(r, ctx, valueType);
                    r.endMapEntry();

                    if (!view.insert(key, val).second)
                    {
                        T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                            "Failed to insert element [%zu] in map container !", i);
                    }
                    ++i;
                }
            }

            r.endMap();
        }

        //----------------------------------------------------------------------

        /**
         * \brief 按声明类型 hint 把已读到标签的值填入 obj（不含值信封收尾）。
         * \remarks 标签与声明类型不匹配时丢弃载荷并告警，使流保持对齐、后续属性
         *          仍能正常读出。
         */
        void readIntoKind(IArchiveReader &r, ReadContext &ctx, ArchiveKind kind,
            const type &hint, variant &obj)
        {
            if (kind == ArchiveKind::Null)
            {
                return;
            }

            if (hint.is_arithmetic())
            {
                variant raw = readAtomicByKind(r, kind);
                if (!raw.is_valid())
                {
                    r.skipPayload(kind);
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
                if (kind == ArchiveKind::Enum || kind == ArchiveKind::String)
                {
                    const String name = r.readString();
                    obj = hint.get_enumeration().name_to_value(name);
                }
                else
                {
                    variant raw = readAtomicByKind(r, kind);
                    if (!raw.is_valid())
                    {
                        // 标签不是叶子类型，载荷未被消费，须丢弃以保持流对齐
                        r.skipPayload(kind);
                    }

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
                if (kind == ArchiveKind::String || kind == ArchiveKind::Enum)
                {
                    obj = r.readString();
                }
                else
                {
                    variant raw = readAtomicByKind(r, kind);
                    if (!raw.is_valid())
                    {
                        // 标签不是叶子类型，载荷未被消费，须丢弃以保持流对齐
                        r.skipPayload(kind);
                    }

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
                if (kind == ArchiveKind::Buffer)
                {
                    r.readBuffer(obj.get_value<Buffer>());
                }
                else
                {
                    r.skipPayload(kind);
                    T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                        "Incompatible value for Buffer property, skipped.");
                }
            }
            else if (hint == type::get<UUID>())
            {
                if (kind == ArchiveKind::Uuid)
                {
                    r.readUuid(obj.get_value<UUID>());
                }
                else if (kind == ArchiveKind::String)
                {
                    const String str = r.readString();
                    obj.get_value<UUID>().fromString(str);
                }
                else
                {
                    r.skipPayload(kind);
                    T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                        "Incompatible value for UUID property, skipped.");
                }
            }
            else if (hint.is_sequential_container())
            {
                if (kind == ArchiveKind::Seq)
                {
                    readSeqInto(r, ctx, obj);
                }
                else
                {
                    r.skipPayload(kind);
                    T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                        "Incompatible value for sequential container, skipped.");
                }
            }
            else if (hint.is_associative_container())
            {
                if (kind == ArchiveKind::Map)
                {
                    readMapInto(r, ctx, obj);
                }
                else
                {
                    r.skipPayload(kind);
                    T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                        "Incompatible value for associative container, skipped.");
                }
            }
            else if (kind == ArchiveKind::Object)
            {
                // 值类型对象（TSTRUCT，非 Object 派生）：用声明类型而非落盘类型名，
                // 后者可能因平台差异对不上。
                String typeName;
                r.beginObject(typeName);
                fillObjectMembers(r, ctx, hint, obj);
                r.endObject();
            }
            else
            {
                r.skipPayload(kind);
                T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                    "Incompatible value for object property [%s], skipped.",
                    hint.get_name().data());
            }
        }

        //----------------------------------------------------------------------

        /**
         * \brief 按声明类型 hint 读入一个完整值（含值信封）。
         */
        void readInto(IArchiveReader &r, ReadContext &ctx, const type &hint,
            variant &obj)
        {
            const ArchiveKind kind = r.readKind();
            readIntoKind(r, ctx, kind, hint, obj);
            r.endValue();
        }

        //----------------------------------------------------------------------

        /**
         * \brief 依次拉取对象属性并填入 obj，末尾触发装载类生命周期回调。
         * \remarks 调用方负责 beginObject / endObject 配对。
         */
        void fillObjectMembers(IArchiveReader &r, ReadContext &ctx,
            const type &klass, variant &obj)
        {
            String pname;
            while (r.nextProperty(pname))
            {
                property prop = findProperty(klass, pname);
                if (!prop.is_valid())
                {
                    T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                        "Property (%s) not found in class (%s), skipping.",
                        pname.c_str(), klass.get_name().data());
                    r.skipValue();
                    continue;
                }

                const type propType = prop.get_type();
                variant val;

                if (isPolymorphic(propType))
                {
                    val = readNew(r, ctx, false);
                    if (!val.is_valid())
                    {
                        continue;
                    }
                    convertToSmartPtr(val, propType);
                }
                else if (propType.is_array())
                {
                    // 原生数组无法整体赋值，取出现有实例原地填充
                    val = prop.get_value(obj);
                    readInto(r, ctx, propType, val);
                }
                else if (propType.is_arithmetic() || propType.is_enumeration()
                    || propType == type::get<std::string>())
                {
                    // 基础类型在 rttr 无注册构造器，无需预构造：readInto 直接赋值
                    readInto(r, ctx, propType, val);
                }
                else
                {
                    // 值类型对象 / 容器 / Buffer / UUID：需先按声明类型默认构造
                    // 再原地填充
                    constructor ctor = propType.get_constructor();
                    if (!ctor.is_valid())
                    {
                        T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                            "No default constructor for property (%s) type in "
                            "class (%s)!", pname.c_str(),
                            klass.get_name().data());
                        r.skipValue();
                        continue;
                    }
                    val = ctor.invoke();
                    readInto(r, ctx, propType, val);
                }

                if (!prop.set_value(obj, val))
                {
                    T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                        "Failed to set property (%s) in class (%s)!",
                        pname.c_str(), klass.get_name().data());
                }
            }

            // 生命周期回调可能重建运行时关系（如层级）并形成智能指针引用环，离线
            // 格式转换工具可关闭以避免引用环泄漏与字节往返不一致（见
            // SerializerManager）。
            const bool invokeLifecycle =
                T3D_SERIALIZER_MGR.isInvokeLifecycleCallbacks();

            if (invokeLifecycle && klass.is_derived_from<Object>())
            {
                auto postLoad = klass.get_method("onPostLoad");
                if (postLoad)
                {
                    postLoad.invoke(obj);
                }
            }

            if (invokeLifecycle && ctx.mTopResource != nullptr
                && klass.is_derived_from<Component>())
            {
                auto addComponent = ctx.mTopResource->get_type().get_method(
                    "onAddComponentForLoadingResource");
                bool ok = false;
                Component *component = obj.convert<Component *>(&ok);
                if (addComponent.is_valid() && ok)
                {
                    Resource *resource = ctx.mTopResource->convert<Resource *>(&ok);
                    if (ok)
                    {
                        addComponent.invoke(resource, component);
                    }
                }
            }
        }

        //----------------------------------------------------------------------

        /**
         * \brief 读入一个自描述值并新建对象（多态入口）。
         * \param [in] topLevel : 是否为根值，根 Resource 需记入上下文供其内
         *                        Component 回调时定位宿主。
         */
        variant readNew(IArchiveReader &r, ReadContext &ctx, bool topLevel)
        {
            const ArchiveKind kind = r.readKind();
            variant result;

            switch (kind)
            {
            case ArchiveKind::Null:
                break;
            case ArchiveKind::Object:
                {
                    String typeName;
                    r.beginObject(typeName);

                    type klass = type::get_by_name(typeName);
                    if (!klass.is_valid())
                    {
                        T3D_LOG_ERROR(LOG_TAG_SERIALIZE,
                            "RTTR type [%s] is not registered !",
                            typeName.c_str());
                        // endObject 会消费掉未读属性，维持流对齐
                        r.endObject();
                        break;
                    }

                    variant obj = klass.get_constructor().invoke();

                    if (topLevel && klass.is_derived_from<Resource>())
                    {
                        ctx.mTopResource = &obj;
                    }

                    if (klass.is_derived_from<Object>())
                    {
                        auto onPostInit = klass.get_method("onPostInit");
                        if (onPostInit)
                        {
                            onPostInit.invoke(obj);
                        }
                    }

                    fillObjectMembers(r, ctx, klass, obj);
                    r.endObject();

                    if (ctx.mTopResource == &obj)
                    {
                        // obj 即将随本作用域销毁，不能留下悬垂指针
                        ctx.mTopResource = nullptr;
                    }

                    result = obj;
                }
                break;
            case ArchiveKind::Buffer:
                {
                    result = type::get<Buffer>().get_constructor().invoke();
                    r.readBuffer(result.get_value<Buffer>());
                }
                break;
            case ArchiveKind::Uuid:
                {
                    result = type::get<UUID>().get_constructor().invoke();
                    r.readUuid(result.get_value<UUID>());
                }
                break;
            case ArchiveKind::Seq:
            case ArchiveKind::Map:
                // 容器没有声明类型可依，无法确定元素类型
                T3D_LOG_WARNING(LOG_TAG_SERIALIZE,
                    "Unexpected self-describing container without type hint, "
                    "skipping.");
                r.skipPayload(kind);
                break;
            default:
                result = readAtomicByKind(r, kind);
                break;
            }

            r.endValue();

            return result;
        }
    }

    //--------------------------------------------------------------------------

    TResult RttrArchive::read(IArchiveReader &reader, RTTRVariant &obj)
    {
        TResult ret = reader.readHeader();
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        ReadContext ctx;
        obj = readNew(reader, ctx, true);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
