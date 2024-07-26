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


namespace Tiny3D
{
    #define IM_BEGIN_MENU(title) \
        { \
            ImMenu *menu = new ImMenu(); \
            menu->create(title, nullptr, nullptr);

    #define IM_MENU_ITEM(title, shortcut, enable, event) \
            { \
                ImMenuItemNormal *item = new ImMenuItemNormal(); \
                item->create(title, shortcut, enable, event, nullptr); \
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
    
    using EventParamMenuItem = EventParamT1<class ImMenuItem*>;
    
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

    using ImMenuItemClickedCallback = TFunction<void(ImMenuItem*)>;
    using ImMenuItemQueryCallback = TFunction<bool(ImMenuItem*)>;

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
        /// 点击事件
        uint32_t mClickedEventID {(uint32_t)-1};
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
        
        TResult create(uint32_t id, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, uint32_t eventID, ImWidget *parent, ImTextureID icon = nullptr);
        
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
         * @param eventID 
         * @param icon 
         * @return 
         */
        TResult addItem(uint32_t id, const String &name, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, uint32_t eventID, ImTextureID icon = nullptr);

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
         * @param eventID 
         * @param icon 
         * @return 
         */
        TResult addItem(uint32_t id, const String &name, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, uint32_t eventID, ImTextureID icon = nullptr);

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

        TResult create(uint32_t id, const String &title, const String &shortcut, const ImMenuItemQueryCallback &queryEnable, const ImMenuItemQueryCallback &queryCheck, uint32_t eventID, ImWidget *parent, ImTextureID icon = nullptr);
        
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

    /**
     * 菜单栏
     */
    class TINYIMGUI_API ImMenuBar : public ImWidget
    {
    public:
        ~ImMenuBar() override;

        WidgetType getWidgetType() const override { return WidgetType::kMenuBar; }
        
        TResult addMenu(ImMenu *menu);

        ImMenu *addMenu(uint32_t id, const String &name, ImTextureID icon = nullptr);

    protected:
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;
    };
    
}
