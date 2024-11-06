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


#include "UIDockingWindow.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    class UIAssetHierarchyView
        : public ImChildView
        , public EventHandler
    {
    public:
        const ImTreeBar::TreeBarNodes &getTreeBarRoots() const { return mRoots; }

        // 重新构建资产树
        TResult rebuild();
        
    protected:
        TResult onCreate() override;

        void onDestroy() override;
        
        void onGUI() override;

        ImGuiChildFlags onGetChildFlags() override;

        TResult populateFavoritesTree(ImTreeWidget *tree, const ImTreeNode::CallbackData &callbacks, const ImTreeNodeDestroyCallback &onDestroy);

        TResult populateAssetsTree(ImTreeWidget *tree, const ImTreeNode::CallbackData &callbacks, const ImTreeNodeDestroyCallback &onDestroy);

        TResult populateAssetsTree(ImTreeWidget *tree, ImTreeNode *uiParent, AssetNode *node, const ImTreeNode::CallbackData &callbacks, const ImTreeNodeDestroyCallback &onDestroy, ImTreeNode *&uiNode);

        void treeNodeClicked(ImTreeNode *node);

        void treeNodeRClicked(ImTreeNode *node);

        void onTreeNodeDestroy(ImTreeNode *node);
        
    protected:
        ImTreeBar::TreeBarNodes mRoots {};

        ImContextMenu *mContextMenu {nullptr};
        ImTreeWidget *mTreeWidget {nullptr};
        ImTreeNode *mAssetsRoot {nullptr};
    };

    class UIAssetPathBar
        : public ImChildView
        , public EventHandler
    {
    public:        
        ~UIAssetPathBar() override = default;

        using ImChildView::create;

        TResult create(uint32_t id, const String &name, ImWidget *parent, const ImTreeBar::TreeBarNodes &roots);

        TResult rebuild();
        
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;

        TResult onCreate() override;

        bool onGUIBegin() override;
        bool onGUIBegin(const ImVec2 &size) override;
        void onGUIEnd() override;

        void onDestroy() override;
        
        ImGuiChildFlags onGetChildFlags() override;

        bool onClickedHierarchyNode(EventParam *param, TINSTANCE sender);
        
    protected:
        ImTreeBar   *mTreeBar {nullptr};
    };
    
    class UIAssetDetailView
        : public ImChildView
        , public EventHandler
    {
    public:
        TResult rebuild();
        
    protected:
        TResult onCreate() override;

        void onDestroy() override;
        
        bool onClickedHierarchyNode(EventParam *param, TINSTANCE sender);

        TResult populateItems(AssetNode *parent);

        TResult createListItem(AssetNode *node);

        ImListWidget *mListWidget {nullptr};
    };

    class UIAssetStatusBar : public ImChildView
    {
    protected:
        ImGuiChildFlags onGetChildFlags() override;
    };

    
    class UIAssetThumbView : public ImChildView
    {
    public:
        using ImChildView::create;
        
        TResult create(uint32_t id, const String &name, ImWidget *parent, const ImTreeBar::TreeBarNodes &roots);

        TResult rebuild();
        
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args) override;

        void onDestroy() override;
        
        bool onGUIBegin() override;
        bool onGUIBegin(const ImVec2 &size) override;
        void onGUIEnd() override;
        
        ImGuiChildFlags onGetChildFlags() override;

    protected:
        UIAssetPathBar *mPathBar {nullptr};
        UIAssetDetailView *mDetailView {nullptr};
        UIAssetStatusBar *mStatusBar {nullptr};
    };
    
    class UIProjectWindow : public UIDockingWindow, public EventHandler
    {
    public:
        UIProjectWindow() = default;
        ~UIProjectWindow() override = default;

    protected:
        TResult onCreate() override;

        void onDestroy() override;
        
        void onGUI() override;

        bool onApplicationWillEnterForeground(EventParam *param, TINSTANCE sender);

        bool onApplicationFocusGained(EventParam *param, TINSTANCE sender);
        
    protected:
        ImSplitView *mSplitView {nullptr};

        UIAssetHierarchyView    *mHierarchyView {nullptr};
        UIAssetThumbView        *mThumbView {nullptr};
    };

    NS_END
}
