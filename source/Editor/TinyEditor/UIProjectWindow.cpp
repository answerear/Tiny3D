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


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    #define ICON_NAME_FAVORITE          "Editor/icons/d_Favorite@32.png"
    #define ICON_NAME_SEARCH            "Editor/icons/d_Search@32.png"
    #define ICON_NAME_FOLDER            "Editor/icons/d_Folder@32.png"
    #define ICON_NAME_FOLDER_OPENED     "Editor/icons/d_FolderOpened@32.png"
    #define ICON_NAME_PREFAB            "Editor/icons/d_Prefab@64.png"

    //--------------------------------------------------------------------------

    class UIAssetNode : public ImTreeBarNode
    {
    public:
        UIAssetNode(ImTreeNode *node)
            : mNode(node)
        {
            
        }
        
        ~UIAssetNode() override = default;

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

    protected:
        ImTreeNode *mNode {nullptr};
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
            
            mContextMenu->addItem(ID_MENU_ITEM_FOLDER, STR(TXT_FOLDER), "", queryDisableDefault);
            
            auto treeNodeClicked = [this](ImTreeNode *node)
            {
                EDITOR_LOG_INFO("Tree node [%s] clicked ", node->getName().c_str())
                EventParamHierarchyNodeClicked param(node);
                sendEvent(kEvtHierarchyNodeClicked, &param);
            };

            auto treeNodeRClicked = [this](ImTreeNode *node)
            {
                EDITOR_LOG_INFO("Tree node [%s] R-Clicked ", node->getName().c_str())
                mContextMenu->show(this);
            };

            auto treeNodeDestroy = [](ImTreeNode *node)
            {
                UIAssetNode *assetNode = static_cast<UIAssetNode*>(node->getUserData());
                T3D_SAFE_DELETE(assetNode);
                node->setUserData(nullptr);
            };
            
            ImTreeWidget *tree = new ImTreeWidget();
            ret = tree->create(ID_PROJECT_ASSET_HIERARCHY_TREE, "Asset Hierarchy Tree", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create asset hierarchy tree failed ! ERROR [%d]", ret)
                break;
            }

            ImTreeNode *favoriteRoot = new ImTreeNode(tree);
            ImTreeNode::CallbackData callbacks(treeNodeClicked, treeNodeRClicked);
            ret = favoriteRoot->createByPath(ICON_NAME_FAVORITE, "Fovorites", callbacks, tree, treeNodeDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create favarites node failed ! ERROR [%d]", ret)
                break;
            }
            UIAssetNode *assetNode = new UIAssetNode(favoriteRoot);
            favoriteRoot->setUserData(assetNode);

            ImTreeNode *node = new ImTreeNode(tree);
            ret = node->createByPath(ICON_NAME_SEARCH, "All Materials", callbacks, favoriteRoot, treeNodeDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create all material node failed ! ERROR [%d]", ret)
                break;
            }
            assetNode = new UIAssetNode(node);
            node->setUserData(assetNode);

            node = new ImTreeNode(tree);
            ret = node->createByPath(ICON_NAME_SEARCH, "All Models", callbacks, favoriteRoot, treeNodeDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create all modles node failed ! ERROR [%d]", ret)
                break;
            }
            assetNode = new UIAssetNode(node);
            node->setUserData(assetNode);

            node = new ImTreeNode(tree);
            ret = node->createByPath(ICON_NAME_SEARCH, "All Prefabs", callbacks, favoriteRoot, treeNodeDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create all prefabs node failed ! ERROR [%d]", ret)
                break;
            }
            assetNode = new UIAssetNode(node);
            node->setUserData(assetNode);

            favoriteRoot->expand(true);

            ImDummyTreeNode *dummyNode = new ImDummyTreeNode(tree);
            ret = dummyNode->create(tree);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create dummy node failed ! ERROR [%d]", ret)
                break;
            }

            ImTreeNode *assetsRoot = new ImTreeNode(tree);
            ret = assetsRoot->createByPath(ICON_NAME_FOLDER, ICON_NAME_FOLDER_OPENED, "Assets", callbacks, tree, treeNodeDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create assets folder node faield ! ERROR [%d]", ret)
                break;
            }
            assetNode = new UIAssetNode(assetsRoot);
            assetsRoot->setUserData(assetNode);

            node = new ImTreeNode(tree);
            ret = node->createByPath(ICON_NAME_FOLDER, ICON_NAME_FOLDER_OPENED, "Scenes", callbacks, assetsRoot, treeNodeDestroy);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create scenes folder node failed ! ERROR [%d]", ret)
                break;
            }
            assetNode = new UIAssetNode(node);
            node->setUserData(assetNode);
            
            assetsRoot->expand(true);
        } while (false);
        
        return ret;
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
            
            UIAssetPathBar *pathBar = new UIAssetPathBar();
            ret = pathBar->create(ID_PROJECT_PATH_BAR, "Asset Path Bar", nullptr, *roots);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create project asset path bar failed ! ERROR [%d]", ret)
                break;
            }

            UIAssetDetailView *detailView = new UIAssetDetailView();
            ret = detailView->create(ID_PROJECT_ASSET_DETAIL_VIEW, "Asset Detail View", nullptr);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create project asset detail view failed ! ERROR [%d]", ret)
                break;
            }

            UIAssetStatusBar *statusBar = new UIAssetStatusBar();
            ret = statusBar->create(ID_PROJECT_STATUS_BAR,  "Asset Status Bar", nullptr);
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
            item.childView = pathBar;
            items.emplace_back(item);
            // 详情视图
            item.size.x = item.size.y = 0;
            item.childView = detailView;
            items.emplace_back(item);
            // 状态栏
            item.size.x = 0;
            item.size.y = -txtHeight;   // 负值，停靠下端，上一个 item 自动计算高度
            item.childView = statusBar;
            items.emplace_back(item);
            
            layout->addWidgets(items);
        } while (false);

        return ret;
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
            auto listItemClicked = [](ImListItem *item)
            {
                EDITOR_LOG_INFO("List item [%s] clicked ", item->getName().c_str());
            };
            
            ImListWidget *list = new ImListWidget();
            ret = list->create(ID_PROJECT_ASSET_THUMB_LIST, "AssetThumbList", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create asset thumb list failed ! ERROR [%d]", ret)
                break;
            }

            const int32_t numOfItems = 50;
            String postfix = "abcdefghijklmn";
            for (int32_t i = 0; i < numOfItems; ++i)
            {
                std::stringstream ss;
                ss << i << postfix;
                ImListItem *item = new ImListItem(list);
                ret = item->createByPath(ICON_NAME_PREFAB, ss.str(), listItemClicked, list);
                if (T3D_FAILED(ret))
                {
                    EDITOR_LOG_ERROR("Create list item failed ! ERROR [%d]", ret)
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                break;
            }
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
            UIAssetHierarchyView *leftView = new UIAssetHierarchyView();
            ret = leftView->create(ID_PROJECT_ASSET_HIERARCHY_VIEW, "ProjectHierarchyView", nullptr);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            UIAssetThumbView *rightView = new UIAssetThumbView();
            ret = rightView->create(ID_PROJECT_ASSET_THUMB_VIEW, "ProjectThumbView", nullptr, leftView->getTreeBarRoots());
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            mSplitView = new ImSplitView();
            ret = mSplitView->create(ID_PROJECT_WINDOW_SPLIT_VIEW, "ProjectSplitView", 0.3f, leftView, 0.7f, rightView, 0, true, this);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
    
    void UIProjectWindow::onGUI()
    {
        
    }
    
    //--------------------------------------------------------------------------

    NS_END
}
