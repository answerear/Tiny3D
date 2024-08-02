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


#include "ImDockItem.h"
#include "ImCommon.h"
#include "ImErrors.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImDockItem::~ImDockItem()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImDockItem::create(uint32_t id, const String &name, ImWidget *parent, ImGuiID dockID)
    {
        return ImWidget::createInternal(id, name, parent, 1, dockID);
    }

    //--------------------------------------------------------------------------

    TResult ImDockItem::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 1);

        TResult ret = IM_OK;

        do
        {
            ret = ImChildView::createInternal(id, name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // dock id
            mDockID = va_arg(args, ImGuiID);
        } while (false);
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool ImDockItem::onGUIBegin()
    {
        PushWidgetID();
        ImGuiWindowClass wndClass;
        wndClass.ClassId = mDockID;
        wndClass.DockNodeFlagsOverrideSet = 0;
        wndClass.DockingAllowUnclassed = true;
        ImGui::SetNextWindowClass(&wndClass);
        
        if (!ImGui::Begin(getDockWindowName(getName(), mDockID).c_str(), &mVisible, ImGuiWindowFlags_NoCollapse))
        {
            ImGui::End();
            PopWidgetID();
            return false;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    void ImDockItem::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImDockItem::onGUIEnd()
    {
        ImGui::End();
        PopWidgetID();
    }

    //--------------------------------------------------------------------------
}
