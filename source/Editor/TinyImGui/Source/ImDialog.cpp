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
#include "../../TinyLauncher/LauncherPrerequisites.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImDialog::DialogQueue ImDialog::msDialogQueue;
    ImDialog::DialogStack ImDialog::msDialogStack;
    
    //--------------------------------------------------------------------------

    void ImDialog::poll()
    {
        if (!msDialogQueue.empty())
        {
            auto dialog = msDialogQueue.front();
            msDialogQueue.pop_front();
            dialog->appear(false);
        }
    }

    //--------------------------------------------------------------------------

    ImDialog::~ImDialog()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImDialog::create(const String &name, ImWidget *parent, bool hasCloseBtn)
    {
        return ImWidget::createInternal(name, parent, 1, hasCloseBtn);
    }

    //--------------------------------------------------------------------------

    TResult ImDialog::createInternal(const String &name, ImWidget *parent, int32_t argc, va_list args)
    {
        T3D_ASSERT(argc == 1, "Invalid number of arguments in ImDialog::create() !");

        mHasCloseBtn = va_arg(args, bool);
        return IM_OK;
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

        msDialogStack.push(this);
        mWillAppear = true;
        
        T3D_ASSERT(parent != nullptr, "DoShow in dialog, parent must not be nullptr !");
        parent->addWidget(this);
        setVisible(true);

        onShow();
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
            onClose();
            ImGui::CloseCurrentPopup();
            msDialogStack.pop();
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
        bool *isOpened = mHasCloseBtn ? &mVisible : nullptr;
        bool ret = ImGui::BeginPopupModal(getName().c_str(), isOpened, ImGuiWindowFlags_AlwaysAutoResize);
        if (!ret)
        {
            if (isOpened != nullptr && visible != mVisible)
            {
                // 点 X 关闭了
                if (msDialogStack.top() == this)
                {
                    msDialogStack.pop();
                }
                onClose();
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
            onClose();
            ImGui::CloseCurrentPopup();
            mShouldClose = false;
            msDialogStack.pop();
        }
        
        ImGui::EndPopup();
        PopWidgetID();
    }

    //--------------------------------------------------------------------------

    void ImDialog::onShow()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImDialog::onClose()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImMessageBox::show(const String &title, const String &message, ShowType type, Buttons &&buttons, const ImVec4 &txtColor)
    {
        if (ImMessageBox::getInstancePtr() == nullptr)
        {
            ImMessageBox *instance = new ImMessageBox();
            instance->create(title, nullptr, false);
        }

        return ImMessageBox::getInstance().appear(title, message, type, std::move(buttons), txtColor);
    }

    //--------------------------------------------------------------------------

    TResult ImMessageBox::appear(const String &title, const String &message, ShowType type, Buttons &&buttons, const ImVec4 &txtColor)
    {
        if (buttons.size() > 2)
        {
            T3D_LOG_ERROR(LOG_TAG_TINYIMGUI, "The number of buttons passed to MessageBox is out of bound !");
            return T3D_ERR_OUT_OF_BOUND;
        }
        
        setName(title);
        mMessage = message;
        mButtons = std::move(buttons);
        mMessageColor = txtColor;
        ImDialog::show(type);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool ImMessageBox::onGUIBegin()
    {
        ImVec2 texSize = ImGui::CalcTextSize(mMessage.c_str());
        const float dialog_w = 256.0f;
        
        // 设置对话框宽度
        if (texSize.x < dialog_w)
        {
            ImGui::SetNextWindowSize(ImVec2(256.0f, 0.0f));
        }
        
        return ImDialog::onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void ImMessageBox::onGUI()
    {
        auto region = ImGui::GetWindowSize();
        
        ImGui::Spacing();

        if (mMessageColor.x > 0 || mMessageColor.y > 0
            || mMessageColor.z > 0 || mMessageColor.w > 0)
        {
            ImGui::TextColored(mMessageColor, mMessage.c_str());
        }
        else
        {
            ImGui::Text(mMessage.c_str());
        }
        
        
        ImGui::Spacing();
        ImGui::Separator();
        
        auto itr = mButtons.begin();
        const float button_w = 80.0f;
        const float button_space = 10.0f;
        
        if (mButtons.size() == 2)
        {
            float x = (region.x - 2 * button_w - button_space) * 0.5f;
            ImGui::SetCursorPosX(x);
            if (ImGui::Button(itr->name.c_str(), ImVec2(button_w, 0)))
            {
                itr->callback();
                close();
            }

            ImGui::SameLine(0, button_space);

            ++itr;
            
            x = x + button_w + button_space;
            ImGui::SetCursorPosX(x);
            // ImGui::PushItemWidth(-FLT_MIN);
            if (ImGui::Button(itr->name.c_str(), ImVec2(button_w, 0)))
            {
                itr->callback();
                close();
            }
        }
        else if (mButtons.size() == 1)
        {
            float x = (region.x - button_w) * 0.5f;
            ImGui::SetCursorPosX(x);
            if (ImGui::Button(itr->name.c_str(), ImVec2(button_w, 0)))
            {
                itr->callback();
                close();
            }
        }
        else
        {
            // NO any buttons, do nothing
        }
        
    }

    //--------------------------------------------------------------------------
}
