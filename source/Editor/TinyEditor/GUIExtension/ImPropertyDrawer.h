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
     * 呈现细节由属性声明处的 metadata 标注驱动（见 UIHints）：取值范围、悬停说明、
     * 分组标题、显示名与是否展示都跟着属性定义走，因此约束与文案的调整不必改动这里。
     *
     * 对调用方而言接口都是无副作用的静态函数，调用方负责在外层建立 ImGui ID 作用域
     * （同一帧绘制多个对象时需要 PushID / PopID 隔离）。内部只保留与当前交互绑定的
     * 临时状态：拖动中的欧拉角缓存、资产选择器弹窗的候选列表与搜索词，以及本次
     * drawObject 是否改写过资源引用的标志，不缓存任何对象数据。
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

        /**
         * 最近一次顶层 drawObject 期间是否有资源引用属性被改写
         * @remarks 资源引用字段存的是资产 UUID，改写它并不会让引擎重新加载资源
         *          （引擎在 onLoadResource 里才按 UUID 取资产），调用方需要据此
         *          让对象重新加载一次资源，改动才会体现到画面上。
         *          标志在 depth 为 0 的 drawObject 入口处清零，因此要在每次
         *          drawObject 返回后立即查询。
         */
        static bool wasAssetReferenceChanged();

    private:
        /**
         * 属性 metadata 给绘制层的呈现提示。
         *
         * 一个属性的提示由 readUIHints 一次性读出后随分派向下传递。容器元素这类
         * 没有自己属性声明的场合沿用宿主属性的提示（如逐元素套用同一取值范围）。
         */
        struct UIHints
        {
            /// 悬停提示文本，空表示不显示
            String  tooltip;
            /// 要插在属性前的分组标题，空表示不分组
            String  header;
            /// 覆盖属性名的显示名，空表示沿用属性名
            String  displayName;
            /**
             * 取值范围是否可用。
             * 两个边界都标注且下界小于上界时才成立，此时数值与角度属性改用滑条
             */
            bool    hasRange {false};
            double  rangeMin {0.0};
            double  rangeMax {0.0};
        };

        /// 读出属性上标注的全部呈现提示
        static UIHints readUIHints(const rttr::property &prop);

        /**
         * 绘制属性的值部分
         * @param [in] obj : 目标对象的反射实例
         * @param [in] prop : 要绘制的属性
         * @param [in] label : 控件显示的名称
         * @param [in] hints : 属性的呈现提示
         * @param [in] depth : 复合类型递归层级
         * @return 属性被修改时返回 true
         * @remarks 从 drawProperty 拆出来，是为了让悬停提示能覆盖属性画出的全部控件：
         *          分组一旦开始就必须配对结束，值的各条绘制分支却要能提前返回
         */
        static bool drawPropertyValue(const RTTRObject &obj, const rttr::property &prop,
            const String &label, const UIHints &hints, int32_t depth);

        /**
         * 按类型把属性值分派到具体控件
         * @param [in] label : 控件显示的名称
         * @param [in] type : 属性的声明类型
         * @param [in,out] value : 读出的属性值，被编辑后回写到该变量
         * @param [in] depth : 复合类型递归层级
         * @param [in] hints : 属性的呈现提示
         * @return 值被修改时返回 true
         */
        static bool drawValue(const String &label, const RTTRType &type,
            RTTRVariant &value, int32_t depth, const UIHints &hints);

        /**
         * 用拖动或滑条控件编辑算术类型的属性值
         * @tparam T : 属性的算术类型，variant 内保存的就是该类型
         */
        template <typename T>
        static bool drawScalar(const String &label, ImGuiDataType dataType,
            RTTRVariant &value, float speed, const UIHints &hints);

        /**
         * 用拖动或滑条控件编辑以度为单位的角度值
         * @param [in,out] degrees : 当前角度，被编辑后写回该变量
         */
        static bool drawAngle(const String &label, float &degrees, const UIHints &hints);

        static bool drawArithmetic(const String &label, const RTTRType &type,
            RTTRVariant &value, const UIHints &hints);
        static bool drawEnumeration(const String &label, const RTTRType &type, RTTRVariant &value);
        static bool drawString(const String &label, RTTRVariant &value);
        static bool drawVector2(const String &label, RTTRVariant &value);
        static bool drawVector3(const String &label, RTTRVariant &value);
        static bool drawVector4(const String &label, RTTRVariant &value);
        static bool drawQuaternion(const String &label, RTTRVariant &value);
        static bool drawRadian(const String &label, RTTRVariant &value, const UIHints &hints);
        static bool drawDegree(const String &label, RTTRVariant &value, const UIHints &hints);
        static bool drawColorRGB(const String &label, RTTRVariant &value);
        static bool drawColorRGBA(const String &label, RTTRVariant &value);

        /**
         * 绘制资源引用字段：显示所引用资产的名字，并提供选择器与清除入口
         * @param [in] label : 控件显示的名称
         * @param [in] assetType : 可被引用的资产种类，用于过滤选择器的候选项
         * @param [in,out] value : 属性里保存的资产 UUID，改选后写回该变量
         * @return 引用被改写时返回 true
         */
        static bool drawAssetReference(const String &label, Meta::Type assetType,
            RTTRVariant &value);

        /**
         * 绘制对象引用字段（裸指针 / 智能指针）
         * @remarks 对象引用指向的是运行期对象而非资产，没有可枚举的候选集合，
         *          因此只展示引用目标并接受拖拽赋值，不提供下拉选择
         */
        static bool drawObjectReference(const String &label, const RTTRType &type,
            RTTRVariant &value);

        /**
         * 以折叠列表展开顺序容器，可编辑元素并增删
         * @param [in] label : 控件显示的名称
         * @param [in,out] value : 属性读出的容器值，元素改动落在该变量上
         * @param [in] depth : 递归层级，元素按 depth + 1 继续分派
         * @param [in] hints : 宿主属性的呈现提示，逐元素沿用
         * @return 容器内容被改写时返回 true
         */
        static bool drawSequentialContainer(const String &label, RTTRVariant &value,
            int32_t depth, const UIHints &hints);

        /**
         * 判断属性是否承载场景结构。
         * 这类属性存的是对象间的组织关系而非可独立编辑的数据，从 inspector 改写会
         * 破坏场景，因此只做展示。
         */
        static bool isStructuralProperty(const rttr::property &prop);

        /// 只显示容器规模的只读摘要
        static void drawContainerSummary(const String &label, const RTTRVariant &value);

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
