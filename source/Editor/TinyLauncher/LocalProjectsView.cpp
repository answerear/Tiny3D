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
#include "ProjectManager.h"
#include "LauncherApp.h"
#include "LauncherEvent.h"


namespace Tiny3D
{
    NS_BEGIN(Launcher)
    
    //--------------------------------------------------------------------------

    TResult LocalProjectViewT::onCreate()
    {
        mCurrentSortType = static_cast<int32_t>(PROJECT_MGR.getSortType());
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void LocalProjectViewT::onGUI()
    {
        auto region = ImGui::GetContentRegionAvail();
        
        float margin_x = 8.0f;
        float margin_y = 4.0f;
            
        float button_w = 100.0f;
        float button_h = region.y - margin_y * 2;

        float x = 0;
        float y = margin_y;
            
        // 新建按钮
        ImGui::SetCursorPosX(x);
        ImGui::SetCursorPosY(y);
        if (ImGui::Button(CH(TXT_NEW_PROJECT), ImVec2(button_w, 0)))
        {
            sendEvent(kEvtOpenNewDialog, nullptr);
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
        if (ImGui::Combo("##Sort_Type", &mCurrentSortType, items, IM_ARRAYSIZE(items)))
        {
            PROJECT_MGR.setSortType(static_cast<SortType>(mCurrentSortType));
            PROJECT_MGR.sort();
            PROJECT_MGR.saveProjects();
        }
        ImGui::PopItemWidth();
    }

    //--------------------------------------------------------------------------

    int32_t LocalProjectViewBL::onGetChildFlags()
    {
        return ImGuiChildFlags_FrameStyle;
    }

    //--------------------------------------------------------------------------

    void LocalProjectViewBL::onGUI()
    {
        if (ImGui::BeginTable("##Projects_LocalProjectsView", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY, ImVec2(-FLT_MIN, 0.0f)))
        {
            std::stringstream ss;
            String spacing = "  ";
            ss << spacing << STR(TXT_FAVORITES);
            ImGui::TableSetupColumn(ss.str().c_str());
            ss.str("");
            ss << spacing << STR(TXT_NAME);
            ImGui::TableSetupColumn(ss.str().c_str());
            ss.str("");
            ss << spacing << STR(TXT_MODIFIED);
            ImGui::TableSetupColumn(ss.str().c_str());
            ss.str("");
            ss << spacing << STR(TXT_TAGS);
            ImGui::TableSetupColumn(ss.str().c_str());
            ImGui::TableSetupScrollFreeze(1, 1);
            ImGui::TableHeadersRow();
            
            const Projects &projects = PROJECT_MGR.getDisplayProjects();

            const int64_t ONE_MINUTE = 60;
            const int64_t ONE_HOUR = ONE_MINUTE * 60;
            const int64_t ONE_DAY = ONE_HOUR * 24;
            const int64_t ONE_MONTH = ONE_DAY * 30;
            const int64_t ONE_YEAR = ONE_DAY * 365;
            const int64_t CURRENT_DATE_TIME = DateTime::currentSecsSinceEpoch(); 

            ImGuiListClipper clipper;
            clipper.Begin((int32_t)projects.size());
            while (clipper.Step())
            {
                for (int32_t row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                {
                    ProjectInfo *project = projects[row];
                    float height = ImGui::GetTextLineHeight() + ImGui::GetTextLineHeightWithSpacing();

                    std::stringstream ssRow;
                    ssRow << "##Row_" << row;
                    ImGui::PushID(ssRow.str().c_str());
                    
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight() * 0.5f));
                    ImGui::Text(project->favourites ? "Star" : "");
                    ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight() * 0.5f));
                    
                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetWindowFontScale(1.1f);
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", project->name.c_str());
                    ImGui::SetWindowFontScale(0.9f);
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", project->path.c_str());
                    ImGui::SetWindowFontScale(1.0f);
                    
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight() / 2));
                    int64_t elapse = CURRENT_DATE_TIME - project->recent;
                    if (elapse < ONE_MINUTE)
                    {
                        // n 秒前
                        ImGui::Text(CH(TXT_BEFORE_X_SECONDS), elapse);
                    }
                    else if (elapse >= ONE_MINUTE && elapse < ONE_HOUR)
                    {
                        // n 分钟前
                        elapse /= ONE_MINUTE;
                        ImGui::Text(CH(TXT_BEFORE_X_MINUTES), elapse);
                    }
                    else if (elapse >= ONE_HOUR && elapse < ONE_DAY)
                    {
                        // n 小时前
                        elapse /= ONE_HOUR;
                        ImGui::Text(CH(TXT_BEFORE_X_HOURS), elapse);
                    }
                    else if (elapse >= ONE_DAY && elapse < ONE_MONTH)
                    {
                        // n 天前
                        elapse /= ONE_DAY;
                        ImGui::Text(CH(TXT_BEFORE_X_DAYS), elapse);
                    }
                    else if (elapse >= ONE_MONTH && elapse < ONE_YEAR)
                    {
                        // n 月前
                        elapse /= ONE_MONTH;
                        ImGui::Text(CH(TXT_BEFORE_X_MONTHS), elapse);
                    }
                    else
                    {
                        // n 年前
                        elapse /= ONE_YEAR;
                        ImGui::Text(CH(TXT_BEFORE_X_YEARS), elapse);
                    }
                    ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight() / 2));
                    
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight() / 2));
                    ImGui::Text("%s", project->tags.c_str());
                    ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight() / 2));
                    
                    ImGui::TableSetColumnIndex(0);
                    bool selected = (mProjectSelectedIndex == row);
                    if (ImGui::Selectable(ssRow.str().c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0, height)))
                    {
                        mProjectSelectedIndex = row;
                    }

                    ImGui::PopID();
                }
            }
            
            ImGui::EndTable();
        }
    }

    //--------------------------------------------------------------------------

    TResult LocalProjectViewBR::onCreate()
    {
        LauncherApp *app = static_cast<LauncherApp*>(T3D_APPLICATION.getInstancePtr());
        mCurrentLanguage = app->getAppSettings().languageIndex;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    int32_t LocalProjectViewBR::onGetChildFlags()
    {
        return ImGuiChildFlags_Border;
    }

    //--------------------------------------------------------------------------

    void LocalProjectViewBR::onGUI()
    {
        auto region = ImGui::GetContentRegionAvail();
        auto margin_x = 8.0f;
        auto margin_y = 8.0f;
        auto button_w = region.x - margin_x;
        auto button_h = 30.0f;
        auto button_size = ImVec2(button_w, button_h);

        float x = margin_x;
        float y = margin_y;

        // 编辑工程
        ImGui::SetCursorPosX(x);
        ImGui::SetCursorPosY(y);
        if (ImGui::Button(CH(TXT_EDIT), button_size))
        {
            sendEvent(kEvtEditProject, nullptr);
        }

        // 运行工程
        y += button_h + margin_y;
        ImGui::SetCursorPosX(x);
        ImGui::SetCursorPosY(y);
        if (ImGui::Button(CH(TXT_RUN), button_size))
        {
            sendEvent(kEvtRunProject, nullptr);
        }

        // 重命名工程
        y += button_h + margin_y;
        ImGui::SetCursorPosX(x);
        ImGui::SetCursorPosY(y);
        if (ImGui::Button(CH(TXT_RENAME), button_size))
        {
            sendEvent(kEvtRenameProject, nullptr);
        }

        // 管理标签
        y += button_h + margin_y;
        ImGui::SetCursorPosX(x);
        ImGui::SetCursorPosY(y);
        if (ImGui::Button(CH(TXT_MANAGE_TAGS), button_size))
        {
            sendEvent(kEvtManageTags, nullptr);
        }

        // 移除工程
        y += button_h + margin_y;
        ImGui::SetCursorPosX(x);
        ImGui::SetCursorPosY(y);
        if (ImGui::Button(CH(TXT_REMOVE_PROJECT), button_size))
        {
            sendEvent(kEvtRemoveProject, nullptr);
        }

        // 选择语言
        y = region.y - (margin_y + button_h) * 2;
        ImGui::SetCursorPosX(x);
        ImGui::SetCursorPosY(y);
        ImGui::PushItemWidth(button_size.x);
        const char *languages[] = {CH(TXT_LANG_EN_US), CH(TXT_LANG_ZH_HANS)};
        int32_t lastLanguage = mCurrentLanguage;
        if (ImGui::Combo("##Language", &mCurrentLanguage, languages, IM_ARRAYSIZE(languages)))
        {
            onComboLanguageChanged(lastLanguage);
        }
        ImGui::PopItemWidth();

        // 关于
        y = region.y - margin_y - button_h;
        ImGui::SetCursorPosX(x);
        ImGui::SetCursorPosY(y);
        if (ImGui::Button(CH(TXT_ABOUT), button_size))
        {
            onBtnAboutClicked();
        }
    }

    //--------------------------------------------------------------------------

    void LocalProjectViewBR::onComboLanguageChanged(int32_t lastIndex)
    {
        ImMessageBox::Buttons buttons;
        ImMessageBox::Button btnRestart;
        btnRestart.name = STR(TXT_BTN_RESTART);
        btnRestart.callback =
            [this]()
            {
                LauncherApp *app = static_cast<LauncherApp*>(T3D_APPLICATION.getInstancePtr());
                app->getAppSettings().languageIndex = mCurrentLanguage;
                const char *languages[] = {"lang-en-us.txt", "lang-zh-hans.txt"};
                app->getAppSettings().languageFileName = languages[mCurrentLanguage];
            };
        buttons.emplace_back(btnRestart);
        ImMessageBox::Button btnCancel;
        btnCancel.name = STR(TXT_CANCEL);
        btnCancel.callback =
            [this, lastIndex]()
            {
                mCurrentLanguage = lastIndex;
            };
        buttons.emplace_back(btnCancel);
        ImMessageBox::show(STR(TXT_INFO), STR(TXT_INFO_CHANGE_LANGUAGE), ImDialog::ShowType::kEnqueueBack, std::move(buttons), ImVec4(1.0f, 0.788f, 0.055f, 1.0f));
    }

    //--------------------------------------------------------------------------

    void LocalProjectViewBR::onBtnAboutClicked()
    {
        
    }

    //--------------------------------------------------------------------------

    void LocalProjectsView::update()
    {
        if (mVisible && onGUIBegin())
        {
            onGUI();

            ImGui::Spacing();

            ImVec2 region = ImGui::GetContentRegionAvail();
            float child_height = 40.0f;
            
            T3D_ASSERT(getChildren().size() == 3, "Must be 3 children !");

            // Part #1 - Top
            auto itr = getChildren().begin();
            ImWidget *widget = *itr;
            T3D_ASSERT(widget->getWidgetType() == WidgetType::kChildView, "Must be child view !");
            ImChildView *child = static_cast<ImChildView*>(widget);
            child->update(ImVec2(region.x, child_height));

            ImGui::Spacing();
            auto child_width = 160.0f;
            
            // Part #2 - Bottom-left
            ++itr;
            widget = *itr;
            T3D_ASSERT(widget->getWidgetType() == WidgetType::kChildView, "Must be child view !");
            child = static_cast<ImChildView*>(widget);
            child->update(ImVec2(region.x - child_width, 0));

            ImGui::SameLine();
            
            // Part #3 - Bottom-right
            ++itr;
            widget = *itr;
            T3D_ASSERT(widget->getWidgetType() == WidgetType::kChildView, "Must be child view !");
            child = static_cast<ImChildView*>(widget);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(54.0f/255.0f, 61.0f/255.0f, 74.0f/255.0f, 1.0f));
            child->update(ImVec2(child_width, 0));
            ImGui::PopStyleColor();
            
            onGUIEnd();
        }
    }

    
    //--------------------------------------------------------------------------

    TResult LocalProjectsView::onCreate()
    {
        ON_MEMBER(kEvtOpenNewDialog, LocalProjectsView::onOpenNewDialog);
        ON_MEMBER(kEvtEditProject, LocalProjectsView::onEditProject);
        ON_MEMBER(kEvtRunProject, LocalProjectsView::onRunProject);
        ON_MEMBER(kEvtRenameProject, LocalProjectsView::onRenameProject);
        ON_MEMBER(kEvtManageTags, LocalProjectsView::onManageTags);
        ON_MEMBER(kEvtRemoveProject, LocalProjectsView::onRemoveProject);
        
        PROJECT_MGR.loadProjects();
        PROJECT_MGR.sort(); 

        mViewT = new LocalProjectViewT();
        mViewT->create("##ProjectView_Top", this);

        mViewBL = new LocalProjectViewBL();
        mViewBL->create("##ProjectView_BottomLeft", this);

        mViewBR = new LocalProjectViewBR();
        mViewBR->create("##ProjectView_BottomRight", this);
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void LocalProjectsView::onDestroy()
    {
        unregisterAllEvent();
    }

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

    bool LocalProjectsView::onOpenNewDialog(EventParam *param, TINSTANCE sender)
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
        
        return true;
    }

    //--------------------------------------------------------------------------

    bool LocalProjectsView::onEditProject(EventParam *param, TINSTANCE sender)
    {
        PROJECT_MGR.startTinyEditor("Path", "Name", false);
        
        return true;
    }

    //--------------------------------------------------------------------------

    bool LocalProjectsView::onRunProject(EventParam *param, TINSTANCE sender)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool LocalProjectsView::onRenameProject(EventParam *param, TINSTANCE sender)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool LocalProjectsView::onManageTags(EventParam *param, TINSTANCE sender)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool LocalProjectsView::onRemoveProject(EventParam *param, TINSTANCE sender)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    NS_END
}
