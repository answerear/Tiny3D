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

    /// prefab 实例节点的文本颜色，与 inspector 上的 prefab 标识保持一致
    static const ImVec4 kPrefabTextColor(0.35f, 0.55f, 1.0f, 1.0f);

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
            ON_MEMBER(kEvtModifyScene, UIHierarchyView::onModifedScene);
            ON_MEMBER(kEvtPrefabInstanceChanged, UIHierarchyView::onPrefabInstanceChanged);
            
            ON_MENU_ITEM_MEMBER(ID_MENU_ITEM_CREATE_EMPTY, UIHierarchyView::onMenuItemCreateEmpty);
            ON_MENU_ITEM_MEMBER(ID_MENU_ITEM_CREATE_CUBE, UIHierarchyView::onMenuItemCreateCube);
            ON_MENU_ITEM_MEMBER(ID_MENU_ITEM_CREATE_SPHERE, UIHierarchyView::onMenuItemCreateSphere);
            ON_MENU_ITEM_MEMBER(ID_MENU_ITEM_CREATE_CAPSULE, UIHierarchyView::onMenuItemCreateCapsule);
            ON_MENU_ITEM_MEMBER(ID_MENU_ITEM_CREATE_CYLINDER, UIHierarchyView::onMenuItemCreateCylinder);
            ON_MENU_ITEM_MEMBER(ID_MENU_ITEM_CREATE_QUAD, UIHierarchyView::onMenuItemCreateQuad);
            ON_MENU_ITEM_MEMBER(ID_MENU_ITEM_CREATE_PLANE, UIHierarchyView::onMenuItemCreatePlane);
            ON_MENU_ITEM_MEMBER(ID_MENU_ITEM_DELETE, UIHierarchyView::onMenuItemDelete);

            ON_MENU_ITEM_QUERY_MEMBER(ID_MENU_ITEM_CREATE_EMPTY, UIHierarchyView::onMenuItemEnabledCreateEmpty);
            ON_MENU_ITEM_QUERY_MEMBER(ID_MENU_ITEM_CREATE_CUBE, UIHierarchyView::onMenuItemEnabledCreateCube);
            ON_MENU_ITEM_QUERY_MEMBER(ID_MENU_ITEM_CREATE_SPHERE, UIHierarchyView::onMenuItemEnabledCreateSphere);
            ON_MENU_ITEM_QUERY_MEMBER(ID_MENU_ITEM_CREATE_CAPSULE, UIHierarchyView::onMenuItemEnabledCreateCapsule);
            ON_MENU_ITEM_QUERY_MEMBER(ID_MENU_ITEM_CREATE_CYLINDER, UIHierarchyView::onMenuItemEnabledCreateCylinder);
            ON_MENU_ITEM_QUERY_MEMBER(ID_MENU_ITEM_CREATE_QUAD, UIHierarchyView::onMenuItemEnabledCreateQuad);
            ON_MENU_ITEM_QUERY_MEMBER(ID_MENU_ITEM_CREATE_PLANE, UIHierarchyView::onMenuItemEnabledCreatePlane);
            ON_MENU_ITEM_QUERY_MEMBER(ID_MENU_ITEM_DELETE, UIHierarchyView::onMenuItemEnabledDelete);
            
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

    void UIHierarchyView::clearTree()
    {
        if (mTreeWidget != nullptr)
        {
            mTreeWidget->removeAllChildren();
        }

        mRoot = nullptr;
    }

    //--------------------------------------------------------------------------

    GameObject *UIHierarchyView::gameObjectFromTreeNode(ImTreeNode *uiNode) const
    {
        if (uiNode == nullptr)
        {
            return nullptr;
        }

        TransformNode *node = static_cast<TransformNode *>(uiNode->getUserData());
        if (node == nullptr)
        {
            return nullptr;
        }

        return node->getGameObject();
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::captureViewStateRecursive(ImWidget *widget)
    {
        if (widget == nullptr)
        {
            return;
        }

        for (auto child : widget->getChildren())
        {
            if (child == nullptr || child->getWidgetType() != WidgetType::kTreeNode)
            {
                continue;
            }

            ImTreeNode *uiNode = static_cast<ImTreeNode *>(child);
            GameObject *go = gameObjectFromTreeNode(uiNode);
            if (go != nullptr)
            {
                mSavedExpandState[go->getUUID()] = uiNode->isExpanded();
            }

            captureViewStateRecursive(uiNode);
        }
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::captureViewState()
    {
        if (mTreeWidget == nullptr || mTreeWidget->getChildren().empty())
        {
            return;
        }

        mSavedExpandState.clear();
        mSavedSelectionUUID = UUID::INVALID;

        GameObject *selected = gameObjectFromTreeNode(mTreeWidget->getSelection());
        if (selected != nullptr)
        {
            mSavedSelectionUUID = selected->getUUID();
        }

        captureViewStateRecursive(mTreeWidget);
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::restoreViewStateRecursive(ImWidget *widget)
    {
        if (widget == nullptr)
        {
            return;
        }

        for (auto child : widget->getChildren())
        {
            if (child == nullptr || child->getWidgetType() != WidgetType::kTreeNode)
            {
                continue;
            }

            ImTreeNode *uiNode = static_cast<ImTreeNode *>(child);
            GameObject *go = gameObjectFromTreeNode(uiNode);
            if (go != nullptr)
            {
                const auto it = mSavedExpandState.find(go->getUUID());
                if (it != mSavedExpandState.end())
                {
                    if (it->second)
                    {
                        uiNode->expand(false);
                    }
                    else
                    {
                        uiNode->collapse(false);
                    }
                }

                if (mSavedSelectionUUID != UUID::INVALID
                    && go->getUUID() == mSavedSelectionUUID)
                {
                    mTreeWidget->setSelection(uiNode);
                }
            }

            restoreViewStateRecursive(uiNode);
        }
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::restoreViewState()
    {
        if (mTreeWidget != nullptr)
        {
            restoreViewStateRecursive(mTreeWidget);

            if (mSavedSelectionUUID != UUID::INVALID)
            {
                postSelectionChanged(mTreeWidget->getSelection());
            }
        }

        mSavedExpandState.clear();
        mSavedSelectionUUID = UUID::INVALID;
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::refresh()
    {
        captureViewState();
        clearTree();

        if (mScene != nullptr)
        {
            populateGameObjectTree();
            restoreViewState();
        }
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
            root->visitAll(
                [this, &ret](int32_t depth, TransformNode *node, const ImTreeNode::CallbackData &callbacks, const ImTreeNodeDestroyCallback &onDestroy)
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
        postSelectionChanged(node);
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::treeNodeRClicked(ImTreeNode *node)
    {
        
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::postSelectionChanged(ImTreeNode *node)
    {
        GameObject *gameObject = nullptr;

        if (node != nullptr)
        {
            TransformNode *transform = static_cast<TransformNode *>(node->getUserData());

            // 场景根节点不是普通的 game object，不作为选中对象派发
            if (transform != nullptr && !EDITOR_SCENE.isSceneRoot(transform))
            {
                gameObject = transform->getGameObject();
            }
        }

        EventParamGameObjectSelected param(gameObject);
        postEvent(kEvtGameObjectSelected, &param);
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::onTreeNodeDestroy(ImTreeNode *uiNode)
    {
        if (uiNode == nullptr)
        {
            return;
        }

        TransformNode *node = static_cast<TransformNode *>(uiNode->getUserData());
        uiNode->setUserData(nullptr);
        if (node != nullptr)
        {
            node->setUserData(nullptr);
        }
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

            updatePrefabDecoration(node, uiNode);

            mTreeWidget->setSelection(uiNode);

            if (mRoot == nullptr)
            {
                mRoot = uiNode;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemEnabledCreateEmpty(uint32_t id, ImWidget *menuItem)
    {
        if (mTreeWidget != nullptr && mTreeWidget->getSelection() != nullptr)
        {
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemCreateEmpty(uint32_t id, ImWidget *menuItem)
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

            // 创建 game object
            GameObjectPtr go = GameObject::create("GameObject");
            if (go == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to create GameObject !");
                break;
            }

            // 创建 transform node
            Transform3DPtr node = go->addComponent<Transform3D>();
            if (node == nullptr)
            {
                EDITOR_LOG_WARNING("Failed to add Transform3D component !");
                break;
            }

            parent->addChild(node);

            // 创建层次结构树节点
            const auto treeNodeClicked = std::bind(&UIHierarchyView::treeNodeClicked, this, std::placeholders::_1);
            const auto treeNodeRClicked = std::bind(&UIHierarchyView::treeNodeRClicked, this, std::placeholders::_1);
            const auto treeNodeDestroy = std::bind(&UIHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);
                    
            createTreeNode(node, callbacks, treeNodeDestroy);

            // 展开上层节点，聚焦到该节点
            ImTreeNode *uiNode = static_cast<ImTreeNode*>(parent->getUserData());
            T3D_ASSERT(uiNode != nullptr);
            uiNode->expand(false);

            // 新建的节点已成为当前选中项，同步通知 inspector
            postSelectionChanged(mTreeWidget->getSelection());

            // 通知修改了场景
            EventParamModifyScene param(true);
            sendEvent(kEvtModifyScene, &param);
        } while (false);

        return true;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemEnabledCreateCube(uint32_t id, ImWidget *menuItem)
    {
        if (mTreeWidget != nullptr && mTreeWidget->getSelection() != nullptr)
        {
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemEnabledCreateSphere(uint32_t id, ImWidget *menuItem)
    {
        if (mTreeWidget != nullptr && mTreeWidget->getSelection() != nullptr)
        {
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemEnabledCreateCapsule(uint32_t id, ImWidget *menuItem)
    {
        if (mTreeWidget != nullptr && mTreeWidget->getSelection() != nullptr)
        {
            return true;
        }

        return false;
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

            // 创建 game object
            GameObjectPtr go = GameObject::create("Cube");
            if (go == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to create GameObject !");
                break;
            }

            // 创建 transform node 组件
            Transform3DPtr node = go->addComponent<Transform3D>();
            if (node == nullptr)
            {
                EDITOR_LOG_WARNING("Failed to add Transform3D component !");
                break;
            }
            
            parent->addChild(node);

            // 加载立方体相关的 geometry 组件
            TResult ret = createCube(go);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to create cube ! ERROR [%d]", ret);
                break;
            }

            // 创建层次结构树节点
            const auto treeNodeClicked = std::bind(&UIHierarchyView::treeNodeClicked, this, std::placeholders::_1);
            const auto treeNodeRClicked = std::bind(&UIHierarchyView::treeNodeRClicked, this, std::placeholders::_1);
            const auto treeNodeDestroy = std::bind(&UIHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);

            createTreeNode(node, callbacks, treeNodeDestroy);

            // 展开上层节点，聚焦到该节点
            ImTreeNode *uiNode = static_cast<ImTreeNode*>(parent->getUserData());
            T3D_ASSERT(uiNode != nullptr);
            uiNode->expand(false);

            // 新建的节点已成为当前选中项，同步通知 inspector
            postSelectionChanged(mTreeWidget->getSelection());

            // 通知修改了场景
            EventParamModifyScene param(true);
            sendEvent(kEvtModifyScene, &param);
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

            // 资源门面已挂载工程搜索链（含 builtin），按名字一步加载内置网格
            MeshPtr mesh = T3D_ASSET_MGR.loadMesh(ProjectManager::BUILTIN_CUBE_MESH_NAME);
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
                Material *material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(submesh.second->getMaterialUUID()));
                T3D_ASSERT(material != nullptr);
                ret = material->switchKeywords(enableKeywrods, disableKeywords);
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

            // bound component（按 mesh 里的包围体种子创建）
            createBound(go, geometry, mesh, submesh);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyView::createBound(GameObject *go, Geometry *geometry, Mesh *mesh, SubMesh *submesh)
    {
        if (go == nullptr || mesh == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        // mesh 在转换期已算好包围体种子，优先按种子播种对应类型的 Bound 组件
        BoundPtr bound = GameObject::seedBoundFromMesh(go, mesh, geometry);
        if (bound != nullptr)
        {
            return T3D_OK;
        }

        if (!go->getComponents<Bound>().empty())
        {
            // 已有 Bound 组件，以组件为准
            return T3D_OK;
        }

        EDITOR_LOG_WARNING("Mesh [%s] has no bound seed, fallback to computing AABB from vertex data !", mesh->getName().c_str());

        AabbBoundPtr aabb = go->addComponent<AabbBound>();
        if (aabb == nullptr)
        {
            EDITOR_LOG_ERROR("Failed to add AabbBound component !");
            return T3D_ERR_FAIL;
        }

        TResult ret = buildAabbFromMeshData(mesh, submesh, aabb);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        if (geometry != nullptr)
        {
            geometry->setRenderBound(aabb);
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyView::buildAabbFromMeshData(Mesh *mesh, SubMesh *submesh, AabbBound *bound)
    {
        if (mesh == nullptr || submesh == nullptr || bound == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        const VertexAttribute *attr = mesh->findVertexAttributeBySemantic(VertexAttribute::Semantic::E_VAS_POSITION, 0);
        if (attr == nullptr)
        {
            EDITOR_LOG_ERROR("Mesh [%s] has no position vertex attribute !", mesh->getName().c_str());
            return T3D_ERR_INVALID_PARAM;
        }

        IndexBuffer *ib = submesh->getIndexBuffer();
        if (ib == nullptr)
        {
            EDITOR_LOG_ERROR("SubMesh [%s] has no index buffer !", submesh->getName().c_str());
            return T3D_ERR_INVALID_POINTER;
        }

        size_t vertexSize = mesh->getVertexStride(attr->getSlot());
        size_t offset = attr->getOffset();
        const Buffer &vertexBuffer = mesh->getVertices()[attr->getSlot()];
        const Buffer &indexBuffer = submesh->getIndices();
        size_t indexSize = ib->getIndexSize();
        size_t pointCount = ib->getIndexCount();
        if (pointCount == 0)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        Vector3 *points = new Vector3[pointCount];
        for (size_t i = 0; i < pointCount; ++i)
        {
            int32_t idx = 0;
            const uint8_t *src = indexBuffer.Data + i * indexSize;
            memcpy(&idx, src, indexSize);
            src = vertexBuffer.Data + idx * vertexSize + offset;
            memcpy(points+i, src, sizeof(Vector3));
        }
        Aabb aabb;
        aabb.build(points, pointCount);
        T3D_SAFE_DELETE_ARRAY(points);
        bound->setParams(aabb.getMinX(), aabb.getMaxX(), aabb.getMinY(), aabb.getMaxY(), aabb.getMinZ(), aabb.getMaxZ());

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemCreateSphere(uint32_t id, ImWidget *menuItem)
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

            // 创建 game object
            GameObjectPtr go = GameObject::create("Sphere");
            if (go == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to create GameObject !");
                break;
            }

            // 创建 transform node 组件
            Transform3DPtr node = go->addComponent<Transform3D>();
            if (node == nullptr)
            {
                EDITOR_LOG_WARNING("Failed to add Transform3D component !");
                break;
            }
            
            parent->addChild(node);

            // 加载球体相关的 geometry 组件
            TResult ret = createSphere(go);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to create sphere ! ERROR [%d]", ret);
                break;
            }

            // 创建层次结构树节点
            const auto treeNodeClicked = std::bind(&UIHierarchyView::treeNodeClicked, this, std::placeholders::_1);
            const auto treeNodeRClicked = std::bind(&UIHierarchyView::treeNodeRClicked, this, std::placeholders::_1);
            const auto treeNodeDestroy = std::bind(&UIHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);

            createTreeNode(node, callbacks, treeNodeDestroy);

            // 展开上层节点，聚焦到该节点
            ImTreeNode *uiNode = static_cast<ImTreeNode*>(parent->getUserData());
            T3D_ASSERT(uiNode != nullptr);
            uiNode->expand(false);

            // 新建的节点已成为当前选中项，同步通知 inspector
            postSelectionChanged(mTreeWidget->getSelection());

            // 通知修改了场景
            EventParamModifyScene param(true);
            sendEvent(kEvtModifyScene, &param);
        } while (false);

        return true;
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyView::createSphere(GameObject *go)
    {
        TResult ret = T3D_OK;

        do
        {
            // geometry component
            GeometryPtr geometry = go->addComponent<Geometry>();

            // 资源门面已挂载工程搜索链（含 builtin），按名字一步加载内置网格
            MeshPtr mesh = T3D_ASSET_MGR.loadMesh(ProjectManager::BUILTIN_SPHERE_MESH_NAME);
            if (mesh == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load sphere mesh assets !");
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            StringArray enableKeywrods;
            enableKeywrods.push_back("");
            StringArray disableKeywords;
            for (auto submesh : mesh->getSubMeshes())
            {
                Material *material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(submesh.second->getMaterialUUID()));
                T3D_ASSERT(material != nullptr);
                ret = material->switchKeywords(enableKeywrods, disableKeywords);
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
            
            SubMesh *submesh = mesh->getSubMesh(ProjectManager::BUILTIN_SPHERE_SUBMESH_NAME);
            geometry->setMeshObject(mesh, submesh);

            // bound component（按 mesh 里的包围体种子创建）
            createBound(go, geometry, mesh, submesh);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemCreateCapsule(uint32_t id, ImWidget *menuItem)
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

            // 创建 game object
            GameObjectPtr go = GameObject::create("Capsule");
            if (go == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to create GameObject !");
                break;
            }

            // 创建 transform node 组件
            Transform3DPtr node = go->addComponent<Transform3D>();
            if (node == nullptr)
            {
                EDITOR_LOG_WARNING("Failed to add Transform3D component !");
                break;
            }
            
            parent->addChild(node);

            // 加载胶囊体相关的 geometry 组件
            TResult ret = createCapsule(go);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to create capsule ! ERROR [%d]", ret);
                break;
            }

            // 创建层次结构树节点
            const auto treeNodeClicked = std::bind(&UIHierarchyView::treeNodeClicked, this, std::placeholders::_1);
            const auto treeNodeRClicked = std::bind(&UIHierarchyView::treeNodeRClicked, this, std::placeholders::_1);
            const auto treeNodeDestroy = std::bind(&UIHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);

            createTreeNode(node, callbacks, treeNodeDestroy);

            // 展开上层节点，聚焦到该节点
            ImTreeNode *uiNode = static_cast<ImTreeNode*>(parent->getUserData());
            T3D_ASSERT(uiNode != nullptr);
            uiNode->expand(false);

            // 新建的节点已成为当前选中项，同步通知 inspector
            postSelectionChanged(mTreeWidget->getSelection());

            // 通知修改了场景
            EventParamModifyScene param(true);
            sendEvent(kEvtModifyScene, &param);
        } while (false);

        return true;
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyView::createCapsule(GameObject *go)
    {
        TResult ret = T3D_OK;

        do
        {
            // geometry component
            GeometryPtr geometry = go->addComponent<Geometry>();

            // 资源门面已挂载工程搜索链（含 builtin），按名字一步加载内置网格
            MeshPtr mesh = T3D_ASSET_MGR.loadMesh(ProjectManager::BUILTIN_CAPSULE_MESH_NAME);
            if (mesh == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load capsule mesh assets !");
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            StringArray enableKeywrods;
            enableKeywrods.push_back("");
            StringArray disableKeywords;
            for (auto submesh : mesh->getSubMeshes())
            {
                Material *material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(submesh.second->getMaterialUUID()));
                T3D_ASSERT(material != nullptr);
                ret = material->switchKeywords(enableKeywrods, disableKeywords);
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
            
            SubMesh *submesh = mesh->getSubMesh(ProjectManager::BUILTIN_CAPSULE_SUBMESH_NAME);
            geometry->setMeshObject(mesh, submesh);

            // bound component（按 mesh 里的包围体种子创建）
            createBound(go, geometry, mesh, submesh);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemEnabledCreateCylinder(uint32_t id, ImWidget *menuItem)
    {
        if (mTreeWidget != nullptr && mTreeWidget->getSelection() != nullptr)
        {
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemCreateCylinder(uint32_t id, ImWidget *menuItem)
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

            // 创建 game object
            GameObjectPtr go = GameObject::create("Cylinder");
            if (go == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to create GameObject !");
                break;
            }

            // 创建 transform node 组件
            Transform3DPtr node = go->addComponent<Transform3D>();
            if (node == nullptr)
            {
                EDITOR_LOG_WARNING("Failed to add Transform3D component !");
                break;
            }
            
            parent->addChild(node);

            // 加载圆柱体相关的 geometry 组件
            TResult ret = createCylinder(go);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to create cylinder ! ERROR [%d]", ret);
                break;
            }

            // 创建层次结构树节点
            const auto treeNodeClicked = std::bind(&UIHierarchyView::treeNodeClicked, this, std::placeholders::_1);
            const auto treeNodeRClicked = std::bind(&UIHierarchyView::treeNodeRClicked, this, std::placeholders::_1);
            const auto treeNodeDestroy = std::bind(&UIHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);

            createTreeNode(node, callbacks, treeNodeDestroy);

            // 展开上层节点，聚焦到该节点
            ImTreeNode *uiNode = static_cast<ImTreeNode*>(parent->getUserData());
            T3D_ASSERT(uiNode != nullptr);
            uiNode->expand(false);

            // 新建的节点已成为当前选中项，同步通知 inspector
            postSelectionChanged(mTreeWidget->getSelection());

            // 通知修改了场景
            EventParamModifyScene param(true);
            sendEvent(kEvtModifyScene, &param);
        } while (false);

        return true;
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyView::createCylinder(GameObject *go)
    {
        TResult ret = T3D_OK;

        do
        {
            // geometry component
            GeometryPtr geometry = go->addComponent<Geometry>();

            // 资源门面已挂载工程搜索链（含 builtin），按名字一步加载内置网格
            MeshPtr mesh = T3D_ASSET_MGR.loadMesh(ProjectManager::BUILTIN_CYLINDER_MESH_NAME);
            if (mesh == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load cylinder mesh assets !");
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            StringArray enableKeywrods;
            enableKeywrods.push_back("");
            StringArray disableKeywords;
            for (auto submesh : mesh->getSubMeshes())
            {
                Material *material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(submesh.second->getMaterialUUID()));
                T3D_ASSERT(material != nullptr);
                ret = material->switchKeywords(enableKeywrods, disableKeywords);
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
            
            SubMesh *submesh = mesh->getSubMesh(ProjectManager::BUILTIN_CYLINDER_SUBMESH_NAME);
            geometry->setMeshObject(mesh, submesh);

            // bound component（按 mesh 里的包围体种子创建）
            createBound(go, geometry, mesh, submesh);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemEnabledCreateQuad(uint32_t id, ImWidget *menuItem)
    {
        if (mTreeWidget != nullptr && mTreeWidget->getSelection() != nullptr)
        {
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemCreateQuad(uint32_t id, ImWidget *menuItem)
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

            // 创建 game object
            GameObjectPtr go = GameObject::create("Quad");
            if (go == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to create GameObject !");
                break;
            }

            // 创建 transform node 组件
            Transform3DPtr node = go->addComponent<Transform3D>();
            if (node == nullptr)
            {
                EDITOR_LOG_WARNING("Failed to add Transform3D component !");
                break;
            }
            
            parent->addChild(node);

            // 加载四边形相关的 geometry 组件
            TResult ret = createQuad(go);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to create quad ! ERROR [%d]", ret);
                break;
            }

            // 创建层次结构树节点
            const auto treeNodeClicked = std::bind(&UIHierarchyView::treeNodeClicked, this, std::placeholders::_1);
            const auto treeNodeRClicked = std::bind(&UIHierarchyView::treeNodeRClicked, this, std::placeholders::_1);
            const auto treeNodeDestroy = std::bind(&UIHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);

            createTreeNode(node, callbacks, treeNodeDestroy);

            // 展开上层节点，聚焦到该节点
            ImTreeNode *uiNode = static_cast<ImTreeNode*>(parent->getUserData());
            T3D_ASSERT(uiNode != nullptr);
            uiNode->expand(false);

            // 新建的节点已成为当前选中项，同步通知 inspector
            postSelectionChanged(mTreeWidget->getSelection());

            // 通知修改了场景
            EventParamModifyScene param(true);
            sendEvent(kEvtModifyScene, &param);
        } while (false);

        return true;
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyView::createQuad(GameObject *go)
    {
        TResult ret = T3D_OK;

        do
        {
            // geometry component
            GeometryPtr geometry = go->addComponent<Geometry>();

            // 资源门面已挂载工程搜索链（含 builtin），按名字一步加载内置网格
            MeshPtr mesh = T3D_ASSET_MGR.loadMesh(ProjectManager::BUILTIN_QUAD_MESH_NAME);
            if (mesh == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load quad mesh assets !");
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            StringArray enableKeywrods;
            enableKeywrods.push_back("");
            StringArray disableKeywords;
            for (auto submesh : mesh->getSubMeshes())
            {
                Material *material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(submesh.second->getMaterialUUID()));
                T3D_ASSERT(material != nullptr);
                ret = material->switchKeywords(enableKeywrods, disableKeywords);
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
            
            SubMesh *submesh = mesh->getSubMesh(ProjectManager::BUILTIN_QUAD_SUBMESH_NAME);
            geometry->setMeshObject(mesh, submesh);

            // bound component（按 mesh 里的包围体种子创建）
            createBound(go, geometry, mesh, submesh);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemEnabledCreatePlane(uint32_t id, ImWidget *menuItem)
    {
        if (mTreeWidget != nullptr && mTreeWidget->getSelection() != nullptr)
        {
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemCreatePlane(uint32_t id, ImWidget *menuItem)
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

            // 创建 game object
            GameObjectPtr go = GameObject::create("Plane");
            if (go == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to create GameObject !");
                break;
            }

            // 创建 transform node 组件
            Transform3DPtr node = go->addComponent<Transform3D>();
            if (node == nullptr)
            {
                EDITOR_LOG_WARNING("Failed to add Transform3D component !");
                break;
            }
            
            parent->addChild(node);

            // 加载平面相关的 geometry 组件
            TResult ret = createPlane(go);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to create plane ! ERROR [%d]", ret);
                break;
            }

            // 创建层次结构树节点
            const auto treeNodeClicked = std::bind(&UIHierarchyView::treeNodeClicked, this, std::placeholders::_1);
            const auto treeNodeRClicked = std::bind(&UIHierarchyView::treeNodeRClicked, this, std::placeholders::_1);
            const auto treeNodeDestroy = std::bind(&UIHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);

            createTreeNode(node, callbacks, treeNodeDestroy);

            // 展开上层节点，聚焦到该节点
            ImTreeNode *uiNode = static_cast<ImTreeNode*>(parent->getUserData());
            T3D_ASSERT(uiNode != nullptr);
            uiNode->expand(false);

            // 新建的节点已成为当前选中项，同步通知 inspector
            postSelectionChanged(mTreeWidget->getSelection());

            // 通知修改了场景
            EventParamModifyScene param(true);
            sendEvent(kEvtModifyScene, &param);
        } while (false);

        return true;
    }

    //--------------------------------------------------------------------------

    TResult UIHierarchyView::createPlane(GameObject *go)
    {
        TResult ret = T3D_OK;

        do
        {
            // geometry component
            GeometryPtr geometry = go->addComponent<Geometry>();

            // 资源门面已挂载工程搜索链（含 builtin），按名字一步加载内置网格
            MeshPtr mesh = T3D_ASSET_MGR.loadMesh(ProjectManager::BUILTIN_PLANE_MESH_NAME);
            if (mesh == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load plane mesh assets !");
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }
            
            StringArray enableKeywrods;
            enableKeywrods.push_back("");
            StringArray disableKeywords;
            for (auto submesh : mesh->getSubMeshes())
            {
                Material *material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(submesh.second->getMaterialUUID()));
                T3D_ASSERT(material != nullptr);
                ret = material->switchKeywords(enableKeywrods, disableKeywords);
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
            
            SubMesh *submesh = mesh->getSubMesh(ProjectManager::BUILTIN_PLANE_SUBMESH_NAME);
            geometry->setMeshObject(mesh, submesh);

            // bound component（按 mesh 里的包围体种子创建）
            createBound(go, geometry, mesh, submesh);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemEnabledDelete(uint32_t id, ImWidget *menuItem)
    {
        if (mTreeWidget == nullptr)
        {
            return false;
        }

        ImTreeNode *selection = mTreeWidget->getSelection();
        if (selection == nullptr)
        {
            return false;
        }

        TransformNode *node = static_cast<TransformNode *>(selection->getUserData());
        if (node == nullptr || EDITOR_SCENE.isSceneRoot(node))
        {
            return false;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onMenuItemDelete(uint32_t id, ImWidget *menuItem)
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

            TransformNode *node = static_cast<TransformNode *>(selection->getUserData());
            if (node == nullptr)
            {
                EDITOR_LOG_WARNING("The selection user data is nullptr !");
                break;
            }

            if (EDITOR_SCENE.isSceneRoot(node))
            {
                EDITOR_LOG_WARNING("Cannot delete scene root !");
                break;
            }

            GameObject *go = node->getGameObject();
            if (go == nullptr)
            {
                EDITOR_LOG_WARNING("The selection has no GameObject !");
                break;
            }

            ImTreeNode *parentUi = static_cast<ImTreeNode *>(selection->getParent());
            ImWidget *parentWidget = selection->getParent();
            if (parentWidget != nullptr)
            {
                parentWidget->removeChild(selection, true);
            }

            GameObject::destroy(go);

            if (parentUi != nullptr)
            {
                mTreeWidget->setSelection(parentUi);
            }

            // 被删除的对象不能继续留在 inspector 里，同步切换到新的选中项
            postSelectionChanged(parentUi);

            EventParamModifyScene param(true);
            sendEvent(kEvtModifyScene, &param);
        } while (false);

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

    bool UIHierarchyView::onModifedScene(EventParam *param, TINSTANCE sender)
    {
        if (param == nullptr)
        {
            return true;
        }
        
        EventParamModifyScene *para = static_cast<EventParamModifyScene *>(param);

        if (mRoot != nullptr)
        {
            if (para->arg1)
            {
                // 修改了
                String name = mScene->getName() + "*";
                mRoot->setName(name);
            }
            else
            {
                // 保存了
                mRoot->setName(mScene->getName());
            }
        }
        
        return true;
    }

    //--------------------------------------------------------------------------

    bool UIHierarchyView::onPrefabInstanceChanged(EventParam *param, TINSTANCE sender)
    {
        refreshPrefabDecoration();
        return true;
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::updatePrefabDecoration(TransformNode *node, ImTreeNode *uiNode)
    {
        if (node == nullptr || uiNode == nullptr || EDITOR_SCENE.isSceneRoot(node))
        {
            return;
        }

        GameObject *go = node->getGameObject();

        // prefab 实例根及其下面的所有节点都算实例的一部分，都要标识出来
        if (go != nullptr && PrefabUtility::getPrefabRoot(go) != nullptr)
        {
            uiNode->setTextColor(kPrefabTextColor);
        }
        else
        {
            uiNode->clearTextColor();
        }
    }

    //--------------------------------------------------------------------------

    void UIHierarchyView::refreshPrefabDecoration()
    {
        if (mScene == nullptr)
        {
            return;
        }

        Transform3D *root = mScene->getRootTransform();
        if (root == nullptr)
        {
            return;
        }

        root->visitAll(
            [this](int32_t depth, TransformNode *node)
            {
                updatePrefabDecoration(node, static_cast<ImTreeNode *>(node->getUserData()));
            });
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

        if (p->arg1 == nullptr)
        {
            // 卸载前先记下展开 / 选中。setScene(nullptr) 之后树还在，但接下来就要拆掉。
            mHierarchyView->captureViewState();
            mHierarchyView->setScene(nullptr);
            mHierarchyView->clearTree();
        }
        else
        {
            mHierarchyView->setScene(p->arg1);
            mHierarchyView->refresh();
        }

        return true;
    }

    //--------------------------------------------------------------------------

    NS_END
}
