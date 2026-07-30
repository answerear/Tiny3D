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
        /// 复合值类型递归展开的层数上限，防止异常的反射注册造成过深递归
        constexpr int32_t kMaxCompoundDepth = 4;

        /// 角度类控件的显示格式，标注单位避免与弧度混淆
        const char * const kAngleFormat = "%.2f deg";

        /// 与序列化层保持一致的元数据 key，标记不参与序列化的属性
        const char * const kMetaNoSerialize = "NO_SERIALIZE";

        /**
         * 拖动过程中缓存的欧拉角。
         *
         * 四元数反解欧拉角的结果并不唯一（万向锁与角度环绕处会取到等价的另一组值），
         * 若每帧都从四元数重新反解作为控件的显示值，拖动到 pitch 超过 90 度一类的
         * 边界时角度会突跳并与用户的拖动方向相抗。因此控件处于激活状态期间沿用缓存
         * 的角度，松手后再回到从四元数反解。
         *
         * ImGui 同一时刻只有一个控件处于激活状态，所以只需缓存一份。
         */
        struct EulerCache
        {
            ImGuiID id {0};
            float   angles[3] {0.0f, 0.0f, 0.0f};
        };

        EulerCache sEulerCache;

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

    bool ImPropertyDrawer::drawObject(const RTTRObject &obj, int32_t depth)
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
            changed = drawProperty(instance, prop, depth) || changed;
        }

        return changed;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawProperty(const RTTRObject &obj, const rttr::property &prop,
        int32_t depth)
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
            drawValue(label, type, value, depth);
            ImGui::EndDisabled();
            return false;
        }

        if (!drawValue(label, type, value, depth))
        {
            return false;
        }

        return prop.set_value(obj, value);
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawValue(const String &label, const RTTRType &type,
        RTTRVariant &value, int32_t depth)
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

        if (type == RTTRType::get<Radian>())
        {
            return drawRadian(label, value);
        }

        if (type == RTTRType::get<Degree>())
        {
            return drawDegree(label, value);
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

        // Viewport / Aabb / Obb 这类已注册的复合值类型，展开逐个编辑子属性
        if (isCompoundValue(type, depth))
        {
            return drawCompound(label, value, depth);
        }

        drawReadOnlyText(label, type.get_name().to_string());

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::isCompoundValue(const RTTRType &type, int32_t depth)
    {
        if (depth >= kMaxCompoundDepth)
        {
            return false;
        }

        // 裸指针与智能指针表示的是对象引用而非值类型，属于资源/对象引用字段的范畴，
        // 不能当成结构体展开
        if (type.is_pointer() || type.is_wrapper()
            || type.is_derived_from(RTTRType::get<Object>()))
        {
            return false;
        }

        return type.is_class() && !type.get_properties().empty();
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawCompound(const String &label, RTTRVariant &value, int32_t depth)
    {
        bool changed = false;

        if (ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            // instance 直接指向 value 内部保存的对象，因此子属性的写入会落在
            // value 上，最终由调用方把整个 value 写回宿主对象
            const RTTRObject sub(value);

            for (auto prop : sub.get_derived_type().get_properties())
            {
                changed = drawProperty(sub, prop, depth + 1) || changed;
            }

            ImGui::TreePop();
        }

        return changed;
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

        const ImGuiID id = ImGui::GetID(label.c_str());

        if (sEulerCache.id == id)
        {
            ::memcpy(angles, sEulerCache.angles, sizeof(angles));
        }

        bool changed = false;

        if (ImGui::DragFloat3(label.c_str(), angles, kAngleDragSpeed,
            0.0f, 0.0f, kAngleFormat))
        {
            Quaternion result;
            result.fromEulerAnglesXYZ(
                Radian(Degree(static_cast<Real>(angles[0])).valueRadians()),
                Radian(Degree(static_cast<Real>(angles[1])).valueRadians()),
                Radian(Degree(static_cast<Real>(angles[2])).valueRadians()));

            value = result;
            changed = true;
        }

        if (ImGui::IsItemActive())
        {
            sEulerCache.id = id;
            ::memcpy(sEulerCache.angles, angles, sizeof(angles));
        }
        else if (sEulerCache.id == id)
        {
            sEulerCache.id = 0;
        }

        return changed;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawRadian(const String &label, RTTRVariant &value)
    {
        const Radian radian = value.get_value<Radian>();

        // 弧度对用户不直观，UI 上统一按角度编辑
        float degrees = static_cast<float>(radian.valueDegrees());

        if (ImGui::DragFloat(label.c_str(), &degrees, kAngleDragSpeed,
            0.0f, 0.0f, kAngleFormat))
        {
            value = Radian(Degree(static_cast<Real>(degrees)).valueRadians());
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawDegree(const String &label, RTTRVariant &value)
    {
        const Degree degree = value.get_value<Degree>();
        float degrees = static_cast<float>(degree.valueDegrees());

        if (ImGui::DragFloat(label.c_str(), &degrees, kAngleDragSpeed,
            0.0f, 0.0f, kAngleFormat))
        {
            value = Degree(static_cast<Real>(degrees));
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
