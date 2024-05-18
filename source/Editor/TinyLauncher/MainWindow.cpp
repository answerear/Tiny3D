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


#include "MainWindow.h"
#include "ImErrors.h"
#include "LauncherApp.h"


namespace Tiny3D
{
    NS_BEGIN(Launcher)

    //--------------------------------------------------------------------------

    TResult MainWindow::onCreate()
    {
        TResult ret = IM_OK;
        
        do
        {
            // Tab bar
            ImTabBar *bar = new ImTabBar();
            ret = bar->create("Project Types", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Create tab bar failed !");
                break;
            }

            // Local projects
            ImTabItem *item = new ImTabItem();
            const String &name0 = STR(TXT_LOCAL_PROJECTS);
            ret = item->create(name0, bar);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Create tab item [%s] failed !", name0.c_str());
                break;
            }

            // Asset store projects
            item = new ImTabItem();
            const String &name1 = STR(TXT_STORE_PROJECTS);
            ret = item->create(name1, bar);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Create tab item [%s] failed !", name1.c_str());
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    bool MainWindow::onGUIBegin()
    {
        PushWidgetID();
        
        ImGuiIO& io = ImGui::GetIO();

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(mainViewport->WorkPos);
        ImGui::SetNextWindowSize(io.DisplaySize);
        const auto windowBorderSize = ImGui::GetStyle().WindowBorderSize; 
        const auto windowRounding   = ImGui::GetStyle().WindowRounding;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        bool ret = ImGui::Begin(getName().c_str(), &mVisible, flags);
        if (ret)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, windowBorderSize);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, windowRounding);
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void MainWindow::onGUIEnd()
    {
        // ImGui::Image(mSceneRT, ImVec2(640, 480));

        ImGui::PopStyleVar(2);
        ImGui::End();
        ImGui::PopStyleVar(2);

        PopWidgetID();
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildMenu()
    {
        mMenuBar = new ImMenuBar();
        mMenuBar->create("Main Menu", this);
        
        // File
        IM_BEGIN_MENU("File")
            // New
            IM_BEGIN_POPUP_MENU("New")
                IM_MENU_ITEM("New Scene", "Ctrl+N", nullptr, 0)
            IM_END_POPUP_MENU()
        IM_END_MENU(mMenuBar)

        return IM_OK;
    }

    //--------------------------------------------------------------------------
    
    NS_END
}
