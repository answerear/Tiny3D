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


#include "LauncherPrerequisites.h"


namespace Tiny3D
{
    NS_BEGIN(Launcher)

    enum AppEventID
    {
        kEvtApp = 0,
        /// 打开新建工程对话框
        kEvtOpenNewDialog,
        /// 新建工程，事件参数：EventParamNewProject
        kEvtNewProject,
        /// 编辑工程，事件参数：EventParamEditProject
        kEvtEditProject,
        /// 运行工程
        kEvtRunProject,
        /// 重命名工程
        kEvtRenameProject,
        /// 管理 tags
        kEvtManageTags,
        /// 移除工程
        kEvtRemoveProject,
        /// 查询是否选中某个工程，事件参数：EventParamQueryProjectSelected
        kEvtQueryProjectSelected,
    };

    /// arg1 : 路径名称
    /// arg2 : 工程名称
    using EventParamNewProject = EventParamT2<String, String>;

    /// arg1 : 选中的 project 索引
    using EventParamEditProject = EventParamT1<int32_t>;

    /// arg1 : 返回选中的 Project 索引
    using EventParamQueryProjectSelected = EventParamT1<int32_t>;

    NS_END
}

