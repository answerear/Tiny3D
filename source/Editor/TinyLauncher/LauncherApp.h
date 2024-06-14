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


#include "LauncherPrerequisites.h"
#include "ImGuiApp.h"
#include "AppSettings.h"


namespace Tiny3D
{
    NS_BEGIN(Launcher)
    
    class MainWindow;
    class NetworkManager;
    class ProjectManager;
    
    class LauncherApp : public ImGuiApp
    {
    public:
        LauncherApp();
        
        ~LauncherApp() override;

        TResult go(int32_t argc, char *argv[]);

        const AppSettings &getAppSettings() const { return mAppSettings; }

        AppSettings &getAppSettings() { return mAppSettings; }

        void setEditorRenderer(ImGuiImpl *impl) override;

        void exitApp() override;

    protected:  /// from Tiny3D::Application
        bool applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

        void applicationDidEnterBackground() override;

        void applicationWillEnterForeground() override;

        void applicationWillTerminate() override;

        void applicationLowMemory() override;

        TResult createImGuiEnv();

        void destroyImGuiEnv();
        
        bool enginePollEvents();

        void engineUpdate();

        void enginePreRender();

        void enginePostRender();

        TResult createEngine(int32_t argc, char *argv[]);

        void readAppSettings();

        void writeAppSettings();

        TResult createLanguageMgr();

        TResult startup(int32_t argc, char *argv[]);
        
        bool run();

        void shutdown();

        String getSettingsPath() const
        {
            return Dir::getCachePath() + Dir::getNativeSeparator() + "Settings.dat";
        }

    protected:
        SDL_Window *mSDLWindow {nullptr};
        ImGuiImpl *mImGuiImpl {nullptr};
        ImTextureID mSceneRT {nullptr};

        MainWindow *mMainWindow {nullptr};
        Agent *mEngine {nullptr};
        LanguageManagerPtr mLangMgr {nullptr};
        ProjectManager *mProjectMgr {nullptr};
        NetworkManager *mNetworkMgr {nullptr};
        
        AppSettings mAppSettings {};
        
        bool mExitApp {false};
    };

    NS_END
}


