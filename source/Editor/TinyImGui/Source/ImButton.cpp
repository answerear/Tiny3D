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
#include "ImTextureManager.h"
#include "ImErrors.h"
#include "ImEventDefine.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImButton::~ImButton()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImButton::create(uint32_t id, const String &title, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 4, nullptr, nullptr, &tips, &shortcut);
    }

    //--------------------------------------------------------------------------

    TResult ImButton::create(uint32_t id, const String &title, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 4, nullptr, &clicked, &tips, &shortcut);
    }

    //--------------------------------------------------------------------------

    TResult ImButton::create(uint32_t id, const String &title, const ImButtonQueryCallback &queryEnabled, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 4, &queryEnabled, nullptr, &tips, &shortcut);
    }

    //--------------------------------------------------------------------------

    TResult ImButton::create(uint32_t id, const String &title, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 4, &queryEnabled, &clicked, &tips, &shortcut);
    }

    //--------------------------------------------------------------------------

    // argc : 4
    // args :
    //  ImButtonQueryCallback* : 可用状态查询回调
    //  ImButtonClickedCallback* : 点击回调
    //  String* : 提示语
    //  String* : 快捷键
    TResult ImButton::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = IM_OK;

        do
        {
            T3D_ASSERT(argc >= 4);

            ImButtonQueryCallback *queryEnabled = va_arg(args, ImButtonQueryCallback*);
            ImButtonClickedCallback *clicked = va_arg(args, ImButtonClickedCallback*);
            String *tips = va_arg(args, String*);
            String *shortcut = va_arg(args, String*);

            mQueryEnabled = *queryEnabled;
            if (clicked != nullptr)
            {
                mClicked = *clicked;
            }
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
            if (mClicked != nullptr)
            {
                mClicked(this);
            }
            else
            {
                EventParamButtonClicked param(this);
                sendEvent(kEvtButtonClicked, &param);
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

    TResult ImPushButton::create(uint32_t id, const String &title, ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 5, nullptr, nullptr, &tips, &shortcut, &queryChecked);
    }

    //--------------------------------------------------------------------------

    TResult ImPushButton::create(uint32_t id, const String &title, ImButtonQueryCallback &queryChecked, ImButtonClickedCallback clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 5, nullptr, &clicked, &tips, &shortcut, &queryChecked);
    }

    //--------------------------------------------------------------------------

    TResult ImPushButton::create(uint32_t id, const String &title, ImButtonQueryCallback &queryEnabled, ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 5, &queryEnabled, nullptr, &tips, &shortcut, &queryChecked);
    }

    //--------------------------------------------------------------------------

    TResult ImPushButton::create(uint32_t id, const String &title, ImButtonQueryCallback &queryEnabled, ImButtonQueryCallback &queryChecked, ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, title, parent, 5, &queryEnabled, clicked, &tips, &shortcut, &queryChecked);
    }

    //--------------------------------------------------------------------------

    // argc : 5
    // args :
    //  ImButtonQueryCallback* : 可用状态查询回调
    //  ImButtonClickedCallback* : 点击回调
    //  String* : 提示语
    //  String* : 快捷键
    //  ImButtonQueryCallback* : 是否按下状态查询回调
    TResult ImPushButton::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = IM_OK;

        do
        {
            T3D_ASSERT(argc >= 5);

            ret = ImButton::createInternal(id, name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                IMGUI_LOG_ERROR("Create ImPushButton %s failed ! ERROR [%d]", name.c_str(), ret)
                break;
            }

            ImButtonQueryCallback *checked = va_arg(args, ImButtonQueryCallback*);
            if (checked != nullptr)
            {
                mQueryChecked = *checked;
            }
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
                sendEvent(kEvtButtonClicked, &param);
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

    TResult ImImageButton::create(uint32_t id, ImTextureID texID, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, nullptr, nullptr, &tips, &shortcut, texID, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult ImImageButton::createEx(uint32_t id, const String &imageName, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, nullptr, nullptr, &tips, &shortcut, nullptr, &imageName);
    }

    //--------------------------------------------------------------------------
    
    TResult ImImageButton::create(uint32_t id, ImTextureID texID, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, nullptr, &clicked, &tips, &shortcut, texID, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult ImImageButton::createEx(uint32_t id, const String &imageName, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, nullptr, &clicked, &tips, &shortcut, nullptr, &imageName);
    }
    
    //--------------------------------------------------------------------------
    
    TResult ImImageButton::create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, &queryEnabled, nullptr, &tips, &shortcut, texID, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult ImImageButton::createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, &queryEnabled, nullptr, &tips, &shortcut, nullptr, &imageName);
    }
    
    //--------------------------------------------------------------------------
    
    TResult ImImageButton::create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, &queryEnabled, &clicked, &tips, &shortcut, texID);
    }
    
    //--------------------------------------------------------------------------

    TResult ImImageButton::createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 6, &queryEnabled, &clicked, &tips, &shortcut, nullptr, &imageName);
    }
    
    //--------------------------------------------------------------------------

    // argc : 6
    // args :
    //  ImButtonQueryCallback* : 可用状态查询回调
    //  ImButtonClickedCallback* : 点击回调
    //  String* : 提示语
    //  String* : 快捷键
    //  ImTextureID : 图标纹理对象
    //  String * 图标图像文件路径
    TResult ImImageButton::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = IM_OK;

        do
        {
            T3D_ASSERT(argc >= 6);
            
            ret = ImButton::createInternal(id, name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                IMGUI_LOG_ERROR("Create ImImageButton %u failed ! ERROR [%d]", id, ret)
                break;
            }
            
            mTexID = va_arg(args, ImTextureID);
            String *imageName = va_arg(args, String*);
            if (mTexID == nullptr)
            {
                mTexID = IM_TEXTURE_MGR.loadTexture(*imageName);
                if (mTexID == nullptr)
                {
                    ret = T3D_ERR_FAIL;
                    IMGUI_LOG_ERROR("Load texture %s failed ! ERROR [%d]", imageName->c_str(), ret);
                    break;
                }
                mIsInternalLoaded = true;
            }
            else
            {
                mIsInternalLoaded = false;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ImImageButton::onDestroy()
    {
        if (mIsInternalLoaded && mTexID != nullptr)
        {
            IM_TEXTURE_MGR.unloadTexture(mTexID);
            mTexID = nullptr;
        }
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
                sendEvent(kEvtButtonClicked, &param);
            }
        }
        ImGui::EndDisabled();
    }

    //--------------------------------------------------------------------------

    ImPushImageButton::~ImPushImageButton()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImPushImageButton::create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, nullptr, nullptr, &tips, &shortcut, texID, nullptr, &queryChecked);
    }

    //--------------------------------------------------------------------------

    TResult ImPushImageButton::createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, nullptr, nullptr, &tips, &shortcut, nullptr, &imageName, &queryChecked);
    }
    
    //--------------------------------------------------------------------------
    
    TResult ImPushImageButton::create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, nullptr, &clicked, &tips, &shortcut, texID, &queryChecked);
    }

    //--------------------------------------------------------------------------

    TResult ImPushImageButton::createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, nullptr, &clicked, &tips, &shortcut, nullptr, &imageName, &queryChecked);
    }
    
    //--------------------------------------------------------------------------
    
    TResult ImPushImageButton::create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, &queryEnabled, nullptr, &tips, &shortcut, texID, nullptr, &queryChecked);
    }

    //--------------------------------------------------------------------------

    TResult ImPushImageButton::createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, &queryEnabled, nullptr, &tips, &shortcut, nullptr, &imageName, &queryChecked);
    }
    
    //--------------------------------------------------------------------------
    
    TResult ImPushImageButton::create(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, &queryEnabled, &clicked, &tips, &shortcut, texID, nullptr, &queryChecked);
    }
    
    //--------------------------------------------------------------------------

    TResult ImPushImageButton::createEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, ImWidget *parent, const String &tips, const String &shortcut)
    {
        return ImWidget::createInternal(id, makeName(id), parent, 7, &queryEnabled, &clicked, &tips, &shortcut, nullptr, &imageName, &queryChecked);
    }
    
    //--------------------------------------------------------------------------

    // argc : 7
    // args :
    //  ImButtonQueryCallback* : 可用状态查询回调
    //  ImButtonClickedCallback* : 点击回调
    //  String* : 提示语
    //  String* : 快捷键
    //  ImTextureID : 图标纹理对象
    //  String * 图标图像文件路径
    TResult ImPushImageButton::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = IM_OK;

        do
        {
            T3D_ASSERT(argc >= 7);
            
            ret = ImImageButton::createInternal(id, name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                IMGUI_LOG_ERROR("Create ImPushImageButton %u failed ! ERROR [%d]", id, ret)
                break;
            }

            ImButtonQueryCallback *checked = va_arg(args, ImButtonQueryCallback*);
            if (checked != nullptr)
            {
                mQueryChecked = *checked;
            }
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
                sendEvent(kEvtButtonClicked, &param);
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
