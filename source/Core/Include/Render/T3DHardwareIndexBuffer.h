

#ifndef __T3D_HARDWARE_INDEX_BUFFER_H__
#define __T3D_HARDWARE_INDEX_BUFFER_H__


#include "Render/T3DHardwareBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API HardwareIndexBuffer : public HardwareBuffer
    {
    public:
        enum Type
        {
            E_IT_16BITS = 0,
            E_IT_32BITS,
        };

        virtual ~HardwareIndexBuffer();

        Type getIndexType() const       { return mType; }
        size_t getIndexSize() const     { return mIndexSize; }
        size_t getIndexCount() const    { return mIndexCount; }

    protected:
        HardwareIndexBuffer(Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer);

        Type    mType;
        size_t  mIndexSize;
        size_t  mIndexCount;
    };
}


#endif  /*__T3D_HARDWARE_INDEX_BUFFER_H__*/
