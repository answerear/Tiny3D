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
#include "EditorApp.h"



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
    
    #define CHECK_RESULT_MESSAGE(ret, message) \
        if (T3D_FAILED(ret)) \
        { \
            T3D_LOG_ERROR(LOG_TAG_EDITOR, "%s ERROR [%d]", (message), (ret)); \
            break; \
        }

    #define CHECK_RESULT(ret) \
        if (T3D_FAILED(ret)) \
        { \
            break; \
        }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildFileMenu(const ImMenuItemQueryCallback &queryEnableDefault)
    {
        TResult ret = IM_OK;
        
        auto menu = mMenuBar->addMenu(STR(TXT_FILE));

        do
        {
            // New Scene
            ret = menu->addItem(STR(TXT_NEW_SCENE), "Ctrl+N", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - New Scene - failed !")
            // Open Scene
            ret = menu->addItem(STR(TXT_OPEN_SCENE), "Ctrl+O", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Open Scene - failed !")
            // Open Recent Scene
            ret = menu->addItem(STR(TXT_OPEN_RECENT_SCENE), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Open Recent Scene - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Save
            ret = menu->addItem(STR(TXT_SAVE), "Ctrl+S", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Save - failed !")
            // Save As
            ret = menu->addItem(STR(TXT_SAVE_AS), "Ctrl+Shift+S", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Save As - failed !")
            // Save As Scene Template
            ret = menu->addItem(STR(TXT_SAVE_AS_SCENE_TEMPLATE), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Save As Scene Template - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // New Project
            ret = menu->addItem(STR(TXT_NEW_PROJECT), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - New Project - failed !")
            // Open Project
            ret = menu->addItem(STR(TXT_OPEN_PROJECT), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Open Project - failed !")
            // Save Project
            ret = menu->addItem(STR(TXT_SAVE_PROJECT), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Save Project - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Build Settings...
            ret = menu->addItem(STR(TXT_BUILD_SETTINGS), "Ctrl+Shift+B", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Build Settings... - failed !")
            // Build And Run
            ret = menu->addItem(STR(TXT_BUILD_AND_RUN), "Ctrl+B", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Build And Run - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Exit
            ret = menu->addItem(STR(TXT_EXIT), "", queryEnableDefault, [](ImMenuItem*) { static_cast<EditorApp*>(Application::getInstancePtr())->exitApp(); });
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Exit - failed !")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildEditMenu(const ImMenuItemQueryCallback &queryEnableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(STR(TXT_EDIT));
            
            // Undo
            ret = menu->addItem(STR(TXT_UNDO), "Ctrl+Z", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Undo - failed !")
            // Redo
            ret = menu->addItem(STR(TXT_REDO), "Ctrl+Y", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Redo - failed !")
            
            // Separator
            ret = menu->addSeparator();
            
            // Select All
            ret = menu->addItem(STR(TXT_SELECT_ALL), "Ctrl+A", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Select All - failed !")
            // Deselect All
            ret = menu->addItem(STR(TXT_DESELECT_ALL), "Shift+D", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Deselect All - failed !")
            // Select Children
            ret = menu->addItem(STR(TXT_SELECT_CHILDREN), "Shift+C", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Select Children - failed !")
            // Select Prefab Root
            ret = menu->addItem(STR(TXT_SELECT_PREFAB_ROOT), "Ctrl+Shift+R", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Select Prefab Root - failed !")
            // Insert Selection
            ret = menu->addItem(STR(TXT_INSERT_SELECTION), "Ctrl+I", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Insert Selection - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Cut
            ret = menu->addItem(STR(TXT_CUT), "Ctrl+X", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Cut - failed !")
            // Copy
            ret = menu->addItem(STR(TXT_COPY), "Ctrl+C", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Copy - failed !")
            // Paste
            ret = menu->addItem(STR(TXT_PASTE), "Ctrl+V", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Paste - failed !")
            
            // Seperator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Duplicate
            ret = menu->addItem(STR(TXT_DUPLICATE), "Ctrl+D", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Duplicate - failed !")
            // Rename
            ret = menu->addItem(STR(TXT_RENAME), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Rename - failed !")
            // Delete
            ret = menu->addItem(STR(TXT_DELETE), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Delete - failed !")
            
            // Separator
            ret = menu->addSeparator();
            
            // Frame Selected
            ret = menu->addItem(STR(TXT_FRAME_SELECTED), "F", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Frame Selected - failed !")
            // Lock View to Selected
            ret = menu->addItem(STR(TXT_LOCK_VIEW_TO_SELECTED), "Shift+F", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Lock View to Selected - failed !")
            
            // Seperator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Find
            ret = menu->addItem(STR(TXT_FIND), "Ctrl+F", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Undo - failed !")
            // Search All...
            ret = menu->addItem(STR(TXT_SEARCH_ALL), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Search All... - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Play
            ret = menu->addItem(STR(TXT_PLAY), "Ctrl+P", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Play - failed !")
            // Pause
            ret = menu->addItem(STR(TXT_PAUSE), "Ctrl+Shift+P", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Pause - failed !")
            // Stop
            ret = menu->addItem(STR(TXT_STOP), "Ctrl+Alt+P", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Stop - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Selection
            {
                auto popup = menu->addItem(STR(TXT_SELECTION));
                {
                    
                }
            }
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Project Settings...
            ret = menu->addItem(STR(TXT_PROJECT_SETTINGS), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Project Settings... - failed !")
            // Preferences...
            ret = menu->addItem(STR(TXT_PREFERENCES), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Preferences... - failed !")
            // Shortcuts...
            ret = menu->addItem(STR(TXT_SHORTCUTS), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Shortcuts... - failed !")
            // Clear All PlayerPrefs
            ret = menu->addItem(STR(TXT_CLEAR_ALL_PLAYERPREFS), "", queryEnableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Clear All PlayerPrefs - failed !")
            // Graphics Tier
            {
                auto popup = menu->addItem(STR(TXT_GRAPHICS_TIER));
                {
                    
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildWindowMenu(const ImMenuItemQueryCallback &queryEnableDefault)
    {
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

        TResult ret = T3D_OK;
        
        do
        {
            auto menu = mMenuBar->addMenu(STR(TXT_WINDOW));
            
            // Panels
            {
                auto popup = menu->addItem(STR(TXT_PANELS));
                {
                    // Console
                    ret = popup->addItem(STR(TXT_CONSOLE), "", queryEnableDefault, QUERY_CHECK(mConsoleWnd), TOGGLE_WINDOW_VISIBLE(mConsoleWnd));
                    CHECK_RESULT_MESSAGE(ret, "Add menu item - Console - failed !")
                    // Game
                    ret = popup->addItem(STR(TXT_GAME), "", queryEnableDefault, QUERY_CHECK(mGameWnd), TOGGLE_WINDOW_VISIBLE(mGameWnd));
                    CHECK_RESULT_MESSAGE(ret, "Add menu item - Game - failed !")
                    // Hierarchy
                    ret = popup->addItem(STR(TXT_HIERARCHY), "", queryEnableDefault, QUERY_CHECK(mHierarchyWnd), TOGGLE_WINDOW_VISIBLE(mHierarchyWnd));
                    CHECK_RESULT_MESSAGE(ret, "Add menu item - Hierarchy - failed !")
                    // Inspector
                    ret = popup->addItem(STR(TXT_INSPECTOR), "", queryEnableDefault,QUERY_CHECK(mInspectorWnd), TOGGLE_WINDOW_VISIBLE(mInspectorWnd));
                    CHECK_RESULT_MESSAGE(ret, "Add menu item - Inspector - failed !")
                    // Project
                    ret = popup->addItem(STR(TXT_PROJECT), "", queryEnableDefault,QUERY_CHECK(mProjectWnd), TOGGLE_WINDOW_VISIBLE(mProjectWnd));
                    CHECK_RESULT_MESSAGE(ret, "Add menu item - Project - failed !")
                    // Scene
                    ret = popup->addItem(STR(TXT_SCENE), "", queryEnableDefault,QUERY_CHECK(mSceneWnd), TOGGLE_WINDOW_VISIBLE(mSceneWnd));
                    CHECK_RESULT_MESSAGE(ret, "Add menu item - Scene - failed !")
                }
            }
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Layouts
            {
                auto popup = menu->addItem(STR(TXT_LAYOUTS));
                {
                    // Reset All Layouts
                    ret = popup->addItem(STR(TXT_RESET_ALL_LAYOUTS), "",
                        queryEnableDefault,
                        [](ImMenuItem*)
                        {
                            // ImGui::LoadIniSettingsFromMemory(kTinyEditorInitData);
                        });
                    CHECK_RESULT_MESSAGE(ret, "Add menu item - Reset All Layouts - failed !")
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildMenu()
    {
        mMenuBar = new ImMenuBar();
        mMenuBar->create("Main Menu", this);

        auto queryEnableDefault = [](ImMenuItem*) {return true;};

        TResult ret = T3D_OK;

        do
        {
            // File
            ret = buildFileMenu(queryEnableDefault);
            CHECK_RESULT_MESSAGE(ret, "Build file menu failed !")

            // Edit
            ret = buildEditMenu(queryEnableDefault);
            CHECK_RESULT_MESSAGE(ret, "Build edit menu failed !")

            // Window
            ret = buildWindowMenu(queryEnableDefault);
            CHECK_RESULT_MESSAGE(ret, "Build window menu failed !");
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
    
    NS_END
}
