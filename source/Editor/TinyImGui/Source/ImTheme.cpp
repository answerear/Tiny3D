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


#include "ImTheme.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    bool              ImTheme::msInitialized = false;
    ImThemeId         ImTheme::msCurrentTheme = ImThemeId::kUnityDark;
    bool              ImTheme::msTintActive = false;
    ImVec4            ImTheme::msBaseColors[ImGuiCol_COUNT] = {};
    ImSemanticPalette ImTheme::msBasePalette = {};
    ImSemanticPalette ImTheme::msActivePalette = {};

    //--------------------------------------------------------------------------

    void ImTheme::ensureInitialized()
    {
        if (!msInitialized)
        {
            setupUnityDarkColors(msBaseColors, msBasePalette);
            msActivePalette = msBasePalette;
            msInitialized = true;
        }
    }

    //--------------------------------------------------------------------------

    const ImSemanticPalette &ImTheme::getPalette()
    {
        ensureInitialized();
        return msActivePalette;
    }

    //--------------------------------------------------------------------------

    void ImTheme::applyTheme(ImThemeId theme)
    {
        // 保留外部此前设置的 globalTint（若已初始化过）
        const ImVec4 savedTint = msInitialized
            ? msBasePalette.globalTint : getDefaultGlobalTint();

        msCurrentTheme = theme;
        msInitialized = true;

        if (ImGui::GetCurrentContext() != nullptr)
        {
            ImGuiStyle &style = ImGui::GetStyle();
            setupGeometryMetrics(style);
        }

        switch (theme)
        {
        case ImThemeId::kImGuiDark:
            setupImGuiDarkColors(msBaseColors, msBasePalette);
            break;

        case ImThemeId::kUnityLight:
        case ImThemeId::kUnityDark:
        default:
            setupUnityDarkColors(msBaseColors, msBasePalette);
            break;
        }

        msBasePalette.globalTint = savedTint;
        refreshActiveColors();
    }

    //--------------------------------------------------------------------------

    void ImTheme::setGlobalTint(const ImVec4 &tint)
    {
        ensureInitialized();
        msBasePalette.globalTint = tint;
        msActivePalette.globalTint = tint;

        if (msTintActive)
        {
            refreshActiveColors();
        }
    }

    //--------------------------------------------------------------------------

    void ImTheme::setTintActive(bool active)
    {
        ensureInitialized();
        if (msTintActive == active)
        {
            return;
        }

        msTintActive = active;
        refreshActiveColors();
    }

    //--------------------------------------------------------------------------

    void ImTheme::setupGeometryMetrics(ImGuiStyle &style)
    {
        style.WindowPadding             = ImVec2(6.0f, 6.0f);
        style.FramePadding              = ImVec2(5.0f, 3.0f);
        style.ItemSpacing               = ImVec2(6.0f, 4.0f);
        style.ItemInnerSpacing          = ImVec2(4.0f, 4.0f);
        style.CellPadding               = ImVec2(4.0f, 2.0f);
        style.IndentSpacing             = 14.0f;
        style.ScrollbarSize             = 12.0f;
        style.GrabMinSize               = 10.0f;

        style.WindowBorderSize          = 1.0f;
        style.ChildBorderSize           = 1.0f;
        style.PopupBorderSize           = 1.0f;
        style.FrameBorderSize           = 1.0f;
        style.TabBorderSize             = 0.0f;
        style.TabBarBorderSize          = 1.0f;
        style.DockingSeparatorSize      = 2.0f;

        style.WindowRounding            = 0.0f;
        style.ChildRounding             = 0.0f;
        style.PopupRounding             = 2.0f;
        style.FrameRounding             = 2.0f;
        style.ScrollbarRounding         = 6.0f;
        style.GrabRounding              = 2.0f;
        style.TabRounding               = 2.0f;

        style.WindowTitleAlign          = ImVec2(0.0f, 0.5f);
        style.WindowMenuButtonPosition  = ImGuiDir_None;
        style.ColorButtonPosition       = ImGuiDir_Right;
        style.ButtonTextAlign           = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign       = ImVec2(0.0f, 0.0f);
    }

    //--------------------------------------------------------------------------

    void ImTheme::setupUnityDarkColors(ImVec4 *colors, ImSemanticPalette &palette)
    {
        // 1. 文本与基础背景
        colors[ImGuiCol_Text]                   = ImVec4(0.824f, 0.824f, 0.824f, 1.00f); // #D2D2D2
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.498f, 0.498f, 0.498f, 1.00f); // #7F7F7F
        colors[ImGuiCol_WindowBg]               = ImVec4(0.220f, 0.220f, 0.220f, 1.00f); // #383838
        colors[ImGuiCol_ChildBg]                = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.188f, 0.188f, 0.188f, 0.98f); // #303030
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.125f, 0.125f, 0.125f, 1.00f); // #202020

        // 2. 边框与分割线
        colors[ImGuiCol_Border]                 = ImVec4(0.137f, 0.137f, 0.137f, 1.00f); // #232323
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);
        colors[ImGuiCol_Separator]              = ImVec4(0.149f, 0.149f, 0.149f, 1.00f); // #262626
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.227f, 0.447f, 0.690f, 0.78f); // #3A72B0
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.227f, 0.447f, 0.690f, 1.00f); // #3A72B0

        // 3. 标题栏、菜单栏与标签页
        colors[ImGuiCol_TitleBg]                = ImVec4(0.157f, 0.157f, 0.157f, 1.00f); // #282828
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.157f, 0.157f, 0.157f, 1.00f); // #282828
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.157f, 0.157f, 0.157f, 0.75f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.157f, 0.157f, 0.157f, 1.00f); // #282828
        colors[ImGuiCol_Tab]                    = ImVec4(0.180f, 0.180f, 0.180f, 1.00f); // #2E2E2E
        colors[ImGuiCol_TabHovered]             = ImVec4(0.243f, 0.243f, 0.243f, 1.00f); // #3E3E3E
        colors[ImGuiCol_TabActive]              = ImVec4(0.220f, 0.220f, 0.220f, 1.00f); // #383838
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.157f, 0.157f, 0.157f, 1.00f); // #282828
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.220f, 0.220f, 0.220f, 1.00f); // #383838

        // 4. 表单输入框、按钮、滑块与复选框
        colors[ImGuiCol_FrameBg]                = ImVec4(0.165f, 0.165f, 0.165f, 1.00f); // #2A2A2A
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.188f, 0.188f, 0.188f, 1.00f); // #303030
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.133f, 0.133f, 0.133f, 1.00f); // #222222
        colors[ImGuiCol_Button]                 = ImVec4(0.345f, 0.345f, 0.345f, 1.00f); // #585858
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.404f, 0.404f, 0.404f, 1.00f); // #676767
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.275f, 0.376f, 0.486f, 1.00f); // #46607C
        colors[ImGuiCol_CheckMark]              = ImVec4(0.800f, 0.800f, 0.800f, 1.00f); // #CCCCCC
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.471f, 0.471f, 0.471f, 1.00f); // #787878
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.227f, 0.475f, 0.733f, 1.00f); // #3A79BB

        // 5. 列表选中、折叠头、表格与滚动条
        colors[ImGuiCol_Header]                 = ImVec4(0.173f, 0.365f, 0.529f, 1.00f); // #2C5D87
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.267f, 0.267f, 0.267f, 1.00f); // #444444
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.227f, 0.447f, 0.690f, 1.00f); // #3A72B0
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.157f, 0.157f, 0.157f, 0.60f); // #282828
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.333f, 0.333f, 0.333f, 1.00f); // #555555
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.408f, 0.408f, 0.408f, 1.00f); // #686868
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.490f, 0.490f, 0.490f, 1.00f); // #7D7D7D
        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.180f, 0.180f, 0.180f, 1.00f); // #2E2E2E
        colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.137f, 0.137f, 0.137f, 1.00f); // #232323
        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.173f, 0.173f, 0.173f, 1.00f); // #2C2C2C
        colors[ImGuiCol_TableRowBg]             = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.000f, 1.000f, 1.000f, 0.03f);

        // 6. 辅助交互色（图表、拖拽、选区、模态遮罩）
        colors[ImGuiCol_PlotLines]              = ImVec4(0.610f, 0.610f, 0.610f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.000f, 0.430f, 0.350f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.900f, 0.700f, 0.000f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.000f, 0.600f, 0.000f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.173f, 0.365f, 0.529f, 0.65f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.227f, 0.475f, 0.733f, 0.90f); // #3A79BB
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.173f, 0.365f, 0.529f, 0.70f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.227f, 0.447f, 0.690f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.000f, 1.000f, 1.000f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.800f, 0.800f, 0.800f, 0.20f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(1.000f, 1.000f, 1.000f, 0.10f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.227f, 0.447f, 0.690f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.227f, 0.447f, 0.690f, 0.95f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.000f, 0.000f, 0.000f, 0.45f);

        // 7. 扩展语义色板
        palette.toolbarBg               = ImVec4(0.235f, 0.235f, 0.235f, 1.00f); // #3C3C3C
        palette.statusBarBg             = ImVec4(0.188f, 0.188f, 0.188f, 1.00f); // #303030
        palette.sectionHeaderBg         = ImVec4(0.243f, 0.243f, 0.243f, 1.00f); // #3E3E3E
        palette.sectionHeaderHovered    = ImVec4(0.278f, 0.278f, 0.278f, 1.00f); // #474747
        palette.sectionHeaderActive     = ImVec4(0.306f, 0.306f, 0.306f, 1.00f); // #4E4E4E
        palette.selectionFocused        = ImVec4(0.173f, 0.365f, 0.529f, 1.00f); // #2C5D87
        palette.selectionUnfocused      = ImVec4(0.302f, 0.302f, 0.302f, 1.00f); // #4D4D4D
        palette.searchBoxBg             = ImVec4(0.145f, 0.145f, 0.145f, 1.00f); // #252525
        palette.searchBoxBorder         = ImVec4(0.110f, 0.110f, 0.110f, 1.00f); // #1C1C1C
        palette.searchBoxBorderFocused  = ImVec4(0.227f, 0.447f, 0.690f, 1.00f); // #3A72B0
        palette.prefabText              = ImVec4(0.490f, 0.655f, 1.000f, 1.00f); // #7DA7FF
        palette.axisColors[0]           = ImVec4(0.910f, 0.310f, 0.310f, 1.00f); // X: #E84F4F
        palette.axisColors[1]           = ImVec4(0.471f, 0.800f, 0.271f, 1.00f); // Y: #78CC45
        palette.axisColors[2]           = ImVec4(0.349f, 0.549f, 0.949f, 1.00f); // Z: #598CF2
        palette.axisColors[3]           = ImVec4(0.749f, 0.749f, 0.749f, 1.00f); // W: #BFBFBF
        palette.logInfo                 = ImVec4(0.824f, 0.824f, 0.824f, 1.00f); // #D2D2D2
        palette.logWarning              = ImVec4(0.961f, 0.718f, 0.153f, 1.00f); // #F5B727
        palette.logError                = ImVec4(0.910f, 0.329f, 0.329f, 1.00f); // #E85454
        palette.globalTint              = getDefaultGlobalTint();
    }

    //--------------------------------------------------------------------------

    void ImTheme::setupImGuiDarkColors(ImVec4 *colors, ImSemanticPalette &palette)
    {
        ImGuiStyle tempStyle;
        ImGui::StyleColorsDark(&tempStyle);
        tempStyle.Colors[ImGuiCol_WindowBg].w = 1.0f;

        for (int i = 0; i < ImGuiCol_COUNT; ++i)
        {
            colors[i] = tempStyle.Colors[i];
        }

        palette.toolbarBg               = tempStyle.Colors[ImGuiCol_MenuBarBg];
        palette.statusBarBg             = tempStyle.Colors[ImGuiCol_MenuBarBg];
        palette.sectionHeaderBg         = tempStyle.Colors[ImGuiCol_Header];
        palette.sectionHeaderHovered    = tempStyle.Colors[ImGuiCol_HeaderHovered];
        palette.sectionHeaderActive     = tempStyle.Colors[ImGuiCol_HeaderActive];
        palette.selectionFocused        = tempStyle.Colors[ImGuiCol_Header];
        palette.selectionUnfocused      = ImVec4(0.240f, 0.240f, 0.240f, 0.80f);
        palette.searchBoxBg             = ImVec4(
            tempStyle.Colors[ImGuiCol_FrameBg].x,
            tempStyle.Colors[ImGuiCol_FrameBg].y,
            tempStyle.Colors[ImGuiCol_FrameBg].z,
            1.0f);
        palette.searchBoxBorder         = tempStyle.Colors[ImGuiCol_Border];
        palette.searchBoxBorderFocused  = tempStyle.Colors[ImGuiCol_FrameBgActive];
        palette.prefabText              = ImVec4(0.350f, 0.550f, 1.000f, 1.00f);
        palette.axisColors[0]           = ImVec4(0.910f, 0.310f, 0.310f, 1.00f);
        palette.axisColors[1]           = ImVec4(0.471f, 0.800f, 0.271f, 1.00f);
        palette.axisColors[2]           = ImVec4(0.349f, 0.549f, 0.949f, 1.00f);
        palette.axisColors[3]           = ImVec4(0.749f, 0.749f, 0.749f, 1.00f);
        palette.logInfo                 = tempStyle.Colors[ImGuiCol_Text];
        palette.logWarning              = ImVec4(1.000f, 0.788f, 0.055f, 1.00f);
        palette.logError                = ImVec4(0.910f, 0.329f, 0.329f, 1.00f);
        palette.globalTint              = getDefaultGlobalTint();
    }

    //--------------------------------------------------------------------------

    ImVec4 ImTheme::multiplyColor(const ImVec4 &color, const ImVec4 &tint)
    {
        return ImVec4(
            ImClamp(color.x * tint.x, 0.0f, 1.0f),
            ImClamp(color.y * tint.y, 0.0f, 1.0f),
            ImClamp(color.z * tint.z, 0.0f, 1.0f),
            color.w);
    }

    //--------------------------------------------------------------------------

    void ImTheme::refreshActiveColors()
    {
        const ImVec4 tint = msTintActive
            ? msBasePalette.globalTint : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        msActivePalette = msBasePalette;
        if (msTintActive)
        {
            msActivePalette.toolbarBg               = multiplyColor(msBasePalette.toolbarBg, tint);
            msActivePalette.statusBarBg             = multiplyColor(msBasePalette.statusBarBg, tint);
            msActivePalette.sectionHeaderBg         = multiplyColor(msBasePalette.sectionHeaderBg, tint);
            msActivePalette.sectionHeaderHovered    = multiplyColor(msBasePalette.sectionHeaderHovered, tint);
            msActivePalette.sectionHeaderActive     = multiplyColor(msBasePalette.sectionHeaderActive, tint);
            msActivePalette.selectionFocused        = multiplyColor(msBasePalette.selectionFocused, tint);
            msActivePalette.selectionUnfocused      = multiplyColor(msBasePalette.selectionUnfocused, tint);
            msActivePalette.searchBoxBg             = multiplyColor(msBasePalette.searchBoxBg, tint);
            msActivePalette.searchBoxBorder         = multiplyColor(msBasePalette.searchBoxBorder, tint);
            msActivePalette.searchBoxBorderFocused  = multiplyColor(msBasePalette.searchBoxBorderFocused, tint);
        }

        if (ImGui::GetCurrentContext() == nullptr)
        {
            return;
        }

        ImGuiStyle &style = ImGui::GetStyle();
        for (int i = 0; i < ImGuiCol_COUNT; ++i)
        {
            style.Colors[i] = msTintActive
                ? multiplyColor(msBaseColors[i], tint) : msBaseColors[i];
        }

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
    }

    //--------------------------------------------------------------------------
}
