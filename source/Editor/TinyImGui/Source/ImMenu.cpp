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


#include "ImMenu.h"

#include "ImErrors.h"
#include "T3DLog.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImMenuItem::~ImMenuItem()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemEnabled::createInternal(const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 2, "Invalid number of arguments in ImMenuItemEnabled::create() !");
        
        TResult ret = IM_OK;

        do
        {
            ret = ImMenuItem::createInternal(name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // query enable
            ImMenuItemQueryCallback *query = va_arg(args, ImMenuItemQueryCallback*);
            mQueryEnable = *query;

            // icon
            mIcon = va_arg(args, ImTextureID);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemClickable::createInternal(const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 3, "Invalid number of arguments in ImMenuItemClickable::create() !");
        
        TResult ret = IM_OK;

        do
        {
            ret = ImMenuItemEnabled::createInternal(name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                break;
            }

            /// 快捷键
            String *shortcut = va_arg(args, String*);
            mShortcut = *shortcut;
            
            /// 点击事件
            mClickedEventID = va_arg(args, uint32_t);
            
            /// 点击回调函数
            ImMenuItemClickedCallback *callback = va_arg(args, ImMenuItemClickedCallback*);
            mClickedCallback = *callback;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    ImMenuItemNormal::~ImMenuItemNormal()
    {
        
    }

    //--------------------------------------------------------------------------

    ImMenuItem::Type ImMenuItemNormal::getType() const
    {
        return Type::kNormal;
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemNormal::create(const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemClickedCallback &callback, ImWidget *parent, ImTextureID icon)
    {
        return ImWidget::createInternal(title, parent, 5, &queryEnable, icon, &shortcut, 0, &callback);
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemNormal::create(const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, uint32_t eventID, ImWidget *parent, ImTextureID icon)
    {
        return ImWidget::createInternal(title, parent, 5, &queryEnable, icon, &shortcut, eventID, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemNormal::createInternal(const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 5, "Invalid number of arguments in ImMenuItemNormal::create() !");

        TResult ret = IM_OK;
        
        do
        {
            ret = ImMenuItemClickable::createInternal(name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ImMenuItemNormal::onGUIBegin()
    {
        return ImMenuItemClickable::onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void ImMenuItemNormal::onGUI()
    {
        if (mIcon != nullptr)
        {
            // 绘制图标
            ImGui::Image(mIcon, ImVec2(20, 20));
            ImGui::SameLine();
        }

        // 实时获取菜单项状态
        bool enable = mQueryEnable ? mQueryEnable(this) : true;

        // 菜单项
        if (ImGui::MenuItem(getName().c_str(), mShortcut.empty() ? nullptr : mShortcut.c_str(), false, enable))
        {
            if (mClickedEventID != (uint32_t)-1)
            {
                EventParamMenuItem param;
                param.arg1 = this;
                sendEvent(mClickedEventID, &param);
            }
            else if (mClickedCallback != nullptr)
            {
                mClickedCallback(this);
            }
        }
    }

    //--------------------------------------------------------------------------

    void ImMenuItemNormal::onGUIEnd()
    {
        ImMenuItemClickable::onGUIEnd();
    }

    //--------------------------------------------------------------------------

    ImMenuItemPopup::~ImMenuItemPopup()
    {
        
    }

    //--------------------------------------------------------------------------

    ImMenuItem::Type ImMenuItemPopup::getType() const
    {
        return Type::kPopup;
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::create(const String &title, const ImMenuItemQueryCallback &queryEnable, ImWidget *parent, ImTextureID icon)
    {
        return ImWidget::createInternal(title, parent, 2, &queryEnable, icon);
    }
    
    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::createInternal(const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 2, "Invalid number of arguments in ImMenuItemPopup::create() !");

        TResult ret = IM_OK;
        
        do
        {
            ret = ImMenuItemEnabled::createInternal(name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }
    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::addItem(ImMenuItem *item)
    {
        return addWidget(item); 
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::insertAfterItem(const String &prevName, ImMenuItem *item)
    {
        return insertAfterWidget(prevName, item);
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::insertAfterItem(const UUID &prevUUID, ImMenuItem *item)
    {
        return insertAfterWidget(prevUUID, item);
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::insertAfterItem(ImMenuItem *prevItem, ImMenuItem *item)
    {
        return insertAfterWidget(prevItem, item);
    }

    //--------------------------------------------------------------------------
    
    bool ImMenuItemPopup::onGUIBegin()
    {
        PushWidgetID();
        bool enable = mQueryEnable ? mQueryEnable(this) : true;
        bool ret = ImGui::BeginMenu(getName().c_str(), enable);
        if (!ret)
        {
            PopWidgetID();
        }

        return ret;
    }
    
    //--------------------------------------------------------------------------

    void ImMenuItemPopup::onGUI()
    {
        
    }
    
    //--------------------------------------------------------------------------

    void ImMenuItemPopup::onGUIEnd()
    {
        ImGui::EndMenu();
        PopWidgetID();
    }
    
    //--------------------------------------------------------------------------

    ImMenuItemCheck::~ImMenuItemCheck()
    {
        
    }

    //--------------------------------------------------------------------------
    
    ImMenu::Type ImMenuItemCheck::getType() const
    {
        return Type::kCheck;
    }
    
    //--------------------------------------------------------------------------

    TResult ImMenuItemCheck::create(const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, ImWidget *parent, ImTextureID icon)
    {
        return ImWidget::createInternal(title, parent, 6, &queryEnable, icon, &shortcut, 0, nullptr, &queryCheck);
    }
    
    //--------------------------------------------------------------------------

    TResult ImMenuItemCheck::createInternal(const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 6, "Invalid number of arguments in ImMenuItemCheck::create() !");

        TResult ret = IM_OK;
        
        do
        {
            ret = ImMenuItemClickable::createInternal(name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // query check
            ImMenuItemQueryCallback *queryCheck = va_arg(args, ImMenuItemQueryCallback*);
            mQueryCheck = *queryCheck;
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    bool ImMenuItemCheck::onGUIBegin()
    {
        return ImMenuItemClickable::onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void ImMenuItemCheck::onGUI()
    {
        auto callback = [this]()
        {
            if (mClickedEventID != (uint32_t)-1)
            {
                EventParamMenuItem param;
                param.arg1 = this;
                sendEvent(mClickedEventID, &param);
            }
            else if (mClickedCallback != nullptr)
            {
                mClickedCallback(this);
            }
        };

        bool enable = mQueryEnable ? mQueryEnable(this) : true;
        
        if (mQueryCheck != nullptr)
        {
            bool check = mQueryCheck(this);
            if (ImGui::MenuItem(getName().c_str(), mShortcut.empty() ? nullptr : mShortcut.c_str(), check, enable))
            {
                callback();
            }
        }
        else
        {
            if (ImGui::MenuItem(getName().c_str(), mShortcut.empty() ? nullptr : mShortcut.c_str(), false, enable))
            {
                callback();
            }
        }
    }
    
    //--------------------------------------------------------------------------

    void ImMenuItemCheck::onGUIEnd()
    {
        ImMenuItemClickable::onGUIEnd();
    }
    
    //--------------------------------------------------------------------------

    ImMenuSeparator::~ImMenuSeparator()
    {
        
    }

    //--------------------------------------------------------------------------

    ImMenu::Type ImMenuSeparator::getType() const
    {
        return Type::kSeparator;
    }
    
    //--------------------------------------------------------------------------

    bool ImMenuSeparator::onGUIBegin()
    {
        return ImMenuItem::onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void ImMenuSeparator::onGUI()
    {
        ImGui::Separator();
    }
    
    //--------------------------------------------------------------------------

    void ImMenuSeparator::onGUIEnd()
    {
        ImMenuItem::onGUIEnd();
    }
    
    //--------------------------------------------------------------------------

    ImMenuBar::~ImMenuBar()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImMenuBar::addMenu(ImMenu *menu)
    {
        return addWidget(menu);
    }

    //--------------------------------------------------------------------------

    bool ImMenuBar::onGUIBegin()
    {
        PushWidgetID();
        bool ret = ImGui::BeginMainMenuBar();

        if (!ret)
        {
            PopWidgetID();
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void ImMenuBar::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImMenuBar::onGUIEnd()
    {
        ImGui::EndMainMenuBar();
        PopWidgetID();
    }

    //--------------------------------------------------------------------------         
}
