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

#include "LogCameraBehaviour.h"


#define LOG_TAG_APP     "APP"


namespace Tiny3D
{
    LogCameraBehaviour::LogCameraBehaviour(const UUID &uuid)
        : CameraBehaviour(uuid)
    {
    }

    bool LogCameraBehaviour::shouldLog() const
    {
        return mFrameLogEnabled || mLoggedFrames < kStartupLogFrames;
    }

    void LogCameraBehaviour::setFrameLogEnabled(bool enabled)
    {
        mFrameLogEnabled = enabled;
    }

    void LogCameraBehaviour::toggleFrameLog()
    {
        mFrameLogEnabled = !mFrameLogEnabled;
        T3D_LOG_INFO(LOG_TAG_APP, "[Camera] frame log %s",
            mFrameLogEnabled ? "on" : "off");
    }

    void LogCameraBehaviour::onPreRender(RHIContext * /*ctx*/)
    {
        if (shouldLog())
        {
            T3D_LOG_INFO(LOG_TAG_APP, "[Camera] onPreRender");
        }
    }

    void LogCameraBehaviour::onPostRender(RHIContext * /*ctx*/)
    {
        if (shouldLog())
        {
            T3D_LOG_INFO(LOG_TAG_APP, "[Camera] onPostRender");
            if (!mFrameLogEnabled)
            {
                ++mLoggedFrames;
            }
        }
    }
}
