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


#include "ImWidget.h"

// #define USE_LEGACY_MENU


namespace Tiny3D
{
#if defined (USE_LEGACY_MENU)
    #define IM_BEGIN_MENU(title) \
        { \
            ImMenu *menu = new ImMenu(); \
            menu->create(title, nullptr, nullptr);

    #define IM_MENU_ITEM(title, shortcut, enable) \
            { \
                ImMenuItemNormal *item = new ImMenuItemNormal(); \
                item->create(title, shortcut, enable, nullptr); \
                menu->addItem(item); \
            }

    #define IM_MENU_ITEM_CB(title, shortcut, enable, callback) \
            { \
                ImMenuItemNormal *item = new ImMenuItemNormal(); \
                item->create(title, shortcut, enable, callback, nullptr); \
                menu->addItem(item); \
            }

    #define IM_MENU_ITEM_CHECK(title, shortcut, enable, check)    \
            { \
                ImMenuItemCheck *item = new ImMenuItemCheck(); \
                item->create(title, shortcut, enable, check, nullptr); \
                menu->addItem(item); \
            }
    
    #define IM_BEGIN_POPUP_MENU(title) \
            { \
                ImMenuItemPopup *subMenu = new ImMenuItemPopup(); \
                subMenu->create(title, nullptr, nullptr); \
                ImMenu *parentMenu = menu; \
                menu = subMenu;
    
    #define IM_END_POPUP_MENU() \
                menu = parentMenu; \
                menu->addItem(subMenu); \
            }
    
    #define IM_END_MENU(menubar) \
            menubar->addMenu(menu); \
        }
    
    using EventParamMenuItem = EventParamT1<class ImWidget*>;
    using ImMenuItemClickedCallback = TFunction<void(ImWidget*)>;
    using ImMenuItemQueryCallback = TFunction<bool(ImWidget*)>;

    /**
     * 菜单项基类
     */
    class TINYIMGUI_API ImMenuItem : public ImWidget
    {
    public:
        enum class Type : uint32_t
        {
            /// 普通菜单项
            kNormal = 0,
            /// 弹出菜单项
            kPopup,
            /// 可勾选菜单项
            kCheck,
            /// 菜单项分隔符
            kSeparator,
        };

        ~ImMenuItem() override;

        WidgetType getWidgetType() const override { return WidgetType::kMenuItem; }
        
        virtual Type getType() const = 0;
    };

    /**
     * 可启用/禁用菜单项
     */
    class TINYIMGUI_API ImMenuItemEnabled : public ImMenuItem
    {
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;

    protected:
        /// 查询 enable 状态
        ImMenuItemQueryCallback mQueryEnable {nullptr};
        /// 图标
        ImTextureID mIcon {nullptr};
    };

    /**
     * 可点击菜单项
     */
    class TINYIMGUI_API ImMenuItemClickable : public ImMenuItemEnabled
    {
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;

    protected:
        /// 点击回调函数
        ImMenuItemClickedCallback mClickedCallback {nullptr};
        /// 快捷键
        String mShortcut {};
    };

    /**
     * 普通菜单项
     */
    class TINYIMGUI_API ImMenuItemNormal : public ImMenuItemClickable, public EventHandler
    {
    public:
        ~ImMenuItemNormal() override;

        Type getType() const override;

        TResult create(uint32_t id, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemClickedCallback &callback, ImWidget *parent, ImTextureID icon = nullptr);
        
        TResult create(uint32_t id, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, ImWidget *parent, ImTextureID icon = nullptr);
        
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;
    };

    /**
     * 弹出菜单项
     */
    class TINYIMGUI_API ImMenuItemPopup : public ImMenuItemEnabled
    {
    public:
        ~ImMenuItemPopup() override;

        Type getType() const override;

        TResult create(uint32_t id, const String &title, const ImMenuItemQueryCallback &queryEnable, ImWidget *parent, ImTextureID icon = nullptr);

        /**
         * 添加菜单项
         * @param item 
         * @return 
         */
        TResult addItem(ImMenuItem *item);

        /**
         * 添加普通菜單項
         * @param id
         * @param name 
         * @param shortcut 
         * @param queryEnable 
         * @param callback 
         * @param icon 
         * @return 
         */
        TResult addItem(uint32_t id, const String &name, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemClickedCallback &callback, ImTextureID icon = nullptr);

        /**
         * 添加普通菜單項
         * @param id
         * @param name 
         * @param shortcut 
         * @param queryEnable 
         * @param icon 
         * @return 
         */
        TResult addItem(uint32_t id, const String &name, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, ImTextureID icon = nullptr);

        /**
         * 添加两态菜单项
         * @param id
         * @param name 
         * @param shortcut 
         * @param queryEnable 
         * @param queryCheck
         * @param callback
         * @param icon 
         * @return 
         */
        TResult addItem(uint32_t id, const String &name, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, const ImMenuItemClickedCallback &callback, ImTextureID icon = nullptr);

        /**
         * 添加两态菜单项
         * @param id
         * @param name 
         * @param shortcut 
         * @param queryEnable 
         * @param queryCheck 
         * @param icon 
         * @return 
         */
        TResult addItem(uint32_t id, const String &name, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, ImTextureID icon = nullptr);

        /**
         * 添加二级弹出式菜单项
         * @param id
         * @param name 
         * @param icon 
         * @return 
         */
        ImMenuItemPopup *addItem(uint32_t id, const String &name, ImTextureID icon = nullptr);

        TResult addSeparator();

        TResult insertAfterItem(const String &prevName, ImMenuItem *item);

        TResult insertAfterItem(const UUID &prevUUID, ImMenuItem *item);

