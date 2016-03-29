

#ifndef __MCONV_SUBMESH_H__
#define __MCONV_SUBMESH_H__


#include "mconv_node.h"


namespace mconv
{
    class SubMesh : public Node
    {
    public:
        SubMesh(const String &ID)
            : Node(ID)
        {

        }

        virtual ~SubMesh()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_SUBMESH;
        }

        String      mMaterialName;
        Indices     mIndices;
    };

    class SubMeshes : public Node
    {
    public:
        SubMeshes(const String &ID)
            : Node(ID)
        {

        }

        virtual ~SubMeshes()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_SUBMESHES;
        }
    };
}

#endif  /*__MCONV_SUBMESH_H__*/
