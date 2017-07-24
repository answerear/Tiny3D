
#include "Render/T3DIndexData.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    IndexDataPtr IndexData::create(HardwareIndexBuffer *buffer)
    {
        IndexData *data = new IndexData(buffer);
        IndexDataPtr ptr(data);
        data->release();
        return ptr;
    }

    IndexData::IndexData(HardwareIndexBuffer *buffer)
        : mIndexBuffer(buffer)
    {

    }

    IndexData::~IndexData()
    {
        mIndexBuffer = nullptr;
    }

    IndexDataPtr IndexData::clone(bool copyData /* = true */) const
    {
        IndexDataPtr ptr;

        if (copyData)
        {
            HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(
                mIndexBuffer->getIndexType(), mIndexBuffer->getIndexCount(), 
                mIndexBuffer->getUsage(), mIndexBuffer->hasShadowBuffer());

            indexBuffer->copyData((HardwareBufferPtr)mIndexBuffer);
            
            IndexData *data = new IndexData(indexBuffer);
            ptr = data;
        }
        else
        {
            IndexData *data = new IndexData(mIndexBuffer);
            ptr = data;
        }
        
        return ptr;
    }
}
