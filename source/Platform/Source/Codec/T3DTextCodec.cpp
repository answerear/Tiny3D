

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