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


#include "ProjectWindow.h"
#include "EditorWidgetID.h"
#include "ImErrors.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    TResult AssetHierarchyView::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            auto treeNodeClicked = [](ImTreeNode *node)
            {
                EDITOR_LOG_INFO("Tree node [%s] clicked ", node->getName().c_str());
            };
            
            ImTreeWidget *tree = new ImTreeWidget();
            ret = tree->create(ID_PROJECT_ASSET_HIERARCHY_TREE, "Asset Hierarchy Tree", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create asset hierarchy tree failed ! ERROR [%d]", ret)
                break;
            }

            ImTreeNode *favoriteRoot = new ImTreeNode(tree);
            ret = favoriteRoot->createEx("Editor/icons/d_Favorite@32.png", "Fovorites", treeNodeClicked, tree);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create favarites node failed ! ERROR [%d]", ret)
                break;
            }

            ImTreeNode *node = new ImTreeNode(tree);
            ret = node->createEx("Editor/icons/d_Search@32.png", "All Materials", treeNodeClicked, favoriteRoot);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create all material node failed ! ERROR [%d]", ret)
                break;
            }

            node = new ImTreeNode(tree);
            ret = node->createEx("Editor/icons/d_Search@32.png", "All Models", treeNodeClicked, favoriteRoot);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create all modles node failed ! ERROR [%d]", ret)
                break;
            }

            node = new ImTreeNode(tree);
            ret = node->createEx("Editor/icons/d_Search@32.png", "All Prefabs", treeNodeClicked, favoriteRoot);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create all prefabs node failed ! ERROR [%d]", ret)
                break;
            }

            favoriteRoot->expand(true);

            ImDummyTreeNode *dummyNode = new ImDummyTreeNode(tree);
            ret = dummyNode->create(tree);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create dummy node failed ! ERROR [%d]", ret)
                break;
            }

            ImTreeNode *assetsRoot = new ImTreeNode(tree);
            ret = assetsRoot->createEx("Editor/icons/d_Folder@32.png", "Editor/icons/d_FolderOpened@32.png", "Assets", treeNodeClicked, tree);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create assets folder node faield ! ERROR [%d]", ret)
                break;
            }

            node = new ImTreeNode(tree);
            ret = node->createEx("Editor/icons/d_Folder@32.png", "Editor/icons/d_FolderOpened@32.png", "Scenes", treeNodeClicked, assetsRoot);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create scenes folder node failed ! ERROR [%d]", ret)
                break;
            }

            assetsRoot->expand(true);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void AssetHierarchyView::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    ImGuiChildFlags AssetHierarchyView::onGetChildFlags()
    {
        return ImGuiChildFlags_Border;
    }

    //--------------------------------------------------------------------------

    void AssetThumbView::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    ImGuiChildFlags AssetThumbView::onGetChildFlags()
    {
        return ImGuiChildFlags_Border;
    }

    //--------------------------------------------------------------------------

    TResult ProjectWindow::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            AssetHierarchyView *leftView = new AssetHierarchyView();
            ret = leftView->create(ID_PROJECT_ASSET_HIERARCHY_VIEW, "ProjectHierarchyView", nullptr);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            AssetThumbView *rightView = new AssetThumbView();
            ret = rightView->create(ID_PROJECT_ASSET_THUMB_VIEW, "ProjectThumbView", nullptr);
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
    
    void ProjectWindow::onGUI()
    {
        
    }
    
    //--------------------------------------------------------------------------

    NS_END
}
