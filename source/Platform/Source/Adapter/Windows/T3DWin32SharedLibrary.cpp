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


#include "Adapter/Windows/T3DWin32SharedLibrary.h"
#include "T3DPlatformErrorDef.h"
#include <windows.h>


namespace Tiny3D
{
    namespace
    {
        // 不走 T3D_LOCALE：ObjectTracer 等调用方可能在 Locale 销毁后仍来查询模块
        WString utf8ToWide(const String &src)
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

        String wideToUTF8(const wchar_t *src, size_t length)
        {
            if (length == 0)
            {
                return String();
            }

            const int len = ::WideCharToMultiByte(CP_UTF8, 0, src, (int)length,
                nullptr, 0, nullptr, nullptr);
            String dst((size_t)len, '\0');
            ::WideCharToMultiByte(CP_UTF8, 0, src, (int)length, &dst[0], len,
                nullptr, nullptr);
            return dst;
        }

        String formatSystemError(DWORD code)
        {
            wchar_t *buffer = nullptr;
            const DWORD len = ::FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                    | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);

            String message;
            if (len > 0 && buffer != nullptr)
            {
                size_t n = len;
                while (n > 0 && (buffer[n - 1] == L'\r' || buffer[n - 1] == L'\n'
                    || buffer[n - 1] == L' ' || buffer[n - 1] == L'.'))
                {
                    --n;
                }
                message = wideToUTF8(buffer, n);
            }

            if (buffer != nullptr)
            {
                ::LocalFree(buffer);
            }

            if (message.empty())
            {
                message = "Unknown error";
            }

            return message + " (error " + std::to_string(code) + ")";
        }

        HMODULE moduleFromAddress(const void *address)
        {
            HMODULE module = nullptr;
            if (!::GetModuleHandleExW(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                        | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCWSTR>(address), &module))
            {
                return nullptr;
            }
            return module;
        }
    }

    //--------------------------------------------------------------------------

    Win32SharedLibrary::~Win32SharedLibrary()
    {
        close();
    }

    //--------------------------------------------------------------------------

    TResult Win32SharedLibrary::open(const String &path, uint32_t flags)
    {
        // Windows 下 flags 没有对应语义，LoadLibrary 本身就等价于 Now|Local
        (void)flags;

        close();

        const WString widePath = utf8ToWide(path);
        HMODULE module = ::LoadLibraryW(widePath.c_str());
        if (module == nullptr)
        {
            mLastError = path + " : " + formatSystemError(::GetLastError());
            return T3D_ERR_SHAREDLIB_OPEN;
        }

        mModule = module;
        mLastError.clear();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void Win32SharedLibrary::close()
    {
        if (mModule != nullptr)
        {
            ::FreeLibrary(static_cast<HMODULE>(mModule));
            mModule = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    bool Win32SharedLibrary::isOpen() const
    {
        return mModule != nullptr;
    }

    //--------------------------------------------------------------------------

    void *Win32SharedLibrary::getSymbol(const String &name) const
    {
        if (mModule == nullptr)
        {
            mLastError = "Shared library is not open";
            return nullptr;
        }

        FARPROC proc = ::GetProcAddress(static_cast<HMODULE>(mModule), name.c_str());
        if (proc == nullptr)
        {
            mLastError = name + " : " + formatSystemError(::GetLastError());
            return nullptr;
        }

        return reinterpret_cast<void *>(proc);
    }

    //--------------------------------------------------------------------------

    String Win32SharedLibrary::getLastError() const
    {
        return mLastError;
    }

    //--------------------------------------------------------------------------

    THandle Win32SharedLibrary::getNativeHandle() const
    {
        return mModule;
    }

    //--------------------------------------------------------------------------

    String Win32SharedLibrary::queryModulePath(const void *address)
    {
        HMODULE module = moduleFromAddress(address);
        if (module == nullptr)
        {
            return String();
        }

        // 长路径可能超过 MAX_PATH，缓冲区不够时 GetModuleFileNameW 返回 size 并截断
        DWORD size = MAX_PATH;
        for (int attempt = 0; attempt < 8; ++attempt)
        {
            WString buffer((size_t)size, L'\0');
            const DWORD len = ::GetModuleFileNameW(module, &buffer[0], size);
            if (len == 0)
            {
                return String();
            }

            if (len < size)
            {
                return wideToUTF8(buffer.c_str(), len);
            }

            size *= 2;
        }

        return String();
    }

    //--------------------------------------------------------------------------

    bool Win32SharedLibrary::queryAddressMapped(const void *address)
    {
        return moduleFromAddress(address) != nullptr;
    }

    //--------------------------------------------------------------------------
}
