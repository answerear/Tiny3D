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

    enum class WidgetType : uint32_t
    {
        kNone = 0,
        kWindow,
        kChildView,
        kDialog,
        kMenuBar,
        kMenuItem,
        kContextMenu,
        kToolBar,
        kTabBar,
        kTabItem,
        kDockingWindow,
        kDockBar,
        kDockItem,
        kSplitView,
        kHorizontalLayout,
        kVerticalLayout,
        kButton,
        kPushButton,
        kImageButton,
        kPushImageButton,
        kTreeNode,
        kTreeWidget,
        kListItem,
        kListWidget,
        kTreeBar,
    };

    using TraverseHierarchyCallback = TFunction<void(ImWidget *, int32_t)>;
    
    class TINYIMGUI_API ImWidget : public Noncopyable
    {
    public:
        /**
         * 释放没有使用的 widgets
         */
        static void GC();

        /**
         * 开始更新，在循环开始更新调用
         */
        static void beginUpdate();

        /**
         * 结束更新，在循环结束更新调用
         */
        static void endUpdate();

        /**
         * Destructor
         */
        ~ImWidget() override;

        /**
         * 获取 widget 具体类型，避免 RTTI  
         * @return : 返回 widget 具体类型
         * @see : enum class WidgetType
         */
        virtual WidgetType getWidgetType() const = 0;

        /**
         * 获取 widget 名称
         * @return : 返回 widget 名称
         */
        const String &getName() const { return mName; }

        /**
         * 设置 widget 名称
         * @param [in] name : widget 名称
         */
        void setName(const String &name) { mName = name; }

        /**
         * 获取 widget 用户定义的唯一 ID 
         * @return : 返回用户定义的唯一 ID
         */
        uint32_t getID() const { return mID; }

        /**
         * 获取 widget 自动生成的唯一 ID
         * @return : 返回自动生成的唯一 ID
         */
        const UUID &getUUID() const { return mUUID; }

        /**
         * 获取 widget 可见性
         * @return : 返回可见性
         */
        bool isVisible() const { return mVisible; }

        /**
         * 设置 widget 可见性
         * @param [in] visible : 可见性
         */
        void setVisible(bool visible);

        /**
         * 获取 widget 可用性
         * @return : 返回 widget 可用性
         */
        bool isEnable() const { return mEnable; }

        /**
         * 设置 widget 可用性
         * @param [in] enable : 可用性
         */
        void setEnable(bool enable);

        /**
         * 默认、简单的创建接口
         * @param [in] id : 用户设置的唯一 ID
         * @param [in] name : widget 名称 
         * @param [in] parent : 父 widget
         * @return : 调用成功返回 IM_OK
         */
        virtual TResult create(uint32_t id, const String &name, ImWidget *parent);

        /**
         * 销毁 widget，包括自动 delete
         * @return : 调用成功返回 IM_OK
         */
        TResult destroy();

        /**
         * 加入到指定名称的 widget 下。 根据名称递归搜索整个子树
         * @param parentName : 父 widget 的名称
         * @param widget : 要加入的 widget
         * @return : 调用成功返回 IM_OK
         */
        virtual TResult addWidget(const String &parentName, ImWidget *widget);

        /**
         * 加入到指定 UUID 的 widget 下。 根据 UUID 递归搜索整个子树
         * @param [in] parentUUID : 父 widget 的 UUID
         * @param [in] widget : 要加入的 widget 
         * @return : 调用成功返回 IM_OK
         */
        virtual TResult addWidget(const UUID &parentUUID, ImWidget *widget);

        /**
         * 加入到指定 ID 的 widget 下。 根据 ID 递归搜索整个子树
         * @param [in] parentID : 父 widget 的 ID
         * @param [in] widget : 要加入的 widget
         * @return : 调用成功返回 IM_OK
         */
        virtual TResult addWidget(uint32_t parentID, ImWidget *widget);

        /**
         * 移除指定 widget。 递归搜索整个子树
         * @param [in] widget : 指定的 widget
         * @param [in] destroy : 是否销毁要移除的 widget 对象，默认是销毁
         * @return : 调用成功返回 IM_OK
         */
        TResult removeWidget(ImWidget *widget, bool destroy = true);

        /**
         * 移除指定名称 widget。 递归搜索整个子树
         * @param [in] name : 要移除的 widget 名称. 递归层级的需要用过 / 隔开。 如："window/panel/button" 
         * @param [in] destroy : 是否销毁要移除的 widget 对象，默认销毁 
         * @return : 调用成功返回要移除的 widget 对象，否则返回 nullptr
         */
        ImWidget *removeWidget(const String &name, bool destroy = true);

        /**
         * 移除指定 UUID 的 widget。 递归搜索整个子树
         * @param [in] uuid : 要移除的 widget 的 UUID
         * @param [in] destroy : 是否销毁要移除的 widget 对象，默认销毁 
         * @return : 调用成功返回要移除的 widget 对象，否则返回 nullptr
         */
        ImWidget *removeWidget(const UUID &uuid, bool destroy = true);

        /**
         * 移除指定 ID 的 widget。 递归搜索整个子树
         * @param [in] id : 要移除的 widget 的 ID
         * @param [in] destroy : 是否销毁要移除的 widget 对象，默认销毁 
         * @return : 调用成功返回要移除的 widget 对象，否则返回 nullptr
         */
        ImWidget *removeWidget(uint32_t id, bool destroy = true);

        /**
         * 可递归获取多层 widget
         * @param [in] name : 指定名称的 widget，递归层级的需要用过 / 隔开。 如："window/panel/button" 
         * @return : 返回对应的 widget 对象
         */
        ImWidget *getWidget(const String &name) const;

        /**
         * 获取所有子 widget 中对应 uuid 的子 widget。 递归搜索子树
         * @param [in] uuid : 要获取的 widget 的 UUID 
         * @return : 返回对应的 widget 对象
         */
        ImWidget *getWidget(const UUID &uuid) const;

        /**
         * 获取所以子 widget 中对应 id 的子 widget，这个调用可以递归搜索
         * @param [in] id : 要获取的 widget 的 ID
         * @return : 返回对应的 widget 对象
         */
        ImWidget *getWidget(uint32_t id) const;

        /**
         * 获取父 widget
         * @return : 返回父 widget
         */
        ImWidget *getParent() const { return mParent; }

        /**
         * 添加子 widget 。 只添加到本 widget 下，成为本 widget 子 widget
         * @param [in] widget : 要添加的 widget 对象
         * @return : 调用成功返回 IM_OK
         */
        virtual TResult addChild(ImWidget *widget);

        /**
         * 插入到指定名字的 widget 后面，不递归查找，只加到当前 widget 子节点里
         * @param [in] prevName : 前一个 widget 名字
         * @param [in] widget : 新的 widget 对象
         * @return : 调用成功返回 IM_OK
         */
        virtual TResult insertAfterChild(const String &prevName, ImWidget *widget);

        /**
         * 添加子 widget 插入到指定 UUID 后，不递归查找，只加到当前 widget 子节点里
         * @param [in] prevUUID : 前置 widget 的 UUID
         * @param [in] widget : 要添加的 widget 对象 
         * @return : 调用成功返回 IM_OK
         */
        virtual TResult insertAfterChild(const UUID &prevUUID, ImWidget *widget);

        /**
         * 添加子 widget 并插入到指定 widget 后，不递归查找，只加到当前 widget 子节点里
         * @param [in] prevWidget : 前置 widget 对象
         * @param [in] widget : 要添加的 widget 对象 
         * @return : 调用成功返回 IM_OK
         */
        virtual TResult insertAfterChild(ImWidget *prevWidget, ImWidget *widget);

        /**
         * 添加子 widget 并插入到指定 ID 的 widget 后，不递归查找，只加到当前 widget 子节点里
         * @param [in] prevID : 前置 widget 的 ID
         * @param [in] widget : 要添加的 widget 对象 
         * @return : 调用成功返回 IM_OK
         */
        virtual TResult insertAfterChild(uint32_t prevID, ImWidget *widget);

        /**
         * 移除子 widget，不递归查找，只查找当前 widget 符合条件的子节点
         * @param [in] widget : 要移除的 widget 对象 
         * @param [in] destroy : 是否销毁要移除的 widget 对象，默认销毁 
         * @return : 调用成功返回 IM_OK
         */
        TResult removeChild(ImWidget *widget, bool destroy = true);

        /**
         * 移除子 widget，不递归查找，只查找当前 widget 符合条件的子节点
         * @param [in] name : 要移除的 widget 的名称
         * @param [in] destroy : 是否销毁要移除的 widget 对象，默认销毁
         * @return : 调用成功返回被移除的 widget 对象，否则返回 nullptr
         */
        ImWidget *removeChild(const String &name, bool destroy = true);

        /**
         * 移除子 widget，不递归查找，只查找当前 widget 符合条件的子节点
         * @param [in] uuid : 要移除的 widget 的 UUID
         * @param [in] destroy : 是否销毁要移除的 widget 对象，默认销毁
         * @return : 调用成功返回被移除的 widget 对象，否则返回 nullptr
         */
        ImWidget *removeChild(const UUID &uuid, bool destroy = true);

        /**
         * 移除子 widget，不递归查找，只查找当前 widget 符合条件的子节点
         * @param [in] id : 要移除的 widget 的 ID
         * @param [in] destroy : 是否销毁要移除的 widget 对象，默认销毁
         * @return : 调用成功返回被移除的 widget 对象，否则返回 nullptr
         */
        ImWidget *removeChild(uint32_t id, bool destroy = true);

        /**
         * 移除所有子 widget
         * @return : 调用成功返回 IM_OK
         */
        TResult removeAllChildren();
        
        /**
         * 只获取第一层子 widget 指定名称的 widget
         * @param [in] name : 子 widget 名称
         * @return 返回对应的 widget 对象
         */
        ImWidget *getChild(const String &name) const;

        /**
         * 只获取第一层子 widget 对应 uuid 的 widget
         * @param [in] uuid : 子 widget 的 uuid
         * @return : 返回对应的 widget 对象
         */
        ImWidget *getChild(const UUID &uuid) const;

        /**
         * 只获取第一层子 widget 对应 id 的 widget
         * @param [in] id : 要获取的 widget 的 ID
         * @return : 返回对应的 widget 对象
         */
        ImWidget *getChild(uint32_t id) const;

        /**
         * 获取前一个兄弟节点
         * @return 返回前一个兄弟节点，如果自己是第一个节点，则返回 nullptr
         */
        ImWidget *getPrevSibling() const;

        /**
         * 获取后一个兄弟节点
         * @return 返回后一个兄弟节点，如果自己是最后一个节点，则返回 nullptr
         */
        ImWidget *getNextSibling() const;

        /**
         * 获取下一级的所有子 widgets
         * @return : 返回下一级所有子 widgets
         */
        const Children &getChildren() const { return mChildren; }

        /**
         * 获取 widget 大小
         * @return : 返回 widget 的大小
         */
        const ImVec2 &getSize() const { return mSize; }

        /**
         * 设置 widget 大小
         * @param [in] w : 宽度
         * @param [in] h : 高度
         */
        void setSize(float w, float h) { mSize.x = w; mSize.y = h;}

        /**
         * 设置 widget 大小
         * @param [in] size : 宽高大小
         */
        void setSize(const ImVec2 &size) { mSize = size; }

        /**
         * 更新，用于更新自己及其子 widget
         */
        virtual void update();

        /**
         * 更新，用于更新自己及其子 widget
         * @param size : 父 widget 的大小
         */
        virtual void update(const ImVec2 &size);

        /**
         * 某些类型子类需要获取大小的回调
         * @return : 返回 widget 大小
         */
        virtual const ImVec2 &onGetSize() const { return getSize(); }

        /**
         * 某些类型子类需要获取可见性的回调
         * @return : 返回 widget 可见性
         */
        virtual bool *onGetVisible();

        /**
         * 设置在 update 函数指定帧输出层级结构信息
         * @param [in] enable : 打开还是关闭
         * @param [in] frame : 输出日志的帧数，当为 -1 的时候，则每帧输出
         */
        void setDebugHierarchy(bool enable, uint32_t frame = 0);

        /**
         * 前序遍历 widget 树
         * @param [in] callback : 访问到每个 widget 时的回调
         * @param [in] depth : 深度值，用于一些格式化打印，一般使用默认值即可
         */
        void traverseHierarchyPreOrder(const TraverseHierarchyCallback &callback, int32_t depth = 0);

        /**
         * 后序遍历 widget 树
         * @param [in] callback : 访问到每个 widget 时的回调
         * @param [in] depth : 深度值，用于一些格式化打印，一般使用默认值即可
         */
        void traverseHierarchyPostOrder(const TraverseHierarchyCallback &callback, int32_t depth = 0);
        
    protected:
        ImWidget() = default;

        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, ...);
        
        virtual TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args);
        
        virtual TResult onCreate();
        
        virtual bool onGUIBegin();

        virtual bool onGUIBegin(const ImVec2 &size);

        virtual void onGUI();

        virtual void onGUIEnd();

        virtual void onDestroy();

        virtual void onVisible();

        virtual void onInvisible();

        virtual void onEnable();

        virtual void onDisable();

        virtual void onChildAdded(ImWidget *widget);

        virtual void onChildRemoved(ImWidget *widget);

        ImWidget *getWidgetRecursively(StringList &names) const;

        void PushWidgetID() { ImGui::PushID(this); }

        void PopWidgetID() { ImGui::PopID(); }

    private:
        void destroyWidget(ImWidget *child);

        struct RemoveWidgetInfo
        {
            Children::iterator itr;
            bool destroy {false};
        };

        void removeChild(Children::iterator itr, bool destroy);

        using WaitingRemoveWidgets = TMultimap<ImWidget*, RemoveWidgetInfo>;
        
        /// 所有待从父节点移除的 widgets
        static WaitingRemoveWidgets msWaitingRemoveWidgets;

        using WaitingDestroyWidgets = TList<ImWidget*>;
        
        /// 所有待删除的 widgets
        static WaitingDestroyWidgets msWaitingDestroyWidgets;

        /// 是否在 update 中
        static bool msInUpdate;
        
        /// UUID
        UUID        mUUID {UUID::INVALID};
        /// User-defined ID
        uint32_t    mID {T3D_INVALID_ID};
        /// Name
        String      mName {};
        /// Parent
        ImWidget    *mParent {nullptr};
        /// Children
        Children    mChildren {};

        /// 自己在子节点的迭代器，也就是位置
        Children::iterator  mSelfItr {};
        
        /// 是否开启调试，调试模式下，会在 update 输出层级关系日志
        bool        mDebugEnabled {false};
        /// 当前帧数
        uint32_t    mFrameCount {0};
        /// 调试输出的帧编号，-1 表示每帧都输出
        uint32_t    mDebugFrame {0};
        
    protected:
        /// 大小
        ImVec2  mSize {0, 0};
        /// 可见性
        bool    mVisible {true};
        /// 可用性
        bool    mEnable {true};
    };
}
