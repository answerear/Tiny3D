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

#include "IO/T3DFileDataStream.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    FileDataStream::FileDataStream()
        : m_pFileHandle(nullptr)
        , m_lSize(0)
        , m_bIsOpened(false)
        , m_pData(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    FileDataStream::~FileDataStream()
    {
        T3D_SAFE_DELETE_ARRAY(m_pData);

        if (isOpened())
        {
            close();
        }
    }

    //--------------------------------------------------------------------------

    bool FileDataStream::open(const char *szFileName, uint32_t unMode)
    {
        if (unMode == E_MODE_READ_ONLY)
        {
            m_pFileHandle = fopen(szFileName, "rb");
        }
        else if (unMode == E_MODE_WRITE_ONLY)
        {
            m_pFileHandle = fopen(szFileName, "wb");
        }
        else if (unMode == (E_MODE_WRITE_ONLY|E_MODE_APPEND)
                 || unMode == E_MODE_APPEND)
        {
            m_pFileHandle = fopen(szFileName, "ab");
        }
        else if (unMode == (E_MODE_WRITE_ONLY|E_MODE_TRUNCATE)
            || unMode == E_MODE_TRUNCATE)
        {
            m_pFileHandle = fopen(szFileName, "wb");
        }
        else if (unMode == E_MODE_READ_WRITE)
        {
            m_pFileHandle = fopen(szFileName, "r+b");
        }
        else if (unMode == (E_MODE_READ_WRITE|E_MODE_TRUNCATE))
        {
            m_pFileHandle = fopen(szFileName, "w+b");
        }
        else if (unMode == (E_MODE_READ_WRITE|E_MODE_APPEND))
        {
            m_pFileHandle = fopen(szFileName, "a+b");
        }
        else if (unMode == (E_MODE_READ_ONLY|E_MODE_TEXT))
        {
            m_pFileHandle = fopen(szFileName, "r+t");
        }
        else if (unMode == (E_MODE_WRITE_ONLY|E_MODE_TEXT))
        {
            m_pFileHandle = fopen(szFileName, "w+t");
        }
        else if (unMode == (E_MODE_WRITE_ONLY|E_MODE_APPEND|E_MODE_TEXT)
                 || unMode == (E_MODE_APPEND|E_MODE_TEXT))
        {
            m_pFileHandle = fopen(szFileName, "a+t");
        }
        else if (unMode == (E_MODE_READ_WRITE|E_MODE_TEXT))
        {
            m_pFileHandle = fopen(szFileName, "r+t");
        }
        else if (unMode == (E_MODE_READ_WRITE|E_MODE_TRUNCATE|E_MODE_TEXT))
        {
            m_pFileHandle = fopen(szFileName, "w+t");
        }
        else if (unMode == (E_MODE_READ_WRITE|E_MODE_APPEND|E_MODE_TEXT))
        {
            m_pFileHandle = fopen(szFileName, "a+t");
        }

        m_lSize = 0;

        return (m_bIsOpened = (m_pFileHandle != nullptr));
    }

    void FileDataStream::close()
    {
        if (m_pFileHandle != nullptr)
        {
            fflush(m_pFileHandle);
            fclose(m_pFileHandle);
            m_pFileHandle = nullptr;
        }

        m_lSize = 0;
        m_bIsOpened = false;
    }

    //--------------------------------------------------------------------------

    size_t FileDataStream::read(void *pBuffer, size_t nSize)
    {
        size_t nBytesOfRead = 0;

        if (m_pFileHandle != nullptr)
        {
            nBytesOfRead = fread(pBuffer, 1, nSize, m_pFileHandle);
        }

        return nBytesOfRead;
    }

    //--------------------------------------------------------------------------

    size_t FileDataStream::write(void *pBuffer, size_t nSize)
    {
        size_t nBytesOfWritten = 0;

        if (m_pFileHandle != nullptr)
        {
            nBytesOfWritten = fwrite(pBuffer, 1, nSize, m_pFileHandle);
        }

        return nBytesOfWritten;
    }

    //--------------------------------------------------------------------------

    void FileDataStream::flush()
    {
        if (m_pFileHandle != nullptr)
        {
            fflush(m_pFileHandle);
        }
    }

    //--------------------------------------------------------------------------

    bool FileDataStream::seek(long_t lPos, bool relative)
    {
        bool ret = false;

        if (m_pFileHandle != nullptr)
        {
            if (relative)
            {
                if (fseek(m_pFileHandle, (long)lPos, SEEK_CUR) == 0)
                {
                    ret = true;
                }
            }
            else
            {
                if (fseek(m_pFileHandle, (long)lPos, SEEK_SET) == 0)
                {
                    ret = true;
                }
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    long_t FileDataStream::tell() const
    {
        long_t lPos = 0;

        if (m_pFileHandle != nullptr)
        {
            lPos = ftell(m_pFileHandle);
        }

        return lPos;
    }

    //--------------------------------------------------------------------------

    long_t FileDataStream::size() const
    {
        if (m_pFileHandle != nullptr)
        {
            if (m_lSize == 0)
            {
                long_t lPos = ftell(m_pFileHandle);
                fseek(m_pFileHandle, 0, SEEK_END);
                m_lSize = ftell(m_pFileHandle);
                fseek(m_pFileHandle, (long)lPos, SEEK_SET);
            }
        }

        return m_lSize;
    }

    //--------------------------------------------------------------------------

    bool FileDataStream::eof() const
    {
        bool bEnd = true;

        if (m_pFileHandle != nullptr)
        {
            long unPos = ftell(m_pFileHandle);
            long unSize = (long)size();
            bEnd = (unPos == unSize);
        }

        return bEnd;
    }

    //--------------------------------------------------------------------------

    size_t FileDataStream::read(uint8_t *&pData)
    {
        size_t bytesOfRead = 0;

        if (m_pFileHandle != nullptr)
        {
            long_t s = size();

            if (m_pData == nullptr)
            {
                m_pData = new uint8_t[s];
            }

            uint32_t pos = ftell(m_pFileHandle);
            fseek(m_pFileHandle, 0, SEEK_SET);
            bytesOfRead = fread(m_pData, 1, s, m_pFileHandle);
            fseek(m_pFileHandle, pos, SEEK_SET);
        }

        return bytesOfRead;
    }
}
