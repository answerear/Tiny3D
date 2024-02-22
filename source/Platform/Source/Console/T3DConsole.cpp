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

#include "Console/T3DConsole.h"
#include "Adapter/T3DConsoleInterface.h"
#include "Adapter/T3DFactoryInterface.h"
#include "T3DPlatform.h"
#include <stdarg.h>

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const uint32_t Console::MAX_CONTENT_SIZE = 4096;

    //--------------------------------------------------------------------------

    Console::Console()
        : mConsole(nullptr)
    {
        mConsole = T3D_PLATFORM_FACTORY.createPlatformConsole();
    }

    //--------------------------------------------------------------------------

    Console::~Console()
    {
        T3D_SAFE_DELETE(mConsole);
    }

    //--------------------------------------------------------------------------

    void Console::print(const char *pszFmt, ...)
    {
        if (nullptr != mConsole)
        {
            char szBuffer[MAX_CONTENT_SIZE] = {0};

            va_list vl;
            va_start(vl, pszFmt);
            vsnprintf(szBuffer, MAX_CONTENT_SIZE, pszFmt, vl);
            va_end(vl);

            mConsole->print(szBuffer);
        }
    }
}
