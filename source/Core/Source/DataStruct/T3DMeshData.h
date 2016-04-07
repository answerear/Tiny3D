

#ifndef __T3D_MESH_DATA_H__
#define __T3D_MESH_DATA_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisitesInternal.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "T3DTypedefInternal.h"
#include "Support/tinyxml2/tinyxml2.h"


namespace Tiny3D
{
    class MeshData : public Object
    {
    public:
        typedef std::list<VertexElement>            VertexAttributes;
        typedef VertexAttributes::iterator          VertexAttributesItr;
        typedef VertexAttributes::const_iterator    VertexAttributesConstItr;

        static MeshDataPtr create();

        virtual ~MeshData();

    protected:
        MeshData();

    private:
        MeshData(const MeshData &);
        MeshData &operator =(const MeshData &);

    public:
        VertexAttributes    mAttributes;
        float               *mVertices;
        size_t              mVertexSize;
        size_t              mVertexCount;
    };
}


#endif  /*__T3D_MESH_H__*/
