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


#include "ImWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImWindow *ImWindow::msFocusedWindow = nullptr;
    
    //--------------------------------------------------------------------------

    ImWindow *ImWindow::getFocusedWindow()
    {
        return msFocusedWindow;
    }

    //--------------------------------------------------------------------------

    ImWindow::~ImWindow()
    {
        
    }

    //--------------------------------------------------------------------------

    bool ImWindow::onGUIBegin()
    {
        PushWidgetID();
        if (!ImGui::Begin(getName().c_str(), &mVisible))
        {
            ImGui::End();
            PopWidgetID();
            return false;
        }

        checkFocused();

        return true;
    }

    //--------------------------------------------------------------------------

    void ImWindow::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImWindow::onGUIEnd()
    {
        ImGui::End();
        PopWidgetID();
    }

    //--------------------------------------------------------------------------

    void ImWindow::checkFocused()
    {
        // if (msFocusedWindow != this && ImGui::IsWindowFocused())
        // {
        //     // 获得焦点
        //     msFocusedWindow = this;
        // }
        // else if (msFocusedWindow == this && !ImGui::IsWindowFocused())
        // {
        //     // 失去焦点
        //     msFocusedWindow = nullptr;
        // }
        msFocusedWindow = this;
    }

    //--------------------------------------------------------------------------
}
