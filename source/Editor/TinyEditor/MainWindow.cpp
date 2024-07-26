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
#include "EditorEventDefine.h"
#include "EditorWidgetID.h"


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
            ret = mHierarchyWnd->create(ID_HIERARCHY_WINDOW, "Hierarchy", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create hierarchy window failed ! ERROR [%d]", ret);
                break;
            }

            mGameWnd = new GameWindow();
            ret = mGameWnd->create(ID_GAME_WINDOW, "Game", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create game window failed ! ERROR [%d]", ret);
                break;
            }

            mSceneWnd = new SceneWindow();
            ret = mSceneWnd->create(ID_SCENE_WINDOW, "Scene", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create scene window failed ! ERROR [%d]", ret);
                break;
            }

            mInspectorWnd = new InspectorWindow();
            ret = mInspectorWnd->create(ID_INSPECTOR_WINDOW, "Inspector", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create inspector window failed ! ERROR [%d]", ret);
                break;
            }

            mProjectWnd = new ProjectWindow();
            ret = mProjectWnd->create(ID_PROJECT_WINDOW, "Project", this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create project window failed ! ERROR [%d]", ret);
                break;
            }

            mConsoleWnd = new ConsoleWindow();
            ret = mConsoleWnd->create(ID_CONSOLE_WINDOW, "Console", this);
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

    #define MENU_ITEM_WITH_EVENT(m, id, txt_id, shortcut, query_enable_cb, event_id) \
        ret = (m)->addItem(id, STR(txt_id), shortcut, query_enable_cb, event_id); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + STR(txt_id) + " - failed !").c_str())

    #define MENU_ITEM_WITH_CB(m, id, txt_id, shortcut, query_enable_cb, clicked_cb) \
        ret =(m)->addItem(id, STR(txt_id), shortcut, query_enable_cb, clicked_cb); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + STR(txt_id) + " - failed !").c_str())

    #define MENU_ITEM_NAME_WITH_EVENT(m, id, name, shortcut, query_enable_cb, event_id) \
        ret = (m)->addItem(id, name, shortcut, query_enable_cb, event_id); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + name + " - failed !").c_str())

    #define MENU_ITEM_NAME_WITH_CB(m, id, name, shortcut, query_enable_cb, clicked_cb) \
        ret = (m)->addItem(id, name, shortcut, query_enable_cb, clicked_cb); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + name + " - failed !").c_str())

    #define MENU_ITEM_CHECK_WITH_EVENT(m, id, txt_id, shortcut, query_enable_cb, query_check_cb, event_id) \
        ret = (m)->addItem(id, STR(txt_id), shortcut, queryEnableDefault, query_check_cb, event_id); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + STR(txt_id) + " - failed !").c_str())
    
    #define MENU_ITEM_CHECK_WITH_CB(m, id, txt_id, shortcut, query_enable_cb, query_check_cb, toggle_cb) \
        ret = (m)->addItem(id, STR(txt_id), shortcut, queryEnableDefault, query_check_cb, toggle_cb); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + STR(txt_id) + " - failed !").c_str())
    
    #define MENU_SEPARATOR(m) \
        ret = (m)->addSeparator(); \
        CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
    
    //--------------------------------------------------------------------------

    TResult MainWindow::buildFileMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = IM_OK;
        
        auto menu = mMenuBar->addMenu(ID_MENU_FILE, STR(TXT_FILE));

        do
        {
            // New Scene
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_NEW_SCENE, TXT_NEW_SCENE, "Ctrl+N", queryDisableDefault, kEvtInvalid)
            // Open Scene
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN_SCENE, TXT_OPEN_SCENE, "Ctrl+O", queryDisableDefault, kEvtInvalid)
            // Open Recent Scene
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN_RECENT_SCENE, TXT_OPEN_RECENT_SCENE, "", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Save
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SAVE, TXT_SAVE, "Ctrl+S", queryDisableDefault, kEvtInvalid)
            // Save As
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SAVE_AS, TXT_SAVE_AS, "Ctrl+Shift+S", queryDisableDefault, kEvtInvalid)
            // Save As Scene Template
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SAVE_AS_SCENE_TEMPLATE, TXT_SAVE_AS_SCENE_TEMPLATE, "", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // New Project
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_NEW_PROJECT, TXT_NEW_PROJECT, "", queryDisableDefault, kEvtInvalid)
            // Open Project
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN_PROJECT, TXT_OPEN_PROJECT, "", queryDisableDefault, kEvtInvalid)
            // Save Project
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SAVE_PROJECT, TXT_SAVE_PROJECT, "", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Build Settings...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_BUILD_SETTINGS, TXT_BUILD_SETTINGS, "Ctrl+Shift+B", queryDisableDefault, kEvtInvalid)
            // Build And Run
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_BUILD_AND_RUN, TXT_BUILD_AND_RUN, "Ctrl+B", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Exit
            MENU_ITEM_WITH_CB(menu, ID_MENU_ITEM_EXIT, TXT_EXIT, "", queryEnableDefault, [](ImWidget*) { static_cast<EditorApp*>(Application::getInstancePtr())->exitApp(); })
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildEditMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(ID_MENU_EDIT, STR(TXT_EDIT));
            
            // Undo
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_UNDO, TXT_UNDO, "Ctrl+Z", queryDisableDefault, kEvtInvalid)
            // Redo
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_REDO, TXT_REDO, "Ctrl+Y", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Select All
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SELECT_ALL, TXT_SELECT_ALL, "Ctrl+A", queryDisableDefault, kEvtInvalid)
            // Deselect All
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_DESELECT_ALL, TXT_DESELECT_ALL, "Shift+D", queryDisableDefault, kEvtInvalid)
            // Select Children
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SELECT_CHILDREN, TXT_SELECT_CHILDREN, "Shift+C", queryDisableDefault, kEvtInvalid)
            // Select Prefab Root
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SELECT_PREFAB_ROOT, TXT_SELECT_PREFAB_ROOT, "Ctrl+Shift+R", queryDisableDefault, kEvtInvalid)
            // Insert Selection
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_INSERT_SELECTION, TXT_INSERT_SELECTION, "Ctrl+I", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Cut
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CUT, TXT_CUT, "Ctrl+X", queryDisableDefault, kEvtInvalid)
            // Copy
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_COPY, TXT_COPY, "Ctrl+C", queryDisableDefault, kEvtInvalid)
            // Paste
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PASTE, TXT_PASTE, "Ctrl+V", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Duplicate
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_DUPLICATE, TXT_DUPLICATE, "Ctrl+D", queryDisableDefault, kEvtInvalid)
            // Rename
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_RENAME, TXT_RENAME, "", queryDisableDefault, kEvtInvalid)
            // Delete
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_DELETE, TXT_DELETE, "", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Frame Selected
            MENU_ITEM_WITH_EVENT(menu, ID_MEHU_ITEM_FRAME_SELECTED, TXT_FRAME_SELECTED, "F", queryDisableDefault, kEvtInvalid)
            // Lock View to Selected
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_LOCK_VIEW_TO_SELECTED, TXT_LOCK_VIEW_TO_SELECTED, "Shift+F", queryDisableDefault, kEvtInvalid)
            
            // Seperator
            MENU_SEPARATOR(menu)
            
            // Find
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_FIND, TXT_FIND, "Ctrl+F", queryDisableDefault, kEvtInvalid)
            // Search All...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SEARCH_ALL, TXT_SEARCH_ALL, "", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Play
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PLAY, TXT_PLAY, "Ctrl+P", queryDisableDefault, kEvtInvalid)
            // Pause
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PAUSE, TXT_PAUSE, "Ctrl+Shift+P", queryDisableDefault, kEvtInvalid)
            // Stop
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_STOP, TXT_STOP, "Ctrl+Alt+P", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Selection
            {
                auto popup = menu->addItem(ID_MENU_ITEM_SELECTION, STR(TXT_SELECTION));
                
                // Load Selection
                const int32_t kMaxSelections = 10;
                for (int32_t i = 0; i < kMaxSelections; ++i)
                {
                    std::stringstream ss;
                    ss << STR(TXT_LOAD_SELECTION) << " " << i;
                    std::stringstream ssKey;
                    ssKey << "Ctrl+Shift+" << i;
                    MENU_ITEM_NAME_WITH_EVENT(popup, ID_MENU_ITEM_LOAD_SELECTION + i, ss.str(), ssKey.str(), queryDisableDefault, kEvtInvalid)
                    ss.str("");
                    ssKey.str("");
                }

                CHECK_RESULT(ret)

                // Save Selection
                for (int32_t i = 0; i < kMaxSelections; ++i)
                {
                    std::stringstream ss;
                    ss << STR(TXT_SAVE_SELECTION) << " " << i;
                    std::stringstream ssKey;
                    ssKey << "Ctrl+Shift+" << i;
                    MENU_ITEM_NAME_WITH_EVENT(popup, ID_MENU_ITEM_SAVE_SELECTION + i, ss.str(), ssKey.str(), queryDisableDefault, kEvtInvalid)
                    ss.str("");
                    ssKey.str("");
                }

                CHECK_RESULT(ret)
            }
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Project Settings...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PROJECT_SETTINGS, TXT_PROJECT_SETTINGS, "", queryDisableDefault, kEvtInvalid)
            // Preferences...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PREFERENCES, TXT_PREFERENCES, "", queryDisableDefault, kEvtInvalid)
            // Shortcuts...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SHORTCUT, TXT_SHORTCUTS, "", queryDisableDefault, kEvtInvalid)
            // Clear All PlayerPrefs
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CLEAR_ALL_PLAYERPREFS, TXT_CLEAR_ALL_PLAYERPREFS, "", queryDisableDefault, kEvtInvalid)
            // Graphics Tier
            {
                auto popup = menu->addItem(ID_MENU_ITEM_GRAPHICS_TIER, STR(TXT_GRAPHICS_TIER));

                // Tier 1
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_GRAPHICS_TIER_1, TXT_GRAPHICS_TIER_1, "", queryDisableDefault, kEvtInvalid)
                // Tier 2
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_GRAPHICS_TIER_2, TXT_GRAPHICS_TIER_2, "", queryDisableDefault, kEvtInvalid)
                // Tier 3
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_GRAPHICS_TIER_3, TXT_GRAPHICS_TIER_3, "", queryDisableDefault, kEvtInvalid)
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
            auto menu = mMenuBar->addMenu(ID_MENU_ASSETS, STR(TXT_ASSETS));

            // Create
            {
                auto popup = menu->addItem(ID_MENU_ITEM_CREATE, STR(TXT_CREATE));

                // Folder
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_FOLDER, TXT_FOLDER, "", queryDisableDefault, kEvtInvalid)
                
                // Separator
                MENU_SEPARATOR(popup)
                
                // Type Script
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_TYPE_SCRIPT, TXT_TYPE_SCRIPT, "", queryDisableDefault, kEvtInvalid)
                // 2D
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_2D, STR(TXT_2D));

                    // Physics Material 2D
                    MENU_ITEM_WITH_EVENT(popup1, ID_MENU_ITEM_PHYSICS_MATERIAL_2D, TXT_PHYSICS_MATERIAL_2D, "", queryDisableDefault, kEvtInvalid)
                }
                // Visual Scripting
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_VISUAL_SCRIPTING, STR(TXT_VISUAL_SCRIPTING));
                }
                // Shader
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_SHADER, STR(TXT_SHADER));
                }
                // Shader Variant Collection
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SHADER_VARIANT_COLLECTION, TXT_SHADER_VARIANT_COLLECTION, "", queryDisableDefault, kEvtInvalid)
                // Testing
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_TESTING, STR(TXT_TESTING));
                }
                // Playables
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_PLAYABLES, STR(TXT_PLAYABLES));
                }
                // Dynamic Link Library
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_DYLIB, TXT_DYLIB, "", queryDisableDefault, kEvtInvalid)
                // Dynamic Link Library Reference
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_DYLIB_REF, TXT_DYLIB_REF, "", queryDisableDefault, kEvtInvalid)

                // Separator
                MENU_SEPARATOR(popup)
                
                // Text 
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_TEXT, STR(TXT_TEXT));
                }
                // Test Mesh Pro
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_TEXT_MESH_PRO, STR(TXT_TEXT_MESH_PRO));
                }

                // Separator
                MENU_SEPARATOR(popup)
                
                // Scene
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SCENE, TXT_SCENE, "", queryDisableDefault, kEvtInvalid)
                // Scene Template
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SCENE_TEMPLATE, TXT_SCENE_TEMPLATE, "", queryDisableDefault, kEvtInvalid)
                // Scene Template From Scene
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SCENE_TEMPLATE_FROM_SCENE, TXT_SCENE_TEMPLATE_FROM_SCENE, "", queryDisableDefault, kEvtInvalid)
                // Scene Template Pipeline
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SCENE_TEMPLATE_PIPELINE, TXT_SCENE_TEMPLATE_PIPELINE, "", queryDisableDefault, kEvtInvalid)
                // Prefab
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_PREFAB, TXT_PREFAB, "", queryDisableDefault, kEvtInvalid)
                // Prefab Variant
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_PREFAB_VARIANT, TXT_PREFAB_VARIANT, "", queryDisableDefault, kEvtInvalid)
                
                // Separator
                MENU_SEPARATOR(popup)
                
                // Audio Mixer
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_AUDIO_MIXER, TXT_AUDIO_MIXER, "", queryDisableDefault, kEvtInvalid)
                
                // Separator
                MENU_SEPARATOR(popup)
                
                // Material
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_MATERIAL, TXT_MATERIAL, "", queryDisableDefault, kEvtInvalid)
                // Render Texture
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_RENDER_TEXTURE, TXT_RENDER_TEXTURE, "", queryDisableDefault, kEvtInvalid)
                // Lightmap Parameters
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_LIGHTMAP_PARAMETERS, TXT_LIGHTMAP_PARAMETERS, "", queryDisableDefault, kEvtInvalid)
                // Lighting Settings
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_LIGHTING_SETTINGS, TXT_LIGHTING_SETTINGS, "", queryDisableDefault, kEvtInvalid)
                // Custom Render Texture
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_CUSTOM_RENDER_TEXTURE, TXT_CUSTOM_RENDER_TEXTURE, "", queryDisableDefault, kEvtInvalid)
                
                // Separator
                MENU_SEPARATOR(popup)
                
                // Animator Controller
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_ANIMATOR_CONTROLLER, TXT_ANIMATOR_CONTROLLER, "", queryDisableDefault, kEvtInvalid)
                // Animation
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_ANIMATION, TXT_ANIMATION, "", queryDisableDefault, kEvtInvalid)
                // Animator Override Controller
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_ANIMATOR_OVERRIDE_CONTROLLER, TXT_ANIMATOR_OVERRIDE_CONTROLLER, "", queryDisableDefault, kEvtInvalid)
                // Avatar Mask
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_AVATAR_MASK, TXT_AVATAR_MASK, "", queryDisableDefault, kEvtInvalid)
                
                // Separator
                MENU_SEPARATOR(popup)
                
                // Timeline
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_TIMELINE, TXT_TIMELINE, "", queryDisableDefault, kEvtInvalid)
                // Signal
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SIGNAL, TXT_SIGNAL, "", queryDisableDefault, kEvtInvalid)

                // Separator
                MENU_SEPARATOR(popup)
                
                // GUI Skin
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_GUI_SKIN, TXT_GUI_SKIN, "", queryDisableDefault, kEvtInvalid)
                // Custom Font
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_CUSTOM_FONT, TXT_CUSTOM_FONT, "", queryDisableDefault, kEvtInvalid)
                
                // Separator
                MENU_SEPARATOR(popup)
                
                // Legacy
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_LEGACY, STR(TXT_LEGACY));
                }
                // UI Toolkit
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_UI_TOOLKIT, STR(TXT_UI_TOOLKIT));
                }

                // Separator
                MENU_SEPARATOR(popup)
                
                // Search
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_SEARCH, STR(TXT_SEARCH));
                }
                // Brush
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_BRUSH, TXT_BRUSH, "", queryDisableDefault, kEvtInvalid)
                // Terrain Layer
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_TERRAIN_LAYER, TXT_TERRAIN_LAYER, "", queryDisableDefault, kEvtInvalid)
            }
            // Show in Explorer
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SHOW_IN_EXPLORER, TXT_SHOW_IN_EXPLORER, "", queryDisableDefault, kEvtInvalid)
            // Open
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN, TXT_OPEN, "", queryDisableDefault, kEvtInvalid)
            // Delete
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_DELETE_ASSET, TXT_DELETE, "", queryDisableDefault, kEvtInvalid)
            // Rename
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_RENAME_ASSET, TXT_RENAME, "", queryDisableDefault, kEvtInvalid)
            // Copy path
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_COPY_PATH, TXT_COPY_PATH, "Alt+Ctrl+C", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)

            // Open Scene Additive
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN_SCENE_ADDITIVE, TXT_OPEN_SCENE_ADDITIVE, "", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)

            // View in Package Manager
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_VIEW_IN_PACKAGE_MANAGER, TXT_VIEW_IN_PACKAGE_MANAGER, "", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)

            // Import New Asset...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_IMPORT_NEW_ASSET, TXT_IMPORT_NEW_ASSET, "", queryDisableDefault, kEvtInvalid)
            // Import Package
            {
                auto popup = menu->addItem(ID_MENU_ITEM_IMPORT_PACKAGE, STR(TXT_IMPORT_PACKAGE));

                // Custom Package
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_CUSTOM_PACKAGE, TXT_CUSTOM_PACKAGE, "", queryDisableDefault, kEvtInvalid)
            }
            // Export Package...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_EXPORT_PACKAGE, TXT_EXPORT_PACKAGE, "", queryDisableDefault, kEvtInvalid)
            // Find References In Scene
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_FIND_REFERENCES_IN_SCENE, TXT_FIND_REFERENCES_IN_SCENE, "", queryDisableDefault, kEvtInvalid)
            // Find References In Project
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_FIND_REFERENCES_IN_PROJECT, TXT_FIND_REFERENCES_IN_PROJECT, "", queryDisableDefault, kEvtInvalid)
            // Select Dependencies
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SELECT_DEPENDENCIES, TXT_SELECT_DEPENDENCIES, "", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)

            // Refresh
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_REFRESH, TXT_REFRESH, "Ctrl+R", queryDisableDefault, kEvtInvalid)
            // Reimport
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_REIMPORT, TXT_REIMPORT, "", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)

            // Reimport All
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_REIMPORT_ALL, TXT_REIMPORT_ALL, "", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)

            // Extract From Prefab
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_EXTRACT_FROM_PREFAB, TXT_EXTRACT_FROM_PREFAB, "", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)

            // Update UXML Schema
            // MENU_ITEM_WITH_EVENT(menu, TXT_UPDATE_UXML_SCHEMA, "", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)

            // Open C++ Project
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN_CPP_PROJECT, TXT_OPEN_CPP_PROJECT, "", queryDisableDefault, kEvtInvalid)
            // View in Import Activity Window
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_IETM_VIEW_IN_IMPORT_ACTIVITY_WINDOW, TXT_VIEW_IN_IMPORT_ACTIVITY_WINDOW, "", queryDisableDefault, kEvtInvalid)
            // Properties
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PROPERTIES, TXT_PROPERTIES, "Alt+P", queryDisableDefault, kEvtInvalid)
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildGameObjectMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(ID_MENU_GAME_OBJECT, STR(TXT_GAMEOBJECT));
            
            // Create Empty
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CREATE_EMPTY, TXT_CREATE_EMPTY, "Ctrl+Shift+N", queryDisableDefault, kEvtInvalid)
            // Create Empty Child
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CREATE_EMPTY_CHILD, TXT_CREATE_EMPTY_CHILD, "Alt+Shift+N", queryDisableDefault, kEvtInvalid)
            // Create Empty Parent
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CREATE_EMPTY_PARENT, TXT_CREATE_EMPTY_PARENT, "Ctrl+Shift+G", queryDisableDefault, kEvtInvalid)
            // 3D Object
            {
                auto popup = menu->addItem(ID_MENU_ITEM_3D_OBJECT, STR(TXT_3D_OBJECT));
            }
            // Effects
            {
                auto popup = menu->addItem(ID_MENU_ITEM_EFFECTS, STR(TXT_EFFECTS));
            }
            // Light
            {
                auto popup = menu->addItem(ID_MENU_ITEM_LIGHT, STR(TXT_LIGHT));
            }
            // Audio
            {
                auto popup = menu->addItem(ID_MENU_ITEM_AUDIO, STR(TXT_AUDIO));
            }
            // Video
            {
                auto popup = menu->addItem(ID_MENU_ITEM_VIDEO, STR(TXT_VIDEO));
            }
            // UI
            {
                auto popup = menu->addItem(ID_MENU_ITEM_UI, STR(TXT_UI));
            }
            // UI Toolkit
            {
                auto popup = menu->addItem(ID_MENU_ITEM_GAMEOJECT_UI_TOOLKIT, STR(TXT_UI_TOOLKIT));
            }
            // Camera
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CAMERA, TXT_CAMERA, "", queryDisableDefault, kEvtInvalid)
            // Visual Scripting Scene Varaibles
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_VISUAL_SCRIPTING_SCENE_VARAIBLES, TXT_VISUAL_SCRIPTING_SCENE_VARAIBLES, "", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Center On Children
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CENTER_ON_CHILDREN, TXT_CENTER_ON_CHILDREN, "", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Make Parent
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_MAKE_PARENT, TXT_MAKE_PARENT, "", queryDisableDefault, kEvtInvalid)
            // Clear Parent
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CLEAR_PARENT, TXT_CLEAR_PARENT, "", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Set as first sibling
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SET_AS_FIRST_SIBLING, TXT_SET_AS_FIRST_SIBLING, "Ctrl+=", queryDisableDefault, kEvtInvalid)
            // Set as last sibling
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SET_AS_LAST_SIBLING, TXT_SET_AS_LAST_SIBLING, "Ctrl+-", queryDisableDefault, kEvtInvalid)
            // Move To View
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_MOVE_TO_VIEW, TXT_MOVE_TO_VIEW, "Ctrl+Alt+F", queryDisableDefault, kEvtInvalid)
            // Align With View
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_ALIGN_WITH_VIEW, TXT_ALIGN_WITH_VIEW, "Ctrl+Shift+F", queryDisableDefault, kEvtInvalid)
            // Align View to Selected
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_ALIGN_VIEW_TO_SELECTED, TXT_ALIGN_VIEW_TO_SELECTED, "", queryDisableDefault, kEvtInvalid)
            // Toggle Active State
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_TOGGLE_ACTIVE_STATE, TXT_TOGGLE_ACTIVE_STATE, "Alt+Shift+A", queryDisableDefault, kEvtInvalid)
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildComponentMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(ID_MENU_COMPONENT, STR(TXT_COMPONENT));

            // Add...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_ADD, TXT_ADD, "Ctrl+Shift+A", queryDisableDefault, kEvtInvalid)
            // Mesh
            {
                auto popup = menu->addItem(ID_MENU_ITEM_MESH, STR(TXT_MESH));
            }
            // Effects
            {
                auto popup = menu->addItem(ID_MENU_ITEM_EFFECTS_COMPONENT, STR(TXT_EFFECTS));
            }
            // Physics
            {
                auto popup = menu->addItem(ID_MENU_ITEM_PHYSICS, STR(TXT_PHYSICS));
            }
            // Physics 2D
            {
                auto popup = menu->addItem(ID_MENU_ITEM_PHYSICS_2D, STR(TXT_PHYSICS_2D));
            }
            // Navigation
            {
                auto popup = menu->addItem(ID_MENU_ITEM_NAVIGATION, STR(TXT_NAVIGATION));
            }
            // Audio
            {
                auto popup = menu->addItem(ID_MENU_ITEM_AUDIO_COMPONENT, STR(TXT_AUDIO));
            }
            // Video
            {
                auto popup = menu->addItem(ID_MENU_ITEM_VIDEO_COMPONENT, STR(TXT_VIDEO));
            }
            // Rendering
            {
                auto popup = menu->addItem(ID_MENU_ITEM_RENDERING, STR(TXT_RENDERING));
            }
            // Tilemap
            {
                auto popup = menu->addItem(ID_MENU_ITEM_TILEMAP, STR(TXT_TILEMAP));
            }
            // Layout
            {
                auto popup = menu->addItem(ID_MENU_ITEM_LAYOUT, STR(TXT_LAYOUT));
            }
            // Playables
            {
                auto popup = menu->addItem(ID_MENU_ITEM_PLAYABLES_COMPONENT, STR(TXT_PLAYABLES));
            }
            // Miscellaneous
            {
                auto popup = menu->addItem(ID_MENU_ITEM_MISCELLANEOUS, STR(TXT_MISCELLANEOUS));
            }
            // Scripts
            {
                auto popup = menu->addItem(ID_MENU_ITEM_SCRIPTS, STR(TXT_SCRIPTS));
            }
            // UI
            {
                auto popup = menu->addItem(ID_MENU_ITEM_UI_COMPONENT, STR(TXT_UI));
            }
            // Visual Scripting
            {
                auto popup = menu->addItem(ID_MENU_ITEM_VISUAL_SCRIPTING_COMPONENT, STR(TXT_VISUAL_SCRIPTING));
            }
            // Event
            {
                auto popup = menu->addItem(ID_MENU_ITEM_EVENT, STR(TXT_EVENT));
            }
            // UI Toolkit
            {
                auto popup = menu->addItem(ID_MENU_ITEM_UI_TOOLKIT_COMPONENT, STR(TXT_UI_TOOLKIT));
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

        #define QUERY_CHECK(widget)             [&window=(widget), &queryCheck](ImWidget*) { return queryCheck(window); }
        #define TOGGLE_WINDOW_VISIBLE(widget)   [&window=(widget), &toggleWindowVisible](ImWidget*) { toggleWindowVisible(window); }

        TResult ret = T3D_OK;
        
        do
        {
            auto menu = mMenuBar->addMenu(ID_MENU_WINDOW, STR(TXT_WINDOW));
            
            // Panels
            {
                auto popup = menu->addItem(ID_MENU_ITEM_PANELS, STR(TXT_PANELS));
                {
                    // Console
                    MENU_ITEM_CHECK_WITH_CB(popup, ID_MENU_ITEM_CONSOLE_WINDOW, TXT_CONSOLE, "", queryEnableDefault, QUERY_CHECK(mConsoleWnd), TOGGLE_WINDOW_VISIBLE(mConsoleWnd))
                    // Game
                    MENU_ITEM_CHECK_WITH_CB(popup, ID_MENU_ITEM_GAME_WIDNOW, TXT_GAME, "", queryEnableDefault, QUERY_CHECK(mGameWnd), TOGGLE_WINDOW_VISIBLE(mGameWnd))
                    // Hierarchy
                    MENU_ITEM_CHECK_WITH_CB(popup, ID_MENU_ITEM_HIERARCHY_WINDOW, TXT_HIERARCHY, "", queryEnableDefault, QUERY_CHECK(mHierarchyWnd), TOGGLE_WINDOW_VISIBLE(mHierarchyWnd))
                    // Inspector
                    MENU_ITEM_CHECK_WITH_CB(popup, ID_MENU_ITEM_INSPECTOR_WINDOW, TXT_INSPECTOR, "", queryEnableDefault,QUERY_CHECK(mInspectorWnd), TOGGLE_WINDOW_VISIBLE(mInspectorWnd))
                    // Project
                    MENU_ITEM_CHECK_WITH_CB(popup, ID_MENU_ITEM_PROJECT_WINDOW, TXT_PROJECT, "", queryEnableDefault,QUERY_CHECK(mProjectWnd), TOGGLE_WINDOW_VISIBLE(mProjectWnd))
                    // Scene
                    MENU_ITEM_CHECK_WITH_CB(popup, ID_MENU_ITEM_SCENE_WINDOW, TXT_SCENE, "", queryEnableDefault,QUERY_CHECK(mSceneWnd), TOGGLE_WINDOW_VISIBLE(mSceneWnd))
                }
            }
            
            // Separator
            MENU_SEPARATOR(menu)

            // Next Window
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_NEXT_WINDOW, TXT_NEXT_WINDOW, "Ctrl+Tab", queryDisableDefault, kEvtInvalid)
            // Previous Window
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PREV_WINDOW, TXT_PREV_WINDOW, "Ctrl+Shift+Tab", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)
            
            // Layouts
            {
                auto popup = menu->addItem(ID_MENU_ITEM_LAYOUTS, STR(TXT_LAYOUTS));
                {
                    // Reset All Layouts
                    MENU_ITEM_WITH_CB(popup, ID_MENU_ITEM_RESET_ALL_LAYOUTS, TXT_RESET_ALL_LAYOUTS, "",
                        queryDisableDefault,
                        [](ImWidget*)
                        {
                            // ImGui::LoadIniSettingsFromMemory(kTinyEditorInitData);
                        })
                }
            }
            // Search
            {
                auto popup = menu->addItem(ID_MENU_ITEM_SEARCH_WINDOW, STR(TXT_SEARCH));
            }

            // Separator
            MENU_SEPARATOR(menu)

            // Package Manager
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PACKAGE_MANAGER, TXT_PACKAGE_MANAGER, "", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)

            // Asset Management
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_ASSET_MANAGEMENT, TXT_ASSET_MANAGEMENT, "", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)

            // Text
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_TEXT_WINDOW, TXT_TEXT, "", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)

            // Text Mesh Pro
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_TEXT_MESH_PRO_WINDOW, TXT_TEXT_MESH_PRO, "", queryDisableDefault, kEvtInvalid)

            // Separator
            MENU_SEPARATOR(menu)
            
            // General
            {
                auto popup = menu->addItem(ID_MENU_ITEM_GENERAL, STR(TXT_GENERAL));
            }
            // Rendering
            {
                auto popup = menu->addItem(ID_MENU_ITEM_RENDERING_WINDOW, STR(TXT_RENDERING));
            }
            // Animation
            {
                auto popup = menu->addItem(ID_MENU_ITEM_ANIMATION_WINDOW, STR(TXT_ANIMATION));
            }
            // Audio
            {
                auto popup = menu->addItem(ID_MENU_ITEM_AUDIO_WINDOW, STR(TXT_AUDIO));
            }
            // Sequencing
            {
                auto popup = menu->addItem(ID_MENU_ITEM_SEQUENCING, STR(TXT_SEQUENCING));
            }
            // Analysis
            {
                auto popup = menu->addItem(ID_MENU_ITEM_ANALYSIS, STR(TXT_ANALYSIS));
            }
            // AI
            {
                auto popup = menu->addItem(ID_MENU_ITEM_AI, STR(TXT_AI));
            }
            // UI Toolkit
            {
                auto popup = menu->addItem(ID_MENU_ITEM_UI_TOOLKIT_WINDOW, STR(TXT_UI_TOOLKIT));
            }
            // Visual Scripting
            {
                auto popup = menu->addItem(ID_MENU_ITEM_VISUAL_SCRIPTING_WINDOW, STR(TXT_VISUAL_SCRIPTING));
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
            auto menu = mMenuBar->addMenu(ID_MENU_HELP, STR(TXT_HELP));

            // About
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_ABOUT, TXT_ABOUT, "", queryDisableDefault, kEvtInvalid)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // User Manual
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_USER_MANUAL, TXT_USER_MANUAL, "", queryDisableDefault, kEvtInvalid)
            // API Document
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_API_DOCUMENT, TXT_API_DOCUMENT, "", queryDisableDefault, kEvtInvalid)
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MainWindow::buildMenu()
    {
        mMenuBar = new ImMenuBar();
        mMenuBar->create(ID_MAIN_MENU, "Main Menu", this);

        auto queryEnableDefault = [](ImWidget*) { return true; };
        auto queryDisableDefault = [](ImWidget*) { return false; };

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
