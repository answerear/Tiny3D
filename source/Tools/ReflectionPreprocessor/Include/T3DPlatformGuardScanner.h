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

#ifndef __T3D_PLATFORM_GUARD_SCANNER_H__
#define __T3D_PLATFORM_GUARD_SCANNER_H__


#include "T3DRPPrerequisites.h"

#include <set>


namespace Tiny3D
{
    /**
     * @brief 源文件条件编译守卫扫描器
     *
     * 对头文件做轻量级文本扫描，识别已知平台宏的 #if/#ifdef 块，
     * 建立「行号 → 条件编译守卫」映射。用于在代码生成阶段将守卫
     * 传递到 .generated.cpp 文件中。
     */
    class PlatformGuardScanner
    {
    public:
        PlatformGuardScanner();

        /**
         * @brief 扫描指定头文件，构建行号→条件映射
         * @param [in] filePath : 头文件完整路径
         */
        void scan(const String &filePath);

        /**
         * @brief 查询指定文件、指定行号所处的条件编译守卫
         * @param [in] filePath : 头文件完整路径
         * @param [in] line : 行号（1-based）
         * @return 条件编译守卫字符串（如 "T3D_OS_DESKTOP"），空串表示无守卫
         *
         * 如果该行被多层已知平台宏嵌套，返回最内层的条件。
         */
        String getGuardAtLine(const String &filePath, uint32_t line) const;

        /**
         * @brief 判断指定文件是否已经扫描过
         */
        bool isScanned(const String &filePath) const;

    private:
        struct GuardRange
        {
            uint32_t startLine;
            uint32_t endLine;
            String   condition;
        };

        /**
         * @brief 判断宏名是否属于已知平台宏集合
         */
        bool isKnownPlatformMacro(const String &macroName) const;

        /**
         * @brief 从 #if 指令行中提取宏名
         *
         * 支持格式：
         *   #if defined(X)
         *   #if defined X
         *   #ifdef X
         *   #if !defined(X)  → 返回 "!X"
         *   #ifndef X        → 返回 "!X"
         */
        String extractMacroFromDirective(const String &line) const;

        /// 已知平台宏集合
        std::set<String>    mKnownMacros;

        /// 文件→守卫区间列表
        TMap<String, std::vector<GuardRange>>   mFileGuards;
    };
}


#endif  /*__T3D_PLATFORM_GUARD_SCANNER_H__*/
