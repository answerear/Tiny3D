

#ifndef __T3D_STRING_H__
#define __T3D_STRING_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
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


#endif  /*__T3D_STRING_H__*/
