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

#include "Adapter/Windows/T3DWin32Process.h"

#include <filesystem>
#include <windows.h>

#include "T3DDir.h"
#include "T3DPlatformErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Win32Process::Win32Process()
    {

    }

    //--------------------------------------------------------------------------

    Win32Process::~Win32Process()
    {
        if (mThread != nullptr)
        {
            ::CloseHandle(mThread);
        }
        
        if (mProcess != nullptr)
        {
            ::CloseHandle(mProcess);
        }
    }

    //--------------------------------------------------------------------------

    ulong_t Win32Process::getID() const
    {
        return mProcessID;
    }

    //--------------------------------------------------------------------------

    const String &Win32Process::getName() const
    {
        return mProcessName;
    }

    //--------------------------------------------------------------------------

    TResult Win32Process::start(const String &exePath, const String &cmdList)
    {
        TResult ret = T3D_OK;

        do
        {
            STARTUPINFO si;
            memset(&si, 0, sizeof(STARTUPINFO));

            PROCESS_INFORMATION pi;
            memset(&pi, 0, sizeof(PROCESS_INFORMATION));

            String fullCmdList = exePath + " " + cmdList;
            if (!::CreateProcess(exePath.c_str(), const_cast<char*>(fullCmdList.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
            {
                DWORD dwError = ::GetLastError();
                ret = T3D_ERR_PROCESS_CREATED;
                break;
            }

            mProcess = pi.hProcess;
            mThread = pi.hThread;
            mProcessID = pi.dwProcessId;

            String path;
            Dir::parsePath(exePath, path, mProcessName);
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult Win32Process::wait(uint32_t timeout)
    {
        TResult ret = T3D_ERR_TIMEOUT;

        do
        {
            if (mProcess == nullptr)
            {
                ret = T3D_ERR_NATIVE_OBJECT_NOT_CREATED;
                break;
            }

            if (::WaitForSingleObject(mProcess, timeout) == WAIT_OBJECT_0)
            {
                ret = T3D_OK;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32Process::terminate(int32_t exitCode)
    {
        TResult ret = T3D_OK;
        
        if (mProcess != nullptr)
        {
            if (!::TerminateProcess(mProcess, exitCode))
            {
                ret = T3D_ERR_PROCESS_TERMINATED;
            }
            else
            {
                ::CloseHandle(mThread);
                mThread = nullptr;
                ::CloseHandle(mProcess);
                mProcess = nullptr;
            }
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
