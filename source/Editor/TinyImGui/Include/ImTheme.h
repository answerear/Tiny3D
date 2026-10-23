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
    /**
     * @brief 编辑器 / UI 主题枚举
     */
    enum class ImThemeId : int32_t
    {
        kUnityDark = 0,         ///< Unity Professional 深色主题（默认）
        kUnityLight = 1,        ///< Unity Personal 浅色主题（预留）
        kImGuiDark = 2          ///< ImGui 原生深色主题（回退/对比用）
    };

    /**
     * @brief 扩展语义色板（覆盖 ImGuiCol_ 未直接区分的编辑器视觉角色）
     */
    struct ImSemanticPalette
    {
        ImVec4 toolbarBg;               ///< 面板顶部工具栏背景色
        ImVec4 statusBarBg;             ///< 路径栏与底部状态栏背景色
        ImVec4 sectionHeaderBg;         ///< Inspector 组件 CollapsingHeader 常态背景色
        ImVec4 sectionHeaderHovered;    ///< Inspector 组件头悬停背景色
        ImVec4 sectionHeaderActive;     ///< Inspector 组件头按下背景色
        ImVec4 selectionFocused;        ///< 树/列表节点在当前面板拥有焦点时的选中背景色
        ImVec4 selectionUnfocused;      ///< 树/列表节点在当前面板失去焦点时的选中背景色
        ImVec4 searchBoxBg;             ///< 搜索框内凹背景色
        ImVec4 searchBoxBorder;         ///< 搜索框常态 1px 边框色
        ImVec4 searchBoxBorderFocused;  ///< 搜索框聚焦/激活时 1px 亮蓝边框色
        ImVec4 prefabText;              ///< Prefab 实例名称文字颜色
        ImVec4 axisColors[4];           ///< Vector / Euler 分量前缀色 (X, Y, Z, W)
        ImVec4 logInfo;                 ///< 普通日志与消息颜色
        ImVec4 logWarning;              ///< 警告日志与消息框提示颜色
        ImVec4 logError;                ///< 错误日志颜色
        ImVec4 globalTint;              ///< 全局色彩乘算因子（供 Play Mode 染色使用）
    };

    /**
     * @brief 全局 ImGui 样式与主题管理器
     */
    class TINYIMGUI_API ImTheme
    {
    public:
        /**
         * @brief 初始化并应用指定主题
         * @param [in] theme : 目标主题 ID
         */
        static void applyTheme(ImThemeId theme = ImThemeId::kUnityDark);

        /**
         * @brief 获取当前主题 ID
         */
        static ImThemeId getCurrentTheme() { return msCurrentTheme; }

        /**
         * @brief 获取当前生效的语义色板（若全局染色处于激活态，返回的 UI 背景/控件色已含染色）
         */
        static const ImSemanticPalette &getPalette();

        /**
         * @brief 设置或更新全局染色乘算因子（例如 Playmode Tint）
         */
        static void setGlobalTint(const ImVec4 &tint);

        /**
         * @brief 获取当前配置的全局染色乘算因子（未染色基准值）
         */
        static const ImVec4 &getGlobalTint() { return msBasePalette.globalTint; }

        /**
         * @brief 获取默认的全局染色乘算因子
         */
        static ImVec4 getDefaultGlobalTint() { return ImVec4(0.800f, 0.800f, 0.800f, 1.00f); }

        /**
         * @brief 切换全局染色激活状态（例如进入/退出 Play 模式时调用）
         */
        static void setTintActive(bool active);

        /**
         * @brief 当前是否处于全局染色激活态
         */
        static bool isTintActive() { return msTintActive; }

    private:
        static void ensureInitialized();
        static void setupGeometryMetrics(ImGuiStyle &style);
        static void setupUnityDarkColors(ImVec4 *colors, ImSemanticPalette &palette);
        static void setupImGuiDarkColors(ImVec4 *colors, ImSemanticPalette &palette);
        static void refreshActiveColors();
        static ImVec4 multiplyColor(const ImVec4 &color, const ImVec4 &tint);

    private:
        static bool               msInitialized;
        static ImThemeId          msCurrentTheme;
        static bool               msTintActive;
        static ImVec4             msBaseColors[ImGuiCol_COUNT];
        static ImSemanticPalette  msBasePalette;
        static ImSemanticPalette  msActivePalette;
    };
}
