/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
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

#ifndef __MCONV_STRING_H__
#define __MCONV_STRING_H__


#include "mconv_prerequisites.h"


namespace mconv
{
    typedef std::vector<String> StringVector;

    class StringUtil
    {
    public:
        static StringVector split(const String &str, const String &delims = " ", size_t maxSplits = 0);

        static void trim(String &str, bool left = true, bool right = true);

        static void toLowerCase(String &str);
        static void toUpperCase(String& str);

        static bool startsWith(const String& str, const String& pattern, bool lowerCase = true);
        static bool endsWith(const String& str, const String& pattern, bool lowerCase = true);

        static void replaceAll(String &str, const String &replaceWhat, const String &replaceWithWhat);
    };


    class StringConverter
    {
    public:
        static float parseReal(const String &val);

        static int parseInt(const String &val);

        static bool parseBool(const String& val);
    };
}


#endif  /*__MCONV_STRING_H__*/
