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

#include "Adapter/Unix/T3DPosixLocale.h"
#include <codecvt>
#include <locale>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    String PosixLocale::ANSIToUTF8(const String &src)
    {
        // On Unix/Android, ANSI is essentially UTF-8
        return src;
    }

    //--------------------------------------------------------------------------

    String PosixLocale::UTF8ToANSI(const String &src)
    {
        // On Unix/Android, ANSI is essentially UTF-8
        return src;
    }

    //--------------------------------------------------------------------------

    String PosixLocale::UnicodeToUTF8(const WString &src)
    {
        if (src.empty())
            return String();

        String result;
        
        for (wchar_t wc : src)
        {
            if (wc <= 0x7F)
            {
                result += (char)wc;
            }
            else if (wc <= 0x7FF)
            {
                result += (char)(0xC0 | ((wc >> 6) & 0x1F));
                result += (char)(0x80 | (wc & 0x3F));
            }
            else if (wc <= 0xFFFF)
            {
                result += (char)(0xE0 | ((wc >> 12) & 0x0F));
                result += (char)(0x80 | ((wc >> 6) & 0x3F));
                result += (char)(0x80 | (wc & 0x3F));
            }
            else if (wc <= 0x10FFFF)
            {
                result += (char)(0xF0 | ((wc >> 18) & 0x07));
                result += (char)(0x80 | ((wc >> 12) & 0x3F));
                result += (char)(0x80 | ((wc >> 6) & 0x3F));
                result += (char)(0x80 | (wc & 0x3F));
            }
        }

        return result;
    }

    //--------------------------------------------------------------------------

    WString PosixLocale::UTF8ToUnicode(const String &src)
    {
        if (src.empty())
            return WString();

        WString result;
        size_t i = 0;

        while (i < src.size())
        {
            uint32_t codepoint = 0;
            unsigned char c = (unsigned char)src[i];

            if (c <= 0x7F)
            {
                codepoint = c;
                i += 1;
            }
            else if ((c & 0xE0) == 0xC0)
            {
                codepoint = c & 0x1F;
                if (i + 1 < src.size())
                {
                    codepoint = (codepoint << 6) | ((unsigned char)src[i + 1] & 0x3F);
                }
                i += 2;
            }
            else if ((c & 0xF0) == 0xE0)
            {
                codepoint = c & 0x0F;
                if (i + 2 < src.size())
                {
                    codepoint = (codepoint << 6) | ((unsigned char)src[i + 1] & 0x3F);
                    codepoint = (codepoint << 6) | ((unsigned char)src[i + 2] & 0x3F);
                }
                i += 3;
            }
            else if ((c & 0xF8) == 0xF0)
            {
                codepoint = c & 0x07;
                if (i + 3 < src.size())
                {
                    codepoint = (codepoint << 6) | ((unsigned char)src[i + 1] & 0x3F);
                    codepoint = (codepoint << 6) | ((unsigned char)src[i + 2] & 0x3F);
                    codepoint = (codepoint << 6) | ((unsigned char)src[i + 3] & 0x3F);
                }
                i += 4;
            }
            else
            {
                // Invalid UTF-8, skip byte
                i += 1;
                continue;
            }

            result += (wchar_t)codepoint;
        }

        return result;
    }

    //--------------------------------------------------------------------------
}
