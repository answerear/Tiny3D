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


#include "ImDialog.h"
#include "ImErrors.h"
#include "ImChildView.h"
#include "ImWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImDialog::DialogQueue ImDialog::msDialogQueue;
    ImDialog::DialogStack ImDialog::msDialogStack;
    
    //--------------------------------------------------------------------------

    ImDialog::~ImDialog()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImDialog::show(ShowType type)
    {
        TResult ret = IM_ERR_FAIL;
        
        switch (type)
        {
        case ShowType::kEnqueueBack:
            {
                ret = enqueueBack();
            }
            break;
        case ShowType::kEnqueueFront:
            {
                ret = enqueueFront();
            }
            break;
        case ShowType::kImmediately:
            {
                ret = showImmediately();
            }
            break;
        case ShowType::kOverlay:
            {
                ret = showOverlay();
            }
            break;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void ImDialog::appear(bool overlay)
    {
        msDialogStack.push(this);
        mWillAppear = true;

        ImWidget *parent = nullptr;
        
        if (overlay)
        {
            parent = msDialogStack.top();
        }
        else
        {
            ImChildView *focusedView = ImChildView::getFocusedView();
            if (focusedView != nullptr && focusedView->isVisible())
            {
                parent = focusedView;
            }
            else
            {
                ImWindow *focusedWindow = ImWindow::getFocusedWindow();
                if (focusedWindow != nullptr)
                {
                    parent = focusedWindow;
                }
            }
        }

        T3D_ASSERT(parent != nullptr, "DoShow in dialog, parent must not be nullptr !");
        parent->addWidget(this);
        setVisible(true);
    }

    //--------------------------------------------------------------------------

    TResult ImDialog::enqueueBack()
    {
        TResult ret = IM_OK;

        do
        {
            if (msDialogStack.empty() && msDialogQueue.empty())
            {
                // 当前没有任何对话框在显示，同时也没有排队等待显示的，立刻显示
                appear(false);
            }
            else
            {
                // 放入队尾
                msDialogQueue.emplace_back(this);
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImDialog::enqueueFront()
    {
        TResult ret = IM_OK;

        do
        {
            if (msDialogStack.empty() && msDialogQueue.empty())
            {
                // 当前没有任何对话框在显示，同时也没有排队等待显示的，立刻显示
                appear(false);
            }
            else
            {
                // 放入队首
                msDialogQueue.emplace_front(this);
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImDialog::showImmediately()
    {
        TResult ret = IM_OK;

        do
        {
            while (!msDialogStack.empty())
            {
                // 正在显示的对话框，直接关闭
                auto dialog = msDialogStack.top();
                dialog->close();
                msDialogStack.pop();
            }

            // 直接显示要出现的对话框
            appear(false);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImDialog::showOverlay()
    {
        TResult ret = IM_OK;

        do
        {
            // 直接叠加在上面显示
            appear(true);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ImDialog::close(bool destroy)
    {
        if (mInUpdate)
        {
            ImGui::CloseCurrentPopup();
        }
        else
        {
            mShouldClose = true;
        }

        if (destroy)
        {
            this->destroy();
        }
    }
    
    //--------------------------------------------------------------------------

    void ImDialog::update()
    {
        mInUpdate = true;
        ImWidget::update();
        mInUpdate = false;
    }

    //--------------------------------------------------------------------------

    bool ImDialog::onGUIBegin()
    {
        ImGuiWindowClass wndclass;
        wndclass.ViewportFlagsOverrideSet = ImGuiViewportFlags_TopMost;
        ImGui::SetNextWindowClass(&wndclass);

        PushWidgetID();
        
        if (mWillAppear)
        {
            ImGui::OpenPopup(getName().c_str());

            ImVec2 center = ImGui::GetWindowViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowFocus();
            
            mWillAppear = false;
        }

        bool visible = mVisible;
        bool ret = ImGui::BeginPopupModal(getName().c_str(), &mVisible, ImGuiWindowFlags_AlwaysAutoResize);
        if (!ret)
        {
            if (visible != mVisible)
            {
                // 点 X 关闭了
                if (msDialogStack.top() == this)
                {
                    msDialogStack.pop();
                }
            }
            
            PopWidgetID();
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ImDialog::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImDialog::onGUIEnd()
    {
        if (mShouldClose)
        {
            ImGui::CloseCurrentPopup();
            mShouldClose = false;
        }
        
        ImGui::EndPopup();
        PopWidgetID();
    }

    //--------------------------------------------------------------------------
}
