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

    class UIHierarchyToolBar : public ImChildView
    {
    public:

    protected:
        TResult onCreate() override;
        
        void update(const ImVec2 &size) override;
        void update() override;

        void updateChildren();
    };

    class UIHierarchyView
        : public ImChildView
        , public EventHandler
        , public ImMenuEventHandler
    {
    public:
        void setScene(Scene *scene)
        {
            mScene = scene;
        }

        void refresh()
        {
            populateGameObjectTree();
        }
        
    protected:
        TResult onCreate() override;

        void onDestroy() override;
        
        bool onGUIBegin(const ImVec2 &size) override;
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;

        TResult populateGameObjectTree();

        void treeNodeClicked(ImTreeNode *node);
        void treeNodeRClicked(ImTreeNode *node);
        void onTreeNodeDestroy(ImTreeNode *node);

        /// 响应处理场景数据被改动通知
        bool onModifedScene(EventParam *param, TINSTANCE sender);

        /// 查询创建空 game object 菜单状态
        bool onMenuItemEnabledCreateEmpty(uint32_t id, ImWidget *menuItem);
        /// 创建空 game object 菜单响应
        bool onMenuItemCreateEmpty(uint32_t id, ImWidget *menuItem);
        
        /// 查询创建 cube 菜单状态
        bool onMenuItemEnabledCreateCube(uint32_t id, ImWidget *menuItem);
        /// 创建 cube 菜单响应
        bool onMenuItemCreateCube(uint32_t id, ImWidget *menuItem);
        
        bool onMenuItemCreateSphere(uint32_t id, ImWidget *menuItem);

        TResult createTreeNode(TransformNode *node, const ImTreeNode::CallbackData &callbacks, const ImTreeNodeDestroyCallback &onDestroy);

        TResult createCube(GameObject *go);
        void createCubeAABB(Mesh *mesh, SubMesh *submesh, AabbBound *bound);
        
    protected:
        Scene *mScene {nullptr};
        ImTreeWidget *mTreeWidget {nullptr};
        ImTreeNode *mRoot {nullptr};
    };

    class UIHierarchyWindow : public UIDockingWindow, public EventHandler
    {
    public:
        UIHierarchyWindow() = default;
        ~UIHierarchyWindow() override = default;

    protected:
        TResult onCreate() override;

        void onDestroy() override;

        bool onOpenScene(EventParam *param, TINSTANCE sender);

    protected:
        UIHierarchyView *mHierarchyView {nullptr};
    };

    NS_END
}
