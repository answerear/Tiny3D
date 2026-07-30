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

        /// 通知场景数据已被修改
        void notifySceneModified();

    protected:
        /// 当前选中的 game object。
        /// 这里用智能指针持有，避免对象在层级面板被删除后 inspector 仍持有悬空指针
        GameObjectPtr mSelectedGameObject {nullptr};
    };

    NS_END
}
