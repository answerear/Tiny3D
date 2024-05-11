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


namespace Tiny3D
{
    using Children = TList<class ImWidget*>;
    
    class TINYIMGUI_API ImWidget : public Noncopyable
    {
    public:
        /**
         * 释放没有使用的 widgets
         */
        static void GC();
        
        ~ImWidget() override;

        const String &getName() const { return mName; }

        const UUID &getUUID() const { return mUUID; }

        TResult create(const String &name, ImWidget *parent);

        /**
         * 删除 widget，包括自动 delete
         * @return 调用成功返回 IM_OK
         */
        TResult destroy();

        TResult addWidget(ImWidget *widget);

        TResult removeWidget(ImWidget *widget, bool destroy = true);

        ImWidget *removeWidget(const String &name, bool destroy = true);

        ImWidget *removeWidget(const UUID &uuid, bool destroy = true);

        TResult removeAllWidgets();

        /**
         * 可递归获取多层 widget
         * @param name : 指定名称的 widget，递归层级的需要用过 / 隔开。 如："window/panel/button" 
         * @return 返回对应的 widget 对象
         */
        ImWidget *getWidget(const String &name) const;

        /**
         * 获取所有子节点中对应 uuid 的子节点，这个调用可以递归搜索o
         * @param uuid 
         * @return 返回对应的 widget 对象
         */
        ImWidget *getWidget(const UUID &uuid) const;

        /**
         * 获取父 widget
         * @return 返回父 widget
         */
        ImWidget *getParent() const { return mParent; }

        /**
         * 只获取第一层子 widget 指定名称的 widget
         * @param [in] name : 子 widget 名称
         * @return 返回对应的 widget 对象
         */
        ImWidget *getChild(const String &name) const;

        /**
         * 只获取第一层子 widget 对应 uuid 的 widget
         * @param [in] uuid : 子 widget 的 uuid
         * @return 返回对应的 widget 对象
         */
        ImWidget *getChild(const UUID &uuid) const;

        /**
         * 获取下一级的所有子 widgets
         * @return 返回下一级所有子 widgets
         */
        const Children &getChildren() const { return mChildren; }

        virtual void update();
        
    protected:
        ImWidget() = default;

        TResult create(const String &name, ImWidget *parent, int32_t argc, ...);
        
        virtual TResult create(const String &name, ImWidget *parent, int32_t argc, va_list args);
        
        virtual TResult onCreate();
        
        virtual bool onGUIBegin();

        virtual void onGUI();

        virtual void onGUIEnd();

        virtual void onDestroy();

        ImWidget *getWidgetRecursively(StringList &names) const;

    private:
        void destroyWidget(ImWidget *child);
        
        using WaitingDestroyWidgets = TList<ImWidget*>;

        /// 所有待删除的 widget
        static WaitingDestroyWidgets msWaitingDestroyWidgets;
        
        /// UUID
        UUID    mUUID {UUID::INVALID};
        /// Name
        String  mName {};
        /// Parent
        ImWidget  *mParent {nullptr};
        /// Children
        Children    mChildren {};
        
    protected:
        bool    mVisible {true};
    };
}
