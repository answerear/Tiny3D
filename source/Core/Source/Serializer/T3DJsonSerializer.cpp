/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "Serializer/T3DJsonSerializer.h"
#include "T3DErrorDef.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <prettywriter.h>
#include <document.h>


namespace Tiny3D
{
    using namespace rapidjson;
    using namespace rttr;

    #define RTTI_TYPE       "RTTI_Type"
    #define RTTI_VALUE      "RTTI_Value"
    #define RTTI_MAP_KEY    "RTTI_Map_Key"
    #define RTTI_MAP_VALUE  "RTTI_Map_Value"

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

#if 0
    class JsonWriter
    {
    public:
        //--------------------------------------------------------------------------

        // static void write_object(const instance& obj, PrettyWriter<JsonStream>& writer, bool has_type);
        //
        // static bool write_variant(const variant& var, PrettyWriter<JsonStream>& writer, bool has_type);

        //--------------------------------------------------------------------------

        static bool write_atomic_types_to_json(const type& t, const variant& var, PrettyWriter<JsonStream>& writer)
        {
            if (t.is_arithmetic())
            {
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
                bool ok = false;
                auto result = var.to_string(&ok);
                if (ok)
                {
                    writer.String(var.to_string());
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
                writer.String(var.to_string());
                return true;
            }

            return false;
        }

        //--------------------------------------------------------------------------

        static void write_sequential_container(const variant_sequential_view& view, PrettyWriter<JsonStream>& writer, bool has_type)
        {
            writer.StartArray();
            for (const auto& item : view)
            {
                if (item.is_sequential_container())
                {
                    write_sequential_container(item.create_sequential_view(), writer, has_type);
                }
                else
                {
                    variant wrapped_var = item.extract_wrapped_value();
                    type value_type = wrapped_var.get_type();
                    if (value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration())
                    {
                        write_atomic_types_to_json(value_type, wrapped_var, writer);
                    }
                    else // object
                    {
                        write_object(wrapped_var, writer, has_type);
                    }
                }
            }
            writer.EndArray();
        }

        //--------------------------------------------------------------------------

        static void write_associative_container(const variant_associative_view& view, PrettyWriter<JsonStream>& writer, bool has_type)
        {
            static const string_view key_name("key");
            static const string_view value_name("value");

            writer.StartArray();

            if (view.is_key_only_type())
            {
                for (auto& item : view)
                {
                    write_variant(item.first, writer, has_type);
                }
            }
            else
            {
                for (auto& item : view)
                {
                    writer.StartObject();
                    
                    writer.String(key_name.data(), static_cast<rapidjson::SizeType>(key_name.length()), false);
                    write_variant(item.first, writer, has_type);

                    writer.String(value_name.data(), static_cast<rapidjson::SizeType>(value_name.length()), false);
                    write_variant(item.second, writer, has_type);

                    writer.EndObject();
                }
            }

            writer.EndArray();
        }

        //--------------------------------------------------------------------------

        static bool write_variant(const variant& var, PrettyWriter<JsonStream>& writer, bool has_type)
        {
            auto value_type = var.get_type();
            auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
            bool is_wrapper = wrapped_type != value_type;

            if (write_atomic_types_to_json(is_wrapper ? wrapped_type : value_type,
                is_wrapper ? var.extract_wrapped_value() : var, writer))
            {
            }
            else if (var.is_sequential_container())
            {
                write_sequential_container(var.create_sequential_view(), writer, has_type);
            }
            else if (var.is_associative_container())
            {
                write_associative_container(var.create_associative_view(), writer, has_type);
            }
            else
            {
                instance child_obj = var;
                instance obj = is_wrapper ? child_obj.get_wrapped_instance() : child_obj;
                auto child_props = obj.get_derived_type().get_properties();
                // auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
                if (!child_props.empty())
                {
                    write_object(var, writer, has_type);
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

            return true;
        }

        //--------------------------------------------------------------------------

        static void write_object(const instance& obj2, PrettyWriter<JsonStream>& writer, bool has_type)
        {
            writer.StartObject();

            instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;

            auto prop_list = obj.get_derived_type().get_properties();
            for (auto prop : prop_list)
            {
                if (prop.get_metadata("NO_SERIALIZE"))
                    continue;

                variant prop_value = prop.get_value(obj);
                if (!prop_value)
                    continue; // cannot serialize, because we cannot retrieve the value

                // property name
                const auto name = prop.get_name();
                writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);

                if (has_type)
                {
                    // property type
                    writer.StartObject();
                    auto class_name = instance(prop_value).get_derived_type().get_name();
                    writer.String(class_name.data(), static_cast<rapidjson::SizeType>(class_name.length()), false);
                }

                if (!write_variant(prop_value, writer, has_type))
                {
                    std::cerr << "cannot serialize property: " << name << std::endl;
                }

                if (has_type)
                {
                    // property type
                    writer.EndObject();
                }
            }

            writer.EndObject();
        }
    };
#endif
    
    //--------------------------------------------------------------------------

    

    //--------------------------------------------------------------------------

    JsonSerializerPtr JsonSerializer::create()
    {
        JsonSerializerPtr serializer = new JsonSerializer();
        // serializer->release();
        return serializer;
    }

    //--------------------------------------------------------------------------

    JsonSerializer::JsonSerializer()
    {

    }

    //--------------------------------------------------------------------------

    JsonSerializer::~JsonSerializer()
    {

    }

    //--------------------------------------------------------------------------

    void WriteObject(rapidjson::PrettyWriter<JsonStream> &writer, const rttr::instance &obj);
    bool WriteVariant(PrettyWriter<JsonStream> &writer, const variant &var);

    bool WriteAtomicType(PrettyWriter<JsonStream> &writer, const type &t, const variant &var)
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
                writer.String(var.to_string());
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

    void WriteSequentialContainer(PrettyWriter<JsonStream> &writer, const variant_sequential_view &view)
    {
        writer.StartArray();
        for (const auto& item : view)
        {
            if (item.is_sequential_container())
            {
                WriteSequentialContainer(writer, item.create_sequential_view());
            }
            else
            {
                variant wrapped_var = item.extract_wrapped_value();
                type value_type = wrapped_var.get_type();
                if (value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration())
                {
                    WriteAtomicType(writer, value_type, wrapped_var);
                }
                else // object
                {
                    WriteObject(writer, wrapped_var);
                }
            }
        }
        writer.EndArray();
    }

    void WriteAssociatvieContainer(PrettyWriter<JsonStream> &writer, const variant_associative_view &view)
    {
        static const string_view key_name(RTTI_MAP_KEY);
        static const string_view value_name(RTTI_MAP_VALUE);

        writer.StartArray();

        if (view.is_key_only_type())
        {
            for (auto& item : view)
            {
                WriteVariant(writer, item.first);
            }
        }
        else
        {
            for (auto& item : view)
            {
                writer.StartObject();
                writer.String(key_name.data(), static_cast<rapidjson::SizeType>(key_name.length()), false);

                WriteVariant(writer, item.first);

                writer.String(value_name.data(), static_cast<rapidjson::SizeType>(value_name.length()), false);

                WriteVariant(writer, item.second);

                writer.EndObject();
            }
        }

        writer.EndArray();
    }

    bool WriteVariant(PrettyWriter<JsonStream> &writer, const variant &var)
    {
        auto value_type = var.get_type();
        auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
        bool is_wrapper = wrapped_type != value_type;

        if (WriteAtomicType(writer, is_wrapper ? wrapped_type : value_type,
                                   is_wrapper ? var.extract_wrapped_value() : var))
        {
        }
        else if (var.is_sequential_container())
        {
            WriteSequentialContainer(writer, var.create_sequential_view());
        }
        else if (var.is_associative_container())
        {
            WriteAssociatvieContainer(writer, var.create_associative_view());
        }
        else
        {
            const instance child_obj = var;
            const instance obj = is_wrapper ? child_obj.get_wrapped_instance() : child_obj;
            const auto child_props = obj.get_derived_type().get_properties();
            //auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
            if (!child_props.empty())
            {
                WriteObject(writer, var);
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

        return true;
    }

    bool WriteProperty(rapidjson::PrettyWriter<JsonStream> &writer, const property &prop, const variant &value)
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

    void WriteObject(rapidjson::PrettyWriter<JsonStream> &writer, const rttr::instance &obj)
    {
        const instance obj2 = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
        
        const auto className = obj2.get_derived_type().get_name();
        
        // Type
        writer.Key(RTTI_TYPE);
        writer.String(className.data(), static_cast<rapidjson::SizeType>(className.length()), false);
        
        // Properties
        writer.Key(RTTI_VALUE);
        writer.StartObject();

        auto prop_list = obj2.get_derived_type().get_properties();
        for (auto prop : prop_list)
        {
            if (prop.get_metadata("NO_SERIALIZE"))
                continue;

            variant prop_value = prop.get_value(obj2);
            if (!prop_value)
                continue; // cannot serialize, because we cannot retrieve the value

            WriteProperty(writer, prop, prop_value);
        }
        writer.EndObject();
    }
    
    TResult JsonSerializer::serialize(DataStream &stream, const RTTRObject &obj)
    {
        TResult ret = T3D_OK;

        JsonStream os(stream);
        PrettyWriter<JsonStream> writer(os);

        writer.StartObject();
        WriteObject(writer, obj);        
        writer.EndObject();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    bool ReadAtomicType(const Value &node, const type &klass, variant &obj)
    {
        bool ret = false;
        
        if (klass.is_arithmetic())
        {
            if (klass == type::get<bool>())
                obj = node.GetBool();
            else if (klass == type::get<int32_t>() || klass == type::get<int16_t>() || klass == type::get<int8_t>())
                obj = node.GetInt();
            else if (klass == type::get<uint32_t>() || klass == type::get<uint16_t>() || klass == type::get<uint8_t>())
                obj = node.GetUint();
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

    variant ReadObject(const Value &node)
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

            obj = klass.create();

            itr = node.FindMember(RTTI_VALUE);
            if (itr == node.MemberEnd())
            {
                klass.destroy(obj);
                break;
            }

            obj = obj.get_type().get_raw_type().is_wrapper() ? obj.extract_wrapped_value() : obj;
            
            const auto &value = itr->value;

            if (ReadAtomicType(value, klass, obj))
            {
                
            }
            else if (klass.is_sequential_container())
            {
                        
            }
            else if (klass.is_associative_container())
            {
                        
            }
            else
            {
                for (Value::ConstMemberIterator it = value.MemberBegin(); it != value.MemberEnd(); ++it)
                {
                    // property name
                    const auto &name = it->name;

                    // property type & value
                    variant prop = ReadObject(it->value);
                    auto pro = klass.get_property(name.GetString());
                    bool isWrapper = pro.get_type().get_raw_type().is_wrapper();
                    if (isWrapper)
                    {
                        // pro.set_value("", prop);
                        bool rt = prop.convert(type::get<SmartPtr<Object>>());
                        bool ret = klass.set_property_value(name.GetString(), obj, pro);
                        T3D_ASSERT(ret);
                    }
                    else
                    {
                        bool ret = klass.set_property_value(name.GetString(), obj, prop);
                        T3D_ASSERT(ret);
                    }
                    
                }
            }
        } while (false);
        
        return obj;
    }
    
    RTTRObject JsonSerializer::deserialize(DataStream &stream)
    {
        RTTRObject ret;
        
        do
        {
            JsonStream is(stream);
            
            Document doc;
            if (doc.ParseStream(is).HasParseError())
            {
                // doc.GetParseError();
                // doc.GetErrorOffset();
                break;
            }

            T3D_ASSERT(doc.IsObject());
            return ReadObject(doc);
        } while (false);
        
        return ret;
    }
}
