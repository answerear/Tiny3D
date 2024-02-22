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

#include "IO/T3DMemoryDataStream.h"
#include <memory.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const uint32_t MemoryDataStream::MEMORY_BUFFER_SIZE = 2 * 1024;

    //--------------------------------------------------------------------------

    MemoryDataStream::MemoryDataStream()
        : MemoryDataStream(0)
    {

    }

    //--------------------------------------------------------------------------

    MemoryDataStream::MemoryDataStream(uchar_t *pBuffer, size_t unSize,
        bool reallocate /* = true */)
        : m_pBuffer(pBuffer)
        , m_lSize(unSize)
        , m_lCurPos(0)
        , m_bCreated(false)
    {
        if (reallocate)
        {
            m_pBuffer = new uint8_t[m_lSize];
            memcpy(m_pBuffer, pBuffer, m_lSize);
            m_bCreated = true;
        }

        m_lCapacity = unSize;
    }

    //--------------------------------------------------------------------------

    MemoryDataStream::MemoryDataStream(size_t unCapacity)
        : m_pBuffer(nullptr)
        , m_lSize(0)
        , m_lCurPos(0)
        , m_lCapacity(unCapacity)
        , m_bCreated(true)
    {
        if (unCapacity > 0)
        {
            m_pBuffer = new uchar_t[unCapacity];
        }
    }

    //--------------------------------------------------------------------------

    MemoryDataStream::MemoryDataStream(const MemoryDataStream &other)
    {
        copy(other);
    }

    //--------------------------------------------------------------------------

    MemoryDataStream::~MemoryDataStream()
    {
        if (m_bCreated)
        {
            T3D_SAFE_DELETE_ARRAY(m_pBuffer);
        }
    }

    //--------------------------------------------------------------------------

    MemoryDataStream &MemoryDataStream::operator=(const MemoryDataStream &other)
    {
        copy(other);
        return *this;
    }

    //--------------------------------------------------------------------------

    size_t MemoryDataStream::read(void *pBuffer, size_t nSize)
    {
        long_t lLeft = m_lCapacity - m_lCurPos;
        long_t lBytesOfRead = (long_t)nSize > lLeft ? lLeft : (long_t)nSize;
        if (lBytesOfRead == 1)
        {
            *(uint8_t*)pBuffer = *(m_pBuffer + m_lCurPos);
        }
        else
        {
            memcpy(pBuffer, m_pBuffer + m_lCurPos, lBytesOfRead);
        }
        m_lCurPos += lBytesOfRead;
        return lBytesOfRead;
    }

    //--------------------------------------------------------------------------

    size_t MemoryDataStream::write(void *pBuffer, size_t nSize)
    {
        long_t lSpace = m_lCapacity - m_lCurPos - 1;
        long_t lBytesOfWritten =
            (long_t)nSize > lSpace ? lSpace : (long_t)nSize;
        if (lBytesOfWritten == 1)
        {
            *(m_pBuffer + m_lCurPos) = *(uint8_t*)pBuffer;
        }
        else
        {
            memcpy(m_pBuffer + m_lCurPos, pBuffer, lBytesOfWritten);
        }
        m_lCurPos += lBytesOfWritten;
        m_lSize += lBytesOfWritten;
        return lBytesOfWritten;
    }

    //--------------------------------------------------------------------------

    bool MemoryDataStream::seek(long_t lPos, bool relative)
    {
        bool ret = false;

        if (relative)
        {
            if (lPos + m_lCurPos < m_lCapacity)
            {
                m_lCurPos += lPos;
                ret = true;
            }
        }
        else
        {
            if (lPos < m_lCapacity)
            {
                m_lCurPos = lPos;
                ret = true;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    long_t MemoryDataStream::tell() const
    {
        return m_lCurPos;
    }

    //--------------------------------------------------------------------------

    long_t MemoryDataStream::size() const
    {
        return m_lSize;
    }

    //--------------------------------------------------------------------------

    bool MemoryDataStream::eof() const
    {
        return (m_lCurPos == m_lCapacity);
    }

    //--------------------------------------------------------------------------

    size_t MemoryDataStream::read(uint8_t *&pData)
    {
        pData = m_pBuffer;
        return m_lSize;
    }

    //--------------------------------------------------------------------------

    void MemoryDataStream::setBuffer(uint8_t *buffer, size_t bufSize,
        bool reallocate /* = true */)
    {
        if (reallocate)
        {
            T3D_SAFE_DELETE_ARRAY(m_pBuffer);
            m_lSize = bufSize;
            m_pBuffer = new uint8_t[m_lSize];
            memcpy(m_pBuffer, buffer, m_lSize);
            m_lCapacity = bufSize;
        }
        else
        {
            m_lCapacity = bufSize;
            m_lSize = bufSize;
            m_pBuffer = buffer;
        }

        m_bCreated = true;
    }

    //--------------------------------------------------------------------------

    void MemoryDataStream::getBuffer(uint8_t *&buffer, size_t &bufSize) const
    {
        buffer = m_pBuffer;
        bufSize = m_lSize;
    }

    //--------------------------------------------------------------------------

    void MemoryDataStream::copy(const MemoryDataStream &other)
    {
        if (m_bCreated)
        {
            T3D_SAFE_DELETE_ARRAY(m_pBuffer);
            m_pBuffer = new uchar_t[other.m_lCapacity];
            memcpy(m_pBuffer, other.m_pBuffer, other.m_lCapacity);
        }
        else
        {
            m_pBuffer = other.m_pBuffer;
        }

        m_lSize = other.m_lSize;
        m_lCurPos = other.m_lCurPos;
        m_bCreated = other.m_bCreated;
        m_lCapacity = other.m_lCapacity;
    }
}
