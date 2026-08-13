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

        /// 拆掉整棵 UI 树并解开与 TransformNode 的双向绑定
        void clearTree();

        /// 按 GameObject UUID 记下当前展开 / 选中，树为空时保留已有快照
        void captureViewState();

        /// 清空后按当前场景重建 Hierarchy，并还原展开 / 选中
        void refresh();

        /// 重新计算整棵树的 prefab 实例标识
        void refreshPrefabDecoration();
        
    protected:
        TResult onCreate() override;

        void onDestroy() override;
        
        bool onGUIBegin(const ImVec2 &size) override;
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;

        TResult populateGameObjectTree();

        void captureViewStateRecursive(ImWidget *widget);
        void restoreViewState();
        void restoreViewStateRecursive(ImWidget *widget);

        GameObject *gameObjectFromTreeNode(ImTreeNode *uiNode) const;

        void treeNodeClicked(ImTreeNode *node);
        void treeNodeRClicked(ImTreeNode *node);
        void onTreeNodeDestroy(ImTreeNode *node);

        /// 广播选中的 game object，node 为空或为场景根时广播取消选中
        void postSelectionChanged(ImTreeNode *node);

        /// 响应处理场景数据被改动通知
        bool onModifedScene(EventParam *param, TINSTANCE sender);

        /// 响应处理 prefab 实例关系变化通知
        bool onPrefabInstanceChanged(EventParam *param, TINSTANCE sender);

        /// 属于 prefab 实例的节点用 prefab 文本色显示，否则恢复默认色
        void updatePrefabDecoration(TransformNode *node, ImTreeNode *uiNode);

        /// 查询创建空 game object 菜单状态
        bool onMenuItemEnabledCreateEmpty(uint32_t id, ImWidget *menuItem);
        /// 创建空 game object 菜单响应
        bool onMenuItemCreateEmpty(uint32_t id, ImWidget *menuItem);
        
        /// 查询创建 cube 菜单状态
        bool onMenuItemEnabledCreateCube(uint32_t id, ImWidget *menuItem);
        /// 创建 cube 菜单响应
        bool onMenuItemCreateCube(uint32_t id, ImWidget *menuItem);

        /// 查询创建 sphere 菜单状态
        bool onMenuItemEnabledCreateSphere(uint32_t id, ImWidget *menuItem);
        /// 创建 sphere 菜单响应
        bool onMenuItemCreateSphere(uint32_t id, ImWidget *menuItem);

        /// 查询创建 capsule 菜单状态
        bool onMenuItemEnabledCreateCapsule(uint32_t id, ImWidget *menuItem);
        /// 创建 capsule 菜单响应
        bool onMenuItemCreateCapsule(uint32_t id, ImWidget *menuItem);

        /// 查询创建 cylinder 菜单状态
        bool onMenuItemEnabledCreateCylinder(uint32_t id, ImWidget *menuItem);
        /// 创建 cylinder 菜单响应
        bool onMenuItemCreateCylinder(uint32_t id, ImWidget *menuItem);

        /// 查询创建 quad 菜单状态
        bool onMenuItemEnabledCreateQuad(uint32_t id, ImWidget *menuItem);
        /// 创建 quad 菜单响应
        bool onMenuItemCreateQuad(uint32_t id, ImWidget *menuItem);

        /// 查询删除 game object 菜单状态
        bool onMenuItemEnabledDelete(uint32_t id, ImWidget *menuItem);
        /// 删除 game object 菜单响应
        bool onMenuItemDelete(uint32_t id, ImWidget *menuItem);

        TResult createTreeNode(TransformNode *node, const ImTreeNode::CallbackData &callbacks, const ImTreeNodeDestroyCallback &onDestroy);

        TResult createCube(GameObject *go);
        TResult createSphere(GameObject *go);
        TResult createCapsule(GameObject *go);
        TResult createCylinder(GameObject *go);
        TResult createQuad(GameObject *go);

        /// 按 mesh 里的包围体种子创建 Bound 组件，种子缺失时回退到遍历顶点计算 AABB
        TResult createBound(GameObject *go, Geometry *geometry, Mesh *mesh, SubMesh *submesh);
        /// 回退路径：遍历 submesh 索引引用到的顶点位置，实时计算 AABB
        TResult buildAabbFromMeshData(Mesh *mesh, SubMesh *submesh, AabbBound *bound);
        
    protected:
        Scene *mScene {nullptr};
        ImTreeWidget *mTreeWidget {nullptr};
        ImTreeNode *mRoot {nullptr};

        using ExpandStateMap = TUnorderedMap<UUID, bool, UUIDHash, UUIDEqual>;
        ExpandStateMap mSavedExpandState {};
        UUID mSavedSelectionUUID {UUID::INVALID};
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
