

#ifndef __T3D_MESH_H__
#define __T3D_MESH_H__


#include "Misc/T3DObject.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Mesh : public Object
    {
    public:
        Mesh();
        virtual ~Mesh();

        HardwareVertexBufferPtr getVertexBuffer();

    protected:
        HardwareVertexBufferPtr     mVertexBuffer;
    };
}


#endif  /*__T3D_MESH_H__*/
