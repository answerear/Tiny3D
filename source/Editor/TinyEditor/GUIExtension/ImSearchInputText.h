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


#include "ImGuiExtensionPrerequisites.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    class ImSearchInputText : public ImInputText
    {
    public:
        using ImInputText::create;
        
        TResult create(uint32_t id, const ImVec2 &size, int32_t maxNumberOfChars, const ImInputTextCallback &callback, bool callbackEdit, ImWidget *parent);

        const ImVec2 &getOriginalSize() const { return mOriginalSize; }
        
    protected:
        using ImWidget::create;

        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list&args) override;

        TResult onCreate() override;

        void onDestroy() override;

        void onGUI() override;

    protected:
        ImTextureID mIconSearch {nullptr};
        ImVec2 mOriginalSize {};
        bool mCallbackEdit {false};
    };

    NS_END
}
