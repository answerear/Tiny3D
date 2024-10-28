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

#include "T3DLogItem.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    uint32_t LogItem::formatContent(const DateTime &dt, const char *filename, 
            int32_t line, const String &level, const char *tag, const char *content)
    {
        const ulong_t threadID = RunnableThread::getCurrentThreadID();
        const String strDateTime = dt.toString();
        std::stringstream ss;
        ss << strDateTime
            << "|" << level 
            << "|" << threadID
            << "|" << filename
            << "(" << line
            << ")|" << tag
            << "|" << content
            <<"\n";
        mContentSize = (uint32_t)ss.str().length();
        mContentSize = (uint32_t)(mContentSize > sizeof(mContent) - 1 
            ? sizeof(mContent) - 1 : ss.str().length());
        memcpy(mContent, ss.str().c_str(), mContentSize);
        mContent[mContentSize] = 0;
        return mContentSize;
    }
    
    //--------------------------------------------------------------------------
}
