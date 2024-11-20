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


#include "UIHierarchyWindow.h"

#include "EditorEventDefine.h"
#include "EditorSceneImpl.h"
#include "GUIExtension/ImGuiExtension.h"
#include "UIEditorWidgetID.h"
#include "ProjectManager.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    TResult UIHierarchyToolBar::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            const float kToolBarHeight = 20.0f;
            // Create game object button
            auto clickedAdd = [](ImWidget *button)
            {
                
            };

            ImCreateButton *btnAdd = new ImCreateButton();
            ret= btnAdd->create(ID_HIERARCHY_ADD_BUTTON, ImVec2(40, kToolBarHeight), nullptr,  clickedAdd, this);
            if (T3D_FAILED(ret))
            {
                T3D_SAFE_DELETE(btnAdd);
                EDITOR_LOG_ERROR("Failed to create adding button !");
                ret = T3D_ERR_FAIL;
                break;
            }

            // Search input text
            auto inputTextCallback = [](ImInputText *inputText, const String &text)
            {
                EDITOR_LOG_DEBUG("Input text : %s", text.c_str());
                return 0;
            };
            
            ImSearchInputText *inputText = new ImSearchInputText();
            ret = inputText->create(ID_HIERARCHY_SEARCH_INPUT, ImVec2(200, kToolBarHeight), 1024, inputTextCallback,  true, this);
            if (T3D_FAILED(ret))
            {
                T3D_SAFE_DELETE(inputText);
                EDITOR_LOG_ERROR("Failed to create input text !");
                ret = T3D_ERR_FAIL;
                break;
            }

            // Search jump button
            auto clickedJump = [](ImWidget *button)
            {
                
            };
            
            ImImageButton *btnSearch = new ImImageButton();
            ret = btnSearch->createByPath(ID_HIERARCHY_SEARCH_JUMP_BUTTON, ICON_NAME_SEARCH_JUMP, nullptr, clickedJump, this);
            if (T3D_FAILED(ret))
            {
                T3D_SAFE_DELETE(btnSearch);
                EDITOR_LOG_ERROR("Failed to create search jump button !");
                ret = T3D_ERR_FAIL;
                break;
            }
            btnSearch->setSize(14, 14);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void UIHierarchyToolBar::update(const ImVec2 &size)
    {
        if (mVisible && onGUIBegin(size))
        {
            onGUI();

            updateChildren();
            
            onGUIEnd();
        }
    }

    //--------------------------------------------------------------------------

    void UIHierarchyToolBar::update()
    {
        if (mVisible && onGUIBegin())
        {
            onGUI();

            updateChildren();
            
            onGUIEnd();
        }
    }

    //--------------------------------------------------------------------------

    void UIHierarchyToolBar::updateChildren()
    {
        T3D_ASSERT(mChildren.size() == 3);

        auto region = ImGui::GetContentRegionAvail();
        ImVec2 startPos = ImGui::GetCursorScreenPos();
        auto itr = mChildren.begin();

        // Add game object button
        auto child1 = *itr;
        child1->update();
        const ImVec2 &size1 = child1->getSize();
        ++itr;

        auto child2 = *itr;
        ImSearchInputText *inputText = static_cast<ImSearchInputText *>(child2);
        const ImVec2 &size2 = inputText->getOriginalSize();
        ++itr;
        
        auto child3 = *itr;
        const ImVec2 &size3 = child3->getSize();

        // 动态计算 input text 的宽度，还要保留原有的大小，后面还原回去
        const ImGuiStyle &style = ImGui::GetStyle();
        float offset = region.x - size2.x - size3.x - 3 * style.ItemSpacing.x;
        bool isSizeAdjusted = false;
        if (region.x <= size1.x + size2.x + size3.x + 3 * style.ItemSpacing.x)
        {
            // 按照原定大小，tool bar 放不下，需要重新调整宽度
            isSizeAdjusted = true;
            ImVec2 newSize = size2;
            newSize.x = region.x - size1.x - size3.x - 3 * style.ItemSpacing.x;
            child2->setSize(newSize);
            // offset = 0;
        }
        
        // ImGui::SameLine(offset, style.ItemSpacing.x);
        ImVec2 pos = ImGui::GetCursorScreenPos();
        pos.x = startPos.x + (region.x - child2->getSize().x - size3.x - 2 * style.ItemSpacing.x);
        pos.y = startPos.y;
        ImGui::SetCursorScreenPos(pos);

        // Search input text
        child2->update();
        
        if (isSizeAdjusted)
        {
            child2->setSize(size2);
        }

        // offset = region.x - size3.x - 2 * style.ItemSpacing.x;
        // ImGui::SameLine(offset, style.ItemSpacing.x);

        pos.x = startPos.x + (region.x - size3.x - style.ItemSpacing.x);
        pos.y = startPos.y;
        ImGui::SetCursorScreenPos(pos);

        // Search jump button
        child3->update();
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyView::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            ON_MENU_ITEM_MEMBER(ID_MENU_ITEM_CREATE_EMPTY, UIHierarchyView::onMenuItemCreateEmpty);
            ON_MENU_ITEM_MEMBER(ID_MENU_ITEM_CREATE_CUBE, UIHierarchyView::onMenuItemCreateCube);
            
            T3D_ASSERT(mTreeWidget == nullptr);
            mTreeWidget = new ImTreeWidget();
            ret = mTreeWidget->create(ID_HIERARCHY_VIEW_TREE, "GameObject Hierarchy Tree", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create game object hierarchy tree ! ERROR [%d]", ret);
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::onDestroy()
    {
        unregisterAllMenuEvents();
        unregisterAllEvent();
        mTreeWidget = nullptr;
        ImChildView::onDestroy();
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyView::populateGameObjectTree()
    {
        TResult ret = T3D_OK;

        do
        {
            const auto treeNodeClicked = std::bind(&UIHierarchyView::treeNodeClicked, this, std::placeholders::_1);
            const auto treeNodeRClicked = std::bind(&UIHierarchyView::treeNodeRClicked, this, std::placeholders::_1);
            const auto treeNodeDestroy = std::bind(&UIHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);
            
            Transform3D *root = mScene->getRootTransform();
            root->dfs_visit(true, nullptr,
                [this, &ret](TransformNode *node, const ImTreeNode::CallbackData &callbacks, const ImTreeNodeDestroyCallback &onDestroy)
                {
                    if (T3D_FAILED(ret))
                        return;

                    ret = createTreeNode(node, callbacks, onDestroy);
                },
                callbacks, treeNodeDestroy);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::treeNodeClicked(ImTreeNode *node)
    {
        
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::treeNodeRClicked(ImTreeNode *node)
    {
        
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::onTreeNodeDestroy(ImTreeNode *uiNode)
    {
        TransformNode *node = static_cast<TransformNode *>(uiNode->getUserData());
        uiNode->setUserData(nullptr);
        node->setUserData(nullptr);
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyView::createTreeNode(TransformNode *node, const ImTreeNode::CallbackData &callbacks, const ImTreeNodeDestroyCallback &onDestroy)
    {
        TResult ret = T3D_OK;

        do
        {
            ImTreeNode *uiParent = nullptr;
            String name;
            String icon;
            
            if (!EDITOR_SCENE.isSceneRoot(node))
            {
                Transform3D *parent = static_cast<Transform3D *>(node->getParent());
                uiParent = static_cast<ImTreeNode *>(parent->getUserData());
                name = node->getGameObject()->getName();
                icon = ICON_NAME_GAMEOBJECT;
            }
            else
            {
                uiParent = mTreeWidget;
                name = mScene->getName();
                icon = ICON_NAME_SCENE_32;
            }

            ImTreeNode *uiNode = new ImTreeNode(mTreeWidget);
            ret = uiNode->createByPath(icon, icon, name, callbacks, uiParent, onDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create ui node [%s] ! ERROR [%d]", name.c_str(), ret);
                break;
            }

            node->setUserData(uiNode);
            uiNode->setUserData(node);

            if (mRoot == nullptr)
            {
                mRoot = uiNode;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemCreateEmpty(uint32_t id, ImWidget *menuItem)
    {
        if (mTreeWidget != nullptr)
        {
            ImTreeNode *selection = mTreeWidget->getSelection();
            if (selection != nullptr)
            {
                GameObjectPtr go = GameObject::create("GameObject");

                if (go != nullptr)
                {
                    TransformNode *parent = static_cast<TransformNode*>(selection->getUserData());
                    
                    Transform3DPtr node = go->addComponent<Transform3D>();
                    
                    if (node != nullptr)
                    {
                        parent->addChild(node);
                        
                        const auto treeNodeClicked = std::bind(&UIHierarchyView::treeNodeClicked, this, std::placeholders::_1);
                        const auto treeNodeRClicked = std::bind(&UIHierarchyView::treeNodeRClicked, this, std::placeholders::_1);
                        const auto treeNodeDestroy = std::bind(&UIHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

                        ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);
                    
                        createTreeNode(node, callbacks, treeNodeDestroy);

                        ImTreeNode *uiNode = static_cast<ImTreeNode*>(parent->getUserData());
                        T3D_ASSERT(uiNode != nullptr);
                        uiNode->expand(false);
                    }
                }
            }
        }
        
        return true;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemCreateCube(uint32_t id, ImWidget *menuItem)
    {
        do
        {
            if (mTreeWidget == nullptr)
            {
                EDITOR_LOG_WARNING("Tree widget has not created !");
                break;
            }

            ImTreeNode *selection = mTreeWidget->getSelection();

            if (selection == nullptr)
            {
                EDITOR_LOG_WARNING("There was no selection !");
                break;
            }

            TransformNode *parent = static_cast<TransformNode*>(selection->getUserData());
            if (parent == nullptr)
            {
                EDITOR_LOG_WARNING("The parent of selection is nullptr !");
                break;
            }

            // transform node for cube
            GameObjectPtr go = GameObject::create("Cube");
            Transform3DPtr node = go->addComponent<Transform3D>();

            if (node == nullptr)
            {
                EDITOR_LOG_WARNING("Failed to add Transform3D component !");
                break;
            }
            
            parent->addChild(node);

            TResult ret = createCube(go);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to create cube ! ERROR [%d]", ret);
                break;
            }

            const auto treeNodeClicked = std::bind(&UIHierarchyView::treeNodeClicked, this, std::placeholders::_1);
            const auto treeNodeRClicked = std::bind(&UIHierarchyView::treeNodeRClicked, this, std::placeholders::_1);
            const auto treeNodeDestroy = std::bind(&UIHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);

            createTreeNode(node, callbacks, treeNodeDestroy);

            ImTreeNode *uiNode = static_cast<ImTreeNode*>(parent->getUserData());
            T3D_ASSERT(uiNode != nullptr);
            uiNode->expand(false);
        } while (false);

        return true;
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyView::createCube(GameObject *go)
    {
        TResult ret = T3D_OK;

        do
        {
            // geometry component
            GeometryPtr geometry = go->addComponent<Geometry>();

            const String &path = PROJECT_MGR.getBuiltinPath();
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kRead);
            T3D_ASSERT(archive != nullptr);
            MeshPtr mesh = T3D_MESH_MGR.loadMesh(archive, ProjectManager::BUILTIN_CUBE_MESH_NAME);
            if (mesh == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load cube mesh assets !");
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            StringArray enableKeywrods;
            enableKeywrods.push_back("");
            StringArray disableKeywords;
            for (auto submesh : mesh->getSubMeshes())
            {
                ret = submesh.second->getMaterial()->switchKeywords(enableKeywrods, disableKeywords);
                if (T3D_FAILED(ret))
                {
                    EDITOR_LOG_ERROR("Failed to switch keywords (submesh : %s) ! ERROR [%d]", submesh.second->getName().c_str(), ret);
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to switch keywords ! ERROR [%d]", ret);
                break;
            }
            
            SubMesh *submesh = mesh->getSubMesh(ProjectManager::BUILTIN_CUBE_SUBMESH_NAME);
            geometry->setMeshObject(mesh, submesh);

            // aabb bound component
            AabbBoundPtr bound = go->addComponent<AabbBound>();
            createCubeAABB(mesh, submesh, bound);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::createCubeAABB(Mesh *mesh, SubMesh *submesh, AabbBound *bound)
    {
        const VertexAttribute *attr = mesh->findVertexAttributeBySemantic(VertexAttribute::Semantic::E_VAS_POSITION, 0);
        size_t vertexSize = mesh->getVertexStride(attr->getSlot());
        size_t offset = mesh->getVertexOffset(attr->getOffset());
        const Buffer &vertexBuffer = mesh->getVertices()[attr->getSlot()];
        const Buffer &indexBuffer = submesh->getIndices();
        size_t indexSize = submesh->getIndexBuffer()->getIndexSize();
        size_t pointCount = submesh->getIndexBuffer()->getIndexCount();
        Vector3 *points = new Vector3[pointCount];
        for (size_t i = 0; i < pointCount; ++i)
        {
            int32_t idx = 0;
            const uint8_t *src = indexBuffer.Data + i * indexSize;
            memcpy(&idx, src, indexSize);
            src = vertexBuffer.Data + idx * vertexSize + offset;
            memcpy(points+i, src, sizeof(Vector3));
            // Vector3 *srcPos = (Vector3*)src;
            // T3D_LOG_INFO(LOG_TAG_APP, "Index = %d, Src : (%f, %f, %f), Dst : (%f, %f, %f)", idx, srcPos->x(), srcPos->y(), srcPos->z(), points[i].x(), points[i].y(), points[i].z());
        }
        Aabb aabb;
        aabb.build(points, pointCount);
        T3D_SAFE_DELETE_ARRAY(points);
        bound->setParams(aabb.getMinX(), aabb.getMaxX(), aabb.getMinY(), aabb.getMaxY(), aabb.getMinZ(), aabb.getMaxZ());
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemCreateSphere(uint32_t id, ImWidget *menuItem)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onGUIBegin(const ImVec2 &size)
    {
        return ImChildView::onGUIBegin(size);
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onGUIBegin()
    {
        return ImChildView::onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::onGUIEnd()
    {
        ImChildView::onGUIEnd();
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyWindow::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            ON_MEMBER(kEvtOpenScene, UIHierarchyWindow::onOpenScene);
            
            // 工具栏
            UIHierarchyToolBar *toolbar = new UIHierarchyToolBar();
            ret = toolbar->create(ID_HIERARCHY_TOOLBAR, "HierarchyToolBar", nullptr);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create the tool bar of hierarchy window !");
                break;
            }

            // 层级视图
            mHierarchyView = new UIHierarchyView();
            ret = mHierarchyView->create(ID_HIERARCHY_VIEW,"HierarchyView", nullptr);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create the hierarchy view of hierarchy window !");
                break;
            }

            // 创建自动布局，上下布局
            ImVerticalLayout *layout = new ImVerticalLayout();
            ret = layout->create(ID_HIERARCHY_WINDOW_LAYOUT, "HierarchyWindowLayout", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create the layout of project window ! ERROR [%d]", ret)
                break;
            }
            
            ImLayout::Items items;
            ImLayout::Item item;
            float txtHeight = 40.0f;
            // 工具栏
            item.size.x = 0;
            item.size.y = txtHeight;
            item.childView = toolbar;
            items.emplace_back(item);
            // 层级视图
            item.size.x = 0;
            item.size.y = 0;
            item.childView = mHierarchyView;
            items.emplace_back(item);

            layout->addWidgets(items);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void UIHierarchyWindow::onDestroy()
    {
        unregisterAllEvent();
        UIDockingWindow::onDestroy();
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyWindow::onOpenScene(EventParam *param, TINSTANCE sender)
    {
        EventParamOpenScene *p = static_cast<EventParamOpenScene *>(param);
        mHierarchyView->setScene(p->arg1);
        mHierarchyView->refresh();
        return true;
    }

    //--------------------------------------------------------------------------

    NS_END
}
