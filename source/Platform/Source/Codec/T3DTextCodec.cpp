/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "Codec/T3DTextCodec.h"
#include "IO/T3DDir.h"
#include <stdio.h>
#include <locale>
#include <codecvt>


namespace Tiny3D
{
    T3D_INIT_SINGLETON(TextCodec);

    TextCodec::TextCodec(const String &strCodecFile)
    {

    }

    TextCodec::~TextCodec()
    {

    }

    bool TextCodec::UTF8ToUTF16(const UTF8String &src, UTF16String &dst)
    {
#if _MSC_VER >= 1900
        std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> converter;
        std::basic_string<int16_t> w = converter.from_bytes(src);
        dst = reinterpret_cast<const char16_t*>(w.data());
#else
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
        dst = converter.from_bytes(src);
#endif
        return true;
    }

    bool TextCodec::UTF16ToUTF8(const UTF16String &src, UTF8String &dst)
    {
#if _MSC_VER >= 1900
        std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> converter;
        auto p = reinterpret_cast<const int16_t *>(src.data());
        converter.to_bytes(p, p + dst.size());
#else
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
        dst = converter.to_bytes(src);
#endif
        return true;
    }
}