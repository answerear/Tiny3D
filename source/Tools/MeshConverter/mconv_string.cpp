

#include "mconv_string.h"


namespace mconv
{
    StringVector StringUtil::split(const String &str, const String &delims/* = " "*/, size_t maxSplits/* = 0*/)
    {
        StringVector ret;
        // Pre-allocate some space for performance
        ret.reserve(maxSplits ? maxSplits + 1 : 10);    // 10 is guessed capacity for most case

        unsigned int numSplits = 0;

        // Use STL methods 
        size_t start, pos;
        start = 0;
        do
        {
            pos = str.find_first_of(delims, start);
            if (pos == start)
            {
                // Do nothing
                start = pos + 1;
            }
            else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
            {
                // Copy the rest of the string
                ret.push_back(str.substr(start));
                break;
            }
            else
            {
                // Copy up to delimiter
                ret.push_back(str.substr(start, pos - start));
                start = pos + 1;
            }
            // parse up to next real data
            start = str.find_first_not_of(delims, start);
            ++numSplits;

        } while (pos != String::npos);

        return ret;
    }

    void StringUtil::trim(String &str, bool left/* = true*/, bool right/* = true*/)
    {
        static const String delims = " \t\r\n";
        if (right)
        {
            size_t pos = str.find_last_not_of(delims);
            str.erase(pos + 1); // trim right
        }
        if (left)
        {
            size_t pos = str.find_first_not_of(delims);
            str.erase(0, pos); // trim left
        }
    }

    void StringUtil::toLowerCase(String &str)
    {
        std::transform(
            str.begin(),
            str.end(),
            str.begin(),
            tolower);
    }

    void StringUtil::toUpperCase(String& str)
    {
        std::transform(
            str.begin(),
            str.end(),
            str.begin(),
            toupper);
    }

    bool StringUtil::startsWith(const String& str, const String& pattern, bool lowerCase/* = true*/)
    {
        size_t thisLen = str.length();
        size_t patternLen = pattern.length();
        if (thisLen < patternLen || patternLen == 0)
            return false;

        String startOfThis = str.substr(0, patternLen);
        if (lowerCase)
            StringUtil::toLowerCase(startOfThis);

        return (startOfThis == pattern);
    }

    bool StringUtil::endsWith(const String& str, const String& pattern, bool lowerCase/* = true*/)
    {
        size_t thisLen = str.length();
        size_t patternLen = pattern.length();
        if (thisLen < patternLen || patternLen == 0)
            return false;

        String endOfThis = str.substr(thisLen - patternLen, patternLen);
        if (lowerCase)
            StringUtil::toLowerCase(endOfThis);

        return (endOfThis == pattern);
    }

    void StringUtil::replaceAll(String &str, const String &replaceWhat, const String &replaceWithWhat)
    {
        String::size_type pos = 0;

        while (1)
        {
            pos = str.find(replaceWhat, pos);
            if (pos == String::npos)
                break;
            str.replace(pos, 1, replaceWithWhat);
            pos++;
        }
    }


    float StringConverter::parseReal(const String &val)
    {
        std::istringstream str(val);
        Real ret = 0;
        str >> ret;
        return ret;
    }

    int StringConverter::parseInt(const String &val)
    {
        std::istringstream str(val);
        int ret = 0;
        str >> ret;
        return ret;
    }

    bool StringConverter::parseBool(const String& val)
    {
        return (StringUtil::startsWith(val, "true") || StringUtil::startsWith(val, "yes")
            || StringUtil::startsWith(val, "1"));
    }
}
