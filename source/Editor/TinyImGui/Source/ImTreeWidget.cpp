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


#include "ImTreeWidget.h"
#include "ImTextureManager.h"
#include "ImErrors.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImTreeNode::ImTreeNode(ImTreeWidget *tree)
        : mTreeWidget(tree)
    {
        
    }
    
    //--------------------------------------------------------------------------

    ImTreeNode::~ImTreeNode()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::create(uint32_t id, ImTextureID texID, const String &title, ImTreeNode *parent)
    {
        return ImWidget::createInternal(id, title, parent, 2, texID, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::createEx(uint32_t id, const String &imageName, const String &title, ImTreeNode *parent)
    {
        return ImWidget::createInternal(id, title, parent, 2, nullptr, &imageName);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc == 2, "Invalid number of arguments in ImTreeNode::create() !");
        
        TResult ret = IM_OK;

        do
        {
            mIconID = va_arg(args, ImTextureID);
            String *imageName = va_arg(args, String*);
            if (mIconID == nullptr)
            {
                mIconID = IM_TEXTURE_MGR.loadTexture(*imageName);
                if (mIconID == nullptr)
                {
                    IMGUI_LOG_ERROR("Load texture %s for TreeNode failed ! ERROR [%d]", imageName->c_str(), ret)
                    ret = T3D_ERR_FAIL;
                    break;
                }
                mIsInternalLoaded = true;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::addNode(ImTreeNode *node)
    {
        return ImTreeNode::addChild(node);
    }

    //--------------------------------------------------------------------------

    void ImTreeNode::onDestroy()
    {
        if (mIsInternalLoaded && mIconID != nullptr)
        {
            IM_TEXTURE_MGR.unloadTexture(mIconID);
            mIconID = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    String ImTreeNode::getUniqueName() const
    {
        std::stringstream ss;
        ss << "##" << getUUID().values.high << getUUID().values.low;
        return ss.str();
    }

    //--------------------------------------------------------------------------

    #define TREENODE_SPACING_X              (2.0f)
    #define TREENODE_ICON_OFFSET_X          (-8.0f)
    #define TREENODE_LEAF_ICON_OFFSET_X     (-16.0f)

    bool ImTreeNode::onGUIBegin(const ImVec2 &size)
    {
        mTreeWidgetSize = size;
        
        // 告诉 imgui 一个唯一 ID
        PushWidgetID();

        // 生成唯一字符串
        String strID = getUniqueName();
        
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow;
        float offset_x = TREENODE_ICON_OFFSET_X;
        if (getChildren().empty())
        {
            // 叶子节点
            flags |= ImGuiTreeNodeFlags_Leaf;
            offset_x = TREENODE_LEAF_ICON_OFFSET_X;
        }

        if (mTreeWidget->getSelectedNode() == strID)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool ret = ImGui::TreeNodeEx(strID.c_str(), flags);
        if (!ret)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(TREENODE_SPACING_X, 0.0f));

            ImGui::SameLine();
            float x = ImGui::GetCursorPosX();
            x += offset_x;
            ImGui::SetCursorPosX(x);
            
            // 图标
            auto itemSize = ImGui::GetItemRectSize();
            ImGui::Image(mIconID, ImVec2(itemSize.y, itemSize.y));
            ImGui::SameLine();
            // 文本
            ImGui::Text(getName().c_str());

            ImGui::PopStyleVar();

            PopWidgetID();
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void ImTreeNode::onGUI()
    {
        String strID = getUniqueName();
        
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            mTreeWidget->getSelectedNode() = strID;
        }
        
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(TREENODE_SPACING_X, 0.0f));

        auto itemSize = ImGui::GetItemRectSize();
        
        ImGui::SameLine();
        
        float x = ImGui::GetCursorPosX();
        if (getChildren().empty())
        {
            x += TREENODE_LEAF_ICON_OFFSET_X;
        }
        else
        {
            x += TREENODE_ICON_OFFSET_X;
        }
        
        ImGui::SetCursorPosX(x);
        
        // 图标
        ImGui::Image(mIconID, ImVec2(itemSize.y, itemSize.y));
        ImGui::SameLine();
        // 文本
        ImGui::Text(getName().c_str());
        
        ImGui::PopStyleVar();
    }

    //--------------------------------------------------------------------------

    void ImTreeNode::onGUIEnd()
    {
        ImGui::TreePop();
        PopWidgetID();
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::addWidget(const String &parentName, ImWidget *widget)
    {
        return ImWidget::addWidget(parentName, widget);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::addWidget(const UUID &parentUUID, ImWidget *widget)
    {
        return ImWidget::addWidget(parentUUID, widget);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::addWidget(uint32_t parentID, ImWidget *widget)
    {
        return ImWidget::addWidget(parentID, widget);
    }
    
    //--------------------------------------------------------------------------

    TResult ImTreeNode::addChild(ImWidget *widget)
    {
        return ImWidget::addChild(widget);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::insertAfterChild(const String &prevName, ImWidget *widget)
    {
        return ImWidget::insertAfterChild(prevName, widget);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::insertAfterChild(const UUID &prevUUID, ImWidget *widget)
    {
        return ImWidget::insertAfterChild(prevUUID, widget);
    }
    
    //--------------------------------------------------------------------------

    TResult ImTreeNode::insertAfterChild(ImWidget *prevWidget, ImWidget *widget)
    {
        return ImWidget::insertAfterChild(prevWidget, widget);
    }

    //--------------------------------------------------------------------------

    
    TResult ImTreeNode::insertAfterChild(uint32_t prevID, ImWidget *widget)
    {
        return ImWidget::insertAfterChild(prevID, widget);
    }
    
    //--------------------------------------------------------------------------

    ImTreeWidget::ImTreeWidget()
        : ImTreeNode(this)
    {
        
    }
    
    //--------------------------------------------------------------------------

    ImTreeWidget::~ImTreeWidget()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImTreeWidget::create(uint32_t id, const String &name, ImWidget *parent)
    {
        return ImWidget::createInternal(id, name, parent, 0);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeWidget::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        return ImWidget::createInternal(id, name, parent, argc, args);
    }

    //--------------------------------------------------------------------------

    void ImTreeWidget::update()
    {
        auto region = ImGui::GetContentRegionAvail();

        mContentPos = ImGui::GetCursorPos();
        
        if (mVisible && onGUIBegin(region))
        {
            onGUI();

            for (auto child : getChildren())
            {
                child->update(region);
            }
            
            onGUIEnd();
        }
    }

    //--------------------------------------------------------------------------

    bool ImTreeWidget::onGUIBegin()
    {
        PushWidgetID();
        return true;
    }

    //--------------------------------------------------------------------------

    bool ImTreeWidget::onGUIBegin(const ImVec2 &size)
    {
        return onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void ImTreeWidget::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImTreeWidget::onGUIEnd()
    {
        PopWidgetID();
    }

    //--------------------------------------------------------------------------
}
