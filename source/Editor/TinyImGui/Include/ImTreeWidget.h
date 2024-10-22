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
#include "ImEventDefine.h"


namespace Tiny3D
{
    /**
     * 树形控件节点
     */
    class TINYIMGUI_API ImTreeNode : public ImWidget
    {
    public:
        struct CallbackData
        {
            CallbackData() = default;
            
            CallbackData(const ImTreeNodeClickedCallback &clk, const ImTreeNodeRClickedCallback &rclk)
                : clicked(clk), rclicked(rclk)
            {}

            CallbackData(ImTreeNodeClickedCallback &&clk, ImTreeNodeRClickedCallback &&rclk)
                : clicked(std::move(clk)), rclicked(std::move(rclk))
            {}
            
            ImTreeNodeClickedCallback clicked {nullptr};
            ImTreeNodeRClickedCallback rclicked {nullptr};
        };
        
        ImTreeNode(ImTreeWidget *tree);
        
        ~ImTreeNode() override;

        WidgetType getWidgetType() const override { return WidgetType::kTreeNode; }

        TResult createByTexture(ImTextureID texID, const String &title, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy = nullptr);
        
        TResult createByTexture(ImTextureID texID, const String &title, const CallbackData &callbacks, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy = nullptr);

        TResult createByPath(const String &imageName, const String &title, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy = nullptr);
        
        TResult createByPath(const String &imageName, const String &title, const CallbackData &callbacks, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy = nullptr);

        TResult createByTexture(ImTextureID collapsedTexID, ImTextureID openedTexID, const String &title, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy = nullptr);
        
        TResult createByTexture(ImTextureID collapsedTexID, ImTextureID openedTexID, const String &title, const CallbackData &callbacks, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy = nullptr);
        
        TResult createByPath(const String &collapsedImage, const String &openedImage, const String &title, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy = nullptr);
        
        TResult createByPath(const String &collapsedImage, const String &openedImage, const String &title, const CallbackData &callbacks, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy = nullptr);

        TResult addNode(ImTreeNode *node);

        ImTreeWidget *getTreeWidget() const { return mTreeWidget; }

        /**
         * 折叠节点
         * @param [in] recursively : 是否递归折叠每一级子节点 
         * @return 调用成功返回 IM_OK
         */
        TResult collapse(bool recursively);

        /**
         * 展开节点
         * @param [in] recursively : 是否递归展开每一级子节点 
         * @return 调用成功返回 IM_OK
         */
        TResult expand(bool recursively);

        /**
         * 设置用户数据，这个指针所指内存，用户自己负责生命周期
         * @param [in] data : 用户数据
         */
        void setUserData(void *data) { mUserData = data; }

        /**
         * 获取用户数据
         * @return 返回用户数据首地址
         */
        void *getUserData() const { return mUserData; }
        
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
        bool onGUIBegin(const ImVec2 &size) override;
        void onGUI() override;
        void onGUIEnd() override;

        void onDestroy() override;

        String getUniqueName() const;

        uint32_t getNewNodeID() const;

        void drawIconAndText(bool opened);

        void fireClickedEvent();

        void fireRClickedEvent();

    private:
        bool addWidget(const String &parentName, ImWidget *widget) override;

        bool addWidget(const UUID &parentUUID, ImWidget *widget) override;

        bool addWidget(uint32_t parentID, ImWidget *widget) override;
        
        bool addChild(ImWidget *widget) override;

        bool insertAfterChild(const String &prevName, ImWidget *widget) override;

        bool insertAfterChild(const UUID &prevUUID, ImWidget *widget) override;

        bool insertAfterChild(ImWidget *prevWidget, ImWidget *widget) override;

        bool insertAfterChild(uint32_t prevID, ImWidget *widget) override;

    protected:
        /// tree 容器对象
        ImTreeWidget *mTreeWidget {nullptr};
        /// 图标对象
        ImTextureID mIconID {nullptr};
        /// 展开状态的图标对象
        ImTextureID mOpenedIconID {nullptr};
        /// 用户自定义数据对象
        void *mUserData {nullptr};
        /// TreeWidget 的大小
        ImVec2 mTreeWidgetSize {};
        /// 单击回调
        ImTreeNodeClickedCallback mClickedCallback {nullptr};
        /// 右键单击回调
        ImTreeNodeRClickedCallback mRClickedCallback {nullptr};
        /// 删除回调
        ImTreeNodeDestroyCallback mDestroyCallback {nullptr};
        /// 是否内部加载图标
        bool mIsInternalLoaded {false};
        /// 是否展开
        bool mIsExpanded {false};
        /// 是否更新展开、收起状态
        bool mIsUpdateExpanding {false};
    };

    class TINYIMGUI_API ImTreeWidget
        : public ImTreeNode
        , public EventHandler
    {
        friend class ImTreeNode;
        
    public:
        ImTreeWidget();
        
        ~ImTreeWidget() override;

        WidgetType getWidgetType() const override { return WidgetType::kTreeWidget; }

        TResult create(uint32_t id, const String &name, ImWidget *parent = nullptr) override;

        void update() override;
        
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
        bool onGUIBegin() override;
        bool onGUIBegin(const ImVec2 &size) override;
        void onGUI() override;
        void onGUIEnd() override;

        String &getSelectedNode() { return mSelectedNode; }

        const ImVec2 &getContentPos() const { return mContentPos; }

        uint32_t generateNodeID() { return mNodeID++; }

        void sendClickedEvent(uint32_t evt, EventParamTreeNodeClicked *param);
        
    protected:
        uint32_t mNodeID {0};
        String  mSelectedNode {};
        ImVec2  mContentPos {};
    };

    class TINYIMGUI_API ImDummyTreeNode : public ImTreeNode
    {
    public:
        ImDummyTreeNode(ImTreeWidget *tree);

        ~ImDummyTreeNode() override;

        TResult create(ImTreeNode *parent = nullptr);

    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
        bool onGUIBegin() override;
        bool onGUIBegin(const ImVec2 &size) override;
        void onGUI() override;
        void onGUIEnd() override;
    };
}
