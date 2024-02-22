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

#include "IO/T3DDataStream.h"
#include <string.h>
#include <memory.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

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
            else if (nBytes == 0)
            {
                break;
            }
        } while (c != '\n');

        if (trim)
        {
            str.erase(str.find_last_not_of(" \t\r\n") + 1); // trim right
            str.erase(0, str.find_first_not_of(" \t\r\n")); // trim left
        }

        return str;
    }

    //--------------------------------------------------------------------------

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

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(bool val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(int8_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(uint8_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(int16_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(uint16_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(int32_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(uint32_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(int64_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(uint64_t val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(float val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(double val)
    {
        write(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(const char *s)
    {
        size_t nLen = strlen(s);
        if (nLen > 0)
        {
            write((void*)s, nLen);
        }
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator<<(const String &s)
    {
        size_t nLength = s.length();
        if (nLength > 0)
        {
            write((void*)s.c_str(), nLength);
        }
        // char c = 0;
        // write(&c, sizeof(c));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::endl(DataStream& stream)
    {
        stream << "\n";
        return stream;
    }

    //--------------------------------------------------------------------------

    DataStream& DataStream::operator<<(StandardEndLine manip)
    {
        return endl(*this);
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator>>(bool &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator>>(int8_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator>>(uint8_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator>>(int16_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator>>(uint16_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator>>(int32_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator>>(uint32_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator>>(int64_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator>>(uint64_t &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator>>(float &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

    DataStream &DataStream::operator>>(double &val)
    {
        read(&val, sizeof(val));
        return *this;
    }

    //--------------------------------------------------------------------------

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

    //--------------------------------------------------------------------------

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
