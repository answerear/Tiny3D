

namespace Tiny3D
{
    inline IndexList::IndexType IndexList::getIndexType() const
    {
        return mIndexType;
    }

    inline uint32_t IndexList::getIndexSize() const
    {
        return mIndexSize;
    }

    inline uint32_t IndexList::getIndexCount() const
    {
        return mIndexCount;
    }

    inline uint32_t IndexList::getIndex(uint32_t idx) const
    {
        if (mIndexType == E_IT_16BITS)
        {
            return ((uint16_t*)mIndices)[idx];
        }

        return ((uint32_t*)mIndices)[idx];
    }

    inline void IndexList::setIndex(uint32_t idx, uint32_t index)
    {
        if (mIndexType == E_IT_16BITS)
        {
            ((uint16_t*)mIndices)[idx] = (uint16_t)index;
        }
        else
        {
            ((uint32_t*)mIndices)[idx] = index;
        }
    }

    inline char *IndexList::getIndices() const
    {
        return mIndices;
    }

    inline void IndexList::setIndices(uint32_t startIdx, uint32_t indexCount, uint16_t *indices)
    {
        setIndices(startIdx, indexCount, (char *)indices);
    }

    inline void IndexList::setIndices(uint32_t startIdx, uint32_t indexCount, uint32_t *indices)
    {
        setIndices(startIdx, indexCount, (char *)indices);
    }
}
