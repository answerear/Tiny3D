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
#include "ImMenu.h"


namespace Tiny3D
{
    #define ON_MENU_ITEM(item_id, func) registerMenuEvent(item_id, func)
    #define ON_MENU_ITEM_MEMBER(item_id, func) ON_MENU_ITEM(item_id, std::bind(&func, this, std::placeholders::_1, std::placeholders::_2))

    #define ON_MENU_ITEM_QUERY(item_id, func) setMenuItemQueryEnabledHandle(item_id, func)
    #define ON_MENU_ITEM_QUERY_MEMBER(item_id, func) ON_MENU_ITEM_QUERY(item_id, std::bind(&func, this, std::placeholders::_1, std::placeholders::_2))

    #define IM_MENU_ITEM_DEFAULT_QUERY_ENABLED()    std::bind(&ImMenuEventManager::onQueryEnabled, ImMenuEventManager::getInstancePtr(), std::placeholders::_1)
    #define IM_MENU_ITEM_DEFAULT_QUERY_CHECKED()    std::bind(&ImMenuEventManager::onQueryChecked, ImMenuEventManager::getInstancePtr(), std::placeholders::_1)

    #define IM_MENU_ITEM_DATA_DEFAULT(type, menuID, title, shortcut, icon_name) \
        IM_MENU_ITEM_DATA(type, menuID, title, shortcut, icon_name, IM_MENU_ITEM_DEFAULT_QUERY_ENABLED(), IM_MENU_ITEM_DEFAULT_QUERY_CHECKED(), nullptr)
    
    using ImMenuEventHandle = TFunction<bool(uint32_t, ImWidget*)>;
    using ImMenuQueryHandle = TFunction<bool(uint32_t, ImWidget*)>;
    
    class TINYIMGUI_API ImMenuEventHandler
    {
    public:
        virtual ~ImMenuEventHandler();

    protected:
        TResult registerMenuEvent(uint32_t menuID, const ImMenuEventHandle &handle);

        TResult unregisterMenuEvent(uint32_t menuID);

        void unregisterAllMenuEvents();

        TResult setMenuItemQueryEnabledHandle(uint32_t menuID, const ImMenuQueryHandle &handle);

        TResult setMenuItemQueryCheckedHandle(uint32_t menuID, const ImMenuQueryHandle &handle);
    };

    class TINYIMGUI_API ImMenuEventManager
        : public EventHandler
        , public Singleton<ImMenuEventManager>
    {
        friend class ImMenuEventHandler;
        
    public:
        ImMenuEventManager();

        ~ImMenuEventManager() override;

        TResult setMenuItemQueryEnabledHandle(uint32_t menuID, const ImMenuQueryHandle &handle);

        TResult setMenuItemQueryCheckedHandle(uint32_t menuID, const ImMenuQueryHandle &handle);

        bool onQueryEnabled(ImWidget *menuItem);

        bool onQueryChecked(ImWidget *menuItem);

    protected:
        TResult registerMenuEvent(uint32_t menuID, ImMenuEventHandler *handler, const ImMenuEventHandle &handle);

        TResult unregisterMenuEvent(uint32_t menuID, ImMenuEventHandler *handler);

        TResult unregisterMenuEventHandler(ImMenuEventHandler *handler);

        bool onHandleMenuItem(EventParam *param, TINSTANCE sender);

    protected:
        using ImMenuItemHandles = TUnorderedMultimap<ImMenuEventHandler*, ImMenuEventHandle>;        
        using ImMenuItemHandlers = TUnorderedMultimap<uint32_t, ImMenuItemHandles>;

        using ImMenuItemQueryHandles = TUnorderedMap<uint32_t, ImMenuQueryHandle>;
        
        /// 点击菜单项的回调集合
        ImMenuItemHandlers mMenuItemHandlers {};

        /// 查询菜单项是否可点击回调集合
        ImMenuItemQueryHandles mMenuItemQueryEnabledHandles {};

        /// 查询菜单项是否check状态回调集合
        ImMenuItemQueryHandles mMenuItemQueryCheckedHandles {};
    };
}
