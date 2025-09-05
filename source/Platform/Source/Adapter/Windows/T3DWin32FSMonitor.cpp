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
#include "Thread/T3DRunnableThread.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Win32FSMonitor::Win32FSMonitor()
    {
        mChangedQMutex = T3D_NEW Mutex();
    }

    //--------------------------------------------------------------------------

    Win32FSMonitor::~Win32FSMonitor()
    {
        T3D_SAFE_DELETE(mChangedQMutex);
        T3D_SAFE_DELETE(mThread);
        close();
    }

    //--------------------------------------------------------------------------

    bool Win32FSMonitor::init()
    {
        bool ret = true;

        do
        {
            WString wstrPath = T3D_LOCALE.UTF8ToUnicode(mPath);
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
                ret = false;
                break;
            }

            memset(&mOverlapped, 0, sizeof(mOverlapped));
            mOverlapped.hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);

            mIsRunning = true;
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32FSMonitor::startWatching(const String &path, const FSMonitorExts &excludeExts, const FSMonitorExcludes &excludeFolders, const FSMonitorOnChanged &onChanged)
    {
        TResult ret = T3D_OK;

        do
        {
            mPath = path;
            mExcludeExts = excludeExts;
            mExcludeFolders = excludeFolders;
            mOnChanged = onChanged;

            if (mThread == nullptr)
            {
                mThread = T3D_NEW RunnableThread();
            }
            
            T3D_ASSERT(mThread);
            ret = mThread->start(this, "FileSystemMonitorThread");
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32FSMonitor::run()
    {
        TResult ret = T3D_OK;

        while (mIsRunning)
        {
            const DWORD dwBufferSize = 1024;
            char buffer[dwBufferSize] = {0};
            DWORD dwBytesReturned = 0;

            if (!::ReadDirectoryChangesW(
                mDirHandle,
                buffer,
                dwBufferSize,
                TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME/* | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION*/,
                &dwBytesReturned,
                &mOverlapped,
                nullptr))
            {
                break;
            }

            if (!GetOverlappedResult(mDirHandle, &mOverlapped, &dwBytesReturned, TRUE))
            {
                if (::GetLastError() == ERROR_OPERATION_ABORTED && !mIsRunning)
                {
                    break;
                }
                else
                {
                    break;
                }
            }

            char *p = buffer;
            FILE_NOTIFY_INFORMATION *info = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(p);

            if (p < (char *)buffer + dwBytesReturned)
            {
                do
                {
                    info = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(p);
                    p += info->NextEntryOffset;
                    WString wstrFilename(info->FileName, info->FileNameLength / sizeof(wchar_t));
                    String filename = T3D_LOCALE.UnicodeToUTF8(wstrFilename);
                    String path = mPath + Dir::getNativeSeparator() + filename;

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
                            ScopeLock lock(mChangedQMutex);
                            mChangedItemsQ.emplace_back(action, path);
                        }
                    }
                } while (p < buffer + dwBytesReturned && info->NextEntryOffset != 0);
            }
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32FSMonitor::stopWatching()
    {
        TResult ret = T3D_OK;

        stop();
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult Win32FSMonitor::poll()
    {
        ScopeLock lock(mChangedQMutex);
        while (!mChangedItemsQ.empty())
        {
            auto &item = mChangedItemsQ.front();
            mOnChanged(item.filePath, item.action);
            mChangedItemsQ.pop_front();
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void Win32FSMonitor::stop()
    {
        mIsRunning = false;
        ::CancelIoEx(mDirHandle, &mOverlapped);
        mThread->wait();
    }

    //--------------------------------------------------------------------------

    void Win32FSMonitor::exit()
    {
        close();
    }

    //--------------------------------------------------------------------------

    void Win32FSMonitor::close()
    {
        if (mOverlapped.hEvent != nullptr)
        {
            ::CloseHandle(mOverlapped.hEvent);
            mOverlapped.hEvent = nullptr;
        }
        
        if (mDirHandle != nullptr)
        {
            ::CloseHandle(mDirHandle);
            mDirHandle = nullptr;
        }
    }

    //--------------------------------------------------------------------------
}
