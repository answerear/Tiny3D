

#ifndef __MCONV_MESH_H__
#define __MCONV_MESH_H__


#include "mconv_node.h"
#include "mconv_vertexbuffer.h"


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

        bool split()
        {
            bool ret = false;
            size_t count = getChildrenCount();

            for (size_t i = 0; i < count; ++i)
            {
                VertexBuffers *vbs = dynamic_cast<VertexBuffers *>(getChild(i));

                if (vbs != nullptr)
                {
                    ret = (vbs->getChildrenCount() > 0);

                    for (size_t j = 0; j < vbs->getChildrenCount(); ++j)
                    {
                        VertexBuffer *vb = dynamic_cast<VertexBuffer *>(vbs->getChild(j));
                        if (vb != nullptr)
                        {
                            ret = ret && vb->split();
                        }
                    }
                }
            }

            return ret;
        }

        Matrix4             mWorldMatrix;
        Matrix4             mGeometryMatrix;
    };
}


#endif  /*__MCONV_MESH_H__*/
