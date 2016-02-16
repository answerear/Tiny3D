
#include "IO/T3DMemoryDataStream.h"


namespace Tiny3D
{
    const uint32_t MemoryDataStream::MEMORY_BUFFER_SIZE = 2 * 1024;

    MemoryDataStream::MemoryDataStream()
        : m_pBuffer(nullptr)
        , m_lSize(0)
        , m_lCurPos(0)
        , m_bCreated(false)
    {

    }

    MemoryDataStream::MemoryDataStream(uchar_t *pBuffer, size_t unSize, bool reallocate /* = true */)
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
    }

    MemoryDataStream::MemoryDataStream(size_t unSize)
        : m_pBuffer(nullptr)
        , m_lSize(0)
        , m_lCurPos(0)
        , m_bCreated(true)
    {
        m_pBuffer = new uchar_t[unSize];
    }

    MemoryDataStream::MemoryDataStream(const MemoryDataStream &other)
    {
        copy(other);
    }

    MemoryDataStream::~MemoryDataStream()
    {
        if (m_bCreated)
        {
            T3D_SAFE_DELETE_ARRAY(m_pBuffer);
        }
    }

    MemoryDataStream &MemoryDataStream::operator=(const MemoryDataStream &other)
    {
        copy(other);
        return *this;
    }

    size_t MemoryDataStream::read(void *pBuffer, size_t nSize)
    {
        long_t lLeft = m_lSize - m_lCurPos;
        long_t lBytesOfRead = (long_t)nSize > lLeft ? lLeft : (long_t)nSize;
        memcpy(pBuffer, m_pBuffer + m_lCurPos, lBytesOfRead);
        m_lCurPos += lBytesOfRead;
        return lBytesOfRead;
    }

    size_t MemoryDataStream::write(void *pBuffer, size_t nSize)
    {
        long_t lSpace = m_lSize - m_lCurPos - 1;
        long_t lBytesOfWritten = (long_t)nSize > lSpace ? lSpace : (long_t)nSize;
        memcpy(m_pBuffer + m_lCurPos, pBuffer, lBytesOfWritten);
        m_lCurPos += lBytesOfWritten;
        return lBytesOfWritten;
    }

    void MemoryDataStream::seek(long_t lPos)
    {
        m_lCurPos = lPos;
    }

    long_t MemoryDataStream::tell() const
    {
        return m_lCurPos;
    }

    long_t MemoryDataStream::size() const
    {
        return m_lSize;
    }

    bool MemoryDataStream::eof() const
    {
        return (m_lCurPos == m_lSize - 1);
    }

    void MemoryDataStream::setBuffer(uint8_t *buffer, size_t bufSize, bool reallocate /* = true */)
    {
        if (reallocate)
        {
            T3D_SAFE_DELETE_ARRAY(m_pBuffer);
            m_lSize = bufSize;
            m_pBuffer = new uint8_t[m_lSize];
            memcpy(m_pBuffer, buffer, m_lSize);
        }
        else
        {
            m_lSize = bufSize;
            m_pBuffer = buffer;
        }

        m_bCreated = true;
    }

    void MemoryDataStream::getBuffer(uint8_t *&buffer, size_t &bufSize) const
    {
        buffer = m_pBuffer;
        bufSize = m_lSize;
    }

    void MemoryDataStream::copy(const MemoryDataStream &other)
    {
        if (m_bCreated)
        {
            T3D_SAFE_DELETE_ARRAY(m_pBuffer);
            m_pBuffer = new uchar_t[other.m_lSize];
            memcpy(m_pBuffer, other.m_pBuffer, other.m_lSize);
        }
        else
        {
            m_pBuffer = other.m_pBuffer;
        }

        m_lSize = other.m_lSize;
        m_lCurPos = other.m_lCurPos;
        m_bCreated = other.m_bCreated;
    }
}
