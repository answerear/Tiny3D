

#ifndef __MCONV_SUBMESH_H__
#define __MCONV_SUBMESH_H__


#include "mconv_node.h"


namespace mconv
{
    class SubMesh : public Node
    {
    public:
        SubMesh()
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
}

#endif  /*__MCONV_SUBMESH_H__*/
