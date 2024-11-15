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


#include "UIMainWindow.h"
#include "UIConsoleWindow.h"
#include "UIHierarchyWindow.h"
#include "UIGameWindow.h"
#include "UISceneWindow.h"
#include "ImErrors.h"
#include "UIInspectorWindow.h"
#include "UIProjectWindow.h"
#include "EditorApp.h"
#include "EditorEventDefine.h"
#include "UIEditorWidgetID.h"
#include "../../Plugins/Renderer/Null/Include/T3DNULLPrerequisites.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)
    
    //--------------------------------------------------------------------------

    extern const char *kTinyEditorInitData;

    //--------------------------------------------------------------------------
    
    TResult UIMainWindow::onCreate()
    {
        TResult ret;

        do
        {
#if defined (USE_LEGACY_MENU)
            ret = buildMenu();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Build main menu failed ! ERROR [%d]", ret);
                break;
            }
#else
            createMenuItemData();

            ret = buildMainMenu();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to build main menu ! ERROR [%d]", ret);
                break;
            }
#endif

            mHierarchyWnd = new UIHierarchyWindow();
            ret = mHierarchyWnd->create(ID_HIERARCHY_WINDOW, "Hierarchy", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create hierarchy window failed ! ERROR [%d]", ret);
                break;
            }

            mGameWnd = new UIGameWindow();
            ret = mGameWnd->create(ID_GAME_WINDOW, "Game", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create game window failed ! ERROR [%d]", ret);
                break;
            }

            mSceneWnd = new UISceneWindow();
            ret = mSceneWnd->create(ID_SCENE_WINDOW, "Scene", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create scene window failed ! ERROR [%d]", ret);
                break;
            }

            mInspectorWnd = new UIInspectorWindow();
            ret = mInspectorWnd->create(ID_INSPECTOR_WINDOW, "Inspector", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create inspector window failed ! ERROR [%d]", ret);
                break;
            }

            mProjectWnd = new UIProjectWindow();
            ret = mProjectWnd->create(ID_PROJECT_WINDOW, "Project", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create project window failed ! ERROR [%d]", ret);
                break;
            }

            mConsoleWnd = new UIConsoleWindow();
            ret = mConsoleWnd->create(ID_CONSOLE_WINDOW, "Console", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Create console window failed ! ERROR [%d]", ret);
                break;
            }
        } while (false);
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool UIMainWindow::onGUIBegin()
    {
        checkFocused();
        
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

    void UIMainWindow::onGUIEnd()
    {
        // ImGui::Image(mSceneRT, ImVec2(640, 480));

        // ImGui::PopStyleVar(2);
        // ImGui::End();
        // ImGui::PopStyleVar(2);
        //
        // PopWidgetID();
    }

    //--------------------------------------------------------------------------

#if defined (USE_LEGACY_MENU)
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

    #define MENU_ITEM_WITH_EVENT(m, id, txt_id, shortcut, query_enable_cb) \
        ret = (m)->addItem(id, STR(txt_id), shortcut, query_enable_cb); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + STR(txt_id) + " - failed !").c_str())

    #define MENU_ITEM_WITH_CB(m, id, txt_id, shortcut, query_enable_cb, clicked_cb) \
        ret =(m)->addItem(id, STR(txt_id), shortcut, query_enable_cb, clicked_cb); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + STR(txt_id) + " - failed !").c_str())

    #define MENU_ITEM_NAME_WITH_EVENT(m, id, name, shortcut, query_enable_cb) \
        ret = (m)->addItem(id, name, shortcut, query_enable_cb); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + name + " - failed !").c_str())

    #define MENU_ITEM_NAME_WITH_CB(m, id, name, shortcut, query_enable_cb, clicked_cb) \
        ret = (m)->addItem(id, name, shortcut, query_enable_cb, clicked_cb); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + name + " - failed !").c_str())

    #define MENU_ITEM_CHECK_WITH_EVENT(m, id, txt_id, shortcut, query_enable_cb, query_check_cb) \
        ret = (m)->addItem(id, STR(txt_id), shortcut, queryEnableDefault, query_check_cb); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + STR(txt_id) + " - failed !").c_str())
    
    #define MENU_ITEM_CHECK_WITH_CB(m, id, txt_id, shortcut, query_enable_cb, query_check_cb, toggle_cb) \
        ret = (m)->addItem(id, STR(txt_id), shortcut, queryEnableDefault, query_check_cb, toggle_cb); \
        CHECK_RESULT_MESSAGE(ret, ("Add menu item - " + STR(txt_id) + " - failed !").c_str())
    
    #define MENU_SEPARATOR(m) \
        ret = (m)->addSeparator(); \
        CHECK_RESULT_MESSAGE(ret, "Add menu item - Separator - failed !")
    
    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildFileMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = IM_OK;
        
        auto menu = mMenuBar->addMenu(ID_MENU_FILE, STR(TXT_FILE));

        do
        {
            // New Scene
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_NEW_SCENE, TXT_NEW_SCENE, "Ctrl+N", queryDisableDefault)
            // Open Scene
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN_SCENE, TXT_OPEN_SCENE, "Ctrl+O", queryDisableDefault)
            // Open Recent Scene
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN_RECENT_SCENE, TXT_OPEN_RECENT_SCENE, "", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Save
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SAVE, TXT_SAVE, "Ctrl+S", queryDisableDefault)
            // Save As
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SAVE_AS, TXT_SAVE_AS, "Ctrl+Shift+S", queryDisableDefault)
            // Save As Scene Template
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SAVE_AS_SCENE_TEMPLATE, TXT_SAVE_AS_SCENE_TEMPLATE, "", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // New Project
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_NEW_PROJECT, TXT_NEW_PROJECT, "", queryDisableDefault)
            // Open Project
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN_PROJECT, TXT_OPEN_PROJECT, "", queryDisableDefault)
            // Save Project
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SAVE_PROJECT, TXT_SAVE_PROJECT, "", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Build Settings...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_BUILD_SETTINGS, TXT_BUILD_SETTINGS, "Ctrl+Shift+B", queryDisableDefault)
            // Build And Run
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_BUILD_AND_RUN, TXT_BUILD_AND_RUN, "Ctrl+B", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Exit
            MENU_ITEM_WITH_CB(menu, ID_MENU_ITEM_EXIT, TXT_EXIT, "", queryEnableDefault, [](ImWidget*) { static_cast<EditorApp*>(Application::getInstancePtr())->exitApp(); })
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildEditMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(ID_MENU_EDIT, STR(TXT_EDIT));
            
            // Undo
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_UNDO, TXT_UNDO, "Ctrl+Z", queryDisableDefault)
            // Redo
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_REDO, TXT_REDO, "Ctrl+Y", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Select All
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SELECT_ALL, TXT_SELECT_ALL, "Ctrl+A", queryDisableDefault)
            // Deselect All
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_DESELECT_ALL, TXT_DESELECT_ALL, "Shift+D", queryDisableDefault)
            // Select Children
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SELECT_CHILDREN, TXT_SELECT_CHILDREN, "Shift+C", queryDisableDefault)
            // Select Prefab Root
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SELECT_PREFAB_ROOT, TXT_SELECT_PREFAB_ROOT, "Ctrl+Shift+R", queryDisableDefault)
            // Insert Selection
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_INSERT_SELECTION, TXT_INSERT_SELECTION, "Ctrl+I", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Cut
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CUT, TXT_CUT, "Ctrl+X", queryDisableDefault)
            // Copy
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_COPY, TXT_COPY, "Ctrl+C", queryDisableDefault)
            // Paste
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PASTE, TXT_PASTE, "Ctrl+V", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Duplicate
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_DUPLICATE, TXT_DUPLICATE, "Ctrl+D", queryDisableDefault)
            // Rename
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_RENAME, TXT_RENAME, "", queryDisableDefault)
            // Delete
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_DELETE, TXT_DELETE, "", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Frame Selected
            MENU_ITEM_WITH_EVENT(menu, ID_MEHU_ITEM_FRAME_SELECTED, TXT_FRAME_SELECTED, "F", queryDisableDefault)
            // Lock View to Selected
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_LOCK_VIEW_TO_SELECTED, TXT_LOCK_VIEW_TO_SELECTED, "Shift+F", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Find
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_FIND, TXT_FIND, "Ctrl+F", queryDisableDefault)
            // Search All...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SEARCH_ALL, TXT_SEARCH_ALL, "", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Play
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PLAY, TXT_PLAY, "Ctrl+P", queryDisableDefault)
            // Pause
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PAUSE, TXT_PAUSE, "Ctrl+Shift+P", queryDisableDefault)
            // Stop
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_STOP, TXT_STOP, "Ctrl+Alt+P", queryDisableDefault)
            
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
                    MENU_ITEM_NAME_WITH_EVENT(popup, ID_MENU_ITEM_LOAD_SELECTION + i, ss.str(), ssKey.str(), queryDisableDefault)
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
                    MENU_ITEM_NAME_WITH_EVENT(popup, ID_MENU_ITEM_SAVE_SELECTION + i, ss.str(), ssKey.str(), queryDisableDefault)
                    ss.str("");
                    ssKey.str("");
                }

                CHECK_RESULT(ret)
            }
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Project Settings...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PROJECT_SETTINGS, TXT_PROJECT_SETTINGS, "", queryDisableDefault)
            // Preferences...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PREFERENCES, TXT_PREFERENCES, "", queryDisableDefault)
            // Shortcuts...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SHORTCUT, TXT_SHORTCUTS, "", queryDisableDefault)
            // Clear All PlayerPrefs
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CLEAR_ALL_PLAYERPREFS, TXT_CLEAR_ALL_PLAYERPREFS, "", queryDisableDefault)
            // Graphics Tier
            {
                auto popup = menu->addItem(ID_MENU_ITEM_GRAPHICS_TIER, STR(TXT_GRAPHICS_TIER));

                // Tier 1
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_GRAPHICS_TIER_1, TXT_GRAPHICS_TIER_1, "", queryDisableDefault)
                // Tier 2
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_GRAPHICS_TIER_2, TXT_GRAPHICS_TIER_2, "", queryDisableDefault)
                // Tier 3
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_GRAPHICS_TIER_3, TXT_GRAPHICS_TIER_3, "", queryDisableDefault)
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildAssetsMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(ID_MENU_ASSETS, STR(TXT_ASSETS));

            // Create
            {
                auto popup = menu->addItem(ID_MENU_ITEM_CREATE, STR(TXT_CREATE));

                // Folder
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_FOLDER, TXT_FOLDER, "", queryDisableDefault)
                
                // Separator
                MENU_SEPARATOR(popup)
                
                // Type Script
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_TYPE_SCRIPT, TXT_TYPE_SCRIPT, "", queryDisableDefault)
                // 2D
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_2D, STR(TXT_2D));

                    // Physics Material 2D
                    MENU_ITEM_WITH_EVENT(popup1, ID_MENU_ITEM_PHYSICS_MATERIAL_2D, TXT_PHYSICS_MATERIAL_2D, "", queryDisableDefault)
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
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SHADER_VARIANT_COLLECTION, TXT_SHADER_VARIANT_COLLECTION, "", queryDisableDefault)
                // Testing
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_TESTING, STR(TXT_TESTING));
                }
                // Playables
                {
                    auto popup1 = popup->addItem(ID_MENU_ITEM_PLAYABLES, STR(TXT_PLAYABLES));
                }
                // Dynamic Link Library
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_DYLIB, TXT_DYLIB, "", queryDisableDefault)
                // Dynamic Link Library Reference
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_DYLIB_REF, TXT_DYLIB_REF, "", queryDisableDefault)

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
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SCENE, TXT_SCENE, "", queryDisableDefault)
                // Scene Template
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SCENE_TEMPLATE, TXT_SCENE_TEMPLATE, "", queryDisableDefault)
                // Scene Template From Scene
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SCENE_TEMPLATE_FROM_SCENE, TXT_SCENE_TEMPLATE_FROM_SCENE, "", queryDisableDefault)
                // Scene Template Pipeline
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SCENE_TEMPLATE_PIPELINE, TXT_SCENE_TEMPLATE_PIPELINE, "", queryDisableDefault)
                // Prefab
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_PREFAB, TXT_PREFAB, "", queryDisableDefault)
                // Prefab Variant
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_PREFAB_VARIANT, TXT_PREFAB_VARIANT, "", queryDisableDefault)
                
                // Separator
                MENU_SEPARATOR(popup)
                
                // Audio Mixer
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_AUDIO_MIXER, TXT_AUDIO_MIXER, "", queryDisableDefault)
                
                // Separator
                MENU_SEPARATOR(popup)
                
                // Material
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_MATERIAL, TXT_MATERIAL, "", queryDisableDefault)
                // Render Texture
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_RENDER_TEXTURE, TXT_RENDER_TEXTURE, "", queryDisableDefault)
                // Lightmap Parameters
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_LIGHTMAP_PARAMETERS, TXT_LIGHTMAP_PARAMETERS, "", queryDisableDefault)
                // Lighting Settings
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_LIGHTING_SETTINGS, TXT_LIGHTING_SETTINGS, "", queryDisableDefault)
                // Custom Render Texture
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_CUSTOM_RENDER_TEXTURE, TXT_CUSTOM_RENDER_TEXTURE, "", queryDisableDefault)
                
                // Separator
                MENU_SEPARATOR(popup)
                
                // Animator Controller
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_ANIMATOR_CONTROLLER, TXT_ANIMATOR_CONTROLLER, "", queryDisableDefault)
                // Animation
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_ANIMATION, TXT_ANIMATION, "", queryDisableDefault)
                // Animator Override Controller
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_ANIMATOR_OVERRIDE_CONTROLLER, TXT_ANIMATOR_OVERRIDE_CONTROLLER, "", queryDisableDefault)
                // Avatar Mask
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_AVATAR_MASK, TXT_AVATAR_MASK, "", queryDisableDefault)
                
                // Separator
                MENU_SEPARATOR(popup)
                
                // Timeline
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_TIMELINE, TXT_TIMELINE, "", queryDisableDefault)
                // Signal
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_SIGNAL, TXT_SIGNAL, "", queryDisableDefault)

                // Separator
                MENU_SEPARATOR(popup)
                
                // GUI Skin
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_GUI_SKIN, TXT_GUI_SKIN, "", queryDisableDefault)
                // Custom Font
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_CUSTOM_FONT, TXT_CUSTOM_FONT, "", queryDisableDefault)
                
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
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_BRUSH, TXT_BRUSH, "", queryDisableDefault)
                // Terrain Layer
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_TERRAIN_LAYER, TXT_TERRAIN_LAYER, "", queryDisableDefault)
            }
            // Show in Explorer
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SHOW_IN_EXPLORER, TXT_SHOW_IN_EXPLORER, "", queryDisableDefault)
            // Open
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN, TXT_OPEN, "", queryDisableDefault)
            // Delete
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_DELETE_ASSET, TXT_DELETE, "", queryDisableDefault)
            // Rename
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_RENAME_ASSET, TXT_RENAME, "", queryDisableDefault)
            // Copy path
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_COPY_PATH, TXT_COPY_PATH, "Alt+Ctrl+C", queryDisableDefault)

            // Separator
            MENU_SEPARATOR(menu)

            // Open Scene Additive
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN_SCENE_ADDITIVE, TXT_OPEN_SCENE_ADDITIVE, "", queryDisableDefault)

            // Separator
            MENU_SEPARATOR(menu)

            // View in Package Manager
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_VIEW_IN_PACKAGE_MANAGER, TXT_VIEW_IN_PACKAGE_MANAGER, "", queryDisableDefault)

            // Separator
            MENU_SEPARATOR(menu)

            // Import New Asset...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_IMPORT_NEW_ASSET, TXT_IMPORT_NEW_ASSET, "", queryDisableDefault)
            // Import Package
            {
                auto popup = menu->addItem(ID_MENU_ITEM_IMPORT_PACKAGE, STR(TXT_IMPORT_PACKAGE));

                // Custom Package
                MENU_ITEM_WITH_EVENT(popup, ID_MENU_ITEM_CUSTOM_PACKAGE, TXT_CUSTOM_PACKAGE, "", queryDisableDefault)
            }
            // Export Package...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_EXPORT_PACKAGE, TXT_EXPORT_PACKAGE, "", queryDisableDefault)
            // Find References In Scene
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_FIND_REFERENCES_IN_SCENE, TXT_FIND_REFERENCES_IN_SCENE, "", queryDisableDefault)
            // Find References In Project
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_FIND_REFERENCES_IN_PROJECT, TXT_FIND_REFERENCES_IN_PROJECT, "", queryDisableDefault)
            // Select Dependencies
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SELECT_DEPENDENCIES, TXT_SELECT_DEPENDENCIES, "", queryDisableDefault)

            // Separator
            MENU_SEPARATOR(menu)

            // Refresh
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_REFRESH, TXT_REFRESH, "Ctrl+R", queryDisableDefault)
            // Reimport
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_REIMPORT, TXT_REIMPORT, "", queryDisableDefault)

            // Separator
            MENU_SEPARATOR(menu)

            // Reimport All
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_REIMPORT_ALL, TXT_REIMPORT_ALL, "", queryDisableDefault)

            // Separator
            MENU_SEPARATOR(menu)

            // Extract From Prefab
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_EXTRACT_FROM_PREFAB, TXT_EXTRACT_FROM_PREFAB, "", queryDisableDefault)

            // Separator
            MENU_SEPARATOR(menu)

            // Update UXML Schema
            // MENU_ITEM_WITH_EVENT(menu, TXT_UPDATE_UXML_SCHEMA, "", queryDisableDefault)

            // Separator
            MENU_SEPARATOR(menu)

            // Open C++ Project
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_OPEN_CPP_PROJECT, TXT_OPEN_CPP_PROJECT, "", queryDisableDefault)
            // View in Import Activity Window
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_IETM_VIEW_IN_IMPORT_ACTIVITY_WINDOW, TXT_VIEW_IN_IMPORT_ACTIVITY_WINDOW, "", queryDisableDefault)
            // Properties
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PROPERTIES, TXT_PROPERTIES, "Alt+P", queryDisableDefault)
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildGameObjectMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(ID_MENU_GAME_OBJECT, STR(TXT_GAMEOBJECT));
            
            // Create Empty
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CREATE_EMPTY, TXT_CREATE_EMPTY, "Ctrl+Shift+N", queryDisableDefault)
            // Create Empty Child
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CREATE_EMPTY_CHILD, TXT_CREATE_EMPTY_CHILD, "Alt+Shift+N", queryDisableDefault)
            // Create Empty Parent
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CREATE_EMPTY_PARENT, TXT_CREATE_EMPTY_PARENT, "Ctrl+Shift+G", queryDisableDefault)
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
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CAMERA, TXT_CAMERA, "", queryDisableDefault)
            // Visual Scripting Scene Varaibles
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_VISUAL_SCRIPTING_SCENE_VARAIBLES, TXT_VISUAL_SCRIPTING_SCENE_VARAIBLES, "", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Center On Children
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CENTER_ON_CHILDREN, TXT_CENTER_ON_CHILDREN, "", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Make Parent
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_MAKE_PARENT, TXT_MAKE_PARENT, "", queryDisableDefault)
            // Clear Parent
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_CLEAR_PARENT, TXT_CLEAR_PARENT, "", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // Set as first sibling
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SET_AS_FIRST_SIBLING, TXT_SET_AS_FIRST_SIBLING, "Ctrl+=", queryDisableDefault)
            // Set as last sibling
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_SET_AS_LAST_SIBLING, TXT_SET_AS_LAST_SIBLING, "Ctrl+-", queryDisableDefault)
            // Move To View
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_MOVE_TO_VIEW, TXT_MOVE_TO_VIEW, "Ctrl+Alt+F", queryDisableDefault)
            // Align With View
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_ALIGN_WITH_VIEW, TXT_ALIGN_WITH_VIEW, "Ctrl+Shift+F", queryDisableDefault)
            // Align View to Selected
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_ALIGN_VIEW_TO_SELECTED, TXT_ALIGN_VIEW_TO_SELECTED, "", queryDisableDefault)
            // Toggle Active State
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_TOGGLE_ACTIVE_STATE, TXT_TOGGLE_ACTIVE_STATE, "Alt+Shift+A", queryDisableDefault)
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildComponentMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(ID_MENU_COMPONENT, STR(TXT_COMPONENT));

            // Add...
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_ADD, TXT_ADD, "Ctrl+Shift+A", queryDisableDefault)
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

    TResult UIMainWindow::buildWindowMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
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
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_NEXT_WINDOW, TXT_NEXT_WINDOW, "Ctrl+Tab", queryDisableDefault)
            // Previous Window
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PREV_WINDOW, TXT_PREV_WINDOW, "Ctrl+Shift+Tab", queryDisableDefault)

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
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_PACKAGE_MANAGER, TXT_PACKAGE_MANAGER, "", queryDisableDefault)

            // Separator
            MENU_SEPARATOR(menu)

            // Asset Management
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_ASSET_MANAGEMENT, TXT_ASSET_MANAGEMENT, "", queryDisableDefault)

            // Separator
            MENU_SEPARATOR(menu)

            // Text
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_TEXT_WINDOW, TXT_TEXT, "", queryDisableDefault)

            // Separator
            MENU_SEPARATOR(menu)

            // Text Mesh Pro
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_TEXT_MESH_PRO_WINDOW, TXT_TEXT_MESH_PRO, "", queryDisableDefault)

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

    TResult UIMainWindow::buildHelpMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        TResult ret = T3D_OK;

        do
        {
            auto menu = mMenuBar->addMenu(ID_MENU_HELP, STR(TXT_HELP));

            // About
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_ABOUT, TXT_ABOUT, "", queryDisableDefault)
            
            // Separator
            MENU_SEPARATOR(menu)
            
            // User Manual
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_USER_MANUAL, TXT_USER_MANUAL, "", queryDisableDefault)
            // API Document
            MENU_ITEM_WITH_EVENT(menu, ID_MENU_ITEM_API_DOCUMENT, TXT_API_DOCUMENT, "", queryDisableDefault)
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildMenu()
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
#else
    
    //--------------------------------------------------------------------------

    TResult UIMainWindow::createMenuItemData()
    {
        TResult ret = T3D_OK;

        do
        {
            auto queryDisableDefault = [](ImWidget *widget) { return false; };
            auto queryEnableDefault = [](ImWidget *widget) { return true; };

            // Separator
            IM_MENU_ITEM_DATA(ImMenuItemType::kSeparator, ID_MENU_SEPARATOR, "", "", "", nullptr, nullptr, nullptr)

            ret = createFileMenuItemData(queryEnableDefault, queryDisableDefault);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create file menu item data ! ERROR [%d]", ret);
                break;
            }

            ret = createEditMenuItemData(queryEnableDefault, queryDisableDefault);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create edit menu item data ! ERROR [%d]", ret);
                break;
            }

            ret = createAssetMenuItemData(queryEnableDefault, queryDisableDefault);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create assets menu item data ! ERROR [%d]", ret);
                break;
            }

            ret = createGameObjectMenuItemData(queryEnableDefault, queryDisableDefault);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create gameobject menu item data ! ERROR [%d]", ret);
                break;
            }

            ret = createComponentMenuItemData(queryEnableDefault, queryDisableDefault);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create component menu item data ! ERROR [%d]", ret);
                break;
            }

            ret = createWindowMenuItemData(queryEnableDefault, queryDisableDefault);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create window menu item data ! ERROR [%d]", ret);
                break;
            }

            ret = createHelpMenuItemData(queryEnableDefault, queryDisableDefault);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create help menu item data ! ERROR [%d]", ret);
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::createFileMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        // File
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_FILE, STR(TXT_FILE), "", "", nullptr, nullptr, nullptr)

        // New Scene
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_NEW_SCENE, STR(TXT_NEW_SCENE), "Ctrl+N", "", queryDisableDefault, nullptr, nullptr)
        // Open Scene
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_OPEN_SCENE, STR(TXT_OPEN_SCENE), "Ctrl+O", "", queryDisableDefault, nullptr, nullptr)
        // Open Recent Scene
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_OPEN_RECENT_SCENE, STR(TXT_OPEN_RECENT_SCENE), "", "", queryDisableDefault, nullptr, nullptr)

        // Save
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SAVE, STR(TXT_SAVE), "Ctrl+S", "", queryDisableDefault, nullptr, nullptr)
        // Save As
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SAVE_AS, STR(TXT_SAVE_AS), "Ctrl+Shift+S", "", queryDisableDefault, nullptr, nullptr)
        // Save As Scene Template
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SAVE_AS_SCENE_TEMPLATE, STR(TXT_SAVE_AS_SCENE_TEMPLATE), "", "", queryDisableDefault, nullptr, nullptr)

        // New Project
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_NEW_PROJECT, STR(TXT_NEW_PROJECT), "", "", queryDisableDefault, nullptr, nullptr)
        // Open Project
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_OPEN_PROJECT, STR(TXT_OPEN_PROJECT), "", "", queryDisableDefault, nullptr, nullptr)
        // Save Project
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SAVE_PROJECT, STR(TXT_SAVE_PROJECT), "", "", queryDisableDefault, nullptr, nullptr)

        // Build Settings...
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_BUILD_SETTINGS,  STR(TXT_BUILD_SETTINGS), "Ctrl+Shift+B", "", queryDisableDefault, nullptr, nullptr)
        // Build And Run
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_BUILD_AND_RUN,  STR(TXT_BUILD_AND_RUN), "Ctrl+B", "", queryDisableDefault, nullptr, nullptr)
        
        // Exit
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_EXIT, STR(TXT_EXIT), "", "", queryEnableDefault, nullptr, [](ImWidget*) { static_cast<EditorApp*>(Application::getInstancePtr())->exitApp(); })
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::createEditMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        // Edit
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_EDIT, STR(TXT_EDIT), "", "", nullptr, nullptr, nullptr)

        // Undo
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_UNDO, STR(TXT_UNDO), "Ctrl+Z", "", queryDisableDefault, nullptr, nullptr)
        // Redo
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_REDO, STR(TXT_REDO), "Ctrl+Y", "", queryDisableDefault, nullptr, nullptr)

        // Select All
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SELECT_ALL, STR(TXT_SELECT_ALL), "Ctrl+A", "", queryDisableDefault, nullptr, nullptr)
        // Deselect All
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_DESELECT_ALL, STR(TXT_DESELECT_ALL), "Shift+D", "", queryDisableDefault, nullptr, nullptr)
        // Select Children
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SELECT_CHILDREN, STR(TXT_SELECT_CHILDREN), "Shift+C", "", queryDisableDefault, nullptr, nullptr)
        // Select Prefab Root
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SELECT_PREFAB_ROOT, STR(TXT_SELECT_PREFAB_ROOT), "Ctrl+Shift+R", "", queryDisableDefault, nullptr, nullptr)
        // Insert Selection
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_INSERT_SELECTION, STR(TXT_INSERT_SELECTION), "Ctrl+I", "", queryDisableDefault, nullptr, nullptr)

        // Cut
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CUT, STR(TXT_CUT), "Ctrl+X", "", queryDisableDefault, nullptr, nullptr)
        // Copy
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_COPY, STR(TXT_COPY), "Ctrl+C", "", queryDisableDefault, nullptr, nullptr)
        // Paste
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_PASTE, STR(TXT_PASTE), "Ctrl+V", "", queryDisableDefault, nullptr, nullptr)

        // Duplicate
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_DUPLICATE, STR(TXT_DUPLICATE), "Ctrl+D", "", queryDisableDefault, nullptr, nullptr)
        // Rename
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_RENAME, STR(TXT_RENAME), "", "", queryDisableDefault, nullptr, nullptr)
        // Delete
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_DELETE, STR(TXT_DELETE), "", "", queryDisableDefault, nullptr, nullptr)

        // Frame Selected
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MEHU_ITEM_FRAME_SELECTED, STR(TXT_FRAME_SELECTED), "F", "", queryDisableDefault, nullptr, nullptr)
        // Lock View to Selected
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_LOCK_VIEW_TO_SELECTED, STR(TXT_LOCK_VIEW_TO_SELECTED), "Shift+F", "", queryDisableDefault, nullptr, nullptr)

        // Find
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_FIND, STR(TXT_FIND), "Ctrl+F", "", queryDisableDefault, nullptr, nullptr)
        // Search All...
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SEARCH_ALL, STR(TXT_SEARCH_ALL), "", "", queryDisableDefault, nullptr, nullptr)

        // Play
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_PLAY, STR(TXT_PLAY), "Ctrl+P", "", queryDisableDefault, nullptr, nullptr)
        // Pause
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_PAUSE, STR(TXT_PAUSE), "Ctrl+Shift+P", "", queryDisableDefault, nullptr, nullptr)
        // Stop
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_STOP, STR(TXT_STOP), "Ctrl+Alt+P", "", queryDisableDefault, nullptr, nullptr)

        // Selection
        {
            // auto popup = menu->addItem(ID_MENU_ITEM_SELECTION, STR(TXT_SELECTION));
            IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_SELECTION, STR(TXT_SELECTION), "", "", nullptr, nullptr, nullptr)

            // Load Selection
            const int32_t kMaxSelections = 10;
            for (int32_t i = 0; i < kMaxSelections; ++i)
            {
                std::stringstream ss;
                ss << STR(TXT_LOAD_SELECTION) << " " << i;
                std::stringstream ssKey;
                ssKey << "Ctrl+Shift+" << i;
                IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_LOAD_SELECTION + i, ss.str(), ssKey.str(), "", queryDisableDefault, nullptr, nullptr)
                ss.str("");
                ssKey.str("");
            }

            // Save Selection
            for (int32_t i = 0; i < kMaxSelections; ++i)
            {
                std::stringstream ss;
                ss << STR(TXT_SAVE_SELECTION) << " " << i;
                std::stringstream ssKey;
                ssKey << "Ctrl+Alt+" << i;
                IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SAVE_SELECTION + i, ss.str(), ssKey.str(), "", queryDisableDefault, nullptr, nullptr)
                ss.str("");
                ssKey.str("");
            }
        }

        // Project Settings...
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_PROJECT_SETTINGS, STR(TXT_PROJECT_SETTINGS), "", "", queryDisableDefault, nullptr, nullptr)
        // Preferences...
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_PREFERENCES, STR(TXT_PREFERENCES), "", "", queryDisableDefault, nullptr, nullptr)
        // Shortcuts...
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SHORTCUT, STR(TXT_SHORTCUTS), "", "", queryDisableDefault, nullptr, nullptr)
        // Clear All PlayerPrefs
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CLEAR_ALL_PLAYERPREFS, STR(TXT_CLEAR_ALL_PLAYERPREFS), "", "", queryDisableDefault, nullptr, nullptr)
        
        // Graphics Tier
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_GRAPHICS_TIER, STR(TXT_GRAPHICS_TIER), "", "", nullptr, nullptr, nullptr)
        // Tier 1
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_GRAPHICS_TIER_1, STR(TXT_GRAPHICS_TIER_1), "", "", queryDisableDefault, nullptr, nullptr)
        // Tier 2
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_GRAPHICS_TIER_2, STR(TXT_GRAPHICS_TIER_2), "", "", queryDisableDefault, nullptr, nullptr)
        // Tier 3
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_GRAPHICS_TIER_3, STR(TXT_GRAPHICS_TIER_3), "", "", queryDisableDefault, nullptr, nullptr)
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::createAssetMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        // Asssets
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ASSETS, STR(TXT_ASSETS), "", "", nullptr, nullptr, nullptr)

        // Create
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_CREATE, STR(TXT_CREATE), "", "", nullptr, nullptr, nullptr)

        // Folder
        IM_MENU_ITEM_DATA_DEFAULT(ImMenuItemType::kNormal, ID_MENU_ITEM_FOLDER, STR(TXT_FOLDER), "", "")

        // Type Script
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_TYPE_SCRIPT, STR(TXT_TYPE_SCRIPT), "", "", queryDisableDefault, nullptr, nullptr)
        // 2D
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_2D, STR(TXT_2D), "", "", nullptr, nullptr, nullptr)
        // Physics Material 2D
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_PHYSICS_MATERIAL_2D, STR(TXT_PHYSICS_MATERIAL_2D), "", "", queryDisableDefault, nullptr, nullptr)
        // Visual Scripting
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_VISUAL_SCRIPTING, STR(TXT_VISUAL_SCRIPTING), "", "", nullptr, nullptr, nullptr)
        // Shader
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_SHADER, STR(TXT_SHADER), "", "", nullptr, nullptr, nullptr)
        // Shader Variant Collection
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SHADER_VARIANT_COLLECTION, STR(TXT_SHADER_VARIANT_COLLECTION), "", "", queryDisableDefault, nullptr, nullptr)
        // Testing
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_TESTING, STR(TXT_TESTING), "", "", nullptr, nullptr, nullptr)
        // Playables
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_PLAYABLES, STR(TXT_PLAYABLES), "", "", nullptr, nullptr, nullptr)
        // Dynamic Link Library
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_DYLIB, STR(TXT_DYLIB), "", "", queryDisableDefault, nullptr, nullptr)
        // Dynamic Link Library Reference
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_DYLIB_REF, STR(TXT_DYLIB_REF), "", "", queryDisableDefault, nullptr, nullptr)
        // Text
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_TEXT, STR(TXT_TEXT), "", "", nullptr, nullptr, nullptr)
        // Test Mesh Pro
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_TEXT_MESH_PRO, STR(TXT_TEXT_MESH_PRO), "", "", nullptr, nullptr, nullptr)

        // Scene
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SCENE, STR(TXT_SCENE), "", "", queryDisableDefault, nullptr, nullptr)
        // Scene Template
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SCENE_TEMPLATE, STR(TXT_SCENE_TEMPLATE), "", "", queryDisableDefault, nullptr, nullptr)
        // Scene Template From Scene
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SCENE_TEMPLATE_FROM_SCENE, STR(TXT_SCENE_TEMPLATE_FROM_SCENE), "", "", queryDisableDefault, nullptr, nullptr)
        // Scene Template Pipeline
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SCENE_TEMPLATE_PIPELINE, STR(TXT_SCENE_TEMPLATE_PIPELINE), "", "", queryDisableDefault, nullptr, nullptr)
        // Prefab
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_PREFAB, STR(TXT_PREFAB), "", "", queryDisableDefault, nullptr, nullptr)
        // Prefab Variant
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_PREFAB_VARIANT, STR(TXT_PREFAB_VARIANT), "", "", queryDisableDefault, nullptr, nullptr)
                
        // Audio Mixer
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_AUDIO_MIXER, STR(TXT_AUDIO_MIXER), "", "", queryDisableDefault, nullptr, nullptr)

        // Material
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_MATERIAL, STR(TXT_MATERIAL), "", "", queryDisableDefault, nullptr, nullptr)
        // Render Texture
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_RENDER_TEXTURE, STR(TXT_RENDER_TEXTURE), "", "", queryDisableDefault, nullptr, nullptr)
        // Lightmap Parameters
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_LIGHTMAP_PARAMETERS, STR(TXT_LIGHTMAP_PARAMETERS), "", "", queryDisableDefault, nullptr, nullptr)
        // Lighting Settings
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_LIGHTING_SETTINGS, STR(TXT_LIGHTING_SETTINGS), "", "", queryDisableDefault, nullptr, nullptr)
        // Custom Render Texture
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CUSTOM_RENDER_TEXTURE, STR(TXT_CUSTOM_RENDER_TEXTURE), "", "", queryDisableDefault, nullptr, nullptr)

        // Animator Controller
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_ANIMATOR_CONTROLLER, STR(TXT_ANIMATOR_CONTROLLER), "", "", queryDisableDefault, nullptr, nullptr)
        // Animation
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_ANIMATION, STR(TXT_ANIMATION), "", "", queryDisableDefault, nullptr, nullptr)
        // Animator Override Controller
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_ANIMATOR_OVERRIDE_CONTROLLER, STR(TXT_ANIMATOR_OVERRIDE_CONTROLLER), "", "", queryDisableDefault, nullptr, nullptr)
        // Avatar Mask
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_AVATAR_MASK, STR(TXT_AVATAR_MASK), "", "", queryDisableDefault, nullptr, nullptr)

        // Timeline
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_TIMELINE, STR(TXT_TIMELINE), "", "", queryDisableDefault, nullptr, nullptr)
        // Signal
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SIGNAL, STR(TXT_SIGNAL), "", "", queryDisableDefault, nullptr, nullptr)

        // GUI Skin
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_GUI_SKIN, STR(TXT_GUI_SKIN), "", "", queryDisableDefault, nullptr, nullptr)
        // Custom Font
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CUSTOM_FONT, STR(TXT_CUSTOM_FONT), "", "", queryDisableDefault, nullptr, nullptr)

        // Legacy
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_LEGACY, STR(TXT_LEGACY), "", "", nullptr, nullptr, nullptr);
        // UI Toolkit
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_UI_TOOLKIT, STR(TXT_UI_TOOLKIT), "", "", nullptr, nullptr, nullptr);

        // Search
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_SEARCH, STR(TXT_SEARCH), "", "", nullptr, nullptr, nullptr);
        
        // Brush
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_BRUSH, STR(TXT_BRUSH), "", "", queryDisableDefault, nullptr, nullptr)
        // Terrain Layer
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_TERRAIN_LAYER, STR(TXT_TERRAIN_LAYER), "", "", queryDisableDefault, nullptr, nullptr)
        
        // Show in Explorer
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SHOW_IN_EXPLORER, STR(TXT_SHOW_IN_EXPLORER), "", "", queryDisableDefault, nullptr, nullptr)
        // Open
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_OPEN, STR(TXT_OPEN), "", "", queryDisableDefault, nullptr, nullptr)
        // Delete
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_DELETE_ASSET, STR(TXT_DELETE), "", "", queryDisableDefault, nullptr, nullptr)
        // Rename
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_RENAME_ASSET, STR(TXT_RENAME), "", "", queryDisableDefault, nullptr, nullptr)
        // Copy path
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_COPY_PATH, STR(TXT_COPY_PATH), "Alt+Ctrl+C", "", queryDisableDefault, nullptr, nullptr)

        // Open Scene Additive
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_OPEN_SCENE_ADDITIVE, STR(TXT_OPEN_SCENE_ADDITIVE), "", "", queryDisableDefault, nullptr, nullptr)

        // View in Package Manager
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_VIEW_IN_PACKAGE_MANAGER, STR(TXT_VIEW_IN_PACKAGE_MANAGER), "", "", queryDisableDefault, nullptr, nullptr)

        // Import New Asset...
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_IMPORT_NEW_ASSET, STR(TXT_IMPORT_NEW_ASSET), "", "", queryDisableDefault, nullptr, nullptr)
        // Import Package
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_IMPORT_PACKAGE, STR(TXT_IMPORT_PACKAGE), "", "", nullptr, nullptr, nullptr);

        // Custom Package
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CUSTOM_PACKAGE, STR(TXT_CUSTOM_PACKAGE), "", "", queryDisableDefault, nullptr, nullptr)
        
        // Export Package...
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_EXPORT_PACKAGE, STR(TXT_EXPORT_PACKAGE), "", "", queryDisableDefault, nullptr, nullptr)
        // Find References In Scene
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_FIND_REFERENCES_IN_SCENE, STR(TXT_FIND_REFERENCES_IN_SCENE), "", "", queryDisableDefault, nullptr, nullptr)
        // Find References In Project
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_FIND_REFERENCES_IN_PROJECT, STR(TXT_FIND_REFERENCES_IN_PROJECT), "", "", queryDisableDefault, nullptr, nullptr)
        // Select Dependencies
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SELECT_DEPENDENCIES, STR(TXT_SELECT_DEPENDENCIES), "", "", queryDisableDefault, nullptr, nullptr)

        // Refresh
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_REFRESH, STR(TXT_REFRESH), "Ctrl+R", "", queryDisableDefault, nullptr, nullptr)
        // Reimport
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_REIMPORT, STR(TXT_REIMPORT), "", "", queryDisableDefault, nullptr, nullptr)

        // Reimport All
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_REIMPORT_ALL, STR(TXT_REIMPORT_ALL), "", "", queryDisableDefault, nullptr, nullptr)

        // Extract From Prefab
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_EXTRACT_FROM_PREFAB, STR(TXT_EXTRACT_FROM_PREFAB), "", "", queryDisableDefault, nullptr, nullptr)

        // Open C++ Project
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_OPEN_CPP_PROJECT, STR(TXT_OPEN_CPP_PROJECT), "", "", queryDisableDefault, nullptr, nullptr)
        // View in Import Activity Window
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_IETM_VIEW_IN_IMPORT_ACTIVITY_WINDOW, STR(TXT_VIEW_IN_IMPORT_ACTIVITY_WINDOW), "", "", queryDisableDefault, nullptr, nullptr)
        // Properties
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_PROPERTIES, STR(TXT_PROPERTIES), "Alt+P", "", queryDisableDefault, nullptr, nullptr)
    
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::createGameObjectMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        // GameObject
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_GAME_OBJECT, STR(TXT_GAMEOBJECT), "", "", nullptr, nullptr, nullptr)
            
        // Create Empty
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CREATE_EMPTY, STR(TXT_CREATE_EMPTY), "Ctrl+Shift+N", "", queryDisableDefault, nullptr, nullptr)
        // Create Empty Child
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CREATE_EMPTY_CHILD, STR(TXT_CREATE_EMPTY_CHILD), "Alt+Shift+N", "", queryDisableDefault, nullptr, nullptr)
        // Create Empty Parent
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CREATE_EMPTY_PARENT, STR(TXT_CREATE_EMPTY_PARENT), "Ctrl+Shift+G", "", queryDisableDefault, nullptr, nullptr)
        // 3D Object
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_3D_OBJECT, STR(TXT_3D_OBJECT), "", "", queryDisableDefault, nullptr, nullptr)
        // Effects
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_EFFECTS, STR(TXT_EFFECTS), "", "", queryDisableDefault, nullptr, nullptr)
        // Light
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_LIGHT, STR(TXT_LIGHT), "", "", queryDisableDefault, nullptr, nullptr)
        // Audio
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_AUDIO, STR(TXT_AUDIO), "", "", queryDisableDefault, nullptr, nullptr)
        // Video
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_VIDEO, STR(TXT_VIDEO), "", "", queryDisableDefault, nullptr, nullptr)
        // UI
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_UI, STR(TXT_UI), "", "", queryDisableDefault, nullptr, nullptr)
        // UI Toolkit
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_GAMEOJECT_UI_TOOLKIT, STR(TXT_UI_TOOLKIT), "", "", queryDisableDefault, nullptr, nullptr)
        // Camera
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CAMERA, STR(TXT_CAMERA), "", "", queryDisableDefault, nullptr, nullptr)
        // Visual Scripting Scene Varaibles
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_VISUAL_SCRIPTING_SCENE_VARAIBLES, STR(TXT_VISUAL_SCRIPTING_SCENE_VARAIBLES), "", "", queryDisableDefault, nullptr, nullptr)

        // Center On Children
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CENTER_ON_CHILDREN, STR(TXT_CENTER_ON_CHILDREN), "", "", queryDisableDefault, nullptr, nullptr)

        // Make Parent
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_MAKE_PARENT, STR(TXT_MAKE_PARENT), "", "", queryDisableDefault, nullptr, nullptr)
        // Clear Parent
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CLEAR_PARENT, STR(TXT_CLEAR_PARENT), "", "", queryDisableDefault, nullptr, nullptr)

        // Set as first sibling
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SET_AS_FIRST_SIBLING, STR(TXT_SET_AS_FIRST_SIBLING), "Ctrl+=", "", queryDisableDefault, nullptr, nullptr)
        // Set as last sibling
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_SET_AS_LAST_SIBLING, STR(TXT_SET_AS_LAST_SIBLING), "Ctrl+-", "", queryDisableDefault, nullptr, nullptr)
        // Move To View
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_MOVE_TO_VIEW, STR(TXT_MOVE_TO_VIEW), "Ctrl+Alt+F", "", queryDisableDefault, nullptr, nullptr)
        // Align With View
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_ALIGN_WITH_VIEW, STR(TXT_ALIGN_WITH_VIEW), "Ctrl+Shift+F", "", queryDisableDefault, nullptr, nullptr)
        // Align View to Selected
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_ALIGN_VIEW_TO_SELECTED, STR(TXT_ALIGN_VIEW_TO_SELECTED), "", "", queryDisableDefault, nullptr, nullptr)
        // Toggle Active State
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_TOGGLE_ACTIVE_STATE, STR(TXT_TOGGLE_ACTIVE_STATE), "Alt+Shift+A", "", queryDisableDefault, nullptr, nullptr)

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::createComponentMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        // Component
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_COMPONENT, STR(TXT_COMPONENT), "", "", nullptr, nullptr, nullptr);

        // Add...
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_ADD, STR(TXT_ADD), "Ctrl+Shift+A", "", queryDisableDefault, nullptr, nullptr)
        // Mesh
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_MESH, STR(TXT_MESH), "", "", nullptr, nullptr, nullptr)
        // Effects
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_EFFECTS_COMPONENT, STR(TXT_EFFECTS), "", "", nullptr, nullptr, nullptr)
        // Physics
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_PHYSICS, STR(TXT_PHYSICS), "", "", nullptr, nullptr, nullptr)
        // Physics 2D
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_PHYSICS_2D, STR(TXT_PHYSICS_2D), "", "", nullptr, nullptr, nullptr)
        // Navigation
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_NAVIGATION, STR(TXT_NAVIGATION), "", "", nullptr, nullptr, nullptr)
        // Audio
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_AUDIO_COMPONENT, STR(TXT_AUDIO), "", "", nullptr, nullptr, nullptr)
        // Video
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_VIDEO_COMPONENT, STR(TXT_VIDEO), "", "", nullptr, nullptr, nullptr)
        // Rendering
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_RENDERING, STR(TXT_RENDERING), "", "", nullptr, nullptr, nullptr)
        // Tilemap
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_TILEMAP, STR(TXT_TILEMAP), "", "", nullptr, nullptr, nullptr)
        // Layout
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_LAYOUT, STR(TXT_LAYOUT), "", "", nullptr, nullptr, nullptr)
        // Playables
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_PLAYABLES_COMPONENT, STR(TXT_PLAYABLES), "", "", nullptr, nullptr, nullptr)
        // Miscellaneous
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_MISCELLANEOUS, STR(TXT_MISCELLANEOUS), "", "", nullptr, nullptr, nullptr)
        // Scripts
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_SCRIPTS, STR(TXT_SCRIPTS), "", "", nullptr, nullptr, nullptr)
        // UI
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_UI_COMPONENT, STR(TXT_UI), "", "", nullptr, nullptr, nullptr)
        // Visual Scripting
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_VISUAL_SCRIPTING_COMPONENT, STR(TXT_VISUAL_SCRIPTING), "", "", nullptr, nullptr, nullptr)
        // Event
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_EVENT, STR(TXT_EVENT), "", "", nullptr, nullptr, nullptr)
        // UI Toolkit
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_UI_TOOLKIT_COMPONENT, STR(TXT_UI_TOOLKIT), "", "", nullptr, nullptr, nullptr)

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::createWindowMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
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
        
        // Window
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_WINDOW, STR(TXT_WINDOW), "", "", nullptr, nullptr, nullptr)
            
        // Panels
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_PANELS, STR(TXT_PANELS), "", "", nullptr, nullptr, nullptr)
        // Console
        IM_MENU_ITEM_DATA(ImMenuItemType::kCheck, ID_MENU_ITEM_CONSOLE_WINDOW, STR(TXT_CONSOLE), "", "", queryEnableDefault, QUERY_CHECK(mConsoleWnd), TOGGLE_WINDOW_VISIBLE(mConsoleWnd))
        // Game
        IM_MENU_ITEM_DATA(ImMenuItemType::kCheck, ID_MENU_ITEM_GAME_WIDNOW, STR(TXT_GAME), "", "", queryEnableDefault, QUERY_CHECK(mGameWnd), TOGGLE_WINDOW_VISIBLE(mGameWnd))
        // Hierarchy
        IM_MENU_ITEM_DATA(ImMenuItemType::kCheck, ID_MENU_ITEM_HIERARCHY_WINDOW, STR(TXT_HIERARCHY), "", "", queryEnableDefault, QUERY_CHECK(mHierarchyWnd), TOGGLE_WINDOW_VISIBLE(mHierarchyWnd))
        // Inspector
        IM_MENU_ITEM_DATA(ImMenuItemType::kCheck, ID_MENU_ITEM_INSPECTOR_WINDOW, STR(TXT_INSPECTOR), "", "", queryEnableDefault,QUERY_CHECK(mInspectorWnd), TOGGLE_WINDOW_VISIBLE(mInspectorWnd))
        // Project
        IM_MENU_ITEM_DATA(ImMenuItemType::kCheck, ID_MENU_ITEM_PROJECT_WINDOW, STR(TXT_PROJECT), "", "", queryEnableDefault,QUERY_CHECK(mProjectWnd), TOGGLE_WINDOW_VISIBLE(mProjectWnd))
        // Scene
        IM_MENU_ITEM_DATA(ImMenuItemType::kCheck, ID_MENU_ITEM_SCENE_WINDOW, STR(TXT_SCENE), "", "", queryEnableDefault,QUERY_CHECK(mSceneWnd), TOGGLE_WINDOW_VISIBLE(mSceneWnd))

        // Next Window
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_NEXT_WINDOW, STR(TXT_NEXT_WINDOW), "Ctrl+Tab", "", queryDisableDefault, nullptr, nullptr)
        // Previous Window
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_PREV_WINDOW, STR(TXT_PREV_WINDOW), "Ctrl+Shift+Tab", "", queryDisableDefault, nullptr, nullptr)

        // Layouts
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_LAYOUTS, STR(TXT_LAYOUTS), "", "", queryEnableDefault, nullptr, nullptr)
        // Reset All Layouts
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_RESET_ALL_LAYOUTS, STR(TXT_RESET_ALL_LAYOUTS), "", "", queryDisableDefault, nullptr, nullptr)
        // Search
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_SEARCH_WINDOW, STR(TXT_SEARCH), "", "", queryEnableDefault, nullptr, nullptr)

        // Package Manager
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_PACKAGE_MANAGER, STR(TXT_PACKAGE_MANAGER), "", "", queryDisableDefault, nullptr, nullptr)

        // Asset Management
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_ASSET_MANAGEMENT, STR(TXT_ASSET_MANAGEMENT), "", "", queryDisableDefault, nullptr, nullptr)

        // Text
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_TEXT_WINDOW, STR(TXT_TEXT), "", "", queryDisableDefault, nullptr, nullptr)

        // Text Mesh Pro
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_TEXT_MESH_PRO_WINDOW, STR(TXT_TEXT_MESH_PRO), "", "", queryDisableDefault, nullptr, nullptr)

        // General
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_GENERAL, STR(TXT_GENERAL), "", "", queryDisableDefault, nullptr, nullptr);
        // Rendering
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_RENDERING_WINDOW, STR(TXT_RENDERING), "", "", queryDisableDefault, nullptr, nullptr);
        // Animation
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_ANIMATION_WINDOW, STR(TXT_ANIMATION), "", "", queryDisableDefault, nullptr, nullptr);
        // Audio
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_AUDIO_WINDOW, STR(TXT_AUDIO), "", "", queryDisableDefault, nullptr, nullptr);
        // Sequencing
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_SEQUENCING, STR(TXT_SEQUENCING), "", "", queryDisableDefault, nullptr, nullptr);
        // Analysis
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_ANALYSIS, STR(TXT_ANALYSIS), "", "", queryDisableDefault, nullptr, nullptr);
        // AI
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_AI, STR(TXT_AI), "", "", queryDisableDefault, nullptr, nullptr);
        // UI Toolkit
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_UI_TOOLKIT_WINDOW, STR(TXT_UI_TOOLKIT), "", "", queryDisableDefault, nullptr, nullptr);
        // Visual Scripting
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_VISUAL_SCRIPTING_WINDOW, STR(TXT_VISUAL_SCRIPTING), "", "", queryDisableDefault, nullptr, nullptr);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::createHelpMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault)
    {
        // Help
        IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_HELP, STR(TXT_HELP), "", "", queryEnableDefault, nullptr, nullptr)

        // About
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_ABOUT, STR(TXT_ABOUT), "", "", queryDisableDefault, nullptr, nullptr)

        // User Manual
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_USER_MANUAL, STR(TXT_USER_MANUAL), "", "", queryDisableDefault, nullptr, nullptr)
        // API Document
        IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_API_DOCUMENT, STR(TXT_API_DOCUMENT), "", "", queryDisableDefault, nullptr, nullptr)
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildMainMenu()
    {
        TResult ret = T3D_OK;

        do
        {
            mMenuBar = new ImMenuBar();
            ret = mMenuBar->create(ID_MAIN_MENU, "Main Menu", this);
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to create main menu bar ! ERROR [%d]", ret);
                break;
            }
            
            ret = buildFileMenu();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to build file menu ! ERROR [%d]", ret);
                break;
            }

            ret = buildEditMenu();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to build edit menu ! ERROR [%d]", ret);
                break;
            }

            ret = buildAssetMenu();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to build asset menu ! ERROR [%d]", ret);
                break;
            }

            ret = buildGameObjectMenu();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to build game object menu ! ERROR [%d]", ret);
                break;
            }

            ret = buildComponentMenu();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to build component menu ! ERROR [%d]", ret);
                break;
            }

            ret = buildWindowMenu();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to build window menu ! ERROR [%d]", ret);
                break;
            }

            ret = buildHelpMenu();
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to build help menu ! ERROR [%d]", ret);
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildFileMenu()
    {
        TResult ret = T3D_OK;

        do
        {
            IM_MENU_BEGIN(mMenuBar)
                IM_MENU_POPUP_BEGIN(ID_MENU_FILE)
                    IM_MENU_ITEM(ID_MENU_ITEM_NEW_SCENE)    // New Scene
                    IM_MENU_ITEM(ID_MENU_ITEM_OPEN_SCENE)   // Open Scene
                    IM_MENU_ITEM(ID_MENU_ITEM_OPEN_RECENT_SCENE)    // Open Recent Scene

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_ITEM(ID_MENU_ITEM_SAVE) // Save
                    IM_MENU_ITEM(ID_MENU_ITEM_SAVE_AS)  // Save As
                    IM_MENU_ITEM(ID_MENU_ITEM_SAVE_AS_SCENE_TEMPLATE)   // Save As Scene Template

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_ITEM(ID_MENU_ITEM_NEW_PROJECT)  // New Project
                    IM_MENU_ITEM(ID_MENU_ITEM_OPEN_PROJECT) // Open Project
                    IM_MENU_ITEM(ID_MENU_ITEM_SAVE_PROJECT) // Save Project

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_ITEM(ID_MENU_ITEM_BUILD_SETTINGS)   // Build Settings...
                    IM_MENU_ITEM(ID_MENU_ITEM_BUILD_AND_RUN)    // Build And Run

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_ITEM(ID_MENU_ITEM_EXIT) // Exit
                IM_MENU_POPUP_END()
            IM_MENU_END()
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildEditMenu()
    {
        IM_MENU_BEGIN(mMenuBar)
            IM_MENU_POPUP_BEGIN(ID_MENU_EDIT)
                IM_MENU_ITEM(ID_MENU_ITEM_UNDO) // Undo
                IM_MENU_ITEM(ID_MENU_ITEM_REDO) // Redo

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_SELECT_ALL) // Select All
                IM_MENU_ITEM(ID_MENU_ITEM_DESELECT_ALL) // Deselect All
                IM_MENU_ITEM(ID_MENU_ITEM_SELECT_CHILDREN) // Select Children
                IM_MENU_ITEM(ID_MENU_ITEM_SELECT_PREFAB_ROOT) // Select Prefab Root
                IM_MENU_ITEM(ID_MENU_ITEM_INSERT_SELECTION) // Insert Selection

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_CUT) // Cut
                IM_MENU_ITEM(ID_MENU_ITEM_COPY) // Copy
                IM_MENU_ITEM(ID_MENU_ITEM_PASTE) // Paste

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_DUPLICATE) // Duplicate
                IM_MENU_ITEM(ID_MENU_ITEM_RENAME) // Rename
                IM_MENU_ITEM(ID_MENU_ITEM_DELETE) // Delete

                IM_MENU_SEPARATOR() // Separator
            
                IM_MENU_ITEM(ID_MEHU_ITEM_FRAME_SELECTED) // Frame Selected
                IM_MENU_ITEM(ID_MENU_ITEM_LOCK_VIEW_TO_SELECTED) // Lock View to Selected

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_FIND) // Find
                IM_MENU_ITEM(ID_MENU_ITEM_SEARCH_ALL) // Search All...

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_PLAY) // Play
                IM_MENU_ITEM(ID_MENU_ITEM_PAUSE) // Pause
                IM_MENU_ITEM(ID_MENU_ITEM_STOP) // Stop
            
                IM_MENU_SEPARATOR() // Separator

                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_SELECTION) // Selection

                    // Load Selection
                    const int32_t kMaxSelections = 10;
                    for (int32_t i = 0; i < kMaxSelections; ++i)
                    {
                        uint32_t menuID = ID_MENU_ITEM_LOAD_SELECTION + i;
                        IM_MENU_ITEM(menuID)
                    }

                    // Save Selection
                    for (int32_t i = 0; i < kMaxSelections; ++i)
                    {
                        uint32_t menuID = ID_MENU_ITEM_SAVE_SELECTION + i;
                        IM_MENU_ITEM(menuID)
                    }

                IM_MENU_POPUP_END()

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_PROJECT_SETTINGS) // Project Settings...
                IM_MENU_ITEM(ID_MENU_ITEM_PREFERENCES) // Preferences...
                IM_MENU_ITEM(ID_MENU_ITEM_SHORTCUT) // Shortcuts...
                IM_MENU_ITEM(ID_MENU_ITEM_CLEAR_ALL_PLAYERPREFS) // Clear All PlayerPrefs

                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_GRAPHICS_TIER)// Graphics Tier
                    IM_MENU_ITEM(ID_MENU_ITEM_GRAPHICS_TIER_1) // Tier 1
                    IM_MENU_ITEM(ID_MENU_ITEM_GRAPHICS_TIER_2) // Tier 2
                    IM_MENU_ITEM(ID_MENU_ITEM_GRAPHICS_TIER_3) // Tier 3
                IM_MENU_POPUP_END()
            IM_MENU_POPUP_END()
        IM_MENU_END()
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildAssetMenu()
    {
        IM_MENU_BEGIN(mMenuBar)
            IM_MENU_POPUP_BEGIN(ID_MENU_ASSETS)
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_CREATE)
                    IM_MENU_ITEM(ID_MENU_ITEM_FOLDER)   // Folder

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_ITEM(ID_MENU_ITEM_TYPE_SCRIPT)  // Type Script
                    IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_2D)    // 2D
                        IM_MENU_ITEM( ID_MENU_ITEM_PHYSICS_MATERIAL_2D)    // Physics Material 2D
                    IM_MENU_POPUP_END()
                    IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_VISUAL_SCRIPTING)  // Visual Scripting
                    IM_MENU_POPUP_END()
                    IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_SHADER)    // Shader
                    IM_MENU_POPUP_END()
                    IM_MENU_ITEM(ID_MENU_ITEM_SHADER_VARIANT_COLLECTION)    // Shader Variant Collection
                    IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_TESTING)   // Testing
                    IM_MENU_POPUP_END()
                    IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_PLAYABLES) // Playables
                    IM_MENU_POPUP_END()
                    IM_MENU_ITEM(ID_MENU_ITEM_DYLIB)    // Dynamic Link Library
                    IM_MENU_ITEM(ID_MENU_ITEM_DYLIB_REF)    // Dynamic Link Library Reference

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_TEXT)  // Text
                    IM_MENU_POPUP_END()
                    IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_TEXT_MESH_PRO) // Test Mesh Pro
                    IM_MENU_POPUP_END()

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_ITEM(ID_MENU_ITEM_SCENE)    // Scene
                    IM_MENU_ITEM(ID_MENU_ITEM_SCENE_TEMPLATE)   // Scene Template
                    IM_MENU_ITEM(ID_MENU_ITEM_SCENE_TEMPLATE_FROM_SCENE)    // Scene Template From Scene
                    IM_MENU_ITEM(ID_MENU_ITEM_SCENE_TEMPLATE_PIPELINE)  // Scene Template Pipeline
                    IM_MENU_ITEM(ID_MENU_ITEM_PREFAB)   // Prefab
                    IM_MENU_ITEM(ID_MENU_ITEM_PREFAB_VARIANT)   // Prefab Variant

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_ITEM(ID_MENU_ITEM_AUDIO_MIXER)  // Audio Mixer
        
                    IM_MENU_SEPARATOR() // Separator
        
                    IM_MENU_ITEM(ID_MENU_ITEM_MATERIAL) // Material
                    IM_MENU_ITEM(ID_MENU_ITEM_RENDER_TEXTURE)   // Render Texture
                    IM_MENU_ITEM(ID_MENU_ITEM_LIGHTMAP_PARAMETERS)  // Lightmap Parameters
                    IM_MENU_ITEM(ID_MENU_ITEM_LIGHTING_SETTINGS)    // Lighting Settings
                    IM_MENU_ITEM(ID_MENU_ITEM_CUSTOM_RENDER_TEXTURE)    // Custom Render Texture

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_ITEM(ID_MENU_ITEM_ANIMATOR_CONTROLLER)  // Animator Controller
                    IM_MENU_ITEM(ID_MENU_ITEM_ANIMATION)    // Animation
                    IM_MENU_ITEM(ID_MENU_ITEM_ANIMATOR_OVERRIDE_CONTROLLER) // Animator Override Controller
                    IM_MENU_ITEM(ID_MENU_ITEM_AVATAR_MASK)  // Avatar Mask

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_ITEM(ID_MENU_ITEM_TIMELINE) // Timeline
                    IM_MENU_ITEM(ID_MENU_ITEM_SIGNAL)   // Signal

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_ITEM(ID_MENU_ITEM_GUI_SKIN) // GUI Skin
                    IM_MENU_ITEM( ID_MENU_ITEM_CUSTOM_FONT) // Custom Font

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_LEGACY)    // Legacy
                    IM_MENU_POPUP_END()
                    IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_UI_TOOLKIT)    // UI Toolkit
                    IM_MENU_POPUP_END()

                    IM_MENU_SEPARATOR() // Separator

                    IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_SEARCH)    // Search
                    IM_MENU_POPUP_END()

                    IM_MENU_ITEM(ID_MENU_ITEM_BRUSH)    // Brush
                    IM_MENU_ITEM(ID_MENU_ITEM_TERRAIN_LAYER)    // Terrain Layer

                IM_MENU_POPUP_END()

                IM_MENU_ITEM(ID_MENU_ITEM_SHOW_IN_EXPLORER) // Show in Explorer
                IM_MENU_ITEM(ID_MENU_ITEM_OPEN) // Open
                IM_MENU_ITEM(ID_MENU_ITEM_DELETE_ASSET) // Delete
                IM_MENU_ITEM(ID_MENU_ITEM_RENAME_ASSET) // Rename
                IM_MENU_ITEM(ID_MENU_ITEM_COPY_PATH)    // Copy path
        
                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_OPEN_SCENE_ADDITIVE)  // Open Scene Additive

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_VIEW_IN_PACKAGE_MANAGER)  // View in Package Manager

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_IMPORT_NEW_ASSET) // Import New Asset...            
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_IMPORT_PACKAGE)    // Import Package
                    IM_MENU_ITEM(ID_MENU_ITEM_CUSTOM_PACKAGE)   // Custom Package
                IM_MENU_POPUP_END()        
                IM_MENU_ITEM(ID_MENU_ITEM_EXPORT_PACKAGE)   // Export Package...            
                IM_MENU_ITEM(ID_MENU_ITEM_FIND_REFERENCES_IN_SCENE) // Find References In Scene
                IM_MENU_ITEM(ID_MENU_ITEM_FIND_REFERENCES_IN_PROJECT)   // Find References In Project
                IM_MENU_ITEM(ID_MENU_ITEM_SELECT_DEPENDENCIES)  // Select Dependencies

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_REFRESH)  // Refresh
                IM_MENU_ITEM(ID_MENU_ITEM_REIMPORT) // Reimport

                IM_MENU_SEPARATOR() // Separator

                // Reimport All
                IM_MENU_ITEM(ID_MENU_ITEM_REIMPORT_ALL)

                IM_MENU_SEPARATOR() // Separator

                // Extract From Prefab
                IM_MENU_ITEM(ID_MENU_ITEM_EXTRACT_FROM_PREFAB)

                IM_MENU_SEPARATOR() // Separator

                // Update UXML Schema

                // IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_OPEN_CPP_PROJECT) // Open C++ Project
                IM_MENU_ITEM(ID_MENU_IETM_VIEW_IN_IMPORT_ACTIVITY_WINDOW)   // View in Import Activity Window
                IM_MENU_ITEM(ID_MENU_ITEM_PROPERTIES)   // Properties
            IM_MENU_POPUP_END()
        IM_MENU_END()

        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildGameObjectMenu()
    {
        IM_MENU_BEGIN(mMenuBar)
            IM_MENU_POPUP_BEGIN(ID_MENU_GAME_OBJECT)    // GameObject
                IM_MENU_ITEM(ID_MENU_ITEM_CREATE_EMPTY) // Create Empty
                IM_MENU_ITEM(ID_MENU_ITEM_CREATE_EMPTY_CHILD)   // Create Empty Child
                IM_MENU_ITEM(ID_MENU_ITEM_CREATE_EMPTY_PARENT)  // Create Empty Parent
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_3D_OBJECT) // 3D Object
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_EFFECTS)   // Effects
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_LIGHT) // Light
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_AUDIO) // Audio
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_VIDEO) // Video
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_UI)    // UI
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_GAMEOJECT_UI_TOOLKIT)  // UI Toolkit
                IM_MENU_POPUP_END()
                IM_MENU_ITEM(ID_MENU_ITEM_CAMERA)   // Camera
                IM_MENU_ITEM(ID_MENU_ITEM_VISUAL_SCRIPTING_SCENE_VARAIBLES) // Visual Scripting Scene Variables

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_CENTER_ON_CHILDREN)   // Center On Children

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_MAKE_PARENT)  // Make Parent
                IM_MENU_ITEM(ID_MENU_ITEM_CLEAR_PARENT) // Clear Parent

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_SET_AS_FIRST_SIBLING) // Set as first sibling
                IM_MENU_ITEM(ID_MENU_ITEM_SET_AS_LAST_SIBLING)  // Set as last sibling
                IM_MENU_ITEM(ID_MENU_ITEM_MOVE_TO_VIEW) // Move To View
                IM_MENU_ITEM(ID_MENU_ITEM_ALIGN_WITH_VIEW)  // Align With View
                IM_MENU_ITEM(ID_MENU_ITEM_ALIGN_VIEW_TO_SELECTED)   // Align View to Selected
                IM_MENU_ITEM(ID_MENU_ITEM_TOGGLE_ACTIVE_STATE)  // Toggle Active State
            IM_MENU_POPUP_END()
        IM_MENU_END()
        
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildComponentMenu()
    {
        IM_MENU_BEGIN(mMenuBar)
            IM_MENU_POPUP_BEGIN(ID_MENU_COMPONENT)
                IM_MENU_ITEM(ID_MENU_ITEM_ADD)  // Add...
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_MESH)  // Mesh
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_EFFECTS_COMPONENT) // Effects
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_PHYSICS)   // Physics
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_PHYSICS_2D)    // Physics 2D
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_NAVIGATION)    // Navigation
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_AUDIO_COMPONENT)   // Audio
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_VIDEO_COMPONENT)   // Video
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_RENDERING) // Rendering
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_TILEMAP)   // Tilemap
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_LAYOUT)    // Layout
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_PLAYABLES_COMPONENT)   // Playables
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_MISCELLANEOUS) // Miscellaneous
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_SCRIPTS)   // Scripts
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_UI_COMPONENT)  // UI
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_VISUAL_SCRIPTING_COMPONENT)    // Visual Scripting
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_EVENT) // Event
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_UI_TOOLKIT_COMPONENT)  // UI Toolkit
                IM_MENU_POPUP_END()
            IM_MENU_POPUP_END()
        IM_MENU_END()

        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult UIMainWindow::buildWindowMenu()
    {
        IM_MENU_BEGIN(mMenuBar)
            IM_MENU_POPUP_BEGIN(ID_MENU_WINDOW) // Window
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_PANELS)   // Panels
                    IM_MENU_CHECK(ID_MENU_ITEM_CONSOLE_WINDOW)   // Console
                    IM_MENU_CHECK(ID_MENU_ITEM_GAME_WIDNOW)  // Game
                    IM_MENU_CHECK(ID_MENU_ITEM_HIERARCHY_WINDOW) // Hierarchy
                    IM_MENU_CHECK(ID_MENU_ITEM_INSPECTOR_WINDOW) // Inspector
                    IM_MENU_CHECK(ID_MENU_ITEM_PROJECT_WINDOW)   // Project
                    IM_MENU_CHECK(ID_MENU_ITEM_SCENE_WINDOW) // Scene
                IM_MENU_POPUP_END()
            
                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_NEXT_WINDOW)  // Next Window
                IM_MENU_ITEM(ID_MENU_ITEM_PREV_WINDOW)  // Previous Window

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_LAYOUTS)   // Layouts
                    IM_MENU_ITEM(ID_MENU_ITEM_RESET_ALL_LAYOUTS)    // Reset All Layouts
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_SEARCH_WINDOW) // Search
                IM_MENU_POPUP_END()
            
                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_PACKAGE_MANAGER)  // Package Manager

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_ASSET_MANAGEMENT) // Asset Management

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_TEXT_WINDOW)  // Text

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_TEXT_MESH_PRO_WINDOW) // Text Mesh Pro

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_GENERAL)   // General
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_RENDERING_WINDOW)  // Rendering
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_ANIMATION_WINDOW)  // Animation
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_AUDIO_WINDOW)  // Audio
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_SEQUENCING)    // Sequencing
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_ANALYSIS)  // Analysis
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_AI)    // AI
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_UI_TOOLKIT_WINDOW) // UI Toolkit
                IM_MENU_POPUP_END()
                IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_VISUAL_SCRIPTING_WINDOW)   // Visual Scripting
                IM_MENU_POPUP_END()
            IM_MENU_POPUP_END()
        IM_MENU_END()

        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult UIMainWindow::buildHelpMenu()
    {
        IM_MENU_BEGIN(mMenuBar)
            IM_MENU_POPUP_BEGIN(ID_MENU_HELP);
                IM_MENU_ITEM(ID_MENU_ITEM_ABOUT)  // About

                IM_MENU_SEPARATOR() // Separator

                IM_MENU_ITEM(ID_MENU_ITEM_USER_MANUAL)  // User Manual
                IM_MENU_ITEM(ID_MENU_ITEM_API_DOCUMENT) // API Document
            IM_MENU_POPUP_END()
        IM_MENU_END()
        
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
#endif
    
    NS_END
}
