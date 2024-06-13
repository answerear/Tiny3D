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


#include "NewProjectDlg.h"
#include "ProjectManager.h"


namespace Tiny3D
{
    NS_BEGIN(Launcher)
    
    //--------------------------------------------------------------------------

    NewProjectDialog::~NewProjectDialog()
    {
        
    }

    //--------------------------------------------------------------------------

    bool NewProjectDialog::onGUIBegin()
    {
        return ImDialog::onGUIBegin();
    }

    //--------------------------------------------------------------------------

    void NewProjectDialog::onGUI()
    {
        auto region = ImGui::GetWindowSize();

        const ImVec2 txtNameSize = ImGui::CalcTextSize(CH(TXT_PROJECT_NAME));
        const ImVec2 txtPathSize = ImGui::CalcTextSize(CH(TXT_PROJECT_PATH));

        const float txtSpacing = 16.0f;
        const float labelWidth = txtNameSize.x > txtPathSize.x ? txtNameSize.x : txtPathSize.x;
        float x = 0.0f;
        ImGui::Text(CH(TXT_PROJECT_NAME));
        ImGui::SameLine();
        x = labelWidth + txtSpacing;
        ImGui::SetCursorPosX(x);
        ImGui::InputText("##Name", mProjectName, sizeof(mProjectName));

        ImGui::Spacing();

        ImGui::Text(CH(TXT_PROJECT_PATH));
        
        ImGui::SameLine();

        const float button_brw_w = 40.0f;
        const float button_brw_space = 8.0f;
        x = labelWidth + txtSpacing;
        ImGui::SetCursorPosX(x);
        ImGui::PushItemWidth(-button_brw_w-button_brw_space);
        ImGui::InputText("##Path", mProjectPath, sizeof(mProjectPath));
        ImGui::PopItemWidth();
        
        ImGui::SameLine();
        
        ImGui::PushItemWidth(-FLT_MIN);
        // x = region.x - button_brw_w;
        // ImGui::SetCursorPosX(x);
        if (ImGui::Button("...", ImVec2(button_brw_w, 0)))
        {
            onClickedBrowser();
        }
        ImGui::PopItemWidth();

        ImGui::Spacing();

        const float button_w = 80.0f;
        const float button_space = 10.0f;
        x = (region.x - 2 * button_w - button_space) * 0.5f;
        ImGui::SetCursorPosX(x);
        if (ImGui::Button(CH(TXT_OK), ImVec2(button_w, 0)))
        {
            onClickedOK();
        }

        ImGui::SameLine(0, button_space);

        x = x + button_w + button_space;
        ImGui::SetCursorPosX(x);
        // ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button(CH(TXT_CANCEL), ImVec2(button_w, 0)))
        {
            onClickedCancel();
        }
        // ImGui::PopItemWidth();
    }

    //--------------------------------------------------------------------------

    void NewProjectDialog::onGUIEnd()
    {
        ImDialog::onGUIEnd();
    }

    //--------------------------------------------------------------------------

    void NewProjectDialog::onShow()
    {
        memset(mProjectName, 0, sizeof(mProjectName));
        memset(mProjectPath, 0, sizeof(mProjectPath));
    }

    //--------------------------------------------------------------------------

    void NewProjectDialog::onClickedBrowser()
    {
        String path = ImOpenFileDialog::openExplorerFolderDialog();
        int32_t pathLength = sizeof(mProjectPath) - 1;
        int32_t len = static_cast<int32_t>(path.length()) > pathLength ? pathLength : static_cast<int32_t>(path.length());
        strncpy(mProjectPath, path.c_str(), len);
        mProjectPath[len] = 0;
    }

    //--------------------------------------------------------------------------

    void NewProjectDialog::onClickedOK()
    {
        if (mProjectName[0] == 0)
        {
            // 没有输入名字
            ImMessageBox::show(STR(TXT_WARNING), STR(TXT_WARNING_EMPTY_NAME), STR(TXT_OK), ImDialog::ShowType::kOverlay);
            return;
        }

        if (mProjectPath[0] == 0)
        {
            // 没有输入路径
            ImMessageBox::show(STR(TXT_WARNING), STR(TXT_WARNING_EMPTY_PATH), STR(TXT_OK), ImDialog::ShowType::kOverlay);
            return;
        }

        String path = String(mProjectPath) + Dir::getNativeSeparator() + String(mProjectName);
        if (Dir::exists(path))
        {
            ImMessageBox::show(STR(TXT_WARNING), STR(TXT_WARNING_EXISTS_PROJECT), STR(TXT_OK), ImDialog::ShowType::kOverlay);
            return;
        }
        
        PROJECT_MGR.createProject(mProjectPath, mProjectName);
        PROJECT_MGR.saveProjects();
        close();
    }

    //--------------------------------------------------------------------------

    void NewProjectDialog::onClickedCancel()
    {
        close();
    }

    //--------------------------------------------------------------------------

    NS_END
}
