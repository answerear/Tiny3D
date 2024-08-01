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


#include "ImToolBar.h"
#include "ImButton.h"
#include "ImErrors.h"
#include "ImTextureManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImToolBar::~ImToolBar()
    {
        
    }

    //--------------------------------------------------------------------------

    String ImToolBar::makeName(uint32_t id)
    {
        String name;
        std::stringstream ss;
        ss << getName() << "##" << id;
        return ss.str();
    }

    
    //--------------------------------------------------------------------------

    TResult ImToolBar::addImageButton(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, const String &tips, const String &shortcut)
    {
        ImImageButton *button = new ImImageButton();
        TResult ret = button->create(id, texID, queryEnabled, clicked, this, tips, shortcut);
        if (T3D_FAILED(ret))
        {
            T3D_SAFE_DELETE(button)
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::addImageButtonEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, const String &tips, const String &shortcut)
    {
        ImImageButton *button = new ImImageButton();
        TResult ret = button->createEx(id, imageName, queryEnabled, clicked, this, tips, shortcut);
        if (T3D_FAILED(ret))
        {
            T3D_SAFE_DELETE(button)
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::addImageButton(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, uint32_t clickedEvtID, const String &tips, const String &shortcut)
    {
        ImImageButton *button = new ImImageButton();
        TResult ret = button->create(id, texID, queryEnabled, clickedEvtID, this, tips, shortcut);
        if (T3D_FAILED(ret))
        {
            T3D_SAFE_DELETE(button)
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::addImageButtonEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, uint32_t clickedEvtID, const String &tips, const String &shortcut)
    {
        ImImageButton *button = new ImImageButton();
        TResult ret = button->createEx(id, imageName, queryEnabled, clickedEvtID, this, tips, shortcut);
        if (T3D_FAILED(ret))
        {
            T3D_SAFE_DELETE(button)
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::addPushImageButton(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, const String &tips, const String &shortcut)
    {
        ImPushImageButton *button = new ImPushImageButton();
        TResult ret = button->create(id, texID, queryEnabled, queryChecked, clicked, this, tips, shortcut);
        if (T3D_FAILED(ret))
        {
            T3D_SAFE_DELETE(button)
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::addPushImageButtonEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, const String &tips, const String &shortcut)
    {
        ImPushImageButton *button = new ImPushImageButton();
        TResult ret = button->createEx(id, imageName, queryEnabled, queryChecked, clicked, this, tips, shortcut);
        if (T3D_FAILED(ret))
        {
            T3D_SAFE_DELETE(button)
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::addPushImageButton(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, uint32_t clickedEvtID, const String &tips, const String &shortcut)
    {
        ImPushImageButton *button = new ImPushImageButton();
        TResult ret = button->create(id, texID, queryEnabled, queryChecked, clickedEvtID, this, tips, shortcut);
        if (T3D_FAILED(ret))
        {
            T3D_SAFE_DELETE(button)
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::addPushImageButtonEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, uint32_t clickedEvtID, const String &tips, const String &shortcut)
    {
        ImPushImageButton *button = new ImPushImageButton();
        TResult ret = button->createEx(id, imageName, queryEnabled, queryChecked, clickedEvtID, this, tips, shortcut);
        if (T3D_FAILED(ret))
        {
            T3D_SAFE_DELETE(button)
        }
        return ret;
    }

    //--------------------------------------------------------------------------
    
    void ImToolBar::update(const ImVec2 &size)
    {
        if (isVisible() && onGUIBegin())
        {
            onGUI();

            auto region = ImGui::GetContentRegionAvail();

            if (mAlignment == Alignment::kMiddle)
            {
                uint32_t count = static_cast<uint32_t>(getChildren().size());
                float x = (region.x - mChildrenWidth) * 0.5f;
                ImGui::SetCursorPosX(x);
            }
            else if (mAlignment == Alignment::kRight)
            {
                uint32_t count = static_cast<uint32_t>(getChildren().size());
                float x = region.x - mChildrenWidth;
                ImGui::SetCursorPosX(x);
            }

            for (auto child : getChildren())
            {
                child->update();
                ImGui::SameLine();
            }

            onGUIEnd();
        }
    }

    //--------------------------------------------------------------------------

    TResult ImToolBar::onCreate()
    {
        mSize.y = mButtonSize.y + mHorzSpacing * 2.0f;
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    bool ImToolBar::onGUIBegin()
    {
        return ImChildView::onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void ImToolBar::onGUI()
    {
    }

    //--------------------------------------------------------------------------

    void ImToolBar::onGUIEnd()
    {
        // ImGui::End();
        ImChildView::onGUIEnd();
        ImGui::Separator();
    }

    //--------------------------------------------------------------------------

    ImGuiChildFlags ImToolBar::onGetChildFlags()
    {
        return ImGuiChildFlags_None;
    }
    
    //--------------------------------------------------------------------------

    ImGuiWindowFlags ImToolBar::onGetWindowFlags()
    {
        return ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;
    }

    //--------------------------------------------------------------------------

    void ImToolBar::onChildAdded(ImWidget *widget)
    {
        switch (widget->getWidgetType())
        {
        case WidgetType::kButton:
        case WidgetType::kPushButton:
        case WidgetType::kImageButton:
        case WidgetType::kPushImageButton:
            {
                widget->setSize(mButtonSize);
            }
            break;
        default:
            break;
        }

        mChildrenWidth += widget->getSize().x + ImGui::GetStyle().ItemSpacing.x * 2.0f;
    }

    //--------------------------------------------------------------------------

    void ImToolBar::onChildRemoved(ImWidget *widget)
    {
        mChildrenWidth -= widget->getSize().x + ImGui::GetStyle().ItemSpacing.x * 2.0f;
    }

    //--------------------------------------------------------------------------
}
