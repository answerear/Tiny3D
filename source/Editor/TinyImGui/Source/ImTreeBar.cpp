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


#include "ImTreeBar.h"
#include "ImEventDefine.h"
#include "ImErrors.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImTreeBar::~ImTreeBar()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImTreeBar::create(uint32_t id, const String &name, ImWidget *parent, const TreeBarNodes &roots, const ImTreeBarNodeClickedCallback &clickedNode)
    {
        return ImWidget::createInternal(id, name, parent, 2, &roots, &clickedNode);
    }

    //--------------------------------------------------------------------------

    // argc : 2
    // args :
    //  TreeBarNodes* : 根节点列表
    //  ImTreeBarClickedCallback* : 点击回调
    TResult ImTreeBar::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc == 2);
        
        TResult ret = IM_OK;

        do
        {
            TreeBarNodes *roots = va_arg(args, TreeBarNodes*);
            mRoots = *roots;
            ImTreeBarNodeClickedCallback *nodeCB = va_arg(args, ImTreeBarNodeClickedCallback*);
            mClickedNodeCB = *nodeCB;
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ImTreeBar::setSelectedNode(ImTreeBarNode *node)
    {
        if (mSelectedNode != node)
        {
            mSelectedNode = node;

            mSelectedNodes.clear();
            
            ImTreeBarNode *parent = node;
            while (parent != nullptr)
            {
                mSelectedNodes.emplace_front(parent);
                parent = parent->getParent();
            }
        }
    }

    //--------------------------------------------------------------------------

    bool ImTreeBar::onGUIBegin()
    {
        return ImWidget::onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void ImTreeBar::onGUI()
    {
        size_t i = 0;

        ImGuiStyle &style = ImGui::GetStyle();
        ImVec2 separatorSize = ImGui::CalcTextSize(">");
        float textHeight = ImGui::GetTextLineHeightWithSpacing();

        auto startPos = ImGui::GetCursorPos();
        startPos.x += style.ItemSpacing.x;
        ImGui::SetCursorPos(startPos);
        
        for (auto itr = mSelectedNodes.begin(); itr != mSelectedNodes.end(); ++itr, ++i)
        {
            auto node = *itr;

            // 文件夹名称以及点击按钮
            ImVec2 textSize = ImGui::CalcTextSize(node->getName().c_str());
            ImVec2 buttonSize = ImVec2(textSize.x, textHeight);
            auto pos = ImGui::GetCursorPos();
            if (ImGui::InvisibleButton(node->getName().c_str(), buttonSize))
            {
                fireClickedNodeEvent(node);
            }
            ImGui::SetCursorPos(pos);
            ImGui::AlignTextToFramePadding();
            ImGui::Text(node->getName().c_str());

            if (!node->getChildren().empty() && i != mSelectedNodes.size() - 1)
            {
                // 非空，则显示分隔符
                ImGui::SameLine(0, 0);
            
                // 文件夹分隔符
                if (i != mSelectedNodes.size() - 1 || mSelectedNodes.size() == 1)
                {
                    separatorSize.x += style.ItemSpacing.x + style.ItemSpacing.x;
                }
                separatorSize.y = textHeight;
                std::stringstream ss;
                ss << ">##" << i;
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                if (ImGui::Button(ss.str().c_str(), separatorSize))
                {
                    // 点击分隔符，直接出子文件夹菜单
                    ImGui::OpenPopup(node->getName().c_str());
                }
                ImGui::PopStyleColor();

                // 弹出菜单
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                if (ImGui::BeginPopup(node->getName().c_str()))
                {
                    for (auto child : node->getChildren())
                    {
                        if (ImGui::Button(child->getName().c_str()))
                        {
                            fireClickedNodeEvent(child);
                        }
                    }
                    ImGui::EndPopup();
                }
                ImGui::PopStyleColor();
            
                ImGui::SameLine(0, 0);
            }
        }
    }

    //--------------------------------------------------------------------------

    void ImTreeBar::onGUIEnd()
    {
        ImWidget::onGUIEnd();
    }

    //--------------------------------------------------------------------------

    void ImTreeBar::fireClickedNodeEvent(ImTreeBarNode *node)
    {
        if (mClickedNodeCB != nullptr)
        {
            mClickedNodeCB(node);
        }
        else
        {
            EventParamTreeBarNodeClicked param(node);
            sendEvent(kEvtTreeBarNodeClicked, &param);
        }
    }

    //--------------------------------------------------------------------------
}
