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


#include "UIProjectWindow.h"
#include "UIEditorWidgetID.h"
#include "ImErrors.h"
#include "EditorEventDefine.h"
#include "ProjectManager.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    class UIAssetNode : public ImTreeBarNode
    {
    public:
        UIAssetNode(ImTreeNode *node, AssetNode *assetNode)
            : mNode(node)
            , mAssetNode(assetNode)
        {
            
        }
        
        ~UIAssetNode() override
        {
            
        }

        const String &getName() const override
        {
            return mNode->getName();
        }

        ImTreeBarNode *getParent() const override
        {
            ImTreeNode *parent = static_cast<ImTreeNode*>(mNode->getParent());
            return static_cast<ImTreeBarNode *>(parent->getUserData());
        }

        ImTreeBarNode *getChild(const String &name) const override
        {
            ImTreeNode *child = static_cast<ImTreeNode*>(mNode->getChild(name));
            return static_cast<ImTreeBarNode *>(child->getUserData());
        }

        Children getChildren() const override
        {
            Children children;
            for (auto child : mNode->getChildren())
            {
                ImTreeNode *node = static_cast<ImTreeNode*>(child);
                children.emplace_back(static_cast<ImTreeBarNode *>(node->getUserData()));
            }
            return children;
        }

        ImTreeBarNode *getPrevSibling() override
        {
            ImTreeNode *node = static_cast<ImTreeNode *>(mNode->getPrevSibling());
            return static_cast<ImTreeBarNode *>(node->getUserData());
        }

        ImTreeBarNode *getNextSibling() override
        {
            ImTreeNode *node = static_cast<ImTreeNode *>(mNode->getNextSibling());
            return static_cast<ImTreeBarNode *>(node->getUserData());
        }

        AssetNode *getAssetNode() const { return mAssetNode; }

        ImTreeNode *getTreeNode() const { return mNode; }
        
    protected:
        ImTreeNode *mNode {nullptr};
        AssetNode *mAssetNode {nullptr};
    };
    
    //--------------------------------------------------------------------------
    
    TResult UIAssetHierarchyView::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            mContextMenu = new ImContextMenu();
            ret = mContextMenu->create(ID_PROJECT_ASSET_CONTEXT_MENU, "AssetTreeContextMenu");
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create asset hierarchy context menu failed ! ERROR [%d]", ret)
                break;
            }
            mContextMenu->setVisible(false);

            auto queryEnableDefault = [](ImWidget*) { return true; };
            auto queryDisableDefault = [](ImWidget*) { return false; };
            
            mContextMenu->addItem(ID_MENU_ITEM_FOLDER, STR(TXT_FOLDER), "", queryEnableDefault);
            
            auto treeNodeClicked = std::bind(&UIAssetHierarchyView::treeNodeClicked, this, std::placeholders::_1);

            auto treeNodeRClicked = std::bind(&UIAssetHierarchyView::treeNodeRClicked, this, std::placeholders::_1);

            auto treeNodeDestroy = std::bind(&UIAssetHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

            T3D_ASSERT(mTreeWidget == nullptr);
            mTreeWidget = new ImTreeWidget();
            ret = mTreeWidget->create(ID_PROJECT_ASSET_HIERARCHY_TREE, "Asset Hierarchy Tree", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create asset hierarchy tree failed ! ERROR [%d]", ret);
                break;
            }

            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);

            // 收藏树
            ret = populateFavoritesTree(mTreeWidget, callbacks, treeNodeDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to populate favorites tree ! ERROR [%d]", ret);
                break;
            }

            // 分隔用的空行
            ImDummyTreeNode *dummyNode = new ImDummyTreeNode(mTreeWidget);
            ret = dummyNode->create(mTreeWidget);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create dummy node failed ! ERROR [%d]", ret)
                break;
            }

            // 资产树
            ret = populateAssetsTree(mTreeWidget, callbacks, treeNodeDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to populate asset tree ! ERROR [%d]", ret);
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIAssetHierarchyView::populateFavoritesTree(ImTreeWidget *tree, const ImTreeNode::CallbackData &callbacks, const ImTreeNodeDestroyCallback &onDestroy)
    {
        TResult ret = T3D_OK;

        do
        {
            ImTreeNode *favoriteRoot = new ImTreeNode(tree);

            // Favorites
            ret = favoriteRoot->createByPath(ICON_NAME_FAVORITE, "Favorites", callbacks, tree, onDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create favorites node failed ! ERROR [%d]", ret)
                break;
            }
            UIAssetNode *assetNode = new UIAssetNode(favoriteRoot, nullptr);
            favoriteRoot->setUserData(assetNode);

            // All Materials
            ImTreeNode *node = new ImTreeNode(tree);
            ret = node->createByPath(ICON_NAME_SEARCH, "All Materials", callbacks, favoriteRoot, onDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create all material node failed ! ERROR [%d]", ret)
                break;
            }
            assetNode = new UIAssetNode(node, nullptr);
            node->setUserData(assetNode);

            // All Models
            node = new ImTreeNode(tree);
            ret = node->createByPath(ICON_NAME_SEARCH, "All Models", callbacks, favoriteRoot, onDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create all models node failed ! ERROR [%d]", ret)
                break;
            }
            assetNode = new UIAssetNode(node, nullptr);
            node->setUserData(assetNode);

            // All Prefabs
            node = new ImTreeNode(tree);
            ret = node->createByPath(ICON_NAME_SEARCH, "All Prefabs", callbacks, favoriteRoot, onDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create all prefabs node failed ! ERROR [%d]", ret)
                break;
            }
            assetNode = new UIAssetNode(node, nullptr);
            node->setUserData(assetNode);

            favoriteRoot->expand(true);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIAssetHierarchyView::populateAssetsTree(ImTreeWidget *tree, const ImTreeNode::CallbackData &callbacks, const ImTreeNodeDestroyCallback &onDestroy)
    {
        TResult ret = T3D_OK;

        do
        {
            // ImTreeNode *assetsRoot = new ImTreeNode(tree);
            // ret = assetsRoot->createByPath(ICON_NAME_FOLDER, ICON_NAME_FOLDER_OPENED, "Assets", callbacks, tree, onDestroy);
            // if (T3D_FAILED(ret))
            // {
            //     EDITOR_LOG_ERROR("Create assets folder node failed ! ERROR [%d]", ret)
            //     break;
            // }
            // UIAssetNode *assetNode = new UIAssetNode(assetsRoot);
            // assetsRoot->setUserData(assetNode);
            //
            // ImTreeNode *node = new ImTreeNode(tree);
            // ret = node->createByPath(ICON_NAME_FOLDER, ICON_NAME_FOLDER_OPENED, "Scenes", callbacks, assetsRoot, onDestroy);
            // if (T3D_FAILED(ret))
            // {
            //     EDITOR_LOG_ERROR("Create scenes folder node failed ! ERROR [%d]", ret)
            //     break;
            // }
            // assetNode = new UIAssetNode(node);
            // node->setUserData(assetNode);

            AssetNode *root = PROJECT_MGR.getAssetRoot();
            T3D_ASSERT(mAssetsRoot == nullptr);
            ret = populateAssetsTree(tree, tree, root, callbacks, onDestroy, mAssetsRoot);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_WARNING("Failed to populate assets tree ! ERROR [%d]", ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIAssetHierarchyView::populateAssetsTree(ImTreeWidget *tree, ImTreeNode *uiParent, AssetNode *node, const ImTreeNode::CallbackData &callbacks, const ImTreeNodeDestroyCallback &onDestroy, ImTreeNode *&uiNode)
    {
        TResult ret = T3D_OK;

        do
        {
            Meta *meta = node->getMeta();

            if (meta->getType() == Meta::Type::kFolder)
            {
                // 文件夹
                uiNode = new ImTreeNode(tree);
                
                ret = uiNode->createByPath(ICON_NAME_FOLDER, ICON_NAME_FOLDER_OPENED, node->getFilename(), callbacks, uiParent, onDestroy);
                if (T3D_FAILED(ret))
                {
                    EDITOR_LOG_ERROR("Failed to create ui node [%s] ! ERROR [%d]", node->getFilename().c_str(), ret);
                    break;
                }

                UIAssetNode *barNode = new UIAssetNode(uiNode, node);
                uiNode->setUserData(barNode);
                node->setUserData(uiNode);
                
                for (auto child : node->getChildren())
                {
                    ImTreeNode *uiChild = nullptr;
                    ret = populateAssetsTree(tree, uiNode, child, callbacks, onDestroy, uiChild);
                    if (T3D_FAILED(ret))
                    {
                        EDITOR_LOG_WARNING("Failed to populate assets tree for node [%s] ! ERROR [%d]", child->getPath().c_str(), ret);
                    }
                }
            }
            else
            {
                // 普通文件
            }
            
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void UIAssetHierarchyView::onDestroy()
    {
        if (mContextMenu != nullptr)
        {
            mContextMenu->destroy();
        }
    }

    //--------------------------------------------------------------------------

    void UIAssetHierarchyView::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    ImGuiChildFlags UIAssetHierarchyView::onGetChildFlags()
    {
        return ImGuiChildFlags_Border;
    }

    //--------------------------------------------------------------------------

    TResult UIAssetHierarchyView::rebuild()
    {
        TResult ret = T3D_OK;

        do
        {
            mAssetsRoot->getParent()->removeChild(mAssetsRoot, true);
            mAssetsRoot = nullptr;

            auto treeNodeClicked = std::bind(&UIAssetHierarchyView::treeNodeClicked, this, std::placeholders::_1);

            auto treeNodeRClicked = std::bind(&UIAssetHierarchyView::treeNodeRClicked, this, std::placeholders::_1);

            auto treeNodeDestroy = std::bind(&UIAssetHierarchyView::onTreeNodeDestroy, this, std::placeholders::_1);

            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);
        
            ret = populateAssetsTree(mTreeWidget, callbacks, treeNodeDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to populate asset tree ! ERROR [%d]", ret);
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void UIAssetHierarchyView::treeNodeClicked(ImTreeNode *node)
    {
        EDITOR_LOG_INFO("Tree node [%s] clicked ", node->getName().c_str())

        EventParamHierarchyNodeClicked param(node);
        UIAssetNode *uiAssetNOde = (UIAssetNode *)node->getUserData();
        postEvent(kEvtHierarchyNodeClicked, &param);
    }

    //--------------------------------------------------------------------------

    void UIAssetHierarchyView::treeNodeRClicked(ImTreeNode *node)
    {
        EDITOR_LOG_INFO("Tree node [%s] R-Clicked ", node->getName().c_str())
        mContextMenu->show(this);
    }

    //--------------------------------------------------------------------------

    void UIAssetHierarchyView::onTreeNodeDestroy(ImTreeNode *node)
    {
        UIAssetNode *assetNode = static_cast<UIAssetNode*>(node->getUserData());
        if (assetNode->getAssetNode() != nullptr)
        {
            assetNode->getAssetNode()->setUserData(nullptr);
        }
        T3D_SAFE_DELETE(assetNode);
        node->setUserData(nullptr);
    }

    //--------------------------------------------------------------------------

    TResult UIAssetThumbView::create(uint32_t id, const String &name, ImWidget *parent, const ImTreeBar::TreeBarNodes &roots)
    {
        return ImWidget::createInternal(id, name, parent, 1, &roots);
    }

    //--------------------------------------------------------------------------

    TResult UIAssetThumbView::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = T3D_OK;

        do
        {
            ImTreeBar::TreeBarNodes *roots = va_arg(args, ImTreeBar::TreeBarNodes*);

            // 路径栏
            mPathBar = new UIAssetPathBar();
            ret = mPathBar->create(ID_PROJECT_PATH_BAR, "Asset Path Bar", nullptr, *roots);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create project asset path bar failed ! ERROR [%d]", ret)
                break;
            }

            // 详情视图
            mDetailView = new UIAssetDetailView();
            ret = mDetailView->create(ID_PROJECT_ASSET_DETAIL_VIEW, "Asset Detail View", nullptr);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create project asset detail view failed ! ERROR [%d]", ret)
                break;
            }

            // 状态栏
            mStatusBar = new UIAssetStatusBar();
            ret = mStatusBar->create(ID_PROJECT_STATUS_BAR,  "Asset Status Bar", nullptr);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create project asset status bar failed ! ERROR [%d]", ret)
                break;
            }

            // 创建自动布局，上中下布局
            ImVerticalLayout *layout = new ImVerticalLayout();
            ret = layout->create(ID_GAME_WINDOW_LAYOUT, "GameWindowLayout", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create project window layout failed ! ERROR [%d]", ret)
                break;
            }

            ImLayout::Items items;
            ImLayout::Item item;
            float txtHeight = 30.0f;
            // 工具栏
            item.size.x = 0;
            item.size.y = txtHeight;
            item.childView = mPathBar;
            items.emplace_back(item);
            // 详情视图
            item.size.x = item.size.y = 0;
            item.childView = mDetailView;
            items.emplace_back(item);
            // 状态栏
            item.size.x = 0;
            item.size.y = -txtHeight;   // 负值，停靠下端，上一个 item 自动计算高度
            item.childView = mStatusBar;
            items.emplace_back(item);
            
            layout->addWidgets(items);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void UIAssetThumbView::onDestroy()
    {
        mPathBar = nullptr;
        mDetailView = nullptr;
        mStatusBar = nullptr;
    }

    //--------------------------------------------------------------------------

    bool UIAssetThumbView::onGUIBegin()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        bool ret = ImChildView::onGUIBegin();
        if (!ret)
        {
            ImGui::PopStyleVar();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    bool UIAssetThumbView::onGUIBegin(const ImVec2 &size)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        bool ret = ImChildView::onGUIBegin(size);
        if (!ret)
        {
            ImGui::PopStyleVar();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    void UIAssetThumbView::onGUIEnd()
    {
        ImChildView::onGUIEnd();
        ImGui::PopStyleVar();
    }

    //--------------------------------------------------------------------------

    ImGuiChildFlags UIAssetThumbView::onGetChildFlags()
    {
        return ImGuiChildFlags_Border;
    }

    //--------------------------------------------------------------------------

    TResult UIAssetThumbView::rebuild()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mPathBar != nullptr)
            {
                ret = mPathBar->rebuild();
                if (T3D_FAILED(ret))
                {
                    EDITOR_LOG_WARNING("Failed to rebuild path bar !");
                }
            }

            if (mDetailView != nullptr)
            {
                ret = mDetailView->rebuild();
                if (T3D_FAILED(ret))
                {
                    EDITOR_LOG_WARNING("Failed to rebuild detail view !");
                }
            }
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult UIAssetPathBar::create(uint32_t id, const String &name, ImWidget *parent, const ImTreeBar::TreeBarNodes &roots)
    {
        return ImWidget::createInternal(id, name, parent, 1, &roots);
    }

    //--------------------------------------------------------------------------

    TResult UIAssetPathBar::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = T3D_OK;

        do
        {
            ImTreeBar::TreeBarNodes *roots = va_arg(args, ImTreeBar::TreeBarNodes*);
            
            mTreeBar = new ImTreeBar();
            ret = mTreeBar->create(ID_PROJECT_PATH_TREE_BAR, "PathTreeBar", this, *roots,
                [](ImTreeBarNode *node)
                {});
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIAssetPathBar::onCreate()
    {
        ON_MEMBER(kEvtHierarchyNodeClicked, UIAssetPathBar::onClickedHierarchyNode);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void UIAssetPathBar::onDestroy()
    {
        unregisterAllEvent();
    }

    //--------------------------------------------------------------------------

    bool UIAssetPathBar::onGUIBegin()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_MenuBarBg]);
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        bool ret = ImChildView::onGUIBegin();
        if (!ret)
        {
            // ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    bool UIAssetPathBar::onGUIBegin(const ImVec2 &size)
    {
        auto parentSize = ImGui::GetWindowSize();
        parentSize.y = size.y;
        ImGuiStyle &style = ImGui::GetStyle();
        ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_MenuBarBg]);
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        // auto pos = ImGui::GetCursorScreenPos();
        // ImGui::SetNextWindowPos(pos);
        bool ret = ImChildView::onGUIBegin(parentSize);
        if (!ret)
        {
            // ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    void UIAssetPathBar::onGUIEnd()
    {
        ImChildView::onGUIEnd();
        // ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    //--------------------------------------------------------------------------

    ImGuiChildFlags UIAssetPathBar::onGetChildFlags()
    {
        return ImGuiChildFlags_None;
    }
    
    //--------------------------------------------------------------------------

    TResult UIAssetPathBar::rebuild()
    {
        TResult ret = T3D_OK;

        if (mTreeBar != nullptr)
        {
            mTreeBar->setSelectedNode(nullptr);
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    bool UIAssetPathBar::onClickedHierarchyNode(EventParam *param, TINSTANCE sender)
    {
        EventParamHierarchyNodeClicked *p = static_cast<EventParamHierarchyNodeClicked *>(param);
        ImTreeBarNode *node = static_cast<ImTreeBarNode*>(p->arg1->getUserData());
        mTreeBar->setSelectedNode(node);

#if 0
        static ImProgressDialog *dlg = nullptr;
        if (dlg == nullptr)
        {
            dlg = new ImProgressDialog();
            ImDialogButton button;
            button.name = "Cancal";
            button.callback = []() {};
            TResult ret = dlg->create(0x10000, "Progress Demo Dialog", ImProgressDialog::Style::Determinate, button, ImWindow::getFocusedWindow());
            T3D_ASSERT(T3D_SUCCEEDED(ret));
            dlg->setSize(ImVec2(512.0f, 0.0f));
            dlg->setText("Searching...", false);
            uint32_t total = 100;
            dlg->setTotal(total);
            
            T3D_TIMER_MGR.startTimer(1000, true,
                [this, total](ID timerID, uint32_t dt)
                {
                    static uint32_t step = 0;
                    dlg->setProgress(step);
                    step += 10;

                    if (step > total)
                    {
                        T3D_TIMER_MGR.stopTimer(timerID);
                    }
                });
        }
        
        dlg->show(ImDialog::ShowType::kEnqueueBack);
#endif
        
        return true;
    }

    //--------------------------------------------------------------------------

    TResult UIAssetDetailView::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            // auto listItemClicked = [](ImListItem *item)
            // {
            //     EDITOR_LOG_INFO("List item [%s] clicked ", item->getName().c_str());
            // };
            //
            mListWidget = new ImListWidget();
            ret = mListWidget->create(ID_PROJECT_ASSET_THUMB_LIST, "AssetThumbList", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create asset thumb list failed ! ERROR [%d]", ret)
                break;
            }
            //
            // const int32_t numOfItems = 50;
            // String postfix = "abcdefghijklmn";
            // for (int32_t i = 0; i < numOfItems; ++i)
            // {
            //     std::stringstream ss;
            //     ss << i << postfix;
            //     ImListItem *item = new ImListItem(list);
            //     ret = item->createByPath(ICON_NAME_PREFAB, ss.str(), listItemClicked, list);
            //     if (T3D_FAILED(ret))
            //     {
            //         EDITOR_LOG_ERROR("Create list item failed ! ERROR [%d]", ret)
            //         break;
            //     }
            // }
            //
            // if (T3D_FAILED(ret))
            // {
            //     break;
            // }

            ON_MEMBER(kEvtHierarchyNodeClicked, UIAssetDetailView::onClickedHierarchyNode);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void UIAssetDetailView::onDestroy()
    {
        unregisterAllEvent();
    }

    //--------------------------------------------------------------------------

    bool UIAssetDetailView::onClickedHierarchyNode(EventParam *param, TINSTANCE sender)
    {
        if (param != nullptr)
        {
            EventParamHierarchyNodeClicked *p = static_cast<EventParamHierarchyNodeClicked *>(param);
            UIAssetNode *uiAssetNode = static_cast<UIAssetNode *>(p->arg1->getUserData());
            populateItems(uiAssetNode->getAssetNode());
        }
        else
        {
            EDITOR_LOG_ERROR("Invalid param for event (kEvtHierarchyNodeClicked) !");
        }
        
        return true;
    }
    
    //--------------------------------------------------------------------------

    TResult UIAssetDetailView::populateItems(AssetNode *parent)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mListWidget == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            mListWidget->removeAllChildren();

            if (parent != nullptr)
            {
                for (auto child : parent->getChildren())
                {
                    ret = createListItem(child);
                    if (T3D_FAILED(ret))
                    {
                        EDITOR_LOG_WARNING("Create list item [%s] failed !", child->getPath().c_str());
                    }
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIAssetDetailView::createListItem(AssetNode *node)
    {
        TResult ret = T3D_OK;

        do
        {
            auto listItemClicked = [](ImListItem *item)
            {
                EDITOR_LOG_INFO("List item [%s] clicked ", item->getName().c_str());
            };

            Meta *meta = node->getMeta();

            String icon;

            switch (meta->getType())
            {
            case Meta::Type::kFolder:
                {
                    icon = ICON_NAME_FOLDER;
                }
                break;
            case Meta::Type::kFile:
                {
                    icon = ICON_NAME_FILE;
                }
                break;
            case Meta::Type::kTxt:
                {
                    icon = ICON_NAME_TEXT;
                }
                break;
            case Meta::Type::kBin:
                {
                    icon = ICON_NAME_BIN;
                }
                break;
            case Meta::Type::kDylib:
                {
                    icon = ICON_NAME_DYLIB;
                }
                break;
            case Meta::Type::kMaterial:
                {
                    icon = ICON_NAME_MATERIAL;
                }
                break;
            case Meta::Type::kTexture:
                {
                    icon = ICON_NAME_TEXTURE;
                }
                break;
            case Meta::Type::kShader:
                {
                    icon = ICON_NAME_SHADER;
                }
                break;
            case Meta::Type::kMesh:
                {
                    icon = ICON_NAME_MESH;
                }
                break;
            case Meta::Type::kPrefab:
                {
                    icon = ICON_NAME_PREFAB;
                }
                break;
            case Meta::Type::kScene:
                {
                    icon = ICON_NAME_SCENE;
                }
                break;
            case Meta::Type::kShaderLab:
                {
                    icon = ICON_NAME_SHADER;
                }
                break;
            }
            
            ImListItem *item = new ImListItem(mListWidget);
            ret = item->createByPath(icon, node->getTitle(), listItemClicked, mListWidget);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create list item failed ! ERROR [%d]", ret)
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIAssetDetailView::rebuild()
    {
        TResult ret = T3D_OK;

        do
        {
            mListWidget->removeAllChildren();
        } while (false);

        return ret;
    }


    //--------------------------------------------------------------------------

    ImGuiChildFlags UIAssetStatusBar::onGetChildFlags()
    {
        return ImGuiChildFlags_Border;
    }
    
    //--------------------------------------------------------------------------

    TResult UIProjectWindow::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            ON_MEMBER(kEvtAppEnterForeground, UIProjectWindow::onApplicationWillEnterForeground);
            ON_MEMBER(kEvtAppFocusGained, UIProjectWindow::onApplicationFocusGained);
            
            mHierarchyView = new UIAssetHierarchyView();
            ret = mHierarchyView->create(ID_PROJECT_ASSET_HIERARCHY_VIEW, "ProjectHierarchyView", nullptr);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            mThumbView = new UIAssetThumbView();
            ret = mThumbView->create(ID_PROJECT_ASSET_THUMB_VIEW, "ProjectThumbView", nullptr, mHierarchyView->getTreeBarRoots());
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            mSplitView = new ImSplitView();
            ret = mSplitView->create(ID_PROJECT_WINDOW_SPLIT_VIEW, "ProjectSplitView", 0.3f, mHierarchyView, 0.7f, mThumbView, 0, true, this);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void UIProjectWindow::onDestroy()
    {
        mHierarchyView = nullptr;
        mThumbView = nullptr;
        
        unregisterAllEvent();
    }

    //--------------------------------------------------------------------------
    
    void UIProjectWindow::onGUI()
    {
        
    }
    
    //--------------------------------------------------------------------------

    bool UIProjectWindow::onApplicationWillEnterForeground(EventParam *param, TINSTANCE sender)
    {
        if (mHierarchyView != nullptr)
        {
            mHierarchyView->rebuild();
        }

        if (mThumbView != nullptr)
        {
            mThumbView->rebuild();
        }
        
        return true;
    }

    //--------------------------------------------------------------------------

    bool UIProjectWindow::onApplicationFocusGained(EventParam *param, TINSTANCE sender)
    {
        return onApplicationWillEnterForeground(param, sender);
    }
    
    //--------------------------------------------------------------------------

    NS_END
}
