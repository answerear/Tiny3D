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

#pragma once


#include "ImGuiExtensionPrerequisites.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    /**
     * 基于 RTTR 反射的属性绘制器。
     *
     * 遍历对象通过 TPROPERTY 暴露的属性，按属性类型分派到对应的 ImGui 控件，
     * 并把用户的编辑结果写回对象，因此新增组件属性无需改动 inspector 代码。
     *
     * 对调用方而言接口都是无副作用的静态函数，调用方负责在外层建立 ImGui ID 作用域
     * （同一帧绘制多个对象时需要 PushID / PopID 隔离）。内部仅保留拖动过程中的临时
     * 编辑状态（见 drawQuaternion 的欧拉角缓存），不缓存任何对象数据。
     */
    class ImPropertyDrawer
    {
    public:
        /**
         * 遍历并绘制对象的所有属性
         * @param [in] obj : 目标对象的反射实例
         * @param [in] depth : 复合类型递归层级，外部调用保持默认值
         * @return 有任意属性被修改时返回 true
         */
        static bool drawObject(const RTTRObject &obj, int32_t depth = 0);

        /**
         * 绘制单个属性
         * @param [in] obj : 目标对象的反射实例
         * @param [in] prop : 要绘制的属性
         * @param [in] depth : 复合类型递归层级，外部调用保持默认值
         * @return 属性被修改时返回 true
         */
        static bool drawProperty(const RTTRObject &obj, const rttr::property &prop,
            int32_t depth = 0);

    private:
        /**
         * 按类型把属性值分派到具体控件
         * @param [in] label : 控件显示的名称
         * @param [in] type : 属性的声明类型
         * @param [in,out] value : 读出的属性值，被编辑后回写到该变量
         * @param [in] depth : 复合类型递归层级
         * @return 值被修改时返回 true
         */
        static bool drawValue(const String &label, const RTTRType &type,
            RTTRVariant &value, int32_t depth);

        static bool drawArithmetic(const String &label, const RTTRType &type, RTTRVariant &value);
        static bool drawEnumeration(const String &label, const RTTRType &type, RTTRVariant &value);
        static bool drawString(const String &label, RTTRVariant &value);
        static bool drawVector2(const String &label, RTTRVariant &value);
        static bool drawVector3(const String &label, RTTRVariant &value);
        static bool drawVector4(const String &label, RTTRVariant &value);
        static bool drawQuaternion(const String &label, RTTRVariant &value);
        static bool drawRadian(const String &label, RTTRVariant &value);
        static bool drawDegree(const String &label, RTTRVariant &value);
        static bool drawColorRGB(const String &label, RTTRVariant &value);
        static bool drawColorRGBA(const String &label, RTTRVariant &value);

        /**
         * 判断类型是否为需要递归展开子属性的复合值类型
         * @param [in] type : 待判定的类型
         * @param [in] depth : 当前递归层级，超过上限则不再展开
         */
        static bool isCompoundValue(const RTTRType &type, int32_t depth);

        /**
         * 以折叠节点递归展开复合值类型的子属性。
         * 子属性直接写入 value 内部的对象，再由调用方整体写回宿主对象，
         * 因为 RTTR 的值类型属性无法就地修改。
         */
        static bool drawCompound(const String &label, RTTRVariant &value, int32_t depth);

        /// 绘制暂不支持编辑的类型，只显示只读摘要
        static void drawReadOnlyText(const String &label, const String &text);
    };

    NS_END
}
