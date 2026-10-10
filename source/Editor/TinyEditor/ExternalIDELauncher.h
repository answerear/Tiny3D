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

    struct ExternalIDEInfo
    {
        String name {};
        String path {};
    };

    class ExternalIDELauncher
    {
    public:
        /// 用当前 EditorSettings 打开解决方案；path 为空则走系统关联
        static TResult openSolution(const String &slnPath);

        /// 扫描本机 Visual Studio（vswhere），失败时列表为空
        static void discoverVisualStudio(TArray<ExternalIDEInfo> &out);

        /// 比较两条路径是否指向同一文件（Windows 忽略大小写）
        static bool pathsEqual(const String &lhs, const String &rhs);

        /// 只统一分隔符，不解析 . / ..（formatPath 会把 devenv.exe 吃成 devenvexe）
        static String normalizePath(const String &path);

        /// 修复已被 formatPath 吃掉扩展名点号的路径（devenvexe -> devenv.exe）
        static String repairStrippedExtension(const String &path);

    protected:
        static TResult openWithShell(const String &slnPath);

        static String findVswhere();
    };

    NS_END
}
