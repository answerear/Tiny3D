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
#include "ImGuiApp.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)
    
    class MainWindow;
    class ProjectManager;
    class NetworkManager;
    class EditorScene;
    
    class EditorApp : public ImGuiApp
    {
    public:
        EditorApp();
        
        ~EditorApp() override;

        TResult go(int32_t argc, char *argv[]);

        void setEditorRenderer(ImGuiImpl *impl) override;

        void exitApp() override;
        
    protected:  /// from Tiny3D::Application
        bool applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

        void applicationDidEnterBackground() override;

        void applicationWillEnterForeground() override;

        void applicationWillTerminate() override;

        void applicationLowMemory() override;

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

        class TestScene *mTestScene {nullptr};
        
        StartOptions mOptions {};

        ImTextureManager    *mTextureMgr {nullptr};
        MainWindow          *mMainWindow {nullptr};

        String mIniPath {};

        bool mExitApp {false};
    };

    NS_END
}


