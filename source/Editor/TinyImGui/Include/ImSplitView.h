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
    class TINYIMGUI_API ImSplitView : public ImChildView
    {
    public:
        ~ImSplitView() override;

        WidgetType getWidgetType() const override { return WidgetType::kSplitView; }

        TResult create(const String &name, float sizePct0, ImChildView *child0, float sizePct1, ImChildView *child1, int32_t mainIdx, bool isHorz, ImWidget *parent, bool resizable = true);

        float getSplitterThickness() const { return mSplitterThickness; }

        void setSplitterThickness(float thickness) { mSplitterThickness = thickness; }
        
    protected:
        TResult createInternal(const String &name, ImWidget *parent, int32_t argc, va_list &args) override;
        
        bool onGUIBegin() override;
        void onGUI() override;
        void onGUIEnd() override;

        void update() override;

        bool splitter(bool splitVert, float thickness, float *size0, float *size1, float minSize0, float minSize1, float splitterLongAxisSize = -1.0f);

        void initChildrenSize();
        
    protected:
        float mSplitterThickness {4.0f};
        float mSizePct0 {0.5f};
        float mSizePct1 {0.5f};
        bool mResizable {true};
        bool mIsHorz {true};

        ImWidget *mMainChild {nullptr};
        ImWidget *mOtherChild {nullptr};
    };
}
