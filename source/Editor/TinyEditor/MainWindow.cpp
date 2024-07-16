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

    TResult MainWindow::buildFileMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = IM_OK;
        
        auto menu = mMenuBar->addMenu(STR(TXT_FILE));

        do
        {
            // New Scene
            ret = menu->addItem(STR(TXT_NEW_SCENE), "Ctrl+N", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - New Scene - failed !")
            // Open Scene
            ret = menu->addItem(STR(TXT_OPEN_SCENE), "Ctrl+O", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Open Scene - failed !")
            // Open Recent Scene
            ret = menu->addItem(STR(TXT_OPEN_RECENT_SCENE), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Open Recent Scene - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Save
            ret = menu->addItem(STR(TXT_SAVE), "Ctrl+S", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Save - failed !")
            // Save As
            ret = menu->addItem(STR(TXT_SAVE_AS), "Ctrl+Shift+S", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Save As - failed !")
            // Save As Scene Template
            ret = menu->addItem(STR(TXT_SAVE_AS_SCENE_TEMPLATE), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Save As Scene Template - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // New Project
            ret = menu->addItem(STR(TXT_NEW_PROJECT), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - New Project - failed !")
            // Open Project
            ret = menu->addItem(STR(TXT_OPEN_PROJECT), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Open Project - failed !")
            // Save Project
            ret = menu->addItem(STR(TXT_SAVE_PROJECT), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Save Project - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Build Settings...
            ret = menu->addItem(STR(TXT_BUILD_SETTINGS), "Ctrl+Shift+B", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Build Settings... - failed !")
            // Build And Run
            ret = menu->addItem(STR(TXT_BUILD_AND_RUN), "Ctrl+B", queryDisableDefault, 0);
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

    TResult MainWindow::buildEditMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(STR(TXT_EDIT));
            
            // Undo
            ret = menu->addItem(STR(TXT_UNDO), "Ctrl+Z", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Undo - failed !")
            // Redo
            ret = menu->addItem(STR(TXT_REDO), "Ctrl+Y", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Redo - failed !")
            
            // Separator
            ret = menu->addSeparator();
            
            // Select All
            ret = menu->addItem(STR(TXT_SELECT_ALL), "Ctrl+A", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Select All - failed !")
            // Deselect All
            ret = menu->addItem(STR(TXT_DESELECT_ALL), "Shift+D", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Deselect All - failed !")
            // Select Children
            ret = menu->addItem(STR(TXT_SELECT_CHILDREN), "Shift+C", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Select Children - failed !")
            // Select Prefab Root
            ret = menu->addItem(STR(TXT_SELECT_PREFAB_ROOT), "Ctrl+Shift+R", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Select Prefab Root - failed !")
            // Insert Selection
            ret = menu->addItem(STR(TXT_INSERT_SELECTION), "Ctrl+I", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Insert Selection - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Cut
            ret = menu->addItem(STR(TXT_CUT), "Ctrl+X", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Cut - failed !")
            // Copy
            ret = menu->addItem(STR(TXT_COPY), "Ctrl+C", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Copy - failed !")
            // Paste
            ret = menu->addItem(STR(TXT_PASTE), "Ctrl+V", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Paste - failed !")
            
            // Seperator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Duplicate
            ret = menu->addItem(STR(TXT_DUPLICATE), "Ctrl+D", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Duplicate - failed !")
            // Rename
            ret = menu->addItem(STR(TXT_RENAME), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Rename - failed !")
            // Delete
            ret = menu->addItem(STR(TXT_DELETE), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Delete - failed !")
            
            // Separator
            ret = menu->addSeparator();
            
            // Frame Selected
            ret = menu->addItem(STR(TXT_FRAME_SELECTED), "F", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Frame Selected - failed !")
            // Lock View to Selected
            ret = menu->addItem(STR(TXT_LOCK_VIEW_TO_SELECTED), "Shift+F", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Lock View to Selected - failed !")
            
            // Seperator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Find
            ret = menu->addItem(STR(TXT_FIND), "Ctrl+F", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Undo - failed !")
            // Search All...
            ret = menu->addItem(STR(TXT_SEARCH_ALL), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Search All... - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Play
            ret = menu->addItem(STR(TXT_PLAY), "Ctrl+P", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Play - failed !")
            // Pause
            ret = menu->addItem(STR(TXT_PAUSE), "Ctrl+Shift+P", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Pause - failed !")
            // Stop
            ret = menu->addItem(STR(TXT_STOP), "Ctrl+Alt+P", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Stop - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Selection
            {
                auto popup = menu->addItem(STR(TXT_SELECTION));
                
                // Load Selection
                const int32_t kMaxSelections = 10;
                for (int32_t i = 0; i < kMaxSelections; ++i)
                {
                    std::stringstream ss;
                    ss << STR(TXT_LOAD_SELECTION) << " " << i;
                    std::stringstream ssKey;
                    ssKey << "Ctrl+Shift+" << i;
                    ret = popup->addItem(ss.str(), ssKey.str(), queryDisableDefault, 0);
                    std::stringstream ssMsg;
                    ssMsg << "Add menu item - " << ss.str() << " - failed !";
                    CHECK_RESULT_MESSAGE(ret, ssMsg.str())
                    ss.str("");
                    ssKey.str("");
                    ssMsg.str("");
                }

                CHECK_RESULT(ret)

                // Save Selection
                for (int32_t i = 0; i < kMaxSelections; ++i)
                {
                    std::stringstream ss;
                    ss << STR(TXT_SAVE_SELECTION) << " " << i;
                    std::stringstream ssKey;
                    ssKey << "Ctrl+Shift+" << i;
                    popup->addItem(ss.str(), ssKey.str(), queryDisableDefault, 0);
                    std::stringstream ssMsg;
                    ssMsg << "Add menu item - " << ss.str() << " - failed !";
                    CHECK_RESULT_MESSAGE(ret, ssMsg.str())
                    ss.str("");
                    ssKey.str("");
                    ssMsg.str("");
                }

                CHECK_RESULT(ret)
            }
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Project Settings...
            ret = menu->addItem(STR(TXT_PROJECT_SETTINGS), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Project Settings... - failed !")
            // Preferences...
            ret = menu->addItem(STR(TXT_PREFERENCES), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Preferences... - failed !")
            // Shortcuts...
            ret = menu->addItem(STR(TXT_SHORTCUTS), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Shortcuts... - failed !")
            // Clear All PlayerPrefs
            ret = menu->addItem(STR(TXT_CLEAR_ALL_PLAYERPREFS), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Clear All PlayerPrefs - failed !")
            // Graphics Tier
            {
                auto popup = menu->addItem(STR(TXT_GRAPHICS_TIER));

                // Tier 1
                ret = popup->addItem(STR(TXT_GRAPHICS_TIER_1), "", queryDisableDefault, 0);
                CHECK_RESULT_MESSAGE(ret, "Add menu item - Graphics Tier 1 - failed !");
                // Tier 2
                ret = popup->addItem(STR(TXT_GRAPHICS_TIER_2), "", queryDisableDefault, 0);
                CHECK_RESULT_MESSAGE(ret, "Add menu item - Graphics Tier 2 - failed !");
                // Tier 3
                ret = popup->addItem(STR(TXT_GRAPHICS_TIER_3), "", queryDisableDefault, 0);
                CHECK_RESULT_MESSAGE(ret, "Add menu item - Graphics Tier 3 - failed !");
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildWindowMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
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

            // Next Window
            ret = menu->addItem(STR(TXT_NEXT_WINDOW), "Ctrl+Tab", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Next Window - failed !")
            // Previous Window
            ret = menu->addItem(STR(TXT_PREV_WINDOW), "Ctrl+Shift+Tab", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Previous Window - failed !")

            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Layouts
            {
                auto popup = menu->addItem(STR(TXT_LAYOUTS));
                {
                    // Reset All Layouts
                    ret = popup->addItem(STR(TXT_RESET_ALL_LAYOUTS), "",
                        queryDisableDefault,
                        [](ImMenuItem*)
                        {
                            // ImGui::LoadIniSettingsFromMemory(kTinyEditorInitData);
                        });
                    CHECK_RESULT_MESSAGE(ret, "Add menu item - Reset All Layouts - failed !")
                }
            }
            // Search
            {
                auto popup = menu->addItem(STR(TXT_SEARCH));
            }

            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")

            // Package Manager
            ret = menu->addItem(STR(TXT_PACKAGE_MANAGER), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Package Manager - failed !")

            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")

            // Asset Management
            ret = menu->addItem(STR(TXT_ASSET_MANAGEMENT), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Asset Management - failed !")

            // Separator

            // Text
            ret = menu->addItem(STR(TXT_TEXT), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Text - failed !")

            // Separator

            // Text Mesh Pro
            ret = menu->addItem(STR(TXT_TEXT_MESH_PRO), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Text Mesh Pro - failed !")

            // Separator
            
            // General
            {
                auto popup = menu->addItem(STR(TXT_GENERAL));
            }
            // Rendering
            {
                auto popup = menu->addItem(STR(TXT_RENDERING));
            }
            // Animation
            {
                auto popup = menu->addItem(STR(TXT_ANIMATION));
            }
            // Audio
            {
                auto popup = menu->addItem(STR(TXT_AUDIO));
            }
            // Sequencing
            {
                auto popup = menu->addItem(STR(TXT_SEQUENCING));
            }
            // Analysis
            {
                auto popup = menu->addItem(STR(TXT_ANALYSIS));
            }
            // AI
            {
                auto popup = menu->addItem(STR(TXT_AI));
            }
            // UI Toolkit
            {
                auto popup = menu->addItem(STR(TXT_UI_TOOLKIT));
            }
            // Visual Scripting
            {
                auto popup = menu->addItem(STR(TXT_VISUAL_SCRIPTING));
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildAssetsMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(STR(TXT_ASSETS));

            // Create
            {
                auto popup = menu->addItem(STR(TXT_CREATE));
            }
            // Show in Explorer
            ret = menu->addItem(STR(TXT_SHOW_IN_EXPLORER), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Show in Explorer - failed !")
            // Open
            ret = menu->addItem(STR(TXT_OPEN), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Open - failed !")
            // Delete
            ret = menu->addItem(STR(TXT_DELETE), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Delete - failed !")
            // Rename
            ret = menu->addItem(STR(TXT_RENAME), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Rename - failed !")
            // Copy path
            ret = menu->addItem(STR(TXT_COPY_PATH), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Copy Path - failed !")

            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")

            // Open Scene Additive
            ret = menu->addItem(STR(TXT_OPEN_SCENE_ADDITIVE), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Open Scene Additive - failed !")

            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")

            // View in Package Manager
            ret = menu->addItem(STR(TXT_VIEW_IN_PACKAGE_MANAGER), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - View in Package Manager - failed !")

            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")

            // Import New Asset...
            ret = menu->addItem(STR(TXT_IMPORT_NEW_ASSET), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Import New Asset... - failed !")
            // Import Package
            ret = menu->addItem(STR(TXT_IMPORT_PACKAGE), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Import Package - failed !")
            // Export Package...
            ret = menu->addItem(STR(TXT_EXPORT_PACKAGE), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Export Package... - failed !")
            // Find References In Scene
            ret = menu->addItem(STR(TXT_FIND_REFERENCES_IN_SCENE), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Find References In Scene - failed !")
            // Find References In Project
            ret = menu->addItem(STR(TXT_FIND_REFERENCES_IN_PROJECT), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Find References In Project - failed !")
            // Select Dependencies
            ret = menu->addItem(STR(TXT_SELECT_DEPENDENCIES), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Select Dependencies - failed !")

            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")

            // Refresh
            ret = menu->addItem(STR(TXT_REFRESH), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Refresh - failed !")
            // Reimport
            ret = menu->addItem(STR(TXT_REIMPORT), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Reimport - failed !")

            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")

            // Reimport All
            ret = menu->addItem(STR(TXT_REIMPORT_ALL), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Reimport All - failed !")

            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")

            // Extract From Prefab
            ret = menu->addItem(STR(TXT_EXTRACT_FROM_PREFAB), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Extract From Prefab - failed !")

            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")

            // Update UXML Schema
            ret = menu->addItem(STR(TXT_UPDATE_UXML_SCHEMA), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Update UXML Schema - failed !")

            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")

            // Cache Server
            {
                auto popup = menu->addItem(STR(TXT_CACHE_SERVER));
            }

            // Open C++ Project
            ret = menu->addItem(STR(TXT_OPEN_CPP_PROJECT), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Open C++ Project - failed !")
            // View in Import Activity Window
            ret = menu->addItem(STR(TXT_VIEW_IN_IMPORT_ACTIVITY_WINDOW), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - View in Import Activity Window - failed !")
            // Properties
            ret = menu->addItem(STR(TXT_PROPERTIES), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Properties - failed !")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildGameObjectMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(STR(TXT_GAMEOBJECT));
            
            // Create Empty
            ret = menu->addItem(STR(TXT_CREATE_EMPTY), "Ctrl+Shift+N", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Create Empty - failed !")
            // Create Empty Child
            ret = menu->addItem(STR(TXT_CREATE_EMPTY_CHILD), "Alt+Shift+N", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Create Empty Child - failed !")
            // Create Empty Parent
            ret = menu->addItem(STR(TXT_CREATE_EMPTY_PARENT), "Ctrl+Shift+G", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Create Empty Parent - failed !")
            // 3D Object
            {
                auto popup = menu->addItem(STR(TXT_3D_OBJECT));
            }
            // Effects
            {
                auto popup = menu->addItem(STR(TXT_EFFECTS));
            }
            // Light
            {
                auto popup = menu->addItem(STR(TXT_LIGHT));
            }
            // Audio
            {
                auto popup = menu->addItem(STR(TXT_AUDIO));
            }
            // Video
            {
                auto popup = menu->addItem(STR(TXT_VIDEO));
            }
            // UI
            {
                auto popup = menu->addItem(STR(TXT_UI));
            }
            // UI Toolkit
            {
                auto popup = menu->addItem(STR(TXT_UI_TOOLKIT));
            }
            // Camera
            ret = menu->addItem(STR(TXT_CAMERA), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Camera - failed !")
            // Visual Scripting Scene Varaibles
            ret = menu->addItem(STR(TXT_VISUAL_SCRIPTING_SCENE_VARAIBLES), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Visual Scripting Scene Variables - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Center On Children
            ret = menu->addItem(STR(TXT_CENTER_ON_CHILDREN), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Center On Children - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Make Parent
            ret = menu->addItem(STR(TXT_MAKE_PARENT), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Make Parent - failed !")
            // Clear Parent
            ret = menu->addItem(STR(TXT_CLEAR_PARENT), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Clear Parent - failed !")
            
            // Separator
            ret = menu->addSeparator();
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
            
            // Set as first sibling
            ret = menu->addItem(STR(TXT_SET_AS_FIRST_SIBLING), "Ctrl+=", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Set as first sibling - failed !")
            // Set as last sibling
            ret = menu->addItem(STR(TXT_SET_AS_LAST_SIBLING), "Ctrl+-", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Set as last sibling - failed !")
            // Move To View
            ret = menu->addItem(STR(TXT_MOVE_TO_VIEW), "Ctrl+Alt+F", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Move To View - failed !")
            // Align With View
            ret = menu->addItem(STR(TXT_ALIGN_WITH_VIEW), "Ctrl+Shift+F", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Align With View - failed !")
            // Align View to Selected
            ret = menu->addItem(STR(TXT_ALIGN_VIEW_TO_SELECTED), "", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Align View to Selected - failed !")
            // Toggle Active State
            ret = menu->addItem(STR(TXT_TOGGLE_ACTIVE_STATE), "Alt+Shift+A", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Toggle Active State - failed !")
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildComponentMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(STR(TXT_COMPONENT));

            // Add...
            ret = menu->addItem(STR(TXT_ADD), "Ctrl+Shift+A", queryDisableDefault, 0);
            CHECK_RESULT_MESSAGE(ret, "Add menu item - Add... - failed !")
            // Mesh
            {
                auto popup = menu->addItem(STR(TXT_MESH));
            }
            // Effects
            {
                auto popup = menu->addItem(STR(TXT_EFFECTS));
            }
            // Physics
            {
                auto popup = menu->addItem(STR(TXT_PHYSICS));
            }
            // Physics 2D
            {
                auto popup = menu->addItem(STR(TXT_PHYSICS_2D));
            }
            // Navigation
            {
                auto popup = menu->addItem(STR(TXT_NAVIGATION));
            }
            // Audio
            {
                auto popup = menu->addItem(STR(TXT_AUDIO));
            }
            // Video
            {
                auto popup = menu->addItem(STR(TXT_VIDEO));
            }
            // Rendering
            {
                auto popup = menu->addItem(STR(TXT_RENDERING));
            }
            // Tilemap
            {
                auto popup = menu->addItem(STR(TXT_TILEMAP));
            }
            // Layout
            {
                auto popup = menu->addItem(STR(TXT_LAYOUT));
            }
            // Playables
            {
                auto popup = menu->addItem(STR(TXT_PLAYABLES));
            }
            // Miscellaneous
            {
                auto popup = menu->addItem(STR(TXT_MISCELLANEOUS));
            }
            // Scripts
            {
                auto popup = menu->addItem(STR(TXT_SCRIPTS));
            }
            // UI
            {
                auto popup = menu->addItem(STR(TXT_UI));
            }
            // Visual Scripting
            {
                auto popup = menu->addItem(STR(TXT_VISUAL_SCRIPTING));
            }
            // Event
            {
                auto popup = menu->addItem(STR(TXT_EVENT));
            }
            // UI Toolkit
            {
                auto popup = menu->addItem(STR(TXT_UI_TOOLKIT));
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildHelpMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildMenu()
    {
        mMenuBar = new ImMenuBar();
        mMenuBar->create("Main Menu", this);

        auto queryEnableDefault = [](ImMenuItem*) { return true; };
        auto queryDisableDefault = [](ImMenuItem*) { return false; };

        TResult ret = T3D_OK;

        do
        {
            // File
            ret = buildFileMenu(queryEnableDefault, queryDisableDefault);
            CHECK_RESULT_MESSAGE(ret, "Build file menu failed !")

            // Edit
            ret = buildEditMenu(queryEnableDefault, queryDisableDefault);
            CHECK_RESULT_MESSAGE(ret, "Build edit menu failed !")

            // Assets
            ret = buildAssetsMenu(queryEnableDefault, queryDisableDefault);
            CHECK_RESULT_MESSAGE(ret, "Build assets menu failed !")

            // GameObject
            ret = buildGameObjectMenu(queryEnableDefault, queryDisableDefault);
            CHECK_RESULT_MESSAGE(ret, "Build game object menu failed !")

            // Component
            ret = buildComponentMenu(queryEnableDefault, queryDisableDefault);
            CHECK_RESULT_MESSAGE(ret, "Build component menu failed !")
            
            // Window
            ret = buildWindowMenu(queryEnableDefault, queryDisableDefault);
            CHECK_RESULT_MESSAGE(ret, "Build window menu failed !")

            // Help
            ret = buildHelpMenu(queryEnableDefault, queryDisableDefault);
            CHECK_RESULT_MESSAGE(ret, "Build help menu failed !")
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
    
    NS_END
}
