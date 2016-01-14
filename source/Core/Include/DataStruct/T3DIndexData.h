
#ifndef __T3D_INDEX_DATA_H__
#define __T3D_INDEX_DATA_H__


#include "Misc/T3DObject.h"
#include "Render/T3DHardwareIndexBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API IndexData : public Object
    {
    public:
        static IndexDataPtr create(HardwareIndexBuffer *buffer);

        virtual ~IndexData();

        const HardwareIndexBufferPtr &getIndexBuffer() const { return mIndexBuffer; }

        IndexDataPtr clone(bool copyData = true) const;

    protected:
        IndexData(HardwareIndexBuffer *buffer);

    private:
        IndexData(const IndexData &);
        IndexData &operator =(const IndexData &);

    protected:
        HardwareIndexBufferPtr  mIndexBuffer;
    };
}


#endif  /*__T3D_INDEX_DATA_H__*/
