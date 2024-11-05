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

#include "Adapter/Windows/T3DWin32Locale.h"
#include <windows.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Win32Locale::Win32Locale()
    {

    }

    //--------------------------------------------------------------------------

    Win32Locale::~Win32Locale()
    {
        
    }

    //--------------------------------------------------------------------------

    String Win32Locale::ANSIToUTF8(const String &src)
    {
        int wcsLen = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, nullptr, 0);
        wchar_t *pwcsBuf = new wchar_t[wcsLen + 1]; 
        ZeroMemory(pwcsBuf, wcsLen * sizeof(wchar_t));
        ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), (int)src.length(), pwcsBuf, wcsLen);
        
        int dstLen = ::WideCharToMultiByte(CP_UTF8, 0, pwcsBuf, -1, nullptr, 0, nullptr, nullptr);
        String dst(dstLen-1, 0);
        ::WideCharToMultiByte(CP_UTF8, 0, pwcsBuf, wcsLen, dst.data(), dstLen, nullptr, nullptr);

        T3D_SAFE_DELETE_ARRAY(pwcsBuf);
        
        return dst;
    }

    //--------------------------------------------------------------------------
    
    String Win32Locale::UTF8ToANSI(const String &src)
    {
        int wcsLen = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), (int)src.length(), nullptr, 0);
        wchar_t *wszString = new wchar_t[wcsLen + 1];
        ::MultiByteToWideChar(CP_UTF8, NULL, src.c_str(), (int)src.length(), wszString, wcsLen);
        wszString[wcsLen] = 0;

        int dstLen = ::WideCharToMultiByte(CP_ACP, 0, wszString, (int)wcslen(wszString), nullptr, 0, nullptr, nullptr);
        // String dst(dstLen-1, 0);
        char *szDst = new char[dstLen+1]; 
        ::WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), szDst, dstLen, nullptr, nullptr);
        szDst[dstLen] = 0;
        
        String dst = szDst;
        T3D_SAFE_DELETE_ARRAY(szDst);
        T3D_SAFE_DELETE_ARRAY(wszString);
        
        return dst;
    }

    //--------------------------------------------------------------------------
    
    String Win32Locale::UnicodeToUTF8(const WString &src)
    {
#if 0
        int dstLen = ::WideCharToMultiByte(0, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
        String dst(dstLen, 0);
        ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, dst.data(), dstLen, nullptr, nullptr);
        String ret = dst;
        return ret;
#else
        int dstLen = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
        char *szDst = new char[dstLen+1];
        int result = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, szDst, dstLen, nullptr, nullptr);
        szDst[dstLen] = 0;
        String dst = szDst;
        T3D_SAFE_DELETE_ARRAY(szDst);
        if (result > 0)
            return dst;
        return "";
#endif
    }

    //--------------------------------------------------------------------------
    
    WString Win32Locale::UTF8ToUnicode(const String &src)
    {
        int dstLen = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, nullptr, 0);
        wchar_t *wszDst = new wchar_t[dstLen + 1];
        ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, wszDst, dstLen);
        WString dst = wszDst;
        T3D_SAFE_DELETE_ARRAY(wszDst);
        return dst;
    }

    //--------------------------------------------------------------------------
}
