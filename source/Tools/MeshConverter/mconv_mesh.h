

#ifndef __MCONV_MESH_H__
#define __MCONV_MESH_H__


#include "mconv_node.h"
#include "mconv_vertex.h"


namespace mconv
{
    class Mesh : public Node
    {
    public:
        Mesh(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Mesh()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_MESH;
        }

        Vertex  mVertices;
    };
}


#endif  /*__MCONV_MESH_H__*/
