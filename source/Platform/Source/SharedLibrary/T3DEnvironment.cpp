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


#include "SharedLibrary/T3DEnvironment.h"

#if defined (T3D_OS_WINDOWS)
    #include <windows.h>
#else
    #include <stdlib.h>
#endif


namespace Tiny3D
{
#if defined (T3D_OS_WINDOWS)
    namespace
    {
        WString toWide(const String &src)
        {
            if (src.empty())
            {
                return WString();
            }

            const int len = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(),
                (int)src.length(), nullptr, 0);
            WString dst((size_t)len, L'\0');
            ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), (int)src.length(),
                &dst[0], len);
            return dst;
        }

        /// 返回 false 表示变量不存在
        bool query(const String &name, String *value)
        {
            const WString wideName = toWide(name);

            ::SetLastError(ERROR_SUCCESS);
            DWORD size = ::GetEnvironmentVariableW(wideName.c_str(), nullptr, 0);
            if (size == 0)
            {
                // 变量存在但值为空时同样返回 0，只能靠错误码区分
                return ::GetLastError() != ERROR_ENVVAR_NOT_FOUND;
            }

            // 两次调用之间变量可能被别的线程改大，按返回值重试
            for (int attempt = 0; attempt < 4; ++attempt)
            {
                WString buffer((size_t)size, L'\0');
                const DWORD len = ::GetEnvironmentVariableW(wideName.c_str(),
                    &buffer[0], size);
                if (len == 0)
                {
                    return ::GetLastError() != ERROR_ENVVAR_NOT_FOUND;
                }

                if (len < size)
                {
                    if (value != nullptr)
                    {
                        const int n = ::WideCharToMultiByte(CP_UTF8, 0,
                            buffer.c_str(), (int)len, nullptr, 0, nullptr, nullptr);
                        value->assign((size_t)n, '\0');
                        ::WideCharToMultiByte(CP_UTF8, 0, buffer.c_str(),
                            (int)len, &(*value)[0], n, nullptr, nullptr);
                    }
                    return true;
                }

                size = len;
            }

            return true;
        }
    }
#endif

    //--------------------------------------------------------------------------

    String Environment::get(const String &name)
    {
#if defined (T3D_OS_WINDOWS)
        String value;
        query(name, &value);
        return value;
#else
        const char *value = ::getenv(name.c_str());
        return (value != nullptr) ? String(value) : String();
#endif
    }

    //--------------------------------------------------------------------------

    bool Environment::has(const String &name)
    {
#if defined (T3D_OS_WINDOWS)
        return query(name, nullptr);
#else
        return ::getenv(name.c_str()) != nullptr;
#endif
    }

    //--------------------------------------------------------------------------
}
