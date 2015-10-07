

#include "T3DDataStream.h"


namespace Tiny3D
{
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

    DataStream &DataStream::operator<<(const TString &s)
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
        TString str;
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

    DataStream &DataStream::operator>>(TString &s)
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
