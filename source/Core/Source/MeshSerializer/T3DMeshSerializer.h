

#ifndef __T3D_MESH_SERIALIZER_H__
#define __T3D_MESH_SERIALIZER_H__


#include "T3DPrerequisites.h"
#include "T3DMeshSerializerBase.h"
#include "T3DObjMeshSerializer.h"


namespace Tiny3D
{
    class MeshSerializer : public Singleton<MeshSerializer>
    {
    public:
        MeshSerializer();
        virtual ~MeshSerializer();

        bool load(const String &name, const MeshPtr &mesh, MeshSerializerBase::FileType fileType);
        bool load(DataStream &stream, const MeshPtr &mesh, MeshSerializerBase::FileType fileType);
        bool load(uint8_t *data, size_t size, const MeshPtr &mesh, MeshSerializerBase::FileType fileType);

        bool save(const String &name, const MeshPtr &mesh, MeshSerializerBase::FileType fileType);
        bool save(DataStream &stream, const MeshPtr &mesh, MeshSerializerBase::FileType fileType);
        bool save(uint8_t *data, size_t size, const MeshPtr &mesh, MeshSerializerBase::FileType fileType);
    };

    #define T3D_MESH_SERIALIZER     (MeshSerializer::getInstance())
}


#endif  /*__T3D_MESH_SERIALIZER_H__*/
