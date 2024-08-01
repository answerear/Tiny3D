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


#pragma once


#include "ImChildView.h"


namespace Tiny3D
{
    class TINYIMGUI_API ImToolBar : public ImChildView, public EventHandler
    {
    public:
        enum class Alignment : uint32_t
        {
            kLeft = 0,
            kMiddle,
            kRight,
        };
        
        ~ImToolBar() override;

        WidgetType getWidgetType() const override { return WidgetType::kToolBar; }

        void setButtonSize(const ImVec2 &size) { mButtonSize = size; mSize.y = size.y + mHorzSpacing * 2.0f; }

        const ImVec2 &getButtonSize() const { return mButtonSize; }
        
        void setAlignment(Alignment alignment) { mAlignment = alignment; }

        TResult addImageButton(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, const String &tips, const String &shortcut);

        TResult addImageButtonEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonClickedCallback &clicked, const String &tips, const String &shortcut);
        
        TResult addImageButton(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, uint32_t clickedEvtID, const String &tips, const String &shortcut);

        TResult addImageButtonEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, uint32_t clickedEvtID, const String &tips, const String &shortcut);

        TResult addPushImageButton(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, const String &tips, const String &shortcut);
        
        TResult addPushImageButtonEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, const ImButtonClickedCallback &clicked, const String &tips, const String &shortcut);
        
        TResult addPushImageButton(uint32_t id, ImTextureID texID, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, uint32_t clickedEvtID, const String &tips, const String &shortcut);
        
        TResult addPushImageButtonEx(uint32_t id, const String &imageName, const ImButtonQueryCallback &queryEnabled, const ImButtonQueryCallback &queryChecked, uint32_t clickedEvtID, const String &tips, const String &shortcut);

        void update(const ImVec2 &size) override;
        
    protected:
        TResult onCreate() override;
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;

        ImGuiChildFlags onGetChildFlags() override;
        ImGuiWindowFlags onGetWindowFlags() override;

        void onChildAdded(ImWidget *widget) override;

        void onChildRemoved(ImWidget *widget) override;
        
        String makeName(uint32_t id);
        
    protected:
        /// 按钮大小
        ImVec2  mButtonSize {20, 20};
        /// 所有子 widget 的宽度，包括 Item space 
        float   mChildrenWidth {0.0f};
        /// toolbar 上下左右 space
        float   mHorzSpacing {5.0f};
        /// 按钮对齐方式
        Alignment mAlignment {Alignment::kLeft};
    };
}
