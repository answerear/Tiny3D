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


#include "ImCreateButton.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    TResult ImCreateButton::create(uint32_t id, const ImVec2 &size, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        TResult ret = ImButton::create(id, "##AddButton", queryEnabled, clicked, parent, tips, shortcut);
        if (T3D_SUCCEEDED(ret))
        {
            setSize(size);
        }
        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult ImCreateButton::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            mIconAdd = IM_TEXTURE_MGR.loadTexture(ICON_NAME_ADD);
            if (mIconAdd == nullptr)
            {
                GUI_LOG_ERROR("Failed to load icon add texture");
                ret = T3D_ERR_FAIL;
                break;
            }
            
            mIconDropdown = IM_TEXTURE_MGR.loadTexture(ICON_NAME_DROPDOWN);
            if (mIconDropdown == nullptr)
            {
                GUI_LOG_ERROR("Failed to load icon dropdown texture");
                ret = T3D_ERR_FAIL;
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ImCreateButton::onDestroy()
    {
        if (mIconAdd != nullptr)
        {
            IM_TEXTURE_MGR.unloadTexture(mIconAdd);
            mIconAdd = nullptr;
        }

        if (mIconDropdown != nullptr)
        {
            IM_TEXTURE_MGR.unloadTexture(mIconDropdown);
            mIconDropdown = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    void ImCreateButton::onGUI()
    {
        ImVec2 buttonSize = getSize();
        ImVec2 buttonPos = ImGui::GetCursorScreenPos();

        bool enabled = mQueryEnabled != nullptr ? mQueryEnabled(this) : true;
        
        ImGui::BeginDisabled(!enabled);
        if (ImGui::Button("##AddButton", buttonSize))
        {
            if (mClicked != nullptr)
            {
                mClicked(this);
            }
        }

        ImVec2 margin(0.0f, 0.0f);
        float spacing = 2.0f;
        
        ImVec2 leftSize = IM_TEXTURE_MGR.getTextureSize(mIconAdd);
        ImVec2 rightSize = IM_TEXTURE_MGR.getTextureSize(mIconDropdown);
        
        if (buttonSize.x - margin.x - spacing > buttonSize.y)
        {
            // 以高为准，等比缩放，计算宽度
            float targetHeight = buttonSize.y;
            float scale = targetHeight / leftSize.y;
            leftSize.y = targetHeight;
            leftSize.x =  leftSize.x * scale;

            targetHeight *= 0.3f;
            scale = targetHeight / rightSize.y;
            rightSize.y = targetHeight;
            rightSize.x = rightSize.x * scale;
        }
        else
        {
            // 以宽为准，计算高度
            float targetWidth = buttonSize.x - margin.x - spacing;
            float scale = targetWidth / leftSize.x;
            leftSize.x = targetWidth;
            leftSize.y = leftSize.y * scale;

            targetWidth *= 0.3f;
            scale = targetWidth / rightSize.x;
            rightSize.x = targetWidth;
            rightSize.y = rightSize.y * scale;
        }
        
        ImDrawList *drawList = ImGui::GetWindowDrawList();

        // 绘制左侧图片
        ImVec2 leftPos = buttonPos;
        leftPos.x = buttonPos.x + (buttonSize.x - leftSize.x - rightSize.x - margin.x - spacing) * 0.5f;
        leftPos.y = buttonPos.y + margin.y;
        drawList->AddImage(mIconAdd, leftPos, ImVec2(leftPos.x + leftSize.x, leftPos.y + leftSize.y));

        // 绘制右侧图片
        ImVec2 rightPos = buttonPos;
        rightPos.x = leftPos.x + leftSize.x + spacing;
        rightPos.y = buttonPos.y + (buttonSize.y - rightSize.y - margin.y) * 0.5f;
        drawList->AddImage(mIconDropdown, rightPos, ImVec2(rightPos.x + rightSize.x, rightPos.y + rightSize.y));

        ImGui::EndDisabled();
    }

    //--------------------------------------------------------------------------

    NS_END
}
