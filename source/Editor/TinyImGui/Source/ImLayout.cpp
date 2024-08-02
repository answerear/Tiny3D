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


#include "ImLayout.h"
#include "ImChildView.h"
#include "ImErrors.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImChildView *ImLayout::NEWLINE = (ImChildView*)0x1;
    
    //--------------------------------------------------------------------------

    ImLayout::~ImLayout()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImLayout::addWidgets(const Items &items, bool relative)
    {
        TResult ret = removeAllChildren();
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        bool onlyNewLine = true;
        
        for (auto item : items)
        {
            if (item.childView != NEWLINE)
            {
                addChild(item.childView);
                onlyNewLine = false;
            }
        }

        if (!onlyNewLine)
        {
            mItems = items;
            mRelative = relative;
        }
        
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    void ImLayout::update()
    {
        if (mVisible && onGUIBegin())
        {
            onGUI();

            auto region = ImGui::GetContentRegionAvail();

            auto itr = getChildren().begin();
            bool isFirst = true;
            
            for (auto item : mItems)
            {
                if (item.childView == NEWLINE)
                {
                    ImGui::Spacing();
                    isFirst = true;
                }
                else
                {
                    T3D_ASSERT(itr != getChildren().end());
                    T3D_ASSERT((*itr) == item.childView);
                    
                    if (!isFirst)
                    {
                        // 不是第一个，又不是换行，需要同一行继续下一个
                        ImGui::SameLine();
                    }
                    
                    ImVec2 size;
                    
                    if (mRelative)
                    {
                        // 相对大小
                        size.x = item.size.x * region.x;
                        size.y = item.size.y * region.y;
                    }
                    else
                    {
                        // 绝对大小
                        size = item.size;
                    }
                    
                    item.childView->update(size);

                    if (isFirst)
                    {
                        // 第一个结束，标记非第一个
                        isFirst = false;
                    }
                    
                    ++itr;
                }
            }
            
            onGUIEnd();
        }
    }

    //--------------------------------------------------------------------------
}
