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


#include "Adapter/Windows/T3DWin32FSMonitor.h"

#include "T3DDir.h"
#include "T3DPlatformErrorDef.h"
#include "Locale/T3DLocale.h"
#include "IO/T3DDir.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Win32FSMonitor::Win32FSMonitor()
    {
        
    }

    //--------------------------------------------------------------------------

    Win32FSMonitor::~Win32FSMonitor()
    {
        cleanup();
    }

    //--------------------------------------------------------------------------

    TResult Win32FSMonitor::init(const String &path, const FSMonitorExts &excludeExts, const FSMonitorExcludes &excludeFolders, const FSMonitorOnChanged &onChanged)
    {
        TResult ret = T3D_OK;

        do
        {
            mPath = path;
            mExcludeExts = excludeExts;
            mExcludeFolders = excludeFolders;
            mOnChanged = onChanged;
            
            WString wstrPath = T3D_LOCALE.UTF8ToUnicode(path);
            mDirHandle = ::CreateFileW(
                wstrPath.c_str(),
                FILE_LIST_DIRECTORY,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                nullptr,
                OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                nullptr);

            if (mDirHandle == INVALID_HANDLE_VALUE)
            {
                mPath.clear();
                mExcludeExts.clear();
                mExcludeFolders.clear();
                mOnChanged = nullptr;
                ret = T3D_ERR_FS_MONITOR_CREATED;
                break;
            }

            memset(&mOverlapped, 0, sizeof(mOverlapped));
            mOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32FSMonitor::monitor()
    {
        TResult ret = T3D_OK;

        do
        {
            const DWORD buffer_size = 1024;
            char buffer[buffer_size] = {0};
            DWORD bytesReturned = 0;

            if (!::ReadDirectoryChangesW(
                mDirHandle,
                buffer,
                buffer_size,
                TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME/* | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION*/,
                &bytesReturned,
                &mOverlapped,
                nullptr))
            {
                break;
            }

            if (GetOverlappedResult(mDirHandle, &mOverlapped, &bytesReturned, TRUE))
            {
                DWORD offset = 0;
                FILE_NOTIFY_INFORMATION *info = nullptr;

                char *p = buffer;
                while (p < (char*)buffer + bytesReturned)
                {
                    info = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(p);
                    offset += info->NextEntryOffset;
                    WString wstrFilename(info->FileName, info->FileNameLength / sizeof(wchar_t));
                    String path = mPath + Dir::getNativeSeparator() + T3D_LOCALE.UnicodeToUTF8(wstrFilename);

                    if (!mExcludeExts.empty() || !mExcludeFolders.empty())
                    {
                        // 解出路径、名称、扩展名
                        String dir, title, ext;
                        if (!Dir::parsePath(path, dir, title, ext))
                        {
                            continue;
                        }

                        if (!ext.empty())
                        {
                            // 扩展名非空
                            auto it = std::find(mExcludeExts.begin(), mExcludeExts.end(), ext);
                            if (it != mExcludeExts.end())
                            {
                                // 被排除的扩展名
                                continue;
                            }
                        }

                        if (!title.empty())
                        {
                            // 名字非空
                            auto it = std::find(mExcludeFolders.begin(), mExcludeFolders.end(), title);
                            if (it != mExcludeFolders.end())
                            {
                                // 被排除的文件夹名
                                continue;
                            }
                        }
                    }
                    
                    if (mOnChanged != nullptr)
                    {
                        FSMonitorAction action = FSMonitorAction::kNone;
                        
                        switch (info->Action)
                        {
                        case FILE_ACTION_ADDED:
                            action = FSMonitorAction::kAdded;
                            break;
                        case FILE_ACTION_REMOVED:
                            action = FSMonitorAction::kRemoved;
                            break;
                        case FILE_ACTION_MODIFIED:
                            action = FSMonitorAction::kModifed;
                            break;
                        case FILE_ACTION_RENAMED_OLD_NAME:
                            action = FSMonitorAction::kRenamedOld;
                            break;
                        case FILE_ACTION_RENAMED_NEW_NAME:
                            action = FSMonitorAction::kRenamedNew;
                            break;
                        default:
                            // T3D_ASSERT(0);
                            break;
                        }

                        if (action != FSMonitorAction::kNone)
                        {
                            mOnChanged(path, action);
                        }
                    }

                    if (info->NextEntryOffset == 0)
                    {
                        break;
                    }
                    
                    p += info->NextEntryOffset;
                }
            }

        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void Win32FSMonitor::cleanup()
    {
        if (mDirHandle != nullptr)
        {
            ::CloseHandle(mDirHandle);
            mDirHandle = nullptr;
        }
    }

    //--------------------------------------------------------------------------
}
