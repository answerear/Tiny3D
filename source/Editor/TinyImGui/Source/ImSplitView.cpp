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


#include "ImSplitView.h"
#include "ImErrors.h"
#include "imgui_internal.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImSplitView::~ImSplitView()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImSplitView::create(uint32_t id, const String &name, float sizePct1, ImChildView *child1, float sizePct2, ImChildView *child2, int32_t mainIdx, bool isHorz, ImWidget *parent, bool resizable)
    {
        return ImWidget::createInternal(id, name, parent, 7, &sizePct1, child1, &sizePct2, child2, mainIdx, isHorz, resizable);
    }

    //--------------------------------------------------------------------------

    TResult ImSplitView::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        T3D_ASSERT(argc >= 7, "Invalid number of arguments in ImSplitView::create() !");

        TResult ret = IM_OK;

        do
        {
            ret = ImChildView::createInternal(id, name, parent, argc, args);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            float *pct = va_arg(args, float*);
            mSizePct0 = *pct;
            ImWidget *child0 = va_arg(args, ImWidget*);
            pct = va_arg(args, float*);
            mSizePct1 = *pct;
            ImWidget *child1 = va_arg(args, ImWidget*);
            int32_t mainIdx = va_arg(args, int32_t);
            mIsHorz = va_arg(args, bool);
            mResizable = va_arg(args, bool);
            
            if (mainIdx == 0)
            {
                mMainChild = child0;
                mOtherChild = child1;
            }
            else
            {
                mMainChild = child1;
                mOtherChild = child0;
            }
            
            ret = addChild(child0);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ret = addChild(child1);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void ImSplitView::initChildrenSize()
    {
        const Children &children = getChildren();
        auto child0 = children.front();
        auto child1 = children.back();

        auto region = ImGui::GetContentRegionAvail();
        float width = region.x;
        float height = region.y;

        if (mIsHorz)
        {
            float x0 = width * mSizePct0;
            child0->setSize(x0, child0->getSize().y);
            float x1 = width * mSizePct1;
            child1->setSize(x1, child1->getSize().y);
        }
        else
        {
            float y0 = height * mSizePct0;
            child0->setSize(child0->getSize().x, y0);
            float y1 = height * mSizePct1;
            child1->setSize(child1->getSize().x, y1);
        }
    }

    //--------------------------------------------------------------------------

    bool ImSplitView::onGUIBegin()
    {
        bool ret = ImChildView::onGUIBegin();

        ImGui::PushStyleColor(ImGuiCol_Separator, ImGui::GetStyleColorVec4(ImGuiCol_ChildBg));

        const Children &children = getChildren();
        T3D_ASSERT(children.size() == 2, "SplitView must be 2 children !");

        auto child0 = children.front();
        auto child1 = children.back();

        float width = ImGui::GetContentRegionAvail().x;
        float height = ImGui::GetContentRegionAvail().y;

        if (mIsFirst || !mResizable)
        {
            initChildrenSize();
            mIsFirst = false;
        }

        bool isAllChildrenVisible = child0->isVisible() && child1->isVisible();

        if (isAllChildrenVisible)
        {
            if (mIsHorz)
            {
                float *size0 = const_cast<float*>(&mMainChild->getSize().x);
                float *size1 = const_cast<float*>(&mOtherChild->getSize().x);
                *size0 = width - *size1 - mSplitterThickness;
                splitter(true, mSplitterThickness, size0, size1, 50.0f, 50.0f);
            }
            else
            {
                float *size0 = const_cast<float*>(&mMainChild->getSize().y);
                float *size1 = const_cast<float*>(&mOtherChild->getSize().y);
                *size0 = height - *size1 - mSplitterThickness;
                splitter(true, mSplitterThickness, size0, size1, 50.0f, 50.0f);
            }
        }
        else
        {
            if (mIsHorz)
            {
                if (child0->isVisible())
                {
                    child0->setSize(width, child0->getSize().y); 
                }
                if (child1->isVisible())
                {
                    child1->setSize(width, child1->getSize().y);
                }
            }
            else
            {
                if (child0->isVisible())
                {
                    child0->setSize(child0->getSize().x, height); 
                }
                if (child1->isVisible())
                {
                    child1->setSize(child1->getSize().x, height);
                }
            }
        }
        
        ImGui::PopStyleColor();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ImSplitView::onGUI()
    {
        
    }

    //--------------------------------------------------------------------------

    void ImSplitView::onGUIEnd()
    {
        ImChildView::onGUIEnd();
    }

    //--------------------------------------------------------------------------

    void ImSplitView::update()
    {
        if (mVisible && onGUIBegin())
        {
            onGUI();

            auto child0 = getChildren().front();
            auto child1 = getChildren().back();
            child0->update();

            bool isAllChildrenVisible = child0->isVisible() && child1->isVisible();
            if (mIsHorz && isAllChildrenVisible)
            {
                ImGui::SameLine(0, 8.0f);
            }
            child1->update();
            onGUIEnd();
        }
    }

    //--------------------------------------------------------------------------

    bool ImSplitView::splitter(bool splitVert, float thickness, float *size0, float *size1, float minSize0, float minSize1, float splitterLongAxisSize)
    {
        ImGuiContext &g = *GImGui;
        ImGuiWindow *window = g.CurrentWindow;
        ImGuiID id = window->GetID("##Splitter");
        ImRect bb;
        bb.Min = ImVec2(window->DC.CursorPos.x + (splitVert ? ImVec2(*size0, 0.0f) : ImVec2(0.0f, *size0)).x,
                        window->DC.CursorPos.y + (splitVert ? ImVec2(*size0, 0.0f) : ImVec2(0.0f, *size0)).y);
        bb.Max = ImVec2(bb.Min.x + ImGui::CalcItemSize(splitVert
                        ? ImVec2(thickness, splitterLongAxisSize)
                        : ImVec2(splitterLongAxisSize, thickness), 0.0f, 0.0f).x,
                        bb.Min.y + ImGui::CalcItemSize(splitVert
                        ? ImVec2(thickness, splitterLongAxisSize)
                        : ImVec2(splitterLongAxisSize, thickness), 0.0f, 0.0f).y);
        return ImGui::SplitterBehavior(bb, id, splitVert ? ImGuiAxis_X : ImGuiAxis_Y, size0, size1, minSize0,
                                       minSize1, 0.0f);
    }

    //--------------------------------------------------------------------------
}
