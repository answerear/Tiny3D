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
#include "ConsoleWindow.h"
#include "HierarchyWindow.h"
#include "GameWindow.h"
#include "SceneWindow.h"
#include "ImErrors.h"
#include "InspectorWindow.h"
#include "ProjectWindow.h"
#include "../../Plugins/Renderer/Null/Include/T3DNullPrerequisites.h"



namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    TResult MainWindow::onCreate()
    {
        TResult ret;

        do
        {
            ret = buildMenu();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Build main menu failed ! ERROR [%d]", ret);
                break;
            }

            mHierarchyWnd = new HierarchyWindow();
            ret = mHierarchyWnd->create("Hierarchy", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create hierarchy window failed ! ERROR [%d]", ret);
                break;
            }

            mGameWnd = new GameWindow();
            ret = mGameWnd->create("Game", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create game window failed ! ERROR [%d]", ret);
                break;
            }

            mSceneWnd = new SceneWindow();
            ret = mSceneWnd->create("Scene", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create scene window failed ! ERROR [%d]", ret);
                break;
            }

            mInspectorWnd = new InspectorWindow();
            ret = mInspectorWnd->create("Inspector", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create inspector window failed ! ERROR [%d]", ret);
                break;
            }

            mProjectWnd = new ProjectWindow();
            ret = mProjectWnd->create("Project", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create project window failed ! ERROR [%d]", ret);
                break;
            }

            mConsoleWnd = new ConsoleWindow();
            ret = mConsoleWnd->create("Console", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create console window failed ! ERROR [%d]", ret);
                break;
            }
        } while (false);
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool MainWindow::onGUIBegin()
    {
        // PushWidgetID();
        //
        // ImGuiIO& io = ImGui::GetIO();
        //
        // ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
        //         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | /*ImGuiWindowFlags_NoDocking |*/
        //         ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus;
        // ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        // ImGui::SetNextWindowPos(mainViewport->WorkPos);
        // ImGui::SetNextWindowSize(io.DisplaySize);
        // const auto windowBorderSize = ImGui::GetStyle().WindowBorderSize; 
        // const auto windowRounding   = ImGui::GetStyle().WindowRounding;
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        //
        // bool ret = ImGui::Begin(getName().c_str(), &mVisible, flags);
        // if (ret)
        // {
        //     ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, windowBorderSize);
        //     ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, windowRounding);
        // }
        //
        // return ret;

        return true;
    }

    //--------------------------------------------------------------------------

    void MainWindow::onGUIEnd()
    {
        // ImGui::Image(mSceneRT, ImVec2(640, 480));

        // ImGui::PopStyleVar(2);
        // ImGui::End();
        // ImGui::PopStyleVar(2);
        //
        // PopWidgetID();
    }

    //--------------------------------------------------------------------------

    extern const char *kTinyEditorInitData;

    //--------------------------------------------------------------------------

    TResult MainWindow::buildMenu()
    {
        mMenuBar = new ImMenuBar();
        mMenuBar->create("Main Menu", this);

        auto queryEnableDefault = [](ImMenuItem*) {return true;};
        auto queryCheck = [](ImWidget *widget)
        {
            return widget != nullptr ? widget->isVisible() : false;
        };
        auto toggleWindowVisible = [](ImWidget *widget)
        {
            if (widget != nullptr)
            {
                widget->setVisible(!widget->isVisible());
            }
        };

        #define QUERY_CHECK(widget)             [&window=(widget), &queryCheck](ImMenuItem*) { return queryCheck(window); }
        #define TOGGLE_WINDOW_VISIBLE(widget)   [&window=(widget), &toggleWindowVisible](ImMenuItem*) { toggleWindowVisible(window); }
        
        // File
        {
            auto menu = mMenuBar->addMenu(STR(TXT_FILE));
            // New Scene
            menu->addItem(STR(TXT_NEW_SCENE), "Ctrl+N", queryEnableDefault, 0);
            // Open Scene
            menu->addItem(STR(TXT_OPEN_SCENE), "Ctrl+O", queryEnableDefault, 0);
            // Open Recent Scene
            menu->addItem(STR(TXT_OPEN_RECENT_SCENE), "", queryEnableDefault, 0);
            // Separator
            menu->addSeparator();
            // Save
            menu->addItem(STR(TXT_SAVE), "Ctrl+S", queryEnableDefault, 0);
            // Save As
            menu->addItem(STR(TXT_SAVE_AS), "Ctrl+Shift+S", queryEnableDefault, 0);
            // Save As Scene Template
            menu->addItem(STR(TXT_SAVE_AS_SCENE_TEMPLATE), "", queryEnableDefault, 0);
            // Separator
            menu->addSeparator();
            // New Project
            menu->addItem(STR(TXT_NEW_PROJECT), "", queryEnableDefault, 0);
            // Open Project
            menu->addItem(STR(TXT_OPEN_PROJECT), "", queryEnableDefault, 0);
            // Save Project
            menu->addItem(STR(TXT_SAVE_PROJECT), "", queryEnableDefault, 0);
            // Separator
            // Build Settings
            menu->addItem(STR(TXT_BUILD_SETTINGS), "Ctrl+Shift+B", queryEnableDefault, 0);
            // Build And Run
            menu->addItem(STR(TXT_BUILD_AND_RUN), "Ctrl+B", queryEnableDefault, 0);
            // Separator
            menu->addSeparator();
            // Exit
            menu->addItem(STR(TXT_EXIT), "", queryEnableDefault, 0);
        }
        // Window
        {
            auto menu = mMenuBar->addMenu(STR(TXT_WINDOW));
            // Panels
            {
                auto popup = menu->addItem(STR(TXT_PANELS));
                {
                    // Console
                    popup->addItem(STR(TXT_CONSOLE), "", queryEnableDefault, QUERY_CHECK(mConsoleWnd), TOGGLE_WINDOW_VISIBLE(mConsoleWnd));
                    // Game
                    popup->addItem(STR(TXT_GAME), "", queryEnableDefault, QUERY_CHECK(mGameWnd), TOGGLE_WINDOW_VISIBLE(mGameWnd));
                    // Hierarchy
                    popup->addItem(STR(TXT_HIERARCHY), "", queryEnableDefault, QUERY_CHECK(mHierarchyWnd), TOGGLE_WINDOW_VISIBLE(mHierarchyWnd));
                    // Inspector
                    popup->addItem(STR(TXT_INSPECTOR), "", queryEnableDefault,QUERY_CHECK(mInspectorWnd), TOGGLE_WINDOW_VISIBLE(mInspectorWnd));
                    // Project
                    popup->addItem(STR(TXT_PROJECT), "", queryEnableDefault,QUERY_CHECK(mProjectWnd), TOGGLE_WINDOW_VISIBLE(mProjectWnd));
                    // Scene
                    popup->addItem(STR(TXT_SCENE), "", queryEnableDefault,QUERY_CHECK(mSceneWnd), TOGGLE_WINDOW_VISIBLE(mSceneWnd));
                }
            }
            // Separator
            {
                menu->addSeparator();
            }
            // Layouts
            {
                auto popup = menu->addItem(STR(TXT_LAYOUTS));
                {
                    // Reset All Layouts
                    popup->addItem(STR(TXT_RESET_ALL_LAYOUTS), "",
                        queryEnableDefault,
                        [](ImMenuItem*)
                        {
                            // ImGui::LoadIniSettingsFromMemory(kTinyEditorInitData);
                        });
                }
            }
        }

        return IM_OK;
    }

    //--------------------------------------------------------------------------
    
    NS_END
}
