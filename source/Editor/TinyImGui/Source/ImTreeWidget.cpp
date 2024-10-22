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
#include "ImEventDefine.h"


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

    uint32_t ImTreeNode::getNewNodeID() const
    {
        return mTreeWidget != nullptr ? mTreeWidget->generateNodeID() : 0;
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::createByTexture(ImTextureID texID, const String &title, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy)
    {
        uint32_t id = getNewNodeID();
        return ImWidget::createInternal(id, title, parent, 6, texID, nullptr, nullptr, nullptr, nullptr, &onDestroy);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::createByTexture(ImTextureID texID, const String &title, const CallbackData &callbacks, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy)
    {
        uint32_t id = getNewNodeID();
        return ImWidget::createInternal(id, title, parent, 6, texID, nullptr, nullptr, nullptr, &callbacks, &onDestroy);
    }
    
    //--------------------------------------------------------------------------

    TResult ImTreeNode::createByPath(const String &imageName, const String &title, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy)
    {
        uint32_t id = getNewNodeID();
        return ImWidget::createInternal(id, title, parent, 6, nullptr, &imageName, nullptr, nullptr, nullptr, &onDestroy);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::createByPath(const String &imageName, const String &title, const CallbackData &callbacks, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy)
    {
        uint32_t id = getNewNodeID();
        return ImWidget::createInternal(id, title, parent, 6, nullptr, &imageName, nullptr, nullptr, &callbacks, &onDestroy);
    }
    
    //--------------------------------------------------------------------------

    TResult ImTreeNode::createByTexture(ImTextureID collapsedTexID, ImTextureID openedTexID, const String &title, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy)
    {
        uint32_t id = getNewNodeID();
        return ImWidget::createInternal(id, title, parent, 6, collapsedTexID, nullptr, openedTexID, nullptr, nullptr, &onDestroy);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::createByTexture(ImTextureID collapsedTexID, ImTextureID openedTexID, const String &title, const CallbackData &callbacks, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy)
    {
        uint32_t id = getNewNodeID();
        return ImWidget::createInternal(id, title, parent, 6, collapsedTexID, nullptr, openedTexID, nullptr, &callbacks, &onDestroy);
    }
    
    //--------------------------------------------------------------------------

    TResult ImTreeNode::createByPath(const String &collapsedImage, const String &openedImage, const String &title, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy)
    {
        uint32_t id = getNewNodeID();
        return ImWidget::createInternal(id, title, parent, 6, nullptr, &collapsedImage, nullptr, &openedImage, nullptr, &onDestroy);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::createByPath(const String &collapsedImage, const String &openedImage, const String &title, const CallbackData &callbacks, ImTreeNode *parent, const ImTreeNodeDestroyCallback &onDestroy)
    {
        uint32_t id = getNewNodeID();
        return ImWidget::createInternal(id, title, parent, 6, nullptr, &collapsedImage, nullptr, &openedImage, &callbacks, &onDestroy);
    }
    
    //--------------------------------------------------------------------------

    // argc : 6
    // args :
    //  ImTextureID : 折叠状态的图标纹理对象
    //  String* : 折叠状态的图标图像路径
    //  ImTextureID : 展开状态的图标纹理对象
    //  String* : 展开状态的图标图像路径
    //  CallbackData* : 除删除外的各种回调函数
    //  ImTreeNodeDestroyCallback* : 删除回调函数
    TResult ImTreeNode::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc == 6);
        
        TResult ret = IM_OK;

        do
        {
            // collapse icon texture
            mIconID = va_arg(args, ImTextureID);
            // collapse icon path
            String *imageName = va_arg(args, String*);
            if (mIconID == nullptr)
            {
                mIconID = IM_TEXTURE_MGR.loadTexture(*imageName);
                if (mIconID == nullptr)
                {
                    IMGUI_LOG_ERROR("Load texture %s for TreeNode collapsed icon failed ! ERROR [%d]", imageName->c_str(), ret)
                    ret = T3D_ERR_FAIL;
                    break;
                }
                mIsInternalLoaded = true;
            }

            // opened icon texture
            mOpenedIconID = va_arg(args, ImTextureID);
            // opened icon path
            imageName = va_arg(args, String*);
            if (mOpenedIconID == nullptr && imageName != nullptr)
            {
                mOpenedIconID = IM_TEXTURE_MGR.loadTexture(*imageName);
                if (mOpenedIconID == nullptr)
                {
                    IMGUI_LOG_ERROR("Load texture %s for TreeNode opened icon failed ! ERROR [%d]", imageName->c_str(), ret)
                    ret = T3D_ERR_FAIL;
                    break;
                }
                mIsInternalLoaded = true;
            }

            // clicked callback
            CallbackData *callbacks = va_arg(args, CallbackData*);
            if (callbacks != nullptr)
            {
                mClickedCallback = callbacks->clicked;
                mRClickedCallback = callbacks->rclicked;
            }

            ImTreeNodeDestroyCallback *destroyCallback = va_arg(args, ImTreeNodeDestroyCallback*);
            if (destroyCallback != nullptr)
            {
                mDestroyCallback = *destroyCallback;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::addNode(ImTreeNode *node)
    {
        return ImTreeNode::addChild(node) ? IM_OK : IM_ERR_FAIL;
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::collapse(bool recursively)
    {
        TResult ret = IM_OK;

        mIsExpanded = false;
        mIsUpdateExpanding = true;

        if (recursively)
        {
            for (auto child : getChildren())
            {
                T3D_ASSERT(child->getWidgetType() == WidgetType::kTreeNode);
                ImTreeNode *node = static_cast<ImTreeNode*>(child);
                node->collapse(recursively);
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::expand(bool recursively)
    {
        TResult ret = IM_OK;

        mIsExpanded = true;
        mIsUpdateExpanding = true;

        if (recursively)
        {
            for (auto child : getChildren())
            {
                T3D_ASSERT(child->getWidgetType() == WidgetType::kTreeNode);
                ImTreeNode *node = static_cast<ImTreeNode*>(child);
                node->expand(recursively);
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void ImTreeNode::onDestroy()
    {
        if (mIsInternalLoaded)
        {
            if (mIconID != nullptr)
            {
                IM_TEXTURE_MGR.unloadTexture(mIconID);
                mIconID = nullptr;
            }

            if (mOpenedIconID != nullptr)
            {
                IM_TEXTURE_MGR.unloadTexture(mOpenedIconID);
                mOpenedIconID = nullptr;
            }
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

    //--------------------------------------------------------------------------

    void ImTreeNode::drawIconAndText(bool opened)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(TREENODE_SPACING_X, 0.0f));

        ImGui::SameLine();
        
        float x = ImGui::GetCursorPosX();
        bool hasChildren = !getChildren().empty();
        if (hasChildren)
        {
            x += TREENODE_ICON_OFFSET_X;
        }
        else
        {
            x += TREENODE_LEAF_ICON_OFFSET_X;
        }
        
        ImGui::SetCursorPosX(x);
            
        // 图标
        auto itemSize = ImGui::GetItemRectSize();
        ImTextureID icon = (opened && hasChildren && mOpenedIconID != nullptr) ? mOpenedIconID : mIconID;
        ImGui::Image(icon, ImVec2(itemSize.y, itemSize.y));
        ImGui::SameLine();
        // 文本
        ImGui::Text(getName().c_str());

        ImGui::PopStyleVar();
    }

    //--------------------------------------------------------------------------

    void ImTreeNode::fireClickedEvent()
    {
        if (mClickedCallback != nullptr)
        {
            mClickedCallback(this);
        }
        else if (mTreeWidget != nullptr)
        {
            EventParamTreeNodeClicked param(this);
            mTreeWidget->sendClickedEvent(kEvtTreeNodeClicked, &param);
        }
    }

    //--------------------------------------------------------------------------

    void ImTreeNode::fireRClickedEvent()
    {
        if (mRClickedCallback != nullptr)
        {
            mRClickedCallback(this);
        }
        else if (mTreeWidget != nullptr)
        {
            EventParamTreeNodeRClicked param(this);
            mTreeWidget->sendClickedEvent(kEvtTreeNodeRClicked, &param);
        }
    }

    //--------------------------------------------------------------------------

    bool ImTreeNode::onGUIBegin(const ImVec2 &size)
    {
        mTreeWidgetSize = size;
        
        // 告诉 imgui 一个唯一 ID
        PushWidgetID();

        // 生成唯一字符串
        String strID = getUniqueName();
        
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow;
        float offset_x = TREENODE_ICON_OFFSET_X;
        if (getChildren().empty())
        {
            // 叶子节点
            flags |= ImGuiTreeNodeFlags_Leaf;
            offset_x = TREENODE_LEAF_ICON_OFFSET_X;
        }

        if (mTreeWidget != nullptr && mTreeWidget->getSelectedNode() == strID)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        if (mIsUpdateExpanding)
        {
            if (mIsExpanded)
            {
                ImGui::SetNextItemOpen(true);
            }
            else
            {
                ImGui::SetNextItemOpen(false);
            }
            mIsUpdateExpanding = false;
        }

        bool ret = ImGui::TreeNodeEx(strID.c_str(), flags);
        if (!ret)
        {
            drawIconAndText(false);

            PopWidgetID();
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen())
        {
            fireClickedEvent();
        }
        // else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly|ImGuiHoveredFlags_AllowWhenOverlapped))
        {
            fireRClickedEvent();
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void ImTreeNode::onGUI()
    {
        String strID = getUniqueName();
        
        if (mTreeWidget != nullptr)
        {
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen())
            {
                mTreeWidget->getSelectedNode() = strID;
            }
            // else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly|ImGuiHoveredFlags_AllowWhenOverlapped))
            {
                mTreeWidget->getSelectedNode() = strID;
            }
        }

        drawIconAndText(true);
    }

    //--------------------------------------------------------------------------

    void ImTreeNode::onGUIEnd()
    {
        ImGui::TreePop();
        PopWidgetID();
    }

    //--------------------------------------------------------------------------

    bool ImTreeNode::addWidget(const String &parentName, ImWidget *widget)
    {
        return ImWidget::addWidget(parentName, widget);
    }

    //--------------------------------------------------------------------------

    bool ImTreeNode::addWidget(const UUID &parentUUID, ImWidget *widget)
    {
        return ImWidget::addWidget(parentUUID, widget);
    }

    //--------------------------------------------------------------------------

    bool ImTreeNode::addWidget(uint32_t parentID, ImWidget *widget)
    {
        return ImWidget::addWidget(parentID, widget);
    }
    
    //--------------------------------------------------------------------------

    bool ImTreeNode::addChild(ImWidget *widget)
    {
        return ImWidget::addChild(widget);
    }

    //--------------------------------------------------------------------------

    bool ImTreeNode::insertAfterChild(const String &prevName, ImWidget *widget)
    {
        return ImWidget::insertAfterChild(prevName, widget);
    }

    //--------------------------------------------------------------------------

    bool ImTreeNode::insertAfterChild(const UUID &prevUUID, ImWidget *widget)
    {
        return ImWidget::insertAfterChild(prevUUID, widget);
    }
    
    //--------------------------------------------------------------------------

    bool ImTreeNode::insertAfterChild(ImWidget *prevWidget, ImWidget *widget)
    {
        return ImWidget::insertAfterChild(prevWidget, widget);
    }

    //--------------------------------------------------------------------------

    
    bool ImTreeNode::insertAfterChild(uint32_t prevID, ImWidget *widget)
    {
        return ImWidget::insertAfterChild(prevID, widget);
    }
    
    //--------------------------------------------------------------------------

    ImTreeWidget::ImTreeWidget()
        : ImTreeNode(nullptr)
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
        mNodeID = id;
        return IM_OK;
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

    void ImTreeWidget::sendClickedEvent(uint32_t evt, EventParamTreeNodeClicked *param)
    {
        sendEvent(evt, param);
    }

    //--------------------------------------------------------------------------

    ImDummyTreeNode::ImDummyTreeNode(ImTreeWidget *tree)
        : ImTreeNode(tree)
    {
        
    }

    //--------------------------------------------------------------------------

    ImDummyTreeNode::~ImDummyTreeNode()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImDummyTreeNode::create(ImTreeNode *parent)
    {
        uint32_t id = getNewNodeID();
        return ImWidget::create(id, "##DUMMY_TREE_NODE", parent);
    }

    //--------------------------------------------------------------------------

    TResult ImDummyTreeNode::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        return ImWidget::createInternal(id, name, parent, argc, args);
    }

    //--------------------------------------------------------------------------

    bool ImDummyTreeNode::onGUIBegin()
    {
        PushWidgetID();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;
        String strID = getUniqueName();
        bool ret = ImGui::TreeNodeEx(strID.c_str(), flags);
        if (!ret)
        {
            PopWidgetID();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    bool ImDummyTreeNode::onGUIBegin(const ImVec2 &size)
    {
        return onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void ImDummyTreeNode::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImDummyTreeNode::onGUIEnd()
    {
        ImGui::TreePop();
        PopWidgetID();
    }

    //--------------------------------------------------------------------------
}
