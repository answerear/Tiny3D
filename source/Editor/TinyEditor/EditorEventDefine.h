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


#include "EditorPrerequisites.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    enum AppEventID
    {
        /// App 进入后台，参数：nullptr
        kEvtAppEnterBackground = kEvtApp,
        /// App 进入前台，参数：nullptr
        kEvtAppEnterForeground,

        /// App 获得焦点，参数：nullptr
        kEvtAppFocusGained,
        /// App 失去焦点，参数：nullptr
        kEvtAppFocusLost,

        /// 层级节点被点击，参数：EventParamHierarchyNodeClicked
        kEvtHierarchyNodeClicked,

        /// 打开场景，参数：EventParamOpenScene
        kEvtOpenScene,
        /// 场景数据被修改或者被保存，参数：EventParamModifiedScene
        kEvtModifyScene,
        
        kEvtMax
    };

    /// ImTreeNode * : 被点击的节点
    using EventParamHierarchyNodeClicked = EventParamT1<ImTreeNode*>;
    
    /// Scene * : 打开的场景对象
    using EventParamOpenScene = EventParamT1<Scene*>;
    
    /// bool : true 为设置修改，false 为清除修改
    using EventParamModifyScene = EventParamT1<bool>;

    NS_END
}
