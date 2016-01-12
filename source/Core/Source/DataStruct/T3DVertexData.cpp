

#include "DataStruct/T3DVertexData.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    VertexDataPtr VertexData::create(VertexDeclaration *decl, HardwareVertexBuffer *buffer)
    {
        VertexData *data = new VertexData(decl, buffer);
        VertexDataPtr ptr(data);
        data->release();
        return ptr;
    }

    VertexData::VertexData(VertexDeclaration *decl, HardwareVertexBuffer *buffer)
        : mDeclaration(decl)
        , mVertexBuffer(buffer)
    {

    }

    VertexData::~VertexData()
    {
        mVertexBuffer = nullptr;
    }

    VertexDataPtr VertexData::clone(bool copyData /* = true */) const
    {
        VertexDataPtr ptr;

        if (copyData)
        {
            HardwareVertexBufferPtr vertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(
                mVertexBuffer->getVertexSize(), mVertexBuffer->getVertexCount(), mVertexBuffer->getUsage(), mVertexBuffer->hasShadowBuffer());

            vertexBuffer->copyData((HardwareBufferPtr)mVertexBuffer);

            VertexDeclaration *decl = new VertexDeclaration();
            *decl = *mDeclaration;
            VertexData *data = new VertexData(decl, vertexBuffer);
            decl->release();

            ptr = data;
        }
        else
        {
            VertexData *data = new VertexData(mDeclaration, mVertexBuffer);
            ptr = data;
        }

        return ptr;
    }
}
