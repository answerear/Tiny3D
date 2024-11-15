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


#include "ImContextMenu.h"
#include "ImErrors.h"
#include "ImWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImContextMenu::~ImContextMenu()
    {
        
    }

    //--------------------------------------------------------------------------

    // TResult ImContextMenu::create(uint32_t id, const String &name)
    // {
    //     return ImWidget::createInternal(id, name, nullptr, 0); 
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult ImContextMenu::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    // {
    //     TResult ret = IM_OK;
    //
    //     do
    //     {
    //         
    //     } while (false);
    //     
    //     return ret;
    // }

    //--------------------------------------------------------------------------

    TResult ImContextMenu::show(ImWidget *parent)
    {
        TResult ret = IM_OK;

        if (getParent() != nullptr)
        {
            getParent()->removeChild(this, false);
        }
            
        if (parent == nullptr)
        {
            ImWindow::getFocusedWindow()->addChild(this);
        }
        else
        {
            parent->addChild(this);
        }

        if (mUniqueName.empty())
        {
            mUniqueName = getUUID().toString();
        }
        
        setVisible(true);
        mWillAppear = true;
        
        return ret;
    }

    //--------------------------------------------------------------------------

    bool ImContextMenu::onGUIBegin()
    {
        PushWidgetID();

        if (mWillAppear)
        {
            ImGui::OpenPopup(mUniqueName.c_str());
            mWillAppear = false;
        }
        
        bool ret = ImGui::BeginPopupContextItem(mUniqueName.c_str());
        if (!ret)
        {
            if (isVisible())
            {
                setVisible(false);
                // ImGui::CloseCurrentPopup();
            }
            PopWidgetID();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    void ImContextMenu::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImContextMenu::onGUIEnd()
    {
        ImGui::EndPopup();
        PopWidgetID();
    }

    //--------------------------------------------------------------------------
}
