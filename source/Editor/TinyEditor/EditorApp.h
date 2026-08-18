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
#include "EditorSettings.h"
#include "ImGuiApp.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)
    
    class UIMainWindow;
    class ProjectManager;
    class NetworkManager;
    class EditorSceneImpl;
    class AppEventProxy;
    
    class EditorApp : public ImGuiApp
    {
    public:
        EditorApp();
        
        ~EditorApp() override;

        TResult go(int32_t argc, char *argv[]);

        void setEditorRenderer(ImGuiImpl *impl) override;

        void exitApp() override;

        ImFont *getFont(int32_t fontSize);

        EditorSettings &getEditorSettings() { return mEditorSettings; }

        const EditorSettings &getEditorSettings() const { return mEditorSettings; }

        void saveEditorSettings();
        
    protected:  /// from Tiny3D::Application
        TResult applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

        void applicationDidEnterBackground() override;

        void applicationWillEnterForeground() override;

        void applicationWillTerminate() override;

        void applicationLowMemory() override;

        void applicationFocusGained() override;

        void applicationFocusLost() override;

        bool processEvents(const AppEvent &event) override;

        TResult createImGuiEnv();

        void destroyImGuiEnv();

        void engineUpdate();

        void enginePreRender();

        void enginePostRender();

        TResult createEngine(int32_t argc, char *argv[]);

        TResult createLanguageMgr();

        TResult startup(int32_t argc, char *argv[]);
        
        bool run();

        void shutdown();

        String getSettingsPath() const
        {
            return Dir::getCachePath() + Dir::getNativeSeparator() + "Settings.dat";
        }

        bool parseCommandLine(int32_t argc, char *argv[]);

        void readEditorSettings();
        
    protected:
        struct StartOptions
        {
            String path {};
            String name {};
            bool created {false};
        };
        
        SDL_Window    *mSDLWindow {nullptr};
        ImGuiImpl  *mImGuiImpl {nullptr};
        ImTextureID mSceneRT {nullptr};

        Agent *mEngine {nullptr};
        LanguageManagerPtr mLangMgr {nullptr};
        NetworkManager *mNetworkMgr {nullptr};
        ProjectManager *mProjectMgr {nullptr};
        class CppBuildSystem      *mCppBuildSys {nullptr};
        class PlayModeController  *mPlayModeCtrl {nullptr};

        class TestScene *mTestScene {nullptr};
        
        StartOptions mOptions {};

        ImMenuEventManager  *mMenuEventMgr {nullptr};
        ImTextureManager    *mTextureMgr {nullptr};
        UIMainWindow          *mMainWindow {nullptr};

        AppEventProxy   *mAppEventProxy {nullptr};

        String mIniPath {};

        EditorSettings mEditorSettings {};

        using FontMap = TUnorderedMap<int32_t, ImFont*>;
        FontMap mFontMap {};

        bool mExitApp {false};
    };

    NS_END
}


