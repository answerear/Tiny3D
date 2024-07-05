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

    TResult MainWindow::buildMenu()
    {
        mMenuBar = new ImMenuBar();
        mMenuBar->create("Main Menu", this);

        auto queryEnableDefault = [](ImMenuItem*) {return true;};
        
        // File
        {
            auto menu = mMenuBar->addMenu(STR(TXT_FILE));
            // New
            {
                menu->addItem(STR(TXT_NEW_SCENE), "Ctrl+N", queryEnableDefault, 0);
            }
        }
        // Window
        {
            auto menu = mMenuBar->addMenu(STR(TXT_WINDOW));
            // Panels
            {
                auto popup = menu->addItem(STR(TXT_PANELS));
                {
                    // console
                    popup->addItem(STR(TXT_CONSOLE), "",
                        queryEnableDefault,
                        [this](ImMenuItem*) { return mConsoleWnd != nullptr ? mConsoleWnd->isVisible() : false; },
                        [this](ImMenuItem*) { if (mConsoleWnd != nullptr) mConsoleWnd->setVisible(!mConsoleWnd->isVisible()); });
                    // Game
                    popup->addItem(STR(TXT_GAME), "",
                        queryEnableDefault,
                        [this](ImMenuItem*) { return mGameWnd != nullptr ? mGameWnd->isVisible() : false; },
                        [this](ImMenuItem*) { if (mGameWnd != nullptr) mGameWnd->setVisible(!mGameWnd->isVisible()); });
                    // Hierarchy
                    popup->addItem(STR(TXT_HIERARCHY), "",
                        queryEnableDefault,
                        [this](ImMenuItem*) { return mHierarchyWnd != nullptr ? mHierarchyWnd->isVisible() : false; },
                        [this](ImMenuItem*) { if (mHierarchyWnd != nullptr) mHierarchyWnd->setVisible(!mHierarchyWnd->isVisible()); });
                    // Inspector
                    popup->addItem(STR(TXT_INSPECTOR), "",
                        queryEnableDefault,
                        [this](ImMenuItem*) { return mInspectorWnd != nullptr ? mInspectorWnd->isVisible() : false; },
                        [this](ImMenuItem*) { if (mInspectorWnd != nullptr) mInspectorWnd->setVisible(!mInspectorWnd->isVisible()); });
                    // Project
                    popup->addItem(STR(TXT_PROJECT), "",
                        queryEnableDefault,
                        [this](ImMenuItem*) { return mProjectWnd != nullptr ? mProjectWnd->isVisible() : false; },
                        [this](ImMenuItem*) { if (mProjectWnd != nullptr) mProjectWnd->setVisible(!mProjectWnd->isVisible()); });
                    // Scene
                    popup->addItem(STR(TXT_SCENE), "",
                        queryEnableDefault,
                        [this](ImMenuItem*) { return mSceneWnd != nullptr ? mSceneWnd->isVisible() : false; },
                        [this](ImMenuItem*) { if (mSceneWnd != nullptr) mSceneWnd->setVisible(!mSceneWnd->isVisible()); });
                }
            }
        }

        return IM_OK;
    }

    //--------------------------------------------------------------------------
    
    NS_END
}
