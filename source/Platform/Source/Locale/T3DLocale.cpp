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

#include "Locale/T3DLocale.h"
#include "Adapter/T3DLocaleInterface.h"
#include "Adapter/T3DFactoryInterface.h"
#include "T3DPlatform.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Locale::Locale()
    {
        mLocale = T3D_PLATFORM_FACTORY.createPlatformLocale();
    }

    //--------------------------------------------------------------------------

    Locale::~Locale()
    {
        T3D_SAFE_DELETE(mLocale);
    }

    //--------------------------------------------------------------------------

    String Locale::ANSIToUTF8(const String &src)
    {
        if (mLocale != nullptr)
        {
            return mLocale->ANSIToUTF8(src);
        }

        return "";
    }

    //--------------------------------------------------------------------------
    
    String Locale::UTF8ToANSI(const String &src)
    {
        if (mLocale != nullptr)
        {
            return mLocale->UTF8ToANSI(src);
        }

        return "";
    }

    //--------------------------------------------------------------------------
    
    String Locale::UnicodeToUTF8(const WString &src)
    {
        if (mLocale != nullptr)
        {
            return mLocale->UnicodeToUTF8(src);
        }

        return "";
    }

    //--------------------------------------------------------------------------
    
    WString Locale::UTF8ToUnicode(const String &src)
    {
        if (mLocale != nullptr)
        {
            return mLocale->UTF8ToUnicode(src);
        }

        return L"";
    }
    
    //--------------------------------------------------------------------------
}
