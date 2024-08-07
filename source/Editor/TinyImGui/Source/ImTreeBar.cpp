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

#include "ImErrors.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImTreeBar::~ImTreeBar()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImTreeBar::create(uint32_t id, const String &name, ImWidget *parent, ImTreeBarNode *root, const ImTreeBarNodeClickedCallback &clickedNode)
    {
        return ImWidget::createInternal(id, name, parent, 2, root, &clickedNode);
    }

    //--------------------------------------------------------------------------

    // argc : 2
    // args :
    //  Node* : 根节点
    //  ImTreeBarClickedCallback* : 点击回调
    TResult ImTreeBar::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc == 1);
        
        TResult ret = IM_OK;

        do
        {
            mRoot = va_arg(args, ImTreeBarNode*);
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
        int32_t i = 0;

        ImVec2 separatorSize = ImGui::CalcTextSize(">");
        float textHeight = ImGui::GetTextLineHeightWithSpacing();
        
        for (auto node : mSelectedNodes)
        {
            ImVec2 textSize = ImGui::CalcTextSize(node->getName().c_str());
            ImVec2 buttonSize = ImVec2(textSize.x, textHeight);
            if (ImGui::InvisibleButton(node->getName().c_str(), buttonSize))
            {
                
            }
            ImGui::SameLine(0, 0);
            std::stringstream ss;
            ss << ">##" << i;
            separatorSize.y = textHeight;
            if (ImGui::Button(ss.str().c_str(), separatorSize))
            {
                
            }
            ImGui::SameLine(0, 0);
        }
    }

    //--------------------------------------------------------------------------

    void ImTreeBar::onGUIEnd()
    {
        ImWidget::onGUIEnd();
    }

    //--------------------------------------------------------------------------
}
