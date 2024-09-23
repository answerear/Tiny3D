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
#define RAPIDJSON_HAS_STDSTRING 1
#include <prettywriter.h>
#include <document.h>
#include <Resource/T3DPrefab.h>

#include "T3DErrorDef.h"
#include "Component/T3DComponent.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    class JsonStream
    {
    public:
        typedef signed char Ch;

        JsonStream(DataStream& stream) 
            : mStream(stream)
        {
        }

        Ch Peek() const
        {
            Ch c;

            if (mStream.eof())
            {
                c = '\0';
            }
            else
            {
                mStream >> c;
                long_t len = sizeof(Ch);
                mStream.seek(-len, true);
            }

            return c;
        }

        Ch Take() const
        {
            Ch c;

            if (mStream.eof())
            {
                c = '\0';
            }
            else
            {
                mStream >> c;
            }

            return c;
        }

        size_t Tell() const
        {
            return (size_t)mStream.tell();
        }

        Ch* PutBegin()
        {
            return nullptr;
        }

        void Put(Ch c)
        {
            mStream << c;
        }

        void Flush()
        {
        }

        size_t PutEnd(Ch*)
        {
            return 0;
        }

    private:
        DataStream& mStream;
    };
    
    //--------------------------------------------------------------------------

    using namespace rapidjson;
    using namespace rttr;

    #define RTTI_TYPE       "RTTI_Type"
    #define RTTI_VALUE      "RTTI_Value"
    #define RTTI_MAP_KEY    "RTTI_Map_Key"
    #define RTTI_MAP_VALUE  "RTTI_Map_Value"
    
    class RTTRObjectJsonWriter
    {
    public:
        static bool WriteAtomicType(PrettyWriter<JsonStream> &writer, const type &t, const variant &var)
        {
            if (t.is_arithmetic())
            {
                writer.Key(RTTI_TYPE);
                const auto name = t.get_name();
                writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);
                writer.Key(RTTI_VALUE);
                
                if (t == type::get<bool>())
                    writer.Bool(var.to_bool()); 
                else if (t == type::get<char>())
                    writer.Bool(var.to_bool());
                else if (t == type::get<int8_t>())
                    writer.Int(var.to_int8());
                else if (t == type::get<int16_t>())
                    writer.Int(var.to_int16());
                else if (t == type::get<int32_t>())
                    writer.Int(var.to_int32());
                else if (t == type::get<int64_t>())
                    writer.Int64(var.to_int64());
                else if (t == type::get<uint8_t>())
                    writer.Uint(var.to_uint8());
                else if (t == type::get<uint16_t>())
                    writer.Uint(var.to_uint16());
                else if (t == type::get<uint32_t>())
                    writer.Uint(var.to_uint32());
                else if (t == type::get<uint64_t>())
                    writer.Uint64(var.to_uint64());
                else if (t == type::get<float>())
                    writer.Double(var.to_double());
                else if (t == type::get<double>())
                    writer.Double(var.to_double());

                return true;
            }
            else if (t.is_enumeration())
            {
                writer.Key(RTTI_TYPE);
                const auto name = t.get_name();
                writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);
                writer.Key(RTTI_VALUE);
                
                bool ok = false;
                auto result = var.to_string(&ok);
                if (ok)
                {
                    writer.String(result);
                }
                else
                {
                    ok = false;
                    auto value = var.to_uint64(&ok);
                    if (ok)
                        writer.Uint64(value);
                    else
                        writer.Null();
                }

                return true;
            }
            else if (t == type::get<std::string>())
            {
                writer.Key(RTTI_TYPE);
                const auto name = t.get_name();
                writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);
                writer.Key(RTTI_VALUE);            
                writer.String(var.to_string());
                return true;
            }

            return false;
        }

        static void WriteSequentialContainer(PrettyWriter<JsonStream> &writer, const type &t, const variant_sequential_view &view)
        {
            writer.Key(RTTI_TYPE);
            const auto name = t.get_name();
            writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);
            writer.Key(RTTI_VALUE);

            writer.StartArray();
            for (const auto& item : view)
            {
                if (item.is_sequential_container())
                {
                    WriteSequentialContainer(writer, item.get_type(), item.create_sequential_view());
                }
                else
                {
                    variant wrapped_var = item.extract_wrapped_value();
                    type value_type = wrapped_var.get_type();
                    if (value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration())
                    {
                        writer.StartObject();
                        WriteAtomicType(writer, value_type, wrapped_var);
                        writer.EndObject();
                    }
                    else // object
                    {
                        writer.StartObject();
                        WriteObject(writer, wrapped_var);
                        writer.EndObject();
                    }
                }
            }
            writer.EndArray();
        }

        static void WriteObject(rapidjson::PrettyWriter<JsonStream> &writer, const rttr::instance &obj)
        {
            const instance obj2 = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
        
            const auto className = obj2.get_derived_type().get_name();
            
            // Type
            writer.Key(RTTI_TYPE);
            writer.String(className.data(), static_cast<rapidjson::SizeType>(className.length()), false);

            // Properties
            writer.Key(RTTI_VALUE);
            writer.StartObject();
            
            if (className == "Tiny3D::Buffer")
            {
                // Buffer 特殊处理，先转成 base64 再保存
                Buffer *buffer = obj2.try_convert<Buffer>();
                String base64 = base64_encode(buffer->Data, buffer->DataSize);
                WriteAtomicType(writer, type::get<String>(), base64);
            }
            else if (className == "Tiny3D::UUID")
            {
                // UUID 特殊处理
                UUID *uuid = obj2.try_convert<UUID>();
                String str = uuid->toString();
                WriteAtomicType(writer, type::get<String>(), str);
            }
            else
            {
                auto prop_list = obj2.get_derived_type().get_properties();
                for (auto prop : prop_list)
                {
                    if (prop.get_metadata("NO_SERIALIZE"))
                        continue;

                    variant prop_value = prop.get_value(obj2);
                    if (!prop_value)
                        continue; // cannot serialize, because we cannot retrieve the value

                    if (prop_value.get_type().is_pointer())
                    {
                        if (prop_value.convert<void*>() == nullptr)
                        {
                            // 空指针，不写
                            continue;
                        }
                    }
                    else if (prop_value.get_type().is_wrapper() && prop_value.get_type().get_wrapped_type().is_derived_from<Object>())
                    {
                        // auto ptype = prop_value.get_type();
                        // auto wtype = ptype.get_wrapped_type();
                        auto ok = prop_value.convert(type::get<SmartPtr<Object>>());
                        // wtype = prop_value.get_type();
                        auto ptr = prop_value.get_value<SmartPtr<Object>>();
                        if (ptr == nullptr)
                        {
                            // 空智能指针，不写
                            continue;
                        }
                        // int32_t a = 0;
                        // auto var2 = prop_value.extract_wrapped_value();
                        // Object *val = var2.get_value<Object*>();
                        // if (val == nullptr)
                        // {
                        //     continue;
                        // }
                    }
                    
                    WriteProperty(writer, prop, prop_value);
                }
            }
            
            writer.EndObject();
        }

        static bool WriteProperty(rapidjson::PrettyWriter<JsonStream> &writer, const property &prop, const variant &value)
        {
            //const instance obj(prop);
        
            // property name
            const auto name = prop.get_name();
            writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);

            // property value
            writer.StartObject();

            const instance obj2(value);
            const auto className = obj2.get_derived_type().get_name();
        
            // Value
            bool ret = WriteVariant(writer, value);
            writer.EndObject();
            return ret;
        }

        static bool WriteVariant(PrettyWriter<JsonStream> &writer, const variant &var)
        {
            type value_type = var.get_type();
            type wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
            bool is_wrapper = wrapped_type != value_type;

            bool isObject = false;
            if (wrapped_type.is_derived_from<Tiny3D::Object>())
            {
                isObject = true;
                auto preSave = wrapped_type.get_method("onPreSave");
                if (preSave)
                {
                    preSave.invoke(var);
                }
            }

            if (WriteAtomicType(writer, is_wrapper ? wrapped_type : value_type,
                                       is_wrapper ? var.extract_wrapped_value() : var))
            {
            }
            else if (var.is_sequential_container())
            {
                WriteSequentialContainer(writer, value_type, var.create_sequential_view());
            }
            else if (var.is_associative_container())
            {
                WriteAssociatvieContainer(writer, value_type, var.create_associative_view());
            }
            else
            {
                //const instance obj = var;
                const instance child_obj = var;
                instance obj = is_wrapper ? child_obj.get_wrapped_instance() : child_obj;
                type t = obj.get_derived_type();
                variant v = var;
                if (t.is_wrapper())
                {
                    v.convert(obj.get_derived_type().get_wrapped_type());
                    t = v.get_type();
                }
                const auto child_props = t.get_properties();
                // const variant v = var;
                // if (is_wrapper)
                //     v.convert(wrapped_type);
                // auto child_props = is_wrapper ? v.get_type().get_properties() : value_type.get_properties();
                if (!child_props.empty())
                {
                    WriteObject(writer, v);
                }
                else
                {
                    bool ok = false;
                    auto text = var.to_string(&ok);
                    if (!ok)
                    {
                        writer.String(text);
                        return false;
                    }

                    writer.String(text);
                }
            }

            if (isObject)
            {
                auto postSave = wrapped_type.get_method("onPostSave");
                if (postSave)
                {
                    postSave.invoke(var);
                }
            }
            
            return true;
        }

        static void WriteAssociatvieContainer(PrettyWriter<JsonStream> &writer, const type &t, const variant_associative_view &view)
        {
            writer.Key(RTTI_TYPE);
            const auto name = t.get_name();
            writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);
            writer.Key(RTTI_VALUE);
        
            static const string_view key_name(RTTI_MAP_KEY);
            static const string_view value_name(RTTI_MAP_VALUE);

            writer.StartArray();

            if (view.is_key_only_type())
            {
                for (auto& item : view)
                {
                    writer.StartObject();
                    WriteVariant(writer, item.first);
                    writer.EndObject();
                }
            }
            else
            {
                auto kt = view.get_key_type();
                auto vt = view.get_value_type();
                for (const auto &item : view)
                {
                    auto first = item.first.extract_wrapped_value();
                    auto second = item.second.extract_wrapped_value();
                    auto keyType = first.get_type();
                    auto valueType = second.get_type();
                    
                    writer.StartObject();
                    writer.Key(key_name.data(), static_cast<rapidjson::SizeType>(key_name.length()), false);

                    writer.StartObject();
                    WriteVariant(writer, first);
                    writer.EndObject();
                
                    writer.Key(value_name.data(), static_cast<rapidjson::SizeType>(value_name.length()), false);

                    writer.StartObject();
                    WriteVariant(writer, second);
                    writer.EndObject();
                
                    writer.EndObject();
                }
            }

            writer.EndArray();
        }
    };
    
    //--------------------------------------------------------------------------

    class RTTRObjectJsonReader
    {
    public:
        variant *mObj {nullptr};
        
        bool ReadAtomicType(const Value &node, const type &klass, variant &obj)
        {
            bool ret = false;
        
            if (klass.is_arithmetic())
            {
                if (klass == type::get<bool>())
                    obj = node.GetBool();
                else if (klass == type::get<int32_t>())
                    obj = node.GetInt();
                else if (klass == type::get<int16_t>())
                    obj = (int16_t)node.GetInt();
                else if (klass == type::get<int8_t>())
                    obj = (int8_t)node.GetInt();
                else if (klass == type::get<uint32_t>())
                    obj = node.GetUint();
                else if (klass == type::get<uint16_t>())
                    obj = (uint16_t)node.GetUint();
                else if (klass == type::get<uint8_t>())
                    obj = (uint8_t)node.GetUint();
                else if (klass == type::get<int64_t>())
                    obj = node.GetInt64();
                else if (klass == type::get<uint64_t>())
                    obj = node.GetUint64();
                else if (klass == type::get<float>())
                    obj = node.GetFloat();
                else if (klass == type::get<double>())
                    obj = node.GetDouble();

                ret = true;
            }
            else if (klass.is_enumeration())
            {
                const enumeration enumerate = klass.get_enumeration();
                obj = enumerate.name_to_value(node.GetString());
                ret = true;
            }
            else if (klass ==  type::get<std::string>())
            {
                obj = String(node.GetString());
                ret = true;
            }

            return ret;
        }

        void convertToSmartPtr(variant &var, const type &propType)
        {
            const type varType = var.get_type();
            if (propType.is_wrapper() && varType.is_pointer() && propType.get_wrapped_type() == varType.get_raw_type() && varType.get_raw_type().is_derived_from<Object>())
            {
                bool ok = false;
                Object *tempObj = var.convert<Object*>(&ok);
                T3D_ASSERT(ok);
                var = propType.create({tempObj});
            }
        }

        TResult ReadValue(const Value &value, const type &klass, variant &obj)
        {
            TResult ret = T3D_OK;

            do 
            {
                if (ReadAtomicType(value, klass, obj))
                {

                }
                else if (klass.is_sequential_container())
                {
                    const auto& array = value.GetArray();
                    auto view = obj.create_sequential_view();
                    view.set_size(array.Size());
                    const type itemType = view.get_value_type();
                    for (size_t i = 0; i < array.Size(); i++)
                    {
                        const auto& item = array[i];
                        variant var = ReadObject(item);
                        convertToSmartPtr(var, itemType);
                        bool rval = view.set_value(i, var);
                        if (!rval)
                        {
                            T3D_LOG_ERROR(LOG_TAG_SERIALIZE, "Set element [%d] value of array in %s class failed !", i, klass.get_name().data());
                        }
                    }
                }
                else if (klass.is_associative_container())
                {
                    const auto& array = value.GetArray();
                    auto view = obj.create_associative_view();
                    if (view.is_key_only_type())
                    {
                        const type valueType = view.get_value_type();
                        // set
                        for (size_t i = 0; i < array.Size(); i++)
                        {
                            const auto& item = array[i];
                            variant var = ReadObject(item);
                            convertToSmartPtr(var, valueType);
                            // const type varType = var.get_type();
                            // if (valueType.is_wrapper() && varType.is_pointer() && valueType.get_wrapped_type() == varType.get_raw_type() && varType.get_raw_type().is_derived_from<Object>())
                            // {
                            //     bool ok = false;
                            //     Object *tempObj = var.convert<Object*>(&ok);
                            //     T3D_ASSERT(ok);
                            //     var = valueType.create({tempObj});
                            //     // bool ok = var.convert(valueType);
                            //     // T3D_ASSERT(ok);
                            // }
                            view.insert(var);
                        }
                    }
                    else
                    {
                        // map
                        type keyType = view.get_key_type();
                        const type valueType = view.get_value_type();
                        for (size_t i = 0; i < array.Size(); i++)
                        {
                            const auto& item = array[i];
                            const auto& keyNode = item.FindMember(RTTI_MAP_KEY);
                            variant key = ReadObject(keyNode->value);
                            convertToSmartPtr(key, keyType);
                            // const type kt = key.get_type();
                            // if (keyType.is_wrapper() && kt.is_pointer() && keyType.get_wrapped_type() == kt.get_raw_type() && kt.get_raw_type().is_derived_from<Object>())
                            // {
                            //     bool ok = false;
                            //     Object *tempObj = key.convert<Object*>(&ok);
                            //     T3D_ASSERT(ok);
                            //     key = keyType.create({tempObj});
                            // }
                            const auto& valNode = item.FindMember(RTTI_MAP_VALUE);
                            variant val = ReadObject(valNode->value);
                            convertToSmartPtr(val, valueType);
                            // const type varType = val.get_type();
                            // if (valueType.is_wrapper() && varType.is_pointer() && valueType.get_wrapped_type() == varType.get_raw_type() && varType.get_raw_type().is_derived_from<Object>())
                            // {
                            //     bool ok = false;
                            //     Object *tempObj = val.convert<Object*>(&ok);
                            //     T3D_ASSERT(ok);
                            //     val = valueType.create({tempObj});
                            //     // bool ok = val.convert(valueType);
                            //     // T3D_ASSERT(ok);
                            // }

                            view.insert(key, val);
                        }
                    }
                }
                else if (klass == type::get<Tiny3D::Buffer>())
                {
                    // Buffer 特殊处理
                    variant var = ReadObject(value);
                    String &str = var.get_value<String>();
                    Buffer &buffer = obj.get_value<Buffer>();
                    String dst = base64_decode(str);
                    T3D_SAFE_DELETE_ARRAY(buffer.Data);
                    buffer.Data = new uint8_t[dst.size()];
                    buffer.DataSize = dst.size();
                    memcpy(buffer.Data, &dst[0], dst.size());
                }
                else if (klass == type::get<Tiny3D::UUID>())
                {
                    // UUID 特殊处理
                    variant var = ReadObject(value);
                    String &str = var.get_value<String>();
                    UUID &uuid = obj.get_value<UUID>();
                    uuid.fromString(str);
                }
                else
                {
                    for (Value::ConstMemberIterator it = value.MemberBegin(); it != value.MemberEnd(); ++it)
                    {
                        // property name
                        const auto& name = it->name;

                        // property type & value
                        bool isArray = false;
                        variant val = ReadObjectRecursively(it->value, isArray);
                        if (!val.is_valid() && isArray)
                        {
                            // 原生数组
                            val = klass.get_property_value(name.GetString(), obj);
                            ReadNativeArray(it->value, val);
                            bool rval = klass.set_property_value(name.GetString(), obj, val);
                            if (!rval)
                            {
                                T3D_LOG_ERROR(LOG_TAG_SERIALIZE, " There is not the corresponding value of property (%s) in class (%s)!", name.GetString(), klass.get_name().data());
                            }
                        }
                        else
                        {
                            const type propType = klass.get_property(name.GetString()).get_type();
                            convertToSmartPtr(val, propType);
                            // const type varType = val.get_type();
                            // if (propType.is_wrapper() && varType.is_pointer() && propType.get_wrapped_type() == varType.get_raw_type() && varType.get_raw_type().is_derived_from<Object>())
                            // {
                            //     bool ok = false;
                            //     Object *tempObj = val.convert<Object*>(&ok);
                            //     T3D_ASSERT(ok);
                            //     val = propType.create({tempObj});
                            //     // bool ok = val.convert(valueType);
                            //     // T3D_ASSERT(ok);
                            // }
                            
                            // if (val.get_type().is_derived_from<Tiny3D::Object>())
                            // {
                            //     type propType = klass.get_property(name.GetString()).get_type();
                            //     if (propType.is_wrapper())
                            //     {
                            //         Object *tempObj = val.convert<Object*>();
                            //         // val = val.extract_wrapped_value();
                            //         val = tempObj;
                            //     }
                            // }
                            
                            bool rval = klass.set_property_value(name.GetString(), obj, val);
                            if (!rval)
                            {
                                T3D_LOG_ERROR(LOG_TAG_SERIALIZE, " There is not the corresponding value of property (%s) in class (%s)!", name.GetString(), klass.get_name().data());
                                continue;
                            }
                        }
                    }
                }

                if (klass.is_derived_from<Tiny3D::Object>())
                {
                    // Tiny3D::Object
                    auto postLoad = klass.get_method("onPostLoad");
                    if (postLoad)
                    {
                        postLoad.invoke(obj);
                    }
                }

                if (mObj != nullptr && klass.is_derived_from<Tiny3D::Component>())
                {
                    // Tiny3D::Component in Tiny3D:Prefab
                    auto addComponent = mObj->get_type().get_method("addCompnentForLoadingResource");
                    bool ok = false;
                    Component *component = obj.convert<Tiny3D::Component*>(&ok);
                    if (ok)
                    {
                        addComponent.invoke(mObj, component);
                    }
                }
            } while (false);

            return ret;
        }

        TResult ReadObject(const Value& node, RTTRVariant &obj, bool recursively = true)
        {
            TResult ret = T3D_OK;

            do
            {
                auto itr = node.FindMember(RTTI_TYPE);
                if (itr == node.MemberEnd())
                {
                    break;
                }

                type klass = type::get_by_name(itr->value.GetString());
                if (!klass)
                {
                    break;
                }

                constructor ctor = klass.get_constructor();
                obj = ctor.invoke();
                // obj = klass.create();
                if (!recursively && (klass == type::get<Tiny3D::Prefab>()))
                {
                    // prefab
                    mObj = &obj;
                }

                itr = node.FindMember(RTTI_VALUE);
                if (itr == node.MemberEnd())
                {
                    klass.destroy(obj);
                    break;
                }

                //obj = obj.get_type().get_raw_type().is_wrapper() ? obj.extract_wrapped_value() : obj;

                const auto& value = itr->value;
                ReadValue(value, klass, obj);
            } while (false);

            return ret;
        }

        variant ReadObjectRecursively(const Value &node, bool &isArray, bool recursively = true)
        {
            variant obj;

            do
            {
                auto itr = node.FindMember(RTTI_TYPE);
                if (itr == node.MemberEnd())
                {
                    break;
                }

                type klass = type::get_by_name(itr->value.GetString());
                if (!klass)
                {
                    break;
                }

                if (klass.is_array())
                {
                    // 原生数组，单独处理
                    isArray = true;
                    break;
                }
                
                constructor ctor = klass.get_constructor();
                obj = ctor.invoke();
                // obj = klass.create();
                if (!recursively && (klass == type::get<Tiny3D::Prefab>()))
                {
                    // prefab
                    mObj = &obj;
                }

                itr = node.FindMember(RTTI_VALUE);
                if (itr == node.MemberEnd())
                {
                    klass.destroy(obj);
                    break;
                }

                obj = obj.get_type().get_raw_type().is_wrapper() ? obj.extract_wrapped_value() : obj;

                const auto& value = itr->value;
                ReadValue(value, klass, obj);
            } while (false);
            
            return obj;
        }

        variant ReadObject(const Value& node, bool recursively = true)
        {
            bool isArray = false;
            return ReadObjectRecursively(node, isArray, recursively);
        }

        TResult ReadNativeArray(const Value& node, variant &obj)
        {
            TResult ret = T3D_OK;

            do
            {
                auto itr = node.FindMember(RTTI_TYPE);
                if (itr == node.MemberEnd())
                {
                    break;
                }

                type klass = type::get_by_name(itr->value.GetString());
                if (!klass)
                {
                    break;
                }

                itr = node.FindMember(RTTI_VALUE);
                if (itr == node.MemberEnd())
                {
                    break;
                }

                const auto& value = itr->value;
                ReadValue(value, klass, obj);
            } while (false);

            return ret;
        }
    };
    
    //--------------------------------------------------------------------------

    JsonSerializerPtr JsonSerializer::create()
    {
        return new JsonSerializer();
    }

    //--------------------------------------------------------------------------

    #define T3D_FILE_MAGIC_KEY      "Magic"
    #define T3D_FILE_MAGIC_VAL      "T3D"
    #define T3D_FILE_VERSION_KEY    "Version"
    #define T3D_FILE_VERSION_STR    "0.0.1"
    #define T3D_FILE_OBJECT_KEY     "Object"


    TResult JsonSerializer::serialize(DataStream &stream, const RTTRObject &obj)
    {
        TResult ret = T3D_OK;
        JsonStream os(stream);
        PrettyWriter<JsonStream> writer(os);

        writer.StartObject();
        {
            // Magic
            writer.Key(T3D_FILE_MAGIC_KEY);
            writer.String(T3D_FILE_MAGIC_VAL);
            // Version
            writer.Key(T3D_FILE_VERSION_KEY);
            writer.String(T3D_FILE_VERSION_STR);
            // Object
            writer.Key(T3D_FILE_OBJECT_KEY);
            writer.StartObject();
            {
                RTTRObjectJsonWriter::WriteObject(writer, obj);
            }
            writer.EndObject();
        }        
        writer.EndObject();
        
        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult ReadJsonHeader(DataStream &stream, Document &doc, Value &value)
    {
        TResult ret = T3D_OK;

        do
        {
            JsonStream is(stream);

            if (doc.ParseStream(is).HasParseError())
            {
                ParseErrorCode errorCode = doc.GetParseError();
                size_t errorPos = doc.GetErrorOffset();
                ret = T3D_ERR_PARSE_STREAM;
                T3D_LOG_ERROR(LOG_TAG_SERIALIZE, "Failed to parse json stream (pos:%u) ! ERROR [%d]", errorPos, errorCode);
                break;
            }

            T3D_ASSERT(doc.IsObject());

            // Magic
            auto itr = doc.FindMember(T3D_FILE_MAGIC_KEY);
            if (itr == doc.MemberEnd())
            {
                ret = T3D_ERR_MISSING_MAGIC;
                T3D_LOG_ERROR(LOG_TAG_SERIALIZE, "Missing magic field !");
                break;
            }
            String magic = itr->value.GetString();
            // Version
            itr = doc.FindMember(T3D_FILE_VERSION_KEY);
            if (itr == doc.MemberEnd())
            {
                ret = T3D_ERR_MISSING_VERSION;
                T3D_LOG_ERROR(LOG_TAG_SERIALIZE, "Missing version field !");
                break;
            }
            String version = itr->value.GetString();
            // Object
            itr = doc.FindMember(T3D_FILE_OBJECT_KEY);
            if (itr == doc.MemberEnd())
            {
                ret = T3D_ERR_MISSING_OBJECT;
                T3D_LOG_ERROR(LOG_TAG_SERIALIZE, "Missing object contain !");
                break;
            }
            value = itr->value;
        } while (false);

        return ret;
    }
    //--------------------------------------------------------------------------

    RTTRObject JsonSerializer::deserialize(DataStream &stream)
    {
        Document doc;
        Value value;
        ReadJsonHeader(stream, doc, value);
        RTTRObjectJsonReader reader;
        return reader.ReadObject(value, false);
    }

    //--------------------------------------------------------------------------
    
    TResult JsonSerializer::deserialize(DataStream &stream, RTTRVariant &obj)
    {
        Document doc;
        Value value;
        ReadJsonHeader(stream, doc, value);
        RTTRObjectJsonReader reader;
        reader.ReadObject(value, obj, false);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}

