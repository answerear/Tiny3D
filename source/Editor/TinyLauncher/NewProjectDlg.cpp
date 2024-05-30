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
        auto region = ImGui::GetContentRegionAvail();
        
        const float labelWidth = 80.0f;
        float x = 0.0f;
        ImGui::Text(CH(TXT_PROJECT_NAME));
        ImGui::SameLine();
        x = labelWidth;
        ImGui::SetCursorPosX(x);
        ImGui::InputText("##Name", mProjectName, sizeof(mProjectName));

        ImGui::Spacing();

        ImGui::Text(CH(TXT_PROJECT_PATH));
        
        ImGui::SameLine();

        const float button_brw_w = 40.0f;
        const float button_brw_space = 8.0f;
        x = labelWidth;
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
            
        }
        ImGui::PopItemWidth();

        ImGui::Spacing();

        const float button_w = 80.0f;
        const float button_space = 10.0f;
        x = (region.x - 2 * button_w - button_space) * 0.5f;
        ImGui::SetCursorPosX(x);
        if (ImGui::Button(CH(TXT_OK), ImVec2(button_w, 0)))
        {
            
        }

        ImGui::SameLine(0, button_space);

        x = x + button_w + button_space;
        ImGui::SetCursorPosX(x);
        // ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button(CH(TXT_CANCEL), ImVec2(button_w, 0)))
        {
            
        }
        // ImGui::PopItemWidth();
    }

    //--------------------------------------------------------------------------

    void NewProjectDialog::onGUIEnd()
    {
        ImDialog::onGUIEnd();
    }

    //--------------------------------------------------------------------------

    NS_END
}
