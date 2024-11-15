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

#pragma once


#include "EditorPrerequisites.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    class UIHierarchyWindow;
    class UIGameWindow;
    class UISceneWindow;
    class UIInspectorWindow;
    class UIProjectWindow;
    class UIConsoleWindow;
    
    class UIMainWindow : public ImWindow
    {
    public:

    protected:
        TResult onCreate() override;

        bool onGUIBegin() override;

        void onGUIEnd() override;

#if defined (USE_LEGACY_MENU)
        TResult buildMenu();
        TResult buildFileMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult buildEditMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult buildAssetsMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult buildGameObjectMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult buildComponentMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult buildWindowMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult buildHelpMenu(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
#else
        TResult createMenuItemData();

        TResult createFileMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult createEditMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult createAssetMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult createGameObjectMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult createComponentMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult createWindowMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        TResult createHelpMenuItemData(const ImMenuItemQueryCallback &queryEnableDefault, const ImMenuItemQueryCallback &queryDisableDefault);
        
        TResult buildMainMenu();
        TResult buildFileMenu();
        TResult buildEditMenu();
        TResult buildAssetMenu();
        TResult buildGameObjectMenu();
        TResult buildComponentMenu();
        TResult buildWindowMenu();
        TResult buildHelpMenu();
        
#endif
        
    protected:
        ImMenuBar   *mMenuBar {nullptr};

        UIHierarchyWindow *mHierarchyWnd {nullptr};
        UIGameWindow *mGameWnd {nullptr};
        UISceneWindow *mSceneWnd {nullptr};
        UIInspectorWindow *mInspectorWnd {nullptr};
        UIProjectWindow *mProjectWnd {nullptr};
        UIConsoleWindow *mConsoleWnd {nullptr};
    };

    NS_END
}
