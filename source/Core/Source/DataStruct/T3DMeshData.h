

#ifndef __T3D_MESH_DATA_H__
#define __T3D_MESH_DATA_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisitesInternal.h"
#include "DataStruct/T3DVertexData.h"
#include "T3DTypedefInternal.h"
#include "Support/tinyxml2/tinyxml2.h"


namespace Tiny3D
{
    class MeshData : public Object
    {
    public:
        typedef std::vector<VertexElement>          VertexAttributes;
        typedef VertexAttributes::iterator          VertexAttributesItr;
        typedef VertexAttributes::const_iterator    VertexAttributesConstItr;

        typedef std::vector<uint8_t>        Vertices;
        typedef Vertices::iterator          VerticesItr;
        typedef Vertices::const_iterator    VerticesConstItr;

        static MeshDataPtr create(const VertexAttributes &attributes, const Vertices &vertices, size_t vertexSize);

        virtual ~MeshData();

        const VertexDataPtr &getVertexData() const
        {
            return mVertexData;
        }

    protected:
        MeshData();
        bool init(const VertexAttributes &attributes, const Vertices &vertices, size_t vertexSize);

    private:
        MeshData(const MeshData &);
        MeshData &operator =(const MeshData &);

    protected:
        VertexDataPtr           mVertexData;
    };
}


#endif  /*__T3D_MESH_DATA_H__*/
