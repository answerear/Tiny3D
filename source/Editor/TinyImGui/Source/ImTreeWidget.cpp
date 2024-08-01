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

#include "ImErrors.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImTreeNode::~ImTreeNode()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::create(uint32_t id, ImTextureID texID, const String &title, ImTreeNode *parent)
    {
        return ImWidget::createInternal(id, title, parent, 1, texID);
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc == 1, "Invalid number of arguments in ImTreeNode::create() !");
        
        TResult ret = IM_OK;

        do
        {
            mIconID = va_arg(args, ImTextureID);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImTreeNode::addNode(ImTreeNode *node)
    {
        return ImTreeNode::addChild(node);
    }

    //--------------------------------------------------------------------------

    bool ImTreeNode::onGUIBegin()
    {
        PushWidgetID();
        std::stringstream ss;
        ss << "##" << getName();
        if (!ImGui::TreeNode(ss.str().c_str()))
        {
            PopWidgetID();
            return false;
        }
        return true;
    }

    //--------------------------------------------------------------------------

    void ImTreeNode::onGUI()
    {        
        ImGui::SameLine();
        ImGui::Image(mIconID, ImVec2(16, 16));
        ImGui::SameLine();
        ImGui::Text(getName().c_str());
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

    ImTreeWidget::~ImTreeWidget()
    {
        
    }

    //--------------------------------------------------------------------------

    bool ImTreeWidget::onGUIBegin()
    {
        PushWidgetID();
        return true;
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
