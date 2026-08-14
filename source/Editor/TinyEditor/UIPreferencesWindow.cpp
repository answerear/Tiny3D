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


#include "UIPreferencesWindow.h"
#include "EditorApp.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    TResult UIPreferencesWindow::onCreate()
    {
        setVisible(false);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void UIPreferencesWindow::show()
    {
        mNeedRefresh = true;
        mFocusOnShow = true;
        setVisible(true);
    }

    //--------------------------------------------------------------------------

    bool UIPreferencesWindow::onGUIBegin()
    {
        if (mNeedRefresh)
        {
            refreshIDEList();
            mNeedRefresh = false;
        }

        ImGui::SetNextWindowSize(ImVec2(720.0f, 420.0f), ImGuiCond_FirstUseEver);

        PushWidgetID();
        if (!ImGui::Begin(getName().c_str(), &mVisible,
            ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::End();
            PopWidgetID();
            return false;
        }

        if (mFocusOnShow)
        {
            ImGui::SetWindowFocus();
            mFocusOnShow = false;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    void UIPreferencesWindow::onGUI()
    {
        ImGui::BeginChild("##PrefsCategories", ImVec2(180.0f, 0.0f), true);
        const char *externalTools = ESTR(TXT_EXTERNAL_TOOLS, "External Tools");
        if (ImGui::Selectable(externalTools, mCategory == Category::kExternalTools))
        {
            mCategory = Category::kExternalTools;
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("##PrefsContent", ImVec2(0.0f, 0.0f), true);
        if (mCategory == Category::kExternalTools)
        {
            drawExternalTools();
        }
        ImGui::EndChild();
    }

    //--------------------------------------------------------------------------

    void UIPreferencesWindow::refreshIDEList()
    {
        ExternalIDELauncher::discoverVisualStudio(mDetectedIDEs);
    }

    //--------------------------------------------------------------------------

    void UIPreferencesWindow::applyEditor(const String &name, const String &path)
    {
        EditorApp *app = static_cast<EditorApp *>(Application::getInstancePtr());
        if (app == nullptr)
        {
            return;
        }

        EditorSettings &settings = app->getEditorSettings();
        settings.externalEditorName = name;
        settings.externalEditorPath = path;
        app->saveEditorSettings();
    }

    //--------------------------------------------------------------------------

    void UIPreferencesWindow::drawExternalTools()
    {
        EditorApp *app = static_cast<EditorApp *>(Application::getInstancePtr());
        if (app == nullptr)
        {
            return;
        }

        EditorSettings &settings = app->getEditorSettings();
        const String repaired = ExternalIDELauncher::repairStrippedExtension(settings.externalEditorPath);
        if (repaired != settings.externalEditorPath)
        {
            settings.externalEditorPath = repaired;
            app->saveEditorSettings();
        }

        struct ComboItem
        {
            String name;
            String path;
        };

        TArray<ComboItem> items;
        items.push_back({ ESTR(TXT_IDE_SYSTEM_DEFAULT, "System Default"), String() });

        for (const ExternalIDEInfo &ide : mDetectedIDEs)
        {
            items.push_back({ ide.name, ide.path });
        }

        bool currentInList = settings.externalEditorPath.empty();
        if (!currentInList)
        {
            for (size_t i = 1; i < items.size(); ++i)
            {
                if (ExternalIDELauncher::pathsEqual(items[i].path, settings.externalEditorPath))
                {
                    currentInList = true;
                    break;
                }
            }
        }

        if (!currentInList && !settings.externalEditorPath.empty())
        {
            const String label = settings.externalEditorName.empty()
                ? settings.externalEditorPath : settings.externalEditorName;
            items.push_back({ label, settings.externalEditorPath });
        }

        int current = 0;
        if (!settings.externalEditorPath.empty())
        {
            for (size_t i = 0; i < items.size(); ++i)
            {
                if (ExternalIDELauncher::pathsEqual(items[i].path, settings.externalEditorPath))
                {
                    current = static_cast<int>(i);
                    break;
                }
            }
        }

        ImGui::TextUnformatted(ESTR(TXT_EXTERNAL_SCRIPT_EDITOR, "External Script Editor"));
        ImGui::Spacing();

        ImGui::PushItemWidth(-120.0f);
        if (ImGui::BeginCombo("##ExternalEditor", items[static_cast<size_t>(current)].name.c_str()))
        {
            for (int i = 0; i < static_cast<int>(items.size()); ++i)
            {
                const bool selected = (i == current);
                if (ImGui::Selectable(items[static_cast<size_t>(i)].name.c_str(), selected))
                {
                    applyEditor(items[static_cast<size_t>(i)].name, items[static_cast<size_t>(i)].path);
                }
                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        if (ImGui::Button(ESTR(TXT_BROWSE, "Browse...")))
        {
            static const char kFilter[] = "Executables (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";
            const String path = ImOpenFileDialog::openExplorerFileDialog(kFilter);
            if (!path.empty())
            {
                String dir;
                String name;
                Dir::parsePath(path, dir, name);
                applyEditor(name.empty() ? path : name,
                    ExternalIDELauncher::normalizePath(path));
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("%s", settings.externalEditorPath.empty()
            ? ESTR(TXT_IDE_SYSTEM_DEFAULT, "System Default")
            : settings.externalEditorPath.c_str());
    }

    //--------------------------------------------------------------------------

    NS_END
}
