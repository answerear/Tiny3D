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


#include "UIDockingWindow.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    class UIInspectorWindow : public UIDockingWindow, public EventHandler
    {
    public:
        UIInspectorWindow() = default;
        ~UIInspectorWindow() override = default;

    protected:
        TResult onCreate() override;

        void onDestroy() override;

        void onGUI() override;

        /// 响应 hierarchy 选中 game object 的通知
        bool onGameObjectSelected(EventParam *param, TINSTANCE sender);

        /// 绘制 game object 自身的信息（激活状态、名称等）
        void drawHeader();

        /// 逐个绘制挂在 game object 上的组件
        void drawComponents();

        /// 绘制单个组件的折叠面板，以及 Reset / Remove 右键菜单
        void drawComponent(Component *component);

        /// 绘制底部的 Add Component 按钮与带搜索的候选组件菜单
        void drawAddComponentButton();

        /// 从反射信息里收集所有可添加的组件类型，填充候选列表
        void collectComponentCandidates();

        /// 执行本帧被推迟的组件增删与重置操作
        void applyPendingActions();

        /**
         * 把组件的所有属性恢复为该类型的默认值
         * @param [in] component : 待重置的组件
         * @return 有属性被改写时返回 true
         */
        bool resetComponent(Component *component);

        /// 通知场景数据已被修改
        void notifySceneModified();

    protected:
        /// Add Component 菜单里的一个候选组件类型
        struct ComponentCandidate
        {
            /// RTTR 注册的完整类名，addComponent 按它查找类型
            String  fullName;
            /// 去掉命名空间前缀后用于显示的名字
            String  displayName;
            /// 是否为 transform 系组件，一个 game object 上只能挂一个
            bool    isTransform {false};
        };

        /// 当前选中的 game object。
        /// 这里用智能指针持有，避免对象在层级面板被删除后 inspector 仍持有悬空指针
        GameObjectPtr mSelectedGameObject {nullptr};

        /// Add Component 的候选类型列表，菜单弹出时重新收集
        TArray<ComponentCandidate> mComponentCandidates;

        /// Add Component 菜单的搜索过滤器
        ImGuiTextFilter mComponentFilter;

        // 下面三个成员记录本帧待执行的组件操作。ImGui 绘制期间直接增删组件会让
        // drawComponents 正在遍历的组件表失效，因此先记下来，等绘制结束后统一执行。

        /// 待添加组件的完整类名，为空表示无操作
        String mPendingAddComponent;
        /// 待移除的组件，为空表示无操作
        ComponentPtr mPendingRemoveComponent {nullptr};
        /// 待重置的组件，为空表示无操作
        ComponentPtr mPendingResetComponent {nullptr};
    };

    NS_END
}
