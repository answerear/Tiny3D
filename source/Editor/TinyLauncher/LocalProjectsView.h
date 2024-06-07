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


namespace Tiny3D
{
    NS_BEGIN(Launcher)

    enum AppEventID
    {
        kEvtApp = 1,
        kEvtOpenNewDialog,
    };

    class NewProjectDialog;

    class LocalProjectViewT : public ImChildView, public EventHandler
    {
    protected:
        TResult onCreate() override;
        
        void onGUI() override;

        int32_t mCurrentSortType {0};
        char mFilterText[512] {0};
    };

    class LocalProjectViewBL : public ImChildView, public EventHandler
    {
    protected:
        void onGUI() override;

        int32_t onGetChildFlags() override;

        int32_t mProjectSelectedIndex {-1};
    };

    class LocalProjectViewBR : public ImChildView, public EventHandler
    {
    public:
        void onGUI() override;

        int32_t onGetChildFlags() override;

        int32_t mCurrentLanguage {0};
    };
    
    class LocalProjectsView : public ImChildView, public EventHandler
    {
    public:
        void update() override;
        
    protected:
        TResult onCreate() override;

        void onDestroy() override;

        bool onGUIBegin() override;

        // void onGUI() override;
        //
        // void onGUIEnd() override;

        bool onOpenNewDialog(EventParam *param, TINSTANCE sender);
        
        // void onGUITop(const ImVec2 &size);
        //
        // void onGUIBottomLeft(const ImVec2 &size);
        //
        // void onGUIBottomRight(const ImVec2 &size);
        //
        // void openNewProjectDialog();

        
    protected:
        LocalProjectViewT   *mViewT {nullptr};
        LocalProjectViewBL  *mViewBL {nullptr};
        LocalProjectViewBR  *mViewBR {nullptr};
        
        // enum class SortType : int32_t
        // {
        //     kRecentEdit = 0,
        //     kName,
        //     kPath,
        //     kTag,
        // };
        //
        // int32_t mCurrentSortType {0};
        // int32_t mCurrentLanguage {0};
        //
        // char mFilterText[512] {0};
        //
        // int32_t mProjectSelectedIndex {-1};

        NewProjectDialog *mNewProjectDlg {nullptr};
    };

    NS_END
}
