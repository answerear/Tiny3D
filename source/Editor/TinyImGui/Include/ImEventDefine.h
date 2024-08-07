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


#include "ImPrerequisites.h"


namespace Tiny3D
{
    enum TinyImGuiEventID
    {
        /// 无效事件
        kEvtInvalid = 0,

        /// 点击菜单项
        kEvtMenuItemClicked,
        /// 点击按钮
        kEvtButtonClicked,
        /// 点击树形控件节点
        kEvtTreeNodeClicked,
        /// 点击列表控件列表项
        kEvtListItemClicked,
        /// 点击树形工具栏节点
        kEvtTreeBarNodeClicked,
    };

    // 按钮点击事件附带的参数
    using EventParamButtonClicked = EventParamT1<ImWidget*>;
    // 树节点点击事件附带的参数
    using EventParamTreeNodeClicked = EventParamT1<ImTreeNode*>;
    // 列表项点击事件附带的参数
    using EventParamListItemClicked = EventParamT1<ImListItem*>;
    // 树形工具栏点击节点事件附带的参数
    using EventParamTreeBarNodeClicked = EventParamT1<ImTreeBarNode*>;
}
