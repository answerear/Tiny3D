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

#include "Adapter/Windows/T3DWin32Platform.h"
#include <windows.h>
#include <tlhelp32.h>
#include "T3DDir.h"
#include "T3DPlatformErrorDef.h"


namespace Tiny3D
{
    
    //--------------------------------------------------------------------------

    Win32Platform::Win32Platform()
    {
        mMainThreadID = ::GetCurrentThreadId();
    }
    
    //--------------------------------------------------------------------------

    void Win32Platform::memoryBarrier()
    {
        _mm_sfence();
    }

    //--------------------------------------------------------------------------

    ulong_t Win32Platform::getCurrentThreadID()
    {
        return ::GetCurrentThreadId();
    }

    //--------------------------------------------------------------------------

    ulong_t Win32Platform::getMainThreadID()
    {
        return mMainThreadID;
    }

    //--------------------------------------------------------------------------

    void Win32Platform::sleepCurrentThread(uint32_t msec)
    {
        ::Sleep(msec);
    }
    
    //--------------------------------------------------------------------------
    
    uint32_t Win32Platform::getThreadHardwareConcurrency()
    {
        return std::thread::hardware_concurrency();
    }

    //--------------------------------------------------------------------------

    void Win32Platform::traverseAllProcesses(const OnTraverseProcess &callback)
    {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        if (::Process32First(snapshot, &entry))
        {
            do
            {
                if (callback(entry.th32ProcessID, entry.szExeFile))
                {
                    break;
                }
            } while (::Process32Next(snapshot, &entry));
        }

        ::CloseHandle(snapshot);
    }

    //--------------------------------------------------------------------------

    ulong_t Win32Platform::getCurrentProcessID()
    {
        return ::GetCurrentProcessId();
    }

    //--------------------------------------------------------------------------

    const String &Win32Platform::getCurrentProcessName()
    {
        if (mCurrentProcessName.empty())
        {
            const DWORD exePathSize = 1024;
            char exePath[exePathSize];
            DWORD exePathLength = GetModuleFileName(nullptr, exePath, exePathSize);

            String path(exePath);
            String dir;
            Dir::parsePath(path, dir, mCurrentProcessName);
        }

        return mCurrentProcessName;
    }

    //--------------------------------------------------------------------------

    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
    {
        DWORD targetProcessID = (DWORD)lParam;
        DWORD windowProcessID;

        // 获取窗口关联的进程ID
        ::GetWindowThreadProcessId(hwnd, &windowProcessID);

        // 检查窗口关联的进程ID是否与目标进程ID匹配
        if (windowProcessID == targetProcessID)
        {
            // 将窗口置于最上层
            // 检查窗口是否最小化
            if (IsIconic(hwnd))
            {
                // 激活最小化的窗口并将其置于最前端
                ShowWindow(hwnd, SW_RESTORE);
            }
            ::SetForegroundWindow(hwnd);
            return FALSE; // 停止遍历窗口
        }

        return TRUE; // 继续遍历窗口
    }

    
    bool Win32Platform::wakeupProcess(ulong_t pid)
    {
        // 遍历所有窗口，查找与目标进程ID关联的窗口
        ::EnumWindows(EnumWindowsProc, (LPARAM)pid);
        return true;
    }

    //--------------------------------------------------------------------------
}
