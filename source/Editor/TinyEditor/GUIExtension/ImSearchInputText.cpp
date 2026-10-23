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


#include "ImSearchInputText.h"
#include "EditorApp.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------
    
    TResult ImSearchInputText::create(uint32_t id, const ImVec2 &size, int32_t maxNumberOfChars, const ImInputTextCallback &callback, bool callbackEdit, ImWidget *parent)
    {
        return ImWidget::createInternal(id, "##SearchInputText", parent, 4, maxNumberOfChars, &callback, callbackEdit, &size);
    }

    //--------------------------------------------------------------------------

    TResult ImSearchInputText::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = ImInputText::createInternal(id, name, parent, argc, args);

        do
        {
            T3D_ASSERT(argc >= 4);

            mCallbackEdit = static_cast<bool>(va_arg(args, int));
            ImVec2 *size = va_arg(args, ImVec2 *);
            setSize(*size);
            mOriginalSize = *size;
            
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImSearchInputText::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            mIconSearch = IM_TEXTURE_MGR.loadTexture(ICON_NAME_SEARCH);
            if (mIconSearch == nullptr)
            {
                GUI_LOG_ERROR("Failed to load icon search texture");
                ret = T3D_ERR_FAIL;
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void ImSearchInputText::onDestroy()
    {
        if (mIconSearch != nullptr)
        {
            IM_TEXTURE_MGR.unloadTexture(mIconSearch);
            mIconSearch = nullptr;
        }

        ImInputText::onDestroy();
    }

    //--------------------------------------------------------------------------

    void ImSearchInputText::onGUI()
    {
        // 设置输入框的大小
        ImVec2 inputSize = getSize(); // 输入框的宽度和高度
        ImVec2 iconSize = ImVec2(16, 16); // 图标大小
        float iconPadding = 4.0f; // 图标与输入框之间的间隔
        float borderThickness = 1.0f; // 1px 细边框（对齐 Unity Dark 搜索框）
        float cornerRounding = 3.0f; // 微圆角
        float innerPadding = 4.0f; // 输入框内边距

        // 获取当前光标位置
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const ImSemanticPalette &palette = ImTheme::getPalette();

        // 外框颜色与背景颜色
        ImU32 borderColor = ImGui::GetColorU32(palette.searchBoxBorder);
        ImU32 bgColor = ImGui::GetColorU32(palette.searchBoxBg);

        // 绘制输入框背景
        drawList->AddRectFilled(
            cursorPos,
            ImVec2(cursorPos.x + inputSize.x, cursorPos.y + inputSize.y),
            bgColor,
            cornerRounding);

        // 绘制图标
        drawList->AddImage(mIconSearch, ImVec2(cursorPos.x + iconPadding, cursorPos.y + (inputSize.y - iconSize.y) * 0.5f), 
                           ImVec2(cursorPos.x + iconPadding + iconSize.x, cursorPos.y + (inputSize.y + iconSize.y) * 0.5f));

        // 调整输入框字体大小
        EditorApp *app = static_cast<EditorApp*>(Application::getInstancePtr());
        ImFont *font = app->getFont(16);
        ImGui::PushFont(font);

        // 设置输入框的位置
        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + iconSize.x + iconPadding * 2, cursorPos.y)); // 输入框位置

        // 设置输入框的宽度，留出边框和内边距
        ImGui::SetNextItemWidth(inputSize.x - (iconSize.x + iconPadding * 2 + innerPadding * 2)); // 设置输入框的宽度

        // 设置输入框的高度，留出内边距；内部 InputText 关闭自身边框与底色，由外层统一绘制
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(innerPadding, (inputSize.y - ImGui::GetFontSize()) * 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        // 真实输入框
        ImGuiInputFlags flags = mCallbackEdit ? ImGuiInputTextFlags_CallbackEdit : ImGuiInputTextFlags_EnterReturnsTrue;
        ImGui::InputText(getName().c_str(), mText, mMaxNumberOfChars+1, flags, inputTextCallback, this);
        // 恢复颜色与样式变量
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
        // 恢复字体
        ImGui::PopFont();

        // 仅在获得键盘焦点或激活输入时切换亮蓝焦点边框（与 Unity 搜索框一致）
        bool isFocused = ImGui::IsItemActivated() || ImGui::IsItemActive() || ImGui::IsItemFocused();
        if (isFocused)
        {
            borderColor = ImGui::GetColorU32(palette.searchBoxBorderFocused);
        }

        // 绘制输入框的 1px 外框
        drawList->AddRect(
            cursorPos,
            ImVec2(cursorPos.x + inputSize.x, cursorPos.y + inputSize.y),
            borderColor,
            cornerRounding,
            ImDrawFlags_None,
            borderThickness);
    }

    //--------------------------------------------------------------------------

    NS_END
}
