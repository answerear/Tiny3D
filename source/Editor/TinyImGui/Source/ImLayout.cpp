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

    ImLayout::~ImLayout()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImLayout::addWidgets(const Items &items, bool relative)
    {
        removeAllChildren();

        bool onlyNewLine = true;
        
        for (auto item : items)
        {
            if (item.childView->getParent() != nullptr)
            {
                item.childView->getParent()->removeChild(item.childView, false);
            }
            addChild(item.childView);
            onlyNewLine = false;
        }

        if (!onlyNewLine)
        {
            mItems = items;
            mRelative = relative;
        }
        
        return IM_OK;
    }

    //--------------------------------------------------------------------------

    void ImHorizontalLayout::calcItemSizeRelative(const ImVec2 &region)
    {
        
    }

    //--------------------------------------------------------------------------

    void ImHorizontalLayout::calcItemsSizeAbsolute(const ImVec2 &region)
    {
        
    }

    //--------------------------------------------------------------------------

    void ImHorizontalLayout::update()
    {
        if (isVisible() && onGUIBegin())
        {
            onGUI();

            
        }
    }

    //--------------------------------------------------------------------------

    void ImHorizontalLayout::update(const ImVec2 &size)
    {
        update();
    }

    //--------------------------------------------------------------------------

    void ImVerticalLayout::calcItemSizeRelative(const ImVec2 &region)
    {
        float totalHeight = 0.0f;
        Items::iterator itrItem = mItems.end();
        size_t i = 0;

        for (auto itr = mItems.begin(); itr != mItems.end(); ++itr, ++i)
        {
            auto &item = *itr;

            if (item.size.y == 0.0f && i != mItems.size() - 1)
            {
                // 不是最后一行
                T3D_ASSERT(itrItem == mItems.end());    // 只有一个 item 是需要计算的，如果多于一个，则无法计算大小了
                itrItem = itr;
            }
            else
            {
                totalHeight += itr->size.y;

                if (i != mItems.size() - 1)
                {
                    // 非最后一行，要计算 item 间隔
                    totalHeight += ImGui::GetStyle().ItemSpacing.y;
                }
            }
        }

        if (itrItem != mItems.end())
        {
            itrItem->size.y = region.y - totalHeight;
            itrItem->size.y = itrItem->size.y / region.y;
        }
    }

    //--------------------------------------------------------------------------

    void ImVerticalLayout::calcItemsSizeAbsolute(const ImVec2 &region)
    {
        float totalHeight = 0.0f;
        Items::iterator itrItem = mItems.end();
        size_t i = 0;
        float spacingHeight = ImGui::GetStyle().ItemSpacing.y;
        
        for (auto itr = mItems.begin(); itr != mItems.end(); ++itr, ++i)
        {
            auto &item = *itr;

            // 加分隔空间高度
            totalHeight += spacingHeight;
            
            if (item.size.y == 0.0f && i != mItems.size() - 1)
            {
                // 不是最后一行
                T3D_ASSERT(itrItem == mItems.end());    // 只有一个 item 是需要计算的，如果多于一个，则无法计算大小了
                itrItem = itr;
            }
            else
            {
                totalHeight += fabs(itr->size.y);
                item.size.y = fabs(item.size.y) / region.y;
            }
        }

        // 最后一行下面还有一个分隔高度，需要加上去
        totalHeight += spacingHeight;

        if (itrItem != mItems.end())
        {
            itrItem->size.y = region.y - totalHeight;
            itrItem->size.y = itrItem->size.y / region.y;

            // IMGUI_LOG_INFO("Total height : %f, Child view %s size (%f, %f)", totalHeight, itrItem->childView->getName().c_str(), itrItem->size.x, itrItem->size.y)
        }
    }

    //--------------------------------------------------------------------------

    void ImVerticalLayout::update()
    {
        if (isVisible() && onGUIBegin())
        {
            onGUI();

            auto region = ImGui::GetContentRegionAvail();

            if (mIsSizeDirty)
            {
                if (mRelative)
                {
                    calcItemSizeRelative(region);
                }
                else
                {
                    calcItemsSizeAbsolute(region);
                }
                
                mIsSizeDirty = false;
            }
            
            auto itr = getChildren().begin();

            size_t i = 0;
            for (auto item : mItems)
            {
                if (i != 0)
                {
                    ImGui::Spacing();
                }

                T3D_ASSERT(itr != getChildren().end());
                T3D_ASSERT((*itr) == item.childView);
                    
                ImVec2 size;
                    
                if (item.size.x > 0.0f && item.size.x < 1.0f)
                {
                    // 相对大小
                    size.x = item.size.x * region.x;
                }
                else
                {
                    // 绝对大小
                    size.x = item.size.x;
                }

                if (item.size.y > 0.0f && item.size.y < 1.0f)
                {
                    // 相对大小
                    size.y = item.size.y * region.y;
                }
                else
                {
                    // 绝对大小
                    size.y = item.size.y;
                }
                
                item.childView->update(size);

                ++itr;
                ++i;
            }
            
            onGUIEnd();
        }
    }

    //--------------------------------------------------------------------------

    void ImVerticalLayout::update(const ImVec2 &size)
    {
        update();
    }

    //--------------------------------------------------------------------------
}
