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


#include "ImWidget.h"


namespace Tiny3D
{
    using ImInputTextCallback = TFunction<int32_t(ImInputText *, const String &)>;
    
    class TINYIMGUI_API ImInputText : public ImWidget
    {
    public:
        ~ImInputText() override;

        WidgetType getWidgetType() const override { return WidgetType::kInputText; }

        using ImWidget::create;
        
        TResult create(uint32_t id, const String &name, int32_t maxNumberOfChars, const ImInputTextCallback &callback, ImWidget *parent);
        
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list&args) override;
        
        void onDestroy() override;
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;

        static int32_t inputTextCallback(ImGuiInputTextCallbackData *data);

        int32_t inputTextCallback();

    protected:
        char *mText {nullptr};
        int32_t mMaxNumberOfChars {0};
        ImInputTextCallback mCallback {nullptr};
    };
}
