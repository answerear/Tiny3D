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


#include "ImButton.h"

#include "ImErrors.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImButton::~ImButton()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImButton::create(uint32_t id, const String &title, uint32_t clickedEvtID, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 5, nullptr, clickedEvtID, nullptr, &tips, &shortcut);
    }

    //--------------------------------------------------------------------------

    TResult ImButton::create(uint32_t id, const String &title, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 5, nullptr, 0, &clicked, &tips, &shortcut);
    }

    //--------------------------------------------------------------------------

    TResult ImButton::create(uint32_t id, const String &title, const ImButtonQueryCallback &queryEnabled, uint32_t clickedEvtID, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 5, &queryEnabled, clickedEvtID, nullptr, &tips, &shortcut);
    }

    //--------------------------------------------------------------------------

    TResult ImButton::create(uint32_t id, const String &title, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 5, &queryEnabled, 0, &clicked, &tips, &shortcut);
    }

    //--------------------------------------------------------------------------

    TResult ImButton::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = IM_OK;

        do
        {
            T3D_ASSERT(argc == 5, "Invalid number of arguments in ImButton::create() !");

            ImButtonQueryCallback *queryEnabled = va_arg(args, ImButtonQueryCallback*);
            uint32_t clickedEvtID = va_arg(args, uint32_t);
            ImButtonClickedCallback *clicked = va_arg(args, ImButtonClickedCallback*);
            String *tips = va_arg(args, String*);
            String *shortcut = va_arg(args, String*);

            mQueryEnabled = *queryEnabled;
            mClickedEvtID = clickedEvtID;
            mClicked = *clicked;
            mTips = *tips;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void ImButton::onGUI()
    {
        bool enable = mQueryEnabled ? mQueryEnabled(this) : isEnable();
        ImGui::BeginDisabled(!enable);
        if (ImGui::Button(getName().c_str(), getSize()))
        {
            if (mClickedEvtID != 0)
            {
                EventParamButtonClicked param(this);
                sendEvent(mClickedEvtID, &param);
            }
            else if (mClicked != nullptr)
            {
                mClicked(this);
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(mTips.c_str());
            }
        }
        ImGui::EndDisabled();
    }

    //--------------------------------------------------------------------------

    ImPushButton::~ImPushButton()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImPushButton::create(uint32_t id, const String &title, ImButtonQueryCallback &queryChecked, uint32_t clickedEvtID, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 6, nullptr, clickedEvtID, nullptr, &tips, &shortcut, &queryChecked);
    }

    //--------------------------------------------------------------------------

    TResult ImPushButton::create(uint32_t id, const String &title, ImButtonQueryCallback &queryChecked, ImButtonClickedCallback clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 6, nullptr, 0, &clicked, &tips, &shortcut, &queryChecked);
    }

    //--------------------------------------------------------------------------

    TResult ImPushButton::create(uint32_t id, const String &title, ImButtonQueryCallback &queryEnabled, ImButtonQueryCallback &queryChecked, uint32_t clickedEvtID, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 6, &queryEnabled, clickedEvtID, nullptr, &tips, &shortcut, &queryChecked);
    }

    //--------------------------------------------------------------------------

    TResult ImPushButton::create(uint32_t id, const String &title, ImButtonQueryCallback &queryEnabled, ImButtonQueryCallback &queryChecked, ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 6, &queryEnabled, 0, clicked, &tips, &shortcut, &queryChecked);
    }

    //--------------------------------------------------------------------------

    TResult ImPushButton::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = IM_OK;

        do
        {
            T3D_ASSERT(argc == 6, "Invalid number of arguments in ImPsuhButton::create() !");

            ret = ImButton::createInternal(id, name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                IMGUI_LOG_ERROR("Create ImPushButton %s failed ! ERROR [%d]", name.c_str(), ret)
                break;
            }

            ImButtonQueryCallback *checked = va_arg(args, ImButtonQueryCallback*);
            mQueryChecked = *checked;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void ImPushButton::onGUI()
    {
        bool enable = mQueryEnabled ? mQueryEnabled(this) : isEnable();
        bool check = mQueryChecked? mQueryChecked(this) : false;
        ImGuiStyle &style = ImGui::GetStyle();
            
        if (enable && check)
        {
            ImVec4 color = style.Colors[ImGuiCol_ButtonActive];
            ImGui::PushStyleColor(ImGuiCol_Button, color);
        }
            
        ImGui::BeginDisabled(!enable);
        if (ImGui::Button(getName().c_str(), getSize()))
        {
            if (mClicked != nullptr)
            {
                mClicked(this);
            }
            else
            {
                EventParamButtonClicked param(this);
                sendEvent(mClickedEvtID, &param);
            }
        }
        ImGui::EndDisabled();

        if (enable && check)
        {
            ImGui::PopStyleColor();
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(mTips.c_str());
        }
    }

    //--------------------------------------------------------------------------

    ImImageButton::~ImImageButton()
    {
        
    }

    //--------------------------------------------------------------------------

    String ImImageButton::makeName(uint32_t id)
    {
        std::stringstream ss;
        ss << "ImageButton##" << id;
        return ss.str();
    }

    //--------------------------------------------------------------------------

    TResult ImImageButton::create(uint32_t id, ImTextureID texID, uint32_t clickedEvtID, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, nullptr, clickedEvtID, nullptr, &tips, &shortcut, texID);
    }

    //--------------------------------------------------------------------------
    
    TResult ImImageButton::create(uint32_t id, ImTextureID texID, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, nullptr, 0, &clicked, &tips, &shortcut, texID);
    }

    //--------------------------------------------------------------------------
    
    TResult ImImageButton::create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, uint32_t clickedEvtID, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, &queryEnabled, clickedEvtID, nullptr, &tips, &shortcut, texID);
    }

    //--------------------------------------------------------------------------
    
    TResult ImImageButton::create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, &queryEnabled, 0, &clicked, &tips, &shortcut, texID);
    }
    
    //--------------------------------------------------------------------------

    TResult ImImageButton::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = IM_OK;

        do
        {
            T3D_ASSERT(argc == 6, "Invalid number of arguments in ImImageButton::create() !");
            
            ret = ImButton::createInternal(id, name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                IMGUI_LOG_ERROR("Create ImImageButton %u failed ! ERROR [%d]", id, ret)
                break;
            }
            
            mTexID = va_arg(args, ImTextureID);
            if (mTexID == nullptr)
            {
                IMGUI_LOG_ERROR("ImTextureID is nullptr when create ImImageButton %u !", id);
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ImImageButton::onGUI()
    {
        bool enable = mQueryEnabled ? mQueryEnabled(this) : isEnable();
        ImGui::BeginDisabled(!enable);
        if (ImGui::ImageButton(getName().c_str(), mTexID, getSize()))
        {
            if (mClicked != nullptr)
            {
                mClicked(this);
            }
            else
            {
                EventParamButtonClicked param(this);
                sendEvent(mClickedEvtID, &param);
            }
        }
        ImGui::EndDisabled();
    }

    //--------------------------------------------------------------------------

    ImPushImageButton::~ImPushImageButton()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImPushImageButton::create(uint32_t id, ImTextureID texID, ImButtonQueryCallback &queryChecked, uint32_t clickedEvtID, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, nullptr, clickedEvtID, nullptr, &tips, &shortcut, texID, &queryChecked);
    }

    //--------------------------------------------------------------------------
    
    TResult ImPushImageButton::create(uint32_t id, ImTextureID texID, ImButtonQueryCallback &queryChecked, ImButtonClickedCallback clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, nullptr, 0, &clicked, &tips, &shortcut, texID, &queryChecked);
    }

    //--------------------------------------------------------------------------
    
    TResult ImPushImageButton::create(uint32_t id, ImTextureID texID, ImButtonQueryCallback &queryEnabled, ImButtonQueryCallback &queryChecked, uint32_t clickedEvtID, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, &queryEnabled, clickedEvtID, nullptr, &tips, &shortcut, texID, &queryChecked);
    }

    //--------------------------------------------------------------------------
    
    TResult ImPushImageButton::create(uint32_t id, ImTextureID texID, ImButtonQueryCallback &queryEnabled, ImButtonQueryCallback &queryChecked, ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, &queryEnabled, 0, &clicked, &tips, &shortcut, texID, &queryChecked);
    }
    
    //--------------------------------------------------------------------------

    TResult ImPushImageButton::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = IM_OK;

        do
        {
            T3D_ASSERT(argc == 7, "Invalid number of arguments in ImPushImageButton::create() !");
            
            ret = ImImageButton::createInternal(id, name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                IMGUI_LOG_ERROR("Create ImPushImageButton %u failed ! ERROR [%d]", id, ret)
                break;
            }

            ImButtonQueryCallback *check = va_arg(args, ImButtonQueryCallback*);
            mQueryChecked = *check;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void ImPushImageButton::onGUI()
    {
        bool enable = mQueryEnabled ? mQueryEnabled(this) : isEnable();
        bool check = mQueryChecked ? mQueryChecked(this) : false;
        ImGuiStyle &style = ImGui::GetStyle();
            
        if (enable && check)
        {
            ImVec4 color = style.Colors[ImGuiCol_ButtonActive];
            ImGui::PushStyleColor(ImGuiCol_Button, color);
        }
            
        ImGui::BeginDisabled(!enable);
        if (ImGui::ImageButton(getName().c_str(), mTexID, getSize()))
        {
            if (mClicked != nullptr)
            {
                mClicked(this);
            }
            else
            {
                EventParamButtonClicked param(this);
                sendEvent(mClickedEvtID, &param);
            }
        }
        ImGui::EndDisabled();

        if (enable && check)
        {
            ImGui::PopStyleColor();
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(mTips.c_str());
        }
    }

    //--------------------------------------------------------------------------
}
