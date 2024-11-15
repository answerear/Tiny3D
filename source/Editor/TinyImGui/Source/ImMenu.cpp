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
#include "ImWidgetID.h"
#include "ImEventDefine.h"
#include "ImTextureManager.h"


namespace Tiny3D
{
#if defined (USE_LEGACY_MENU)
    //--------------------------------------------------------------------------

    ImMenuItem::~ImMenuItem()
    {
        
    }

    //--------------------------------------------------------------------------

    // argc : >= 2
    // args :
    //  ImMenuItemQueryCallback* : 可用状态查询回调
    //  ImTextureID : 图标纹理对象
    TResult ImMenuItemEnabled::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 2);
        
        TResult ret = IM_OK;

        do
        {
            ret = ImMenuItem::createInternal(id, name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // query enable
            ImMenuItemQueryCallback *query = va_arg(args, ImMenuItemQueryCallback*);
            if (query != nullptr)
            {
                mQueryEnable = *query;
            }

            // icon
            mIcon = va_arg(args, ImTextureID);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    // argc : >= 4
    // args :
    //  ImMenuItemQueryCallback* : 可用状态查询回调
    //  ImTextureID : 图标纹理对象
    //  String* : 快捷键字符串
    //  ImMenuItemClickedCallback* : 点击回调
    TResult ImMenuItemClickable::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 4);
        
        TResult ret = IM_OK;

        do
        {
            ret = ImMenuItemEnabled::createInternal(id, name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                break;
            }

            /// 快捷键
            String *shortcut = va_arg(args, String*);
            mShortcut = *shortcut;
            
            /// 点击回调函数
            ImMenuItemClickedCallback *callback = va_arg(args, ImMenuItemClickedCallback*);
            if (callback != nullptr)
            {
                mClickedCallback = *callback;
            }
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

    TResult ImMenuItemNormal::create(uint32_t id, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemClickedCallback &callback, ImWidget *parent, ImTextureID icon)
    {
        return ImWidget::createInternal(id, title, parent, 4, &queryEnable, icon, &shortcut, &callback);
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemNormal::create(uint32_t id, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, ImWidget *parent, ImTextureID icon)
    {
        return ImWidget::createInternal(id, title, parent, 5, &queryEnable, icon, &shortcut, nullptr);
    }

    //--------------------------------------------------------------------------

    // argc : >= 4
    // args :
    //  ImMenuItemQueryCallback* : 可用状态查询回调
    //  ImTextureID : 图标纹理对象
    //  String* : 快捷键字符串
    //  ImMenuItemClickedCallback* : 点击回调
    TResult ImMenuItemNormal::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 4);

        TResult ret = IM_OK;
        
        do
        {
            ret = ImMenuItemClickable::createInternal(id, name, parent, argc, args);
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
            if (mClickedCallback != nullptr)
            {
                mClickedCallback(this);
            }
            else
            {
                EventParamMenuItem param;
                param.arg1 = this;
                sendEvent(kEvtMenuItemClicked, &param);
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

    TResult ImMenuItemPopup::create(uint32_t id, const String &title, const ImMenuItemQueryCallback &queryEnable, ImWidget *parent, ImTextureID icon)
    {
        return ImWidget::createInternal(id, title, parent, 2, &queryEnable, icon);
    }
    
    //--------------------------------------------------------------------------

    // argc : >= 2
    // args :
    //  ImMenuItemQueryCallback* : 可用状态查询回调
    //  ImTextureID : 图标纹理对象
    TResult ImMenuItemPopup::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 2);

        TResult ret = IM_OK;
        
        do
        {
            ret = ImMenuItemEnabled::createInternal(id, name, parent, argc, args);
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
        return addChild(item) ? IM_OK : IM_ERR_FAIL;
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::addItem(uint32_t id, const String &name, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, ImTextureID icon)
    {
        TResult ret = IM_OK;

        do
        {
            ImMenuItemNormal *item = new ImMenuItemNormal();
            ret = item->create(id, name, shortcut, queryEnable, this, icon);
            if (T3D_FAILED(ret))
            {
                T3D_SAFE_DELETE(item);
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "Add menu item %s failed ! ERROR [%d]", name.c_str(), ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::addItem(uint32_t id, const String &name, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemClickedCallback &callback, ImTextureID icon)
    {
        TResult ret = IM_OK;

        do
        {
            ImMenuItemNormal *item = new ImMenuItemNormal();
            ret = item->create(id, name, shortcut, queryEnable, callback, this, icon);
            if (T3D_FAILED(ret))
            {
                T3D_SAFE_DELETE(item);
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "Add menu item %s failed ! ERROR [%d]", name.c_str(), ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::addItem(uint32_t id, const String &name, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, const ImMenuItemClickedCallback &callback, ImTextureID icon)
    {
        TResult ret = IM_OK;

        do
        {
            ImMenuItemCheck *item = new ImMenuItemCheck();
            ret = item->create(id, name, shortcut, queryEnable, queryCheck, callback, this, icon);
            if (T3D_FAILED(ret))
            {
                T3D_SAFE_DELETE(item);
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "Add check menu item %s failed ! ERROR [%d]", name.c_str(), ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::addItem(uint32_t id, const String &name, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, ImTextureID icon)
    {
        TResult ret = IM_OK;

        do
        {
            ImMenuItemCheck *item = new ImMenuItemCheck();
            ret = item->create(id, name, shortcut, queryEnable, queryCheck, this, icon);
            if (T3D_FAILED(ret))
            {
                T3D_SAFE_DELETE(item);
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "Add check menu item %s failed ! ERROR [%d]", name.c_str(), ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    ImMenuItemPopup *ImMenuItemPopup::addItem(uint32_t id, const String &name, ImTextureID icon)
    {
        ImMenuItemPopup *item = new ImMenuItemPopup();
        TResult ret = item->create(id, name, nullptr, this, icon);
        if (T3D_FAILED(ret))
        {
            T3D_SAFE_DELETE(item);
            T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "Add popup menu item %s failed ! ERROR [%d]", name.c_str(), ret);
            return nullptr;
        }

        return item;
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::addSeparator()
    {
        TResult ret;
        do
        {
            ImMenuSeparator *separator = new ImMenuSeparator();
            ret = separator->create(ID_MENU_SEPARATOR, "", this);
            if (T3D_FAILED(ret))
            {
                T3D_SAFE_DELETE(separator);
                T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "Add separator failed ! ERROR [%d]", ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::insertAfterItem(const String &prevName, ImMenuItem *item)
    {
        return insertAfterChild(prevName, item);
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::insertAfterItem(const UUID &prevUUID, ImMenuItem *item)
    {
        return insertAfterChild(prevUUID, item);
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItemPopup::insertAfterItem(ImMenuItem *prevItem, ImMenuItem *item)
    {
        return insertAfterChild(prevItem, item);
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

    TResult ImMenuItemCheck::create(uint32_t id, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, const ImMenuItemClickedCallback &callback, ImWidget *parent, ImTextureID icon)
    {
        return ImWidget::createInternal(id, title, parent, 5, &queryEnable, icon, &shortcut, &callback, &queryCheck);
    }
    
    //--------------------------------------------------------------------------

    TResult ImMenuItemCheck::create(uint32_t id, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, ImWidget *parent, ImTextureID icon)
    {
        return ImWidget::createInternal(id, title, parent, 5, &queryEnable, icon, &shortcut, nullptr, &queryCheck);
    }

    //--------------------------------------------------------------------------

    // argc : >= 5
    // args :
    //  ImMenuItemQueryCallback* : 可用状态查询回调
    //  ImTextureID : 图标纹理对象
    //  String* : 快捷键字符串
    //  ImMenuItemClickedCallback* : 点击回调
    //  ImMenuItemQueryCallback* : 选中态查询回调
    TResult ImMenuItemCheck::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 5);

        TResult ret = IM_OK;
        
        do
        {
            ret = ImMenuItemClickable::createInternal(id, name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // query check
            ImMenuItemQueryCallback *queryCheck = va_arg(args, ImMenuItemQueryCallback*);
            if (queryCheck != nullptr)
            {
                mQueryCheck = *queryCheck;
            }
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
            if (mClickedCallback != nullptr)
            {
                mClickedCallback(this);
            }
            else
            {
                EventParamMenuItem param;
                param.arg1 = this;
                sendEvent(kEvtMenuItemClicked, &param);
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
#else
    //--------------------------------------------------------------------------

    ImMenuItemData::ImMenuItemData(ImMenuItemType _type, uint32_t _menuID, const String &_title, const String &_shortcut, const ImMenuItemQueryCallback &_queryEnable, const ImMenuItemQueryCallback &_queryCheck, const ImMenuItemClickedCallback &_clicked, const String &iconName)
        : type(_type), menuID(_menuID), title(_title), shortcut(_shortcut), queryEnable(_queryEnable), queryCheck(_queryCheck), clicked(_clicked)
    {
        if (!iconName.empty())
        {
            icon = IM_TEXTURE_MGR.loadTexture(iconName);
        }
    }
    
    //--------------------------------------------------------------------------

    ImMenuItemData::~ImMenuItemData()
    {
        if (icon != nullptr)
        {
            IM_TEXTURE_MGR.unloadTexture(icon);
            icon = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    ImMenuItem::ImMenuItemsPool ImMenuItem::msMenuItemsPool;

    //--------------------------------------------------------------------------
    
    void ImMenuItem::addToPool(ImMenuItemType type, uint32_t menuID, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, const ImMenuItemClickedCallback &clicked, const String &iconName)
    {
        msMenuItemsPool.emplace(menuID, ImMenuItemData(type, menuID, title, shortcut, queryEnable, queryCheck, clicked, iconName));
    }

    //--------------------------------------------------------------------------

    void ImMenuItem::GC()
    {
        msMenuItemsPool.clear();
    }
    
    //--------------------------------------------------------------------------

    ImMenuItem::ImMenuItem()
    {
        
    }
    
    //--------------------------------------------------------------------------

    ImMenuItem::~ImMenuItem()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImMenuItem::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            uint32_t menuID = getID();
            mItr = msMenuItemsPool.find(menuID);
            if (mItr == msMenuItemsPool.end())
            {
                IMGUI_LOG_ERROR("Failed to find menu with ID (%d) !", menuID);
                ret = T3D_ERR_NOT_FOUND;
                break;
            }
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    void ImMenuItem::onGUI()
    {
        const ImMenuItemData &data = mItr->second;
        
        if (data.icon != nullptr)
        {
            // 绘制图标
            ImGui::Image(data.icon, ImVec2(20, 20));
            ImGui::SameLine();
        }

        // 实时获取菜单项状态
        bool enable = data.queryEnable ? data.queryEnable(this) : true;

        // 菜单项
        if (ImGui::MenuItem(data.title.c_str(), data.shortcut.empty() ? nullptr : data.shortcut.c_str(), false, enable))
        {
            if (data.clicked != nullptr)
            {
                data.clicked(this);
            }
            else
            {
                EventParamMenuItem param;
                param.arg1 = this;
                sendEvent(kEvtMenuItemClicked, &param);
            }
        }
    }
    
    //--------------------------------------------------------------------------

    bool ImMenuItemPopup::onGUIBegin()
    {
        PushWidgetID();
        const auto &data = mItr->second;
        bool enable = data.queryEnable ? data.queryEnable(this) : true;
        bool ret = ImGui::BeginMenu(data.title.c_str(), enable);
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

    void ImMenuItemCheck::onGUI()
    {
        const auto &data = mItr->second;
        
        auto callback = [&data, this]()
        {
            if (data.clicked != nullptr)
            {
                data.clicked(this);
            }
            else
            {
                EventParamMenuItem param;
                param.arg1 = this;
                sendEvent(kEvtMenuItemClicked, &param);
            }
        };

        bool enabled = data.queryEnable ? data.queryEnable(this) : true;
        bool checked = false;
        
        if (data.queryCheck != nullptr)
        {
            checked = data.queryCheck(this);
        }

        if (ImGui::MenuItem(data.title.c_str(), data.shortcut.empty() ? nullptr : data.shortcut.c_str(), checked, enabled))
        {
            callback();
        }
    }
    
    //--------------------------------------------------------------------------

    void ImMenuItemSeparator::onGUI()
    {
        ImGui::Separator();
    }
    
    //--------------------------------------------------------------------------
#endif
    
    //--------------------------------------------------------------------------

    ImMenuBar::~ImMenuBar()
    {
        
    }

    //--------------------------------------------------------------------------
#if defined (USE_LEGACY_MENU)
    TResult ImMenuBar::addMenu(ImMenu *menu)
    {
        return addChild(menu) ? IM_OK : IM_ERR_FAIL;
    }

    //--------------------------------------------------------------------------

    ImMenu *ImMenuBar::addMenu(uint32_t id, const String &name, ImTextureID icon)
    {
        ImMenu *menu = new ImMenu();
        TResult ret = menu->create(id, name, nullptr, this, icon);
        if (T3D_FAILED(ret))
        {
            T3D_SAFE_DELETE(menu);
            return nullptr;
        }

        return menu;
    }
#endif
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
