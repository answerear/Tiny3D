
#ifndef __T3D_BIN_MODEL_SERIALIZER_H__
#define __T3D_BIN_MODEL_SERIALIZER_H__


#include "T3DModelSerializer.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DRenderer.h"

namespace Tiny3D
{
    class BinModelSerializer : public ModelSerializer
    {
    public:
        BinModelSerializer();
        virtual ~BinModelSerializer();

        virtual bool load(MemoryDataStream &stream, ModelDataPtr model) override;
        virtual bool save(MemoryDataStream &stream, ModelDataPtr model) override;
    };
}


#endif  /*__T3D_BIN_MODEL_SERIALIZER_H__*/
