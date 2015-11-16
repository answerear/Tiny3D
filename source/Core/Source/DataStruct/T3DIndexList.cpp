

#include "DataStruct/T3DIndexList.h"


namespace Tiny3D
{
    IndexList::IndexList(IndexType type, uint32_t indexCount, char *indices /* = NULL */)
        : mIndexType(type)
        , mIndexSize(0)
        , mIndexCount(indexCount)
        , mIndices(indices)
        , mUserBuffer(indices != NULL)
    {
        switch (mIndexType)
        {
        case E_IT_16BITS:
            {
                mIndexSize = sizeof(uint16_t);
            }
            break;
        case E_IT_32BITS:
            {
                mIndexSize = sizeof(uint32_t);
            }
            break;
        default:
            break;
        }

        if (!mUserBuffer)
        {
            uint32_t bufSize = mIndexSize * mIndexCount;
            mIndices = new char[bufSize];
        }
    }

    IndexList::~IndexList()
    {
        if (!mUserBuffer)
        {
            T3D_SAFE_DELETE_ARRAY(mIndices);
        }
    }

    void IndexList::setIndices(uint32_t startIdx, uint32_t indexCount, char *indices)
    {
        T3D_ASSERT(startIdx < mIndexCount && startIdx + indexCount <= mIndexCount);

        uint32_t offset = startIdx * mIndexSize;
        uint32_t bufSize = indexCount * mIndexSize;
        uint32_t totalSize = mIndexSize * mIndexCount;

        T3D_ASSERT(offset + bufSize <= totalSize);

        memcpy(mIndices+offset, indices, bufSize);
    }
}
