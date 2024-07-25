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
    class ImChildView;
    
    class TINYIMGUI_API ImLayout : public ImWidget
    {
    public:
        static ImChildView *NEWLINE;
        
        struct Item
        {
            /// 大小
            ImVec2  size {0, 0};
            /// 子窗口
            ImChildView *childView {nullptr};
        };

        using Items = TList<Item>;
        
        ~ImLayout() override;

        WidgetType getWidgetType() const override { return WidgetType::kLayout; }

        TResult addWidgets(const Items &items, bool relative = false);

        void update() override;
        
    protected:

    protected:
        /// 所有要布局的子控件
        Items   mItems {};
        /// 相对布局还是绝对布局
        bool    mRelative {false};
    };
}
