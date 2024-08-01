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
    class TINYIMGUI_API ImTreeNode : public ImWidget
    {
    public:
        ~ImTreeNode() override;

        WidgetType getWidgetType() const override { return WidgetType::kTreeNode; }

        TResult create(uint32_t id, ImTextureID texID, const String &title, ImTreeNode *parent);

        TResult addNode(ImTreeNode *node);
        
    protected:
        TResult createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list&args) override;
        
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;

    private:
        TResult addWidget(const String &parentName, ImWidget *widget) override;

        TResult addWidget(const UUID &parentUUID, ImWidget *widget) override;

        TResult addWidget(uint32_t parentID, ImWidget *widget) override;
        
        TResult addChild(ImWidget *widget) override;

        TResult insertAfterChild(const String &prevName, ImWidget *widget) override;

        TResult insertAfterChild(const UUID &prevUUID, ImWidget *widget) override;

        TResult insertAfterChild(ImWidget *prevWidget, ImWidget *widget) override;

        TResult insertAfterChild(uint32_t prevID, ImWidget *widget) override;

    protected:
        ImTextureID mIconID {nullptr};
    };

    class TINYIMGUI_API ImTreeWidget : public ImTreeNode
    {
    public:
        ~ImTreeWidget() override;

        WidgetType getWidgetType() const override { return WidgetType::kTreeWidget; }
        
    protected:
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;
    };
}
