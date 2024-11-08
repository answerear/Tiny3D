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
    
    TResult ImSearchInputText::create(uint32_t id, const ImVec2 &size, ImWidget *parent)
    {
        TResult ret = createInternal(id, "##InputWithIcon", parent, 0);
        if (T3D_SUCCEEDED(ret))
        {
            setSize(size);
        }
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
    }

    //--------------------------------------------------------------------------

    void ImSearchInputText::onGUI()
    {
        static char inputText[128] = "";
            
        // 设置输入框的大小
        ImVec2 inputSize = getSize(); // 输入框的宽度和高度
        ImVec2 iconSize = ImVec2(16, 16); // 图标大小
        float iconPadding = 4.0f; // 图标与输入框之间的间隔
        float borderThickness = 2.0f; // 边框厚度
        float innerPadding = 4.0f; // 输入框内边距

        // 获取当前光标位置
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // 外框颜色
        ImU32 borderColor = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Border));

        // 背景颜色
        ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
        color.w = 1.0f;
        ImU32 bgColor = ImGui::GetColorU32(color);

        // 绘制输入框背景
        drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + inputSize.x, cursorPos.y + inputSize.y), bgColor);

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

        // 设置输入框的高度，留出内边距
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(innerPadding, (inputSize.y - ImGui::GetFontSize()) * 0.5f));
        // 真实输入框背景颜色
        ImGui::PushStyleColor(ImGuiCol_FrameBg, bgColor);
        // 真实输入框
        ImGui::InputText(getName().c_str(), inputText, sizeof(inputText), ImGuiInputTextFlags_EnterReturnsTrue);
        // 恢复颜色
        ImGui::PopStyleColor();
        // 恢复内边距
        ImGui::PopStyleVar();
        // 恢复字体
        ImGui::PopFont();

        // 判断是否获得焦点、激活状态、mouse over 状态
        bool isFocused = ImGui::IsItemActivated() || ImGui::IsItemHovered() || ImGui::IsItemFocused();
        if (isFocused)
        {
            borderColor = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));
        }

        // 绘制输入框的外框
        drawList->AddRect(cursorPos, ImVec2(cursorPos.x + inputSize.x, cursorPos.y + inputSize.y), borderColor, 5.0f, ImDrawFlags_None, borderThickness);
    }

    //--------------------------------------------------------------------------

    NS_END
}
