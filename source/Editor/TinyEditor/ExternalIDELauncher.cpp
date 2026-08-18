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


#include "ExternalIDELauncher.h"
#include "EditorApp.h"
#include "ScriptBuildSystem.h"

#include <cctype>
#include <cstring>

#if defined (T3D_OS_WINDOWS)
    #include <windows.h>
    #include <shellapi.h>
#endif


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    static void splitLines(const String &text, TArray<String> &lines)
    {
        String current;
        for (size_t i = 0; i < text.size(); ++i)
        {
            const char c = text[i];
            if (c == '\n')
            {
                if (!current.empty() && current.back() == '\r')
                {
                    current.pop_back();
                }
                if (!current.empty())
                {
                    lines.push_back(current);
                }
                current.clear();
            }
            else
            {
                current.push_back(c);
            }
        }

        if (!current.empty() && current.back() == '\r')
        {
            current.pop_back();
        }
        if (!current.empty())
        {
            lines.push_back(current);
        }
    }

    //--------------------------------------------------------------------------

    String ExternalIDELauncher::normalizePath(const String &path)
    {
        String result = path;
        const char sep = Dir::getNativeSeparator();
        for (size_t i = 0; i < result.size(); ++i)
        {
            if (result[i] == '/' || result[i] == '\\')
            {
                result[i] = sep;
            }
        }
        return result;
    }

    //--------------------------------------------------------------------------

    String ExternalIDELauncher::repairStrippedExtension(const String &path)
    {
        if (path.empty() || Dir::exists(path))
        {
            return path;
        }

        static const char *kExts[] = { "exe", "com", "bat", "cmd", nullptr };
        for (int i = 0; kExts[i] != nullptr; ++i)
        {
            const size_t n = strlen(kExts[i]);
            if (path.size() <= n)
            {
                continue;
            }

            const String tail = path.substr(path.size() - n);
            bool match = true;
            for (size_t j = 0; j < n; ++j)
            {
                if (std::tolower(static_cast<unsigned char>(tail[j]))
                    != static_cast<unsigned char>(kExts[i][j]))
                {
                    match = false;
                    break;
                }
            }

            if (!match || path[path.size() - n - 1] == '.')
            {
                continue;
            }

            const String repaired = path.substr(0, path.size() - n) + "." + kExts[i];
            if (Dir::exists(repaired))
            {
                return repaired;
            }
        }

        return path;
    }

    //--------------------------------------------------------------------------

    bool ExternalIDELauncher::pathsEqual(const String &lhs, const String &rhs)
    {
        const String a = normalizePath(repairStrippedExtension(lhs));
        const String b = normalizePath(repairStrippedExtension(rhs));
        if (a.size() != b.size())
        {
            return false;
        }

#if defined (T3D_OS_WINDOWS)
        for (size_t i = 0; i < a.size(); ++i)
        {
            if (std::tolower(static_cast<unsigned char>(a[i]))
                != std::tolower(static_cast<unsigned char>(b[i])))
            {
                return false;
            }
        }
        return true;
#else
        return a == b;
#endif
    }

    //--------------------------------------------------------------------------

    String ExternalIDELauncher::findVswhere()
    {
#if defined (T3D_OS_WINDOWS)
        char buffer[MAX_PATH] = { 0 };
        const DWORD n = ::GetEnvironmentVariableA("ProgramFiles(x86)", buffer, MAX_PATH);
        if (n == 0 || n >= MAX_PATH)
        {
            return String();
        }

        const String path = String(buffer)
            + "\\Microsoft Visual Studio\\Installer\\vswhere.exe";
        return Dir::exists(path) ? path : String();
#else
        return String();
#endif
    }

    //--------------------------------------------------------------------------

    void ExternalIDELauncher::discoverVisualStudio(TArray<ExternalIDEInfo> &out)
    {
        out.clear();

        const String vswhere = findVswhere();
        if (vswhere.empty())
        {
            return;
        }

        const String quoted = String("\"") + vswhere + "\"";
        const String args = " -all -prerelease -products * "
            "-requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64";

        String namesOutput;
        String pathsOutput;
        if (T3D_FAILED(ScriptBuildSystem::runCommand(quoted + args + " -property displayName",
            String(), namesOutput)))
        {
            namesOutput.clear();
        }
        if (T3D_FAILED(ScriptBuildSystem::runCommand(quoted + args + " -property productPath",
            String(), pathsOutput)))
        {
            pathsOutput.clear();
        }

        TArray<String> names;
        TArray<String> paths;
        splitLines(namesOutput, names);
        splitLines(pathsOutput, paths);

        const size_t count = names.size() < paths.size() ? names.size() : paths.size();
        for (size_t i = 0; i < count; ++i)
        {
            if (paths[i].empty() || !Dir::exists(paths[i]))
            {
                continue;
            }

            ExternalIDEInfo info;
            info.name = names[i].empty() ? paths[i] : names[i];
            info.path = normalizePath(paths[i]);
            out.push_back(info);
        }
    }

    //--------------------------------------------------------------------------

    TResult ExternalIDELauncher::openWithShell(const String &slnPath)
    {
#if defined (T3D_OS_WINDOWS)
        const WString wpath = T3D_LOCALE.UTF8ToUnicode(slnPath);
        const INT_PTR result = reinterpret_cast<INT_PTR>(
            ::ShellExecuteW(nullptr, L"open", wpath.c_str(), nullptr, nullptr, SW_SHOWNORMAL));
        if (result <= 32)
        {
            EDITOR_LOG_ERROR("ShellExecute failed to open [%s], code %lld",
                slnPath.c_str(), static_cast<long long>(result));
            return T3D_ERR_FAIL;
        }
        return T3D_OK;
#else
        (void)slnPath;
        EDITOR_LOG_ERROR("Opening a solution via the OS file association is only "
            "implemented on Windows.");
        return T3D_ERR_FAIL;
#endif
    }

    //--------------------------------------------------------------------------

    TResult ExternalIDELauncher::openSolution(const String &slnPath)
    {
        if (slnPath.empty() || !Dir::exists(slnPath))
        {
            EDITOR_LOG_ERROR("C++ solution not found [%s]", slnPath.c_str());
            return T3D_ERR_FILE_NOT_EXIST;
        }

        EditorApp *app = static_cast<EditorApp *>(Application::getInstancePtr());
        String editorPath = (app != nullptr)
            ? app->getEditorSettings().externalEditorPath : String();
        editorPath = repairStrippedExtension(editorPath);

        if (app != nullptr && editorPath != app->getEditorSettings().externalEditorPath)
        {
            app->getEditorSettings().externalEditorPath = editorPath;
            app->saveEditorSettings();
        }

        if (editorPath.empty())
        {
            EDITOR_LOG_INFO("Opening C++ solution with the system default: %s", slnPath.c_str());
            return openWithShell(slnPath);
        }

        if (!Dir::exists(editorPath))
        {
            EDITOR_LOG_ERROR("Configured IDE does not exist [%s]", editorPath.c_str());
            return T3D_ERR_FILE_NOT_EXIST;
        }

        EDITOR_LOG_INFO("Opening C++ solution with [%s]: %s",
            editorPath.c_str(), slnPath.c_str());

        Process proc;
        const TResult ret = proc.start(editorPath, String("\"") + slnPath + "\"");
        if (T3D_FAILED(ret))
        {
            EDITOR_LOG_ERROR("Failed to start IDE [%s] ! ERROR [%d]",
                editorPath.c_str(), ret);
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    NS_END
}
