

#ifndef __T3D_MESH_READER_BASE_H__
#define __T3D_MESH_READER_BASE_H__


#include "Misc/T3DObject.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class MeshReaderBase : Object
    {
    public:
        enum FileType
        {
            E_MESH_X = 0,
            E_MESH_OBJ,
            E_MESH_OGRE,
            E_MESH_MS3D,
        };

        MeshReaderBase();
        virtual ~MeshReaderBase();

        virtual FileType getFileType() const = 0;
        virtual bool load(const MeshPtr &mesh) = 0;
    };
}


#endif  /*__T3D_MESH_READER_BASE_H__*/
