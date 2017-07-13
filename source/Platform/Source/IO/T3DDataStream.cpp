

#include "IO/T3DDataStream.h"


namespace Tiny3D
{
    String DataStream::readLine(bool trim)
    {
        char c;
        size_t nNumberOfRead = 0;
        String str;

        do
        {
            size_t nBytes = read(&c, sizeof(c));
            if (nBytes > 0 && c != '\r' && c != '\n')
            {
                str += c;
                nNumberOfRead += nBytes;
            }
        } while (c != '\n');

        if (trim)
        {
            str.erase(str.find_last_not_of(" \t\r\n") + 1); // trim right
            str.erase(0, str.find_first_not_of(" \t\r\n")); // trim left
        }

        return str;
    }

    size_t DataStream::writeLine(const String &strLine)
    {
        size_t bytesOfWritten = 0;
        size_t length = strLine.find_first_not_of("\r\n");

        if (length > 0)
        {
            bytesOfWritten = write((void *)strLine.c_str(), length);
        }
        else if (length == String::npos)
        {
            length = strLine.length();
            bytesOfWritten = write((void *)strLine.c_str(), length);
        }

        return bytesOfWritten;
    }

    DataStream &DataStream::operator<<(bool val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator<<(int8_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator<<(uint8_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator<<(int16_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator<<(uint16_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator<<(int32_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator<<(uint32_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator<<(int64_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator<<(uint64_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator<<(float val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator<<(double val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator<<(const char *s)
    {
        size_t nLen = strlen(s);
        if (nLen > 0)
        {
            write((void*)s, nLen);
        }
        return *this;
    }

    DataStream &DataStream::operator<<(const String &s)
    {
        size_t nLength = s.length();
        if (nLength > 0)
        {
            write((void*)s.c_str(), nLength);
        }
        char c = 0;
        write(&c, sizeof(c));
        return *this;
    }


    DataStream &DataStream::operator>>(bool &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator>>(int8_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator>>(uint8_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator>>(int16_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator>>(uint16_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator>>(int32_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator>>(uint32_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator>>(int64_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator>>(uint64_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator>>(float &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator>>(double &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    DataStream &DataStream::operator>>(char *&s)
    {
        char c;
        String str;
        size_t nNumberOfRead = 0;
        size_t nSize = 0;

        do
        {
            size_t nBytes = read(&c, sizeof(c));
            str += c;
            nNumberOfRead += nBytes;
            nSize++;
        }
        while (!eof() && c != 0);

        s = new char[nSize+1];
        memcpy(s, str.c_str(), nSize);

        return *this;
    }

    DataStream &DataStream::operator>>(String &s)
    {
        char c;
        size_t nNumberOfRead = 0;
        s.clear();

        do
        {
            size_t nBytes = read(&c, sizeof(c));
            s += c;
            nNumberOfRead += nBytes;
        }
        while (!eof() && c != 0);

        return *this;
    }
}
