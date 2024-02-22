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


#include "T3DStringUtil.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    template<typename T>
    String StringConverter::toString(T val, uint16_t precision/* = 6*/,
        uint16_t width/* = 0*/, char fill/* = ' '*/,
        std::ios::fmtflags flags/* = std::ios::fmtflags(0)*/)
    {
        std::stringstream ss;
        ss.precision(precision);
        ss.width(width);
        ss.fill(fill);
        if (flags)
            ss.setf(flags);
        ss << val;
        return ss.str();
    }

    //--------------------------------------------------------------------------
    
    template<>
    inline String StringConverter::toString(bool val, uint16_t precision/* = 6*/,
        uint16_t width/* = 0*/, char fill/* = ' '*/,
        std::ios::fmtflags flags/* = std::ios::fmtflags(0)*/)
    {
        std::stringstream ss;
        ss << (val ? "true" : "false");
        return ss.str();
    }

    //--------------------------------------------------------------------------

    template<typename T>
    T StringConverter::parseValue(const String &val, T defaultValue)
    {
        std::stringstream ss(val);
        T ret = defaultValue;
        if (!(ss >> ret))
            return defaultValue;
        return ret;
    }

    //--------------------------------------------------------------------------

    template<>
    inline bool StringConverter::parseValue(const String &val, bool defaultValue)
    {
        if ((StringUtil::startsWith(val, "true") || StringUtil::startsWith(val, "yes")
            || StringUtil::startsWith(val, "1") || StringUtil::startsWith(val, "on")))
                return true;
        else if ((StringUtil::startsWith(val, "false") || StringUtil::startsWith(val, "no")
            || StringUtil::startsWith(val, "0") || StringUtil::startsWith(val, "off")))
                return false;
        else
            return defaultValue;
    }
    
    //--------------------------------------------------------------------------
}