        TResult insertAfterItem(ImMenuItem *prevItem, ImMenuItem *item);

    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;
    };

    /**
     * 二态可口选菜单项
     */
    class TINYIMGUI_API ImMenuItemCheck : public ImMenuItemClickable, public EventHandler
    {
    public:
        ~ImMenuItemCheck() override;

        Type getType() const override;

        TResult create(uint32_t id, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, const ImMenuItemClickedCallback &callback, ImWidget *parent, ImTextureID icon = nullptr);

        TResult create(uint32_t id, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, ImWidget *parent, ImTextureID icon = nullptr);
        
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;

    protected:
        ImMenuItemQueryCallback mQueryCheck {nullptr};
    };

    /**
     * 菜单分割符
     */
    class TINYIMGUI_API ImMenuSeparator : public ImMenuItem
    {
    public:
        ~ImMenuSeparator() override;

        Type getType() const override;

    protected:
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;
    };

    using ImMenu = ImMenuItemPopup;
#else

    #define IM_MENU_ITEM_DATA(type, menuID, title, shortcut, icon_name, query_enabled, query_checked, clicked) \
        ImMenuItem::addToPool((type), (menuID), (title), (shortcut), (query_enabled), (query_checked), (clicked), (icon_name));

    #define IM_MENU_BEGIN(menubar) \
        { \
            ImWidget *parent = menubar;
    
    #define IM_MENU_POPUP_BEGIN(menuID) \
            { \
                ImMenuItemPopup *item = new ImMenuItemPopup(); \
                TResult rt = item->create(menuID, #menuID, parent); \
                T3D_ASSERT(T3D_SUCCEEDED(rt)); \
                ImWidget *parent = item;

    #define IM_MENU_POPUP_END() \
            }

    #define IM_MENU_ITEM(menuID) \
            { \
                ImMenuItem *item = new ImMenuItem(); \
                TResult rt = item->create(menuID, #menuID, parent); \
                T3D_ASSERT(T3D_SUCCEEDED(rt)); \
            }

    #define IM_MENU_CHECK(menuID) \
            { \
                ImMenuItemCheck *item = new ImMenuItemCheck(); \
                TResult rt = item->create(menuID, #menuID, parent); \
                T3D_ASSERT(T3D_SUCCEEDED(rt)); \
            }

    #define IM_MENU_SEPARATOR() \
            { \
                ImMenuItemSeparator *item = new ImMenuItemSeparator(); \
                TResult rt = item->create(ID_MENU_SEPARATOR, "##MenuSeparator", parent); \
                T3D_ASSERT(T3D_SUCCEEDED(rt)); \
            }

    #define IM_MENU_END() \
        }
    
    using EventParamMenuItem = EventParamT1<class ImWidget*>;
    using ImMenuItemClickedCallback = TFunction<void(ImWidget*)>;
    using ImMenuItemQueryCallback = TFunction<bool(ImWidget*)>;
    
    enum class ImMenuItemType : uint32_t
    {
        /// 默认值
        kInvalid = 0,
        /// 普通菜单项
        kNormal,
        /// 弹出菜单项
        kPopup,
        /// 可勾选菜单项
        kCheck,
        /// 菜单项分隔符
        kSeparator,
    };
    
    struct ImMenuItemData
    {
        ImMenuItemData(ImMenuItemType _type, uint32_t _menuID, const String &_title, const String &_shortcut, const ImMenuItemQueryCallback &_queryEnable, const ImMenuItemQueryCallback &_queryCheck, const ImMenuItemClickedCallback &_clicked, const String &iconName);

        ~ImMenuItemData();

        ImMenuItemType type {ImMenuItemType::kNormal};
        uint32_t menuID {0};
        ImTextureID icon {nullptr};
        String title {};
        String shortcut {};
        ImMenuItemQueryCallback queryEnable {nullptr};
        ImMenuItemQueryCallback queryCheck {nullptr};
        ImMenuItemClickedCallback clicked {nullptr};
    };
    
    class TINYIMGUI_API ImMenuItem : public ImWidget, public EventHandler
    {
    public:
        static void addToPool(ImMenuItemType type, uint32_t menuID, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, const ImMenuItemClickedCallback &clicked, const String &iconName);

        static void GC();

        ImMenuItem();

        ~ImMenuItem() override;

        WidgetType getWidgetType() const override { return WidgetType::kMenuItem; }

    protected:
        TResult onCreate() override;

        void onGUI() override;
        
    protected:
        using ImMenuItemsPool = TUnorderedMap<uint32_t, ImMenuItemData>;

        static ImMenuItemsPool msMenuItemsPool;

        uint32_t mMenuID {0};
        ImMenuItemsPool::iterator mItr {};
    };

    class TINYIMGUI_API ImMenuItemPopup : public ImMenuItem
    {
    protected:
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;
    };

    class TINYIMGUI_API ImMenuItemCheck : public ImMenuItem
    {
    protected:
        void onGUI() override;
    };

    class TINYIMGUI_API ImMenuItemSeparator : public ImMenuItem
    {
    protected:
        void onGUI() override;
    };

    using ImMenu = ImMenuItem;
#endif
    
    /**
     * 菜单栏
     */
    class TINYIMGUI_API ImMenuBar : public ImWidget
    {
    public:
        ~ImMenuBar() override;

        WidgetType getWidgetType() const override { return WidgetType::kMenuBar; }

#if defined (USE_LEGACY_MENU)
        TResult addMenu(ImMenu *menu);

        ImMenu *addMenu(uint32_t id, const String &name, ImTextureID icon = nullptr);
#endif

    protected:
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;
    };
    
}
