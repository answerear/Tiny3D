

#ifndef __T3D_INDEX_LIST_H__
#define __T3D_INDEX_LIST_H__


#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class T3D_ENGINE_API IndexList : public Object
    {
    public:
        enum IndexType
        {
            E_IT_16BITS = 0,
            E_IT_32BITS,
        };

        IndexList(IndexType type, uint32_t indexCount, char *indices = NULL);
        ~IndexList();

        IndexType getIndexType() const;
        uint32_t getIndexSize() const;
        uint32_t getIndexCount() const;

        uint32_t getIndex(uint32_t idx) const;
        void setIndex(uint32_t idx, uint32_t index);

        char *getIndices() const;

        void setIndices(uint32_t startIdx, uint32_t indexCount, uint16_t *indices);
        void setIndices(uint32_t startIdx, uint32_t indexCount, uint32_t *indices);

    private:
        void setIndices(uint32_t startIdx, uint32_t indexCount, char *indices);

        IndexType   mIndexType;

        uint32_t    mIndexSize;
        uint32_t    mIndexCount;

        char        *mIndices;

        bool        mUserBuffer;
    };
}


#include "T3DIndexList.inl"


#endif  /*__T3D_INDEX_LIST_H__*/
