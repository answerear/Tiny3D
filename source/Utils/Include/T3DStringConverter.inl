/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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


