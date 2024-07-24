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


#include "ImToolBar.h"

#include "ImErrors.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImToolBar::~ImToolBar()
    {
        
    }

    //--------------------------------------------------------------------------

    String ImToolBar::makeName(uint32_t id)
    {
        String name;
        std::stringstream ss;
        ss << getName() << "##" << id;
        return ss.str();
    }

    
    //--------------------------------------------------------------------------

    TResult ImToolBar::addButton(uint32_t id, ImTextureID texID, const String &shortcut, const String &tips, const ButtonQueryCallback &query, const ButtonClickedCallback &clicked)
    {
        mButtons.emplace_back(makeName(id), id, 0, texID, shortcut, tips, query, nullptr, clicked);
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::addButton(uint32_t id, ImTextureID texID, const String &shortcut, const String &tips, const ButtonQueryCallback &query, uint32_t clickedEvtID)
    {
        mButtons.emplace_back(makeName(id), id, clickedEvtID, texID, shortcut, tips, query, nullptr, nullptr);
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::addButton(uint32_t id, ImTextureID texID, const String &shortcut, const String &tips, const ButtonQueryCallback &query, const ButtonQueryCallback &check, const ButtonClickedCallback &clicked)
    {
        mButtons.emplace_back(makeName(id), id, 0, texID, shortcut, tips, query, check, clicked);
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::addButton(uint32_t id, ImTextureID texID, const String &shortcut, const String &tips, const ButtonQueryCallback &query, const ButtonQueryCallback &check, uint32_t clickedEvtID)
    {
        mButtons.emplace_back(makeName(id), id, clickedEvtID, texID, shortcut, tips, query, check, nullptr);
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::removeButton(uint32_t id)
    {
        TResult ret = IM_OK;

        do
        {
            auto itr = std::find_if(
            mButtons.begin(),
            mButtons.end(),
            [id](const ToolButton &button)
            {
                return button.id == id;
            });

            if (itr == mButtons.end())
            {
                T3D_LOG_WARNING(LOG_TAG_TINYIMGUI, "Could not found tool button [%u] !", id);
                ret = T3D_ERR_NOT_FOUND;
                break;
            }

            mButtons.erase(itr);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::onCreate()
    {
        mSize.y = mButtonSize.y + mHorzSpacing * 2.0f;
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    bool ImToolBar::onGUIBegin()
    {
        return ImChildView::onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void ImToolBar::onGUI()
    {
        auto region = ImGui::GetContentRegionAvail();

        if (mAlignment == Alignment::kMiddle)
        {
            uint32_t count = static_cast<uint32_t>(mButtons.size());
            float width = count * (mButtonSize.x + ImGui::GetStyle().ItemSpacing.x * 2.0f);
            float x = (region.x - width) * 0.5f;
            ImGui::SetCursorPosX(x);
        }
        else if (mAlignment == Alignment::kRight)
        {
            uint32_t count = static_cast<uint32_t>(mButtons.size());
            float width = count * (mButtonSize.x + ImGui::GetStyle().ItemSpacing.x * 2.0f);
            float x = region.x - width;
            ImGui::SetCursorPosX(x);
        }
        
        for (auto button : mButtons)
        {
            bool enable = button.query ? button.query(button.id) : false;
            bool check = button.check ? button.check(button.id) : false;
            ImGuiStyle &style = ImGui::GetStyle();
            
            if (enable && check)
            {
                ImVec4 color = style.Colors[ImGuiCol_ButtonActive];
                ImGui::PushStyleColor(ImGuiCol_Button, color);
            }
            
            ImGui::BeginDisabled(!enable);
            if (ImGui::ImageButton(button.name.c_str(), button.texID, mButtonSize))
            {
                if (button.clicked != nullptr)
                {
                    button.clicked(button.id);
                }
                else
                {
                    EventParamToolButton param(button.id);
                    sendEvent(button.clickedEvtID, &param);
                }
            }
            ImGui::EndDisabled();

            if (enable && check)
            {
                ImGui::PopStyleColor();
            }
            
            ImGui::SameLine();
        }
    }

    //--------------------------------------------------------------------------

    void ImToolBar::onGUIEnd()
    {
        // ImGui::End();
        ImChildView::onGUIEnd();
        ImGui::Separator();
    }

    //--------------------------------------------------------------------------

    ImGuiChildFlags ImToolBar::onGetChildFlags()
    {
        return ImGuiChildFlags_None;
    }
    
    //--------------------------------------------------------------------------

    ImGuiWindowFlags ImToolBar::onGetWindowFlags()
    {
        return ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;
    }

    //--------------------------------------------------------------------------
}
