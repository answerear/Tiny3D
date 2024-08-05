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
    class TINYIMGUI_API ImTreeNode : public ImWidget
    {
    public:
        ImTreeNode(ImTreeWidget *tree);
        
        ~ImTreeNode() override;

        WidgetType getWidgetType() const override { return WidgetType::kTreeNode; }

        TResult createByTexture(ImTextureID texID, const String &title, uint32_t clickedEvtID, ImTreeNode *parent);
        
        TResult createByTexture(ImTextureID texID, const String &title, const ImTreeNodeClickedCallback &clicked, ImTreeNode *parent);

        TResult createByPath(const String &imageName, const String &title, uint32_t clickedEvtID, ImTreeNode *parent);
        
        TResult createByPath(const String &imageName, const String &title, const ImTreeNodeClickedCallback &clicked, ImTreeNode *parent);

        TResult createByTexture(ImTextureID collapsedTexID, ImTextureID openedTexID, const String &title, uint32_t clickedEvtID, ImTreeNode *parent);
        
        TResult createByTexture(ImTextureID collapsedTexID, ImTextureID openedTexID, const String &title, const ImTreeNodeClickedCallback &clicked, ImTreeNode *parent);
        
        TResult createByPath(const String &collapsedImage, const String &openedImage, const String &title, uint32_t clickedEvtID, ImTreeNode *parent);
        
        TResult createByPath(const String &collapsedImage, const String &openedImage, const String &title, const ImTreeNodeClickedCallback &clicked, ImTreeNode *parent);

        TResult addNode(ImTreeNode *node);

        ImTreeWidget *getTreeWidget() const { return mTreeWidget; }

        TResult collapse(bool recursively);

        TResult expand(bool recursively);
        
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

    private:
        TResult addWidget(const String &parentName, ImWidget *widget) override;

        TResult addWidget(const UUID &parentUUID, ImWidget *widget) override;

        TResult addWidget(uint32_t parentID, ImWidget *widget) override;
        
        TResult addChild(ImWidget *widget) override;

        TResult insertAfterChild(const String &prevName, ImWidget *widget) override;

        TResult insertAfterChild(const UUID &prevUUID, ImWidget *widget) override;

        TResult insertAfterChild(ImWidget *prevWidget, ImWidget *widget) override;

        TResult insertAfterChild(uint32_t prevID, ImWidget *widget) override;

    protected:
        /// tree 容器对象
        ImTreeWidget *mTreeWidget {nullptr};
        /// 图标对象
        ImTextureID mIconID {nullptr};
        /// 展开状态的图标对象
        ImTextureID mOpenedIconID {nullptr};
        /// TreeWidget 的大小
        ImVec2 mTreeWidgetSize {};
        /// 单击发送的事件 ID
        uint32_t mClickedEvtID {0};
        /// 单击回调
        ImTreeNodeClickedCallback mClickedCallback {nullptr};
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
