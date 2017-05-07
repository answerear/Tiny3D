
#ifndef __T3D_MODEL_SERIALIZER_H__
#define __T3D_MODEL_SERIALIZER_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class ModelSerializer
    {
    public:
        virtual ~ModelSerializer();

        virtual bool load(MemoryDataStream &stream, ModelDataPtr model) = 0;
        virtual bool save(MemoryDataStream &stream, ModelDataPtr model) = 0;
    };
}

#endif  /*__T3D_MODEL_SERIALIZER_H__*/