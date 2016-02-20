

#ifndef __T3D_MESH_SERIALIZER_BASE_H__
#define __T3D_MESH_SERIALIZER_BASE_H__


#include "Misc/T3DObject.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class MeshSerializerBase : Object
    {
    public:
        enum FileType
        {
            E_MESH_UNKNOWN = 0,
            E_MESH_X,
            E_MESH_OBJ,
            E_MESH_OGRE,
            E_MESH_MS3D,
            MAX_TYPE_SUPPORTED,
        };

        static const char * const FORMAT_X;
        static const char * const FORMAT_OBJ;

        virtual ~MeshSerializerBase();

        virtual FileType getFileType() const = 0;

        virtual bool load(const String &name, const MeshPtr &mesh) = 0;
        virtual bool load(DataStream &stream, const MeshPtr &mesh) = 0;
        virtual bool load(uint8_t *data, size_t &size, const MeshPtr &mesh) = 0;

        virtual bool save(const String &name, const MeshPtr &mesh) = 0;
        virtual bool save(DataStream &stream, const MeshPtr &mesh) = 0;
        virtual bool save(const uint8_t *data, size_t size, const MeshPtr &mesh) = 0;

    protected:
        MeshSerializerBase();
    };
}


#endif  /*__T3D_MESH_SERIALIZER_BASE_H__*/
