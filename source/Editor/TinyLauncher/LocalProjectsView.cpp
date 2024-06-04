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


#include "LocalProjectsVIew.h"
#include "NewProjectDlg.h"
#include "ImErrors.h"


namespace Tiny3D
{
    NS_BEGIN(Launcher)

    //--------------------------------------------------------------------------

    bool LocalProjectsView::onGUIBegin()
    {
        if (ImChildView::onGUIBegin())
        {
            
        }

        checkFocused();
        
        return true;
    }

    //--------------------------------------------------------------------------

    void LocalProjectsView::onGUI()
    {
        ImGui::Spacing();

        ImVec2 region = ImGui::GetContentRegionAvail();
        float child_height = 40.0f;

        // Top child
        {
            // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.92f, 0.49f, 0.19f, 1.0f));

            ImGui::BeginChild("##ProjectView_Top", ImVec2(region.x, child_height), ImGuiChildFlags_None);

            float margin_x = 8.0f;
            float margin_y = 4.0f;
            
            float button_w = 100.0f;
            float button_h = child_height - margin_y * 2;

            float x = 0;
            float y = margin_y;
            
            // 新建按钮
            ImGui::SetCursorPosX(x);
            ImGui::SetCursorPosY(y);
            if (ImGui::Button(CH(TXT_NEW_PROJECT), ImVec2(button_w, 0)))
            {
                openNewProjectDialog();
            }

            ImGui::SameLine();

            // 打开按钮
            x += button_w + margin_x;
            ImGui::SetCursorPosX(x);
            ImGui::SetCursorPosY(y);
            if (ImGui::Button(CH(TXT_OPEN_PROJECT), ImVec2(button_w, 0)))
            {
            
            }

            ImGui::SameLine();

            // 输入搜索框
            x += button_w + margin_x;
            ImGui::SetCursorPosX(x);
            ImGui::InputTextWithHint("##Filter_Projects", CH(TXT_FILTER_PROJECTS), mFilterText, IM_ARRAYSIZE(mFilterText));
            ImGui::SetItemTooltip(CH(TXT_FILTER_PROJECTS_HINT));
        
            ImGui::SameLine();

            // 排序方式下拉框
            ImGui::PushItemWidth(-FLT_MIN);
            ImGui::Text(CH(TXT_SORT_TYPE));
            ImGui::PopItemWidth();
            
            ImGui::SameLine();
        
            const char *items[] = {CH(TXT_SORT_TYPE_RECENT), CH(TXT_SORT_TYPE_NAME), CH(TXT_SORT_TYPE_PATH), CH(TXT_SORT_TYPE_TAG)};
            ImGui::PushItemWidth(-FLT_MIN);
            ImGui::Combo("##Sort_Type", &mCurrentSortType, items, IM_ARRAYSIZE(items));
            ImGui::PopItemWidth();

            ImGui::EndChild();

            // ImGui::PopStyleColor();
        }

        // ImGui::NewLine();
        // ImGui::Separator();
        ImGui::Spacing();
        
        // Bottom-left child
        region = ImGui::GetContentRegionAvail();
        auto child_width = 160.0f;
        
        {
            ImGui::BeginChild("##ProjectView_BottomLeft", ImVec2(region.x - child_width, 0), ImGuiChildFlags_FrameStyle);

            // ImGui::Spacing();
            // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        
            if (ImGui::BeginTable("##Projects_LocalProjectsView", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoBordersInBody, ImVec2(-FLT_MIN, 0.0f)))
            {
                ImGui::TableSetupColumn(CH(TXT_FAVORITES));
                ImGui::TableSetupColumn(CH(TXT_NAME));
                ImGui::TableSetupColumn(CH(TXT_MODIFIED));
                ImGui::TableSetupColumn(CH(TXT_TAGS));
                ImGui::TableHeadersRow();
                for (int row = 0; row < 4; row++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::Selectable("Star", &mProjectSelected, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        
                    }
                    
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("Name & Path");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("Last Edit");
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("Tags");
                }
                ImGui::EndTable();
            }

            // ImGui::PopStyleVar();

            ImGui::EndChild();
        }

        ImGui::SameLine();

        // Bottom-right child
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(54.0f/255.0f, 61.0f/255.0f, 74.0f/255.0f, 1.0f));
        
            ImGui::BeginChild("##ProjectView_BottomRight", ImVec2(child_width, 0), ImGuiChildFlags_Border);

            region = ImGui::GetContentRegionAvail();
            auto margin_x = 8.0f;
            auto margin_y = 8.0f;
            auto button_w = region.x - margin_x;
            auto button_h = 30.0f;
            auto button_size = ImVec2(button_w, button_h);

            float x = margin_x;
            float y = margin_y;
        
            ImGui::SetCursorPosX(x);
            ImGui::SetCursorPosY(y);
            ImGui::Button(CH(TXT_EDIT), button_size);

            y += button_h + margin_y;
            ImGui::SetCursorPosX(x);
            ImGui::SetCursorPosY(y);
            ImGui::Button(CH(TXT_RUN), button_size);

            y += button_h + margin_y;
            ImGui::SetCursorPosX(x);
            ImGui::SetCursorPosY(y);
            ImGui::Button(CH(TXT_RENAME), button_size);

            y += button_h + margin_y;
            ImGui::SetCursorPosX(x);
            ImGui::SetCursorPosY(y);
            ImGui::Button(CH(TXT_MANAGE_TAGS), button_size);

            y += button_h + margin_y;
            ImGui::SetCursorPosX(x);
            ImGui::SetCursorPosY(y);
            ImGui::Button(CH(TXT_REMOVE_PROJECT), button_size);

            y = region.y - (margin_y + button_h) * 2;
            ImGui::SetCursorPosX(x);
            ImGui::SetCursorPosY(y);
            ImGui::PushItemWidth(button_size.x);
            const char *languages[] = {CH(TXT_LANG_EN_US), CH(TXT_LANG_ZH_HANS)};
            ImGui::Combo("##Language", &mCurrentLanguage, languages, IM_ARRAYSIZE(languages));
            ImGui::PopItemWidth();
            
            y = region.y - margin_y - button_h;
            ImGui::SetCursorPosX(x);
            ImGui::SetCursorPosY(y);
            ImGui::Button(CH(TXT_ABOUT), button_size);
        
            ImGui::EndChild();
        
            ImGui::PopStyleColor();
        }
    }

    //--------------------------------------------------------------------------

    void LocalProjectsView::onGUIEnd()
    {
        ImChildView::onGUIEnd();
    }

    //--------------------------------------------------------------------------

    void LocalProjectsView::openNewProjectDialog()
    {
        if (mNewProjectDlg == nullptr)
        {
            mNewProjectDlg = new NewProjectDialog();
            mNewProjectDlg->create(CH(TXT_NEW_PROJECT_TITLE), nullptr);
            mNewProjectDlg->setVisible(false);
        }

        if (!mNewProjectDlg->isVisible())
        {
            mNewProjectDlg->show(ImDialog::ShowType::kEnqueueBack);
        }
    }

    //--------------------------------------------------------------------------

    NS_END
}
