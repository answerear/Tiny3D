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

#include "Adapter/Unix/T3DPosixProcess.h"
#include "T3DCommonErrorDef.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <vector>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    PosixProcess::PosixProcess()
        : mPid(-1)
        , mExitCode(0)
        , mRunning(false)
    {
    }

    //--------------------------------------------------------------------------

    PosixProcess::~PosixProcess()
    {
    }

    //--------------------------------------------------------------------------

    ulong_t PosixProcess::getID() const
    {
        return (ulong_t)mPid;
    }

    //--------------------------------------------------------------------------

    const String &PosixProcess::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult PosixProcess::start(const String &exePath, const String &cmdLine)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mRunning)
            {
                ret = T3D_ERR_FAIL;
                break;
            }

            // Parse command line into arguments
            std::vector<String> args;
            args.push_back(exePath);

            if (!cmdLine.empty())
            {
                // Simple split by space
                String remaining = cmdLine;
                size_t pos = 0;
                while ((pos = remaining.find(' ')) != String::npos)
                {
                    String arg = remaining.substr(0, pos);
                    if (!arg.empty())
                    {
                        args.push_back(arg);
                    }
                    remaining = remaining.substr(pos + 1);
                }
                if (!remaining.empty())
                {
                    args.push_back(remaining);
                }
            }

            // Build argv array
            std::vector<char *> argv;
            for (auto &arg : args)
            {
                argv.push_back(const_cast<char *>(arg.c_str()));
            }
            argv.push_back(nullptr);

            pid_t pid = fork();
            if (pid < 0)
            {
                ret = T3D_ERR_FAIL;
                break;
            }
            else if (pid == 0)
            {
                // Child process
                execvp(exePath.c_str(), argv.data());
                // If execvp returns, it failed
                _exit(127);
            }
            else
            {
                // Parent process
                mPid = pid;
                mRunning = true;
                mName = exePath;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PosixProcess::wait(uint32_t timeout)
    {
        TResult ret = T3D_OK;

        do
        {
            if (!mRunning)
            {
                ret = T3D_ERR_FAIL;
                break;
            }

            int status = 0;

            if (timeout == (uint32_t)-1)
            {
                // Infinite wait
                waitpid(mPid, &status, 0);
            }
            else
            {
                // Poll-based wait with timeout
                uint32_t elapsed = 0;
                const uint32_t sleepInterval = 10; // 10ms

                while (elapsed < timeout)
                {
                    pid_t result = waitpid(mPid, &status, WNOHANG);
                    if (result == mPid)
                    {
                        break;
                    }
                    else if (result < 0)
                    {
                        ret = T3D_ERR_FAIL;
                        break;
                    }

                    usleep(sleepInterval * 1000);
                    elapsed += sleepInterval;
                }

                if (elapsed >= timeout && ret == T3D_OK)
                {
                    ret = T3D_ERR_TIMEOUT;
                    break;
                }
            }

            if (ret == T3D_OK)
            {
                if (WIFEXITED(status))
                {
                    mExitCode = WEXITSTATUS(status);
                }
                else
                {
                    mExitCode = (uint32_t)-1;
                }
                mRunning = false;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PosixProcess::terminate(int32_t exitCode)
    {
        TResult ret = T3D_OK;

        do
        {
            if (!mRunning)
            {
                ret = T3D_ERR_FAIL;
                break;
            }

            if (kill(mPid, SIGKILL) != 0)
            {
                ret = T3D_ERR_FAIL;
                break;
            }

            int status = 0;
            waitpid(mPid, &status, 0);
            mExitCode = (uint32_t)exitCode;
            mRunning = false;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    uint32_t PosixProcess::getExitCode() const
    {
        return mExitCode;
    }

    //--------------------------------------------------------------------------
}
