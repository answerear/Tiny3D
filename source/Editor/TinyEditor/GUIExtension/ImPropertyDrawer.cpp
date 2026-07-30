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


#include "GUIExtension/ImPropertyDrawer.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    namespace
    {
        /// 数值拖动控件的默认步长
        constexpr float kDragSpeed = 0.01f;
        /// 角度拖动控件的步长，单位为度
        constexpr float kAngleDragSpeed = 0.5f;
        /// 字符串编辑缓冲区大小
        constexpr size_t kTextBufferSize = 512;

        /// 与序列化层保持一致的元数据 key，标记不参与序列化的属性
        const char * const kMetaNoSerialize = "NO_SERIALIZE";

        /**
         * 把字符串安全地拷贝进定长缓冲区，并保证以 '\0' 结尾
         */
        void copyToBuffer(char *buffer, size_t bufferSize, const String &text)
        {
            const size_t length = std::min(text.length(), bufferSize - 1);
            ::memcpy(buffer, text.c_str(), length);
            buffer[length] = '\0';
        }

        /**
         * 用 DragScalar 编辑指定算术类型的属性值
         * @tparam T : 属性的算术类型，variant 内保存的就是该类型
         */
        template <typename T>
        bool dragScalar(const String &label, ImGuiDataType dataType, RTTRVariant &value, float speed)
        {
            T scalar = value.get_value<T>();

            if (ImGui::DragScalar(label.c_str(), dataType, &scalar, speed))
            {
                value = scalar;
                return true;
            }

            return false;
        }
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawObject(const RTTRObject &obj)
    {
        // 智能指针包装的对象需要先解包，否则拿不到真正的属性表
        const RTTRObject instance = obj.get_type().get_raw_type().is_wrapper()
            ? obj.get_wrapped_instance() : obj;

        if (!instance.is_valid())
        {
            return false;
        }

        bool changed = false;

        for (auto prop : instance.get_derived_type().get_properties())
        {
            changed = drawProperty(instance, prop) || changed;
        }

        return changed;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawProperty(const RTTRObject &obj, const rttr::property &prop)
    {
        // 不参与序列化的属性改了也存不下来，直接不展示
        if (prop.get_metadata(kMetaNoSerialize))
        {
            return false;
        }

        const String label = prop.get_name().to_string();
        RTTRVariant value = prop.get_value(obj);

        if (!value.is_valid())
        {
            drawReadOnlyText(label, "<null>");
            return false;
        }

        const RTTRType type = prop.get_type();

        if (prop.is_readonly())
        {
            ImGui::BeginDisabled();
            drawValue(label, type, value);
            ImGui::EndDisabled();
            return false;
        }

        if (!drawValue(label, type, value))
        {
            return false;
        }

        return prop.set_value(obj, value);
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawValue(const String &label, const RTTRType &type, RTTRVariant &value)
    {
        if (type == RTTRType::get<bool>())
        {
            bool flag = value.to_bool();

            if (ImGui::Checkbox(label.c_str(), &flag))
            {
                value = flag;
                return true;
            }

            return false;
        }

        if (type.is_arithmetic())
        {
            return drawArithmetic(label, type, value);
        }

        if (type.is_enumeration())
        {
            return drawEnumeration(label, type, value);
        }

        if (type == RTTRType::get<String>())
        {
            return drawString(label, value);
        }

        if (type == RTTRType::get<Vector2>())
        {
            return drawVector2(label, value);
        }

        if (type == RTTRType::get<Vector3>())
        {
            return drawVector3(label, value);
        }

        if (type == RTTRType::get<Vector4>())
        {
            return drawVector4(label, value);
        }

        if (type == RTTRType::get<Quaternion>())
        {
            return drawQuaternion(label, value);
        }

        if (type == RTTRType::get<ColorRGB>())
        {
            return drawColorRGB(label, value);
        }

        if (type == RTTRType::get<ColorRGBA>())
        {
            return drawColorRGBA(label, value);
        }

        // UUID 由引擎维护，不允许手工编辑，只作展示
        if (type == RTTRType::get<UUID>())
        {
            drawReadOnlyText(label, value.get_value<UUID>().toString());
            return false;
        }

        if (value.is_sequential_container())
        {
            const auto view = value.create_sequential_view();
            drawReadOnlyText(label, std::to_string(view.get_size()) + " items");
            return false;
        }

        if (value.is_associative_container())
        {
            const auto view = value.create_associative_view();
            drawReadOnlyText(label, std::to_string(view.get_size()) + " entries");
            return false;
        }

        drawReadOnlyText(label, type.get_name().to_string());

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawArithmetic(const String &label, const RTTRType &type, RTTRVariant &value)
    {
        if (type == RTTRType::get<float32_t>())
        {
            return dragScalar<float32_t>(label, ImGuiDataType_Float, value, kDragSpeed);
        }

        if (type == RTTRType::get<float64_t>())
        {
            return dragScalar<float64_t>(label, ImGuiDataType_Double, value, kDragSpeed);
        }

        if (type == RTTRType::get<int8_t>())
        {
            return dragScalar<int8_t>(label, ImGuiDataType_S8, value, 1.0f);
        }

        if (type == RTTRType::get<uint8_t>())
        {
            return dragScalar<uint8_t>(label, ImGuiDataType_U8, value, 1.0f);
        }

        if (type == RTTRType::get<int16_t>())
        {
            return dragScalar<int16_t>(label, ImGuiDataType_S16, value, 1.0f);
        }

        if (type == RTTRType::get<uint16_t>())
        {
            return dragScalar<uint16_t>(label, ImGuiDataType_U16, value, 1.0f);
        }

        if (type == RTTRType::get<int32_t>())
        {
            return dragScalar<int32_t>(label, ImGuiDataType_S32, value, 1.0f);
        }

        if (type == RTTRType::get<uint32_t>())
        {
            return dragScalar<uint32_t>(label, ImGuiDataType_U32, value, 1.0f);
        }

        if (type == RTTRType::get<int64_t>())
        {
            return dragScalar<int64_t>(label, ImGuiDataType_S64, value, 1.0f);
        }

        if (type == RTTRType::get<uint64_t>())
        {
            return dragScalar<uint64_t>(label, ImGuiDataType_U64, value, 1.0f);
        }

        bool ok = false;
        const String text = value.to_string(&ok);
        drawReadOnlyText(label, ok ? text : type.get_name().to_string());

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawEnumeration(const String &label, const RTTRType &type, RTTRVariant &value)
    {
        const rttr::enumeration enumeration = type.get_enumeration();

        if (!enumeration.is_valid())
        {
            drawReadOnlyText(label, type.get_name().to_string());
            return false;
        }

        bool ok = false;
        const String current = value.to_string(&ok);
        bool changed = false;

        if (ImGui::BeginCombo(label.c_str(), ok ? current.c_str() : ""))
        {
            for (const auto &name : enumeration.get_names())
            {
                const String item = name.to_string();
                const bool selected = (item == current);

                if (ImGui::Selectable(item.c_str(), selected) && !selected)
                {
                    value = enumeration.name_to_value(name);
                    changed = true;
                }

                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawString(const String &label, RTTRVariant &value)
    {
        char buffer[kTextBufferSize] = { 0 };
        copyToBuffer(buffer, kTextBufferSize, value.to_string());

        if (ImGui::InputText(label.c_str(), buffer, kTextBufferSize))
        {
            value = String(buffer);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawVector2(const String &label, RTTRVariant &value)
    {
        const Vector2 vector = value.get_value<Vector2>();
        float components[2] =
        {
            static_cast<float>(vector.x()),
            static_cast<float>(vector.y())
        };

        if (ImGui::DragFloat2(label.c_str(), components, kDragSpeed))
        {
            value = Vector2(static_cast<Real>(components[0]),
                static_cast<Real>(components[1]));
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawVector3(const String &label, RTTRVariant &value)
    {
        const Vector3 vector = value.get_value<Vector3>();
        float components[3] =
        {
            static_cast<float>(vector.x()),
            static_cast<float>(vector.y()),
            static_cast<float>(vector.z())
        };

        if (ImGui::DragFloat3(label.c_str(), components, kDragSpeed))
        {
            value = Vector3(static_cast<Real>(components[0]),
                static_cast<Real>(components[1]),
                static_cast<Real>(components[2]));
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawVector4(const String &label, RTTRVariant &value)
    {
        const Vector4 vector = value.get_value<Vector4>();
        float components[4] =
        {
            static_cast<float>(vector.x()),
            static_cast<float>(vector.y()),
            static_cast<float>(vector.z()),
            static_cast<float>(vector.w())
        };

        if (ImGui::DragFloat4(label.c_str(), components, kDragSpeed))
        {
            value = Vector4(static_cast<Real>(components[0]),
                static_cast<Real>(components[1]),
                static_cast<Real>(components[2]),
                static_cast<Real>(components[3]));
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawQuaternion(const String &label, RTTRVariant &value)
    {
        const Quaternion quaternion = value.get_value<Quaternion>();

        // 四元数对用户不直观，UI 上统一用欧拉角（度）编辑
        Radian pitch;
        Radian yaw;
        Radian roll;
        quaternion.toEulerAnglesXYZ(pitch, yaw, roll);

        float angles[3] =
        {
            static_cast<float>(pitch.valueDegrees()),
            static_cast<float>(yaw.valueDegrees()),
            static_cast<float>(roll.valueDegrees())
        };

        if (ImGui::DragFloat3(label.c_str(), angles, kAngleDragSpeed))
        {
            Quaternion result;
            result.fromEulerAnglesXYZ(
                Radian(Degree(static_cast<Real>(angles[0])).valueRadians()),
                Radian(Degree(static_cast<Real>(angles[1])).valueRadians()),
                Radian(Degree(static_cast<Real>(angles[2])).valueRadians()));

            value = result;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawColorRGB(const String &label, RTTRVariant &value)
    {
        const ColorRGB color = value.get_value<ColorRGB>();
        float components[3] = { color.red(), color.green(), color.blue() };

        if (ImGui::ColorEdit3(label.c_str(), components))
        {
            value = ColorRGB(components[0], components[1], components[2]);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawColorRGBA(const String &label, RTTRVariant &value)
    {
        const ColorRGBA color = value.get_value<ColorRGBA>();
        float components[4] =
        {
            color.red(), color.green(), color.blue(), color.alpha()
        };

        if (ImGui::ColorEdit4(label.c_str(), components))
        {
            value = ColorRGBA(components[0], components[1],
                components[2], components[3]);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    void ImPropertyDrawer::drawReadOnlyText(const String &label, const String &text)
    {
        ImGui::BeginDisabled();
        ImGui::LabelText(label.c_str(), "%s", text.c_str());
        ImGui::EndDisabled();
    }

    //--------------------------------------------------------------------------
    NS_END
}
