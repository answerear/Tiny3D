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


#include "ImOpenFileDialog.h"
#include <filesystem>

#if defined (T3D_OS_WINDOWS)
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shellapi.h>
#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_LINUX)
#endif


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    String ImOpenFileDialog::openExplorerFolderDialog()
    {
        String path;
        
#if defined (T3D_OS_WINDOWS)
        IFileDialog* pFileDialog = NULL;
        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));
        if (SUCCEEDED(hr))
        {
            DWORD dwOptions;
            pFileDialog->GetOptions(&dwOptions);
            pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);
            hr = pFileDialog->Show(NULL);
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileDialog->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFolderPath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
                    if (SUCCEEDED(hr))
                    {
                        path = T3D_LOCALE.UnicodeToUTF8(pszFolderPath);
                        CoTaskMemFree(pszFolderPath);
                    }
                    pItem->Release();
                }
            }
            pFileDialog->Release();
        }
#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_LINUX)
#endif

        return path;
    }

    //--------------------------------------------------------------------------

    String ImOpenFileDialog::openExplorerFileDialog(const char *filter)
    {
        String path;
        
#if defined (T3D_OS_WINDOWS)
        OPENFILENAME ofn;
        char szFile[260] = { 0 };
        // std::ifstream inputFile(HBPEditor::GetLogicHelper().GetRootPath() + "/temp/LastOpenFilePath.txt");
        // if (inputFile.is_open())
        // {
        //     std::getline(inputFile, path);
        //     inputFile.close();
        // }
        std::filesystem::path filePath(path);
        String folderPath = filePath.parent_path().string();
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = folderPath.empty() ? nullptr : folderPath.c_str();
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if (GetOpenFileName(&ofn) == TRUE)
        {
            path = szFile;
            // std::ofstream outputFile(HBPEditor::GetLogicHelper().GetRootPath() + "/temp/LastOpenFilePath.txt");
            // if (outputFile.is_open())
            // {
            //     outputFile << strPath;
            //     outputFile.close();
            // }
        }
        else
        {
            path.clear();
        }
#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_LINUX)
#endif
        
        return path;
    }

    //--------------------------------------------------------------------------
}
