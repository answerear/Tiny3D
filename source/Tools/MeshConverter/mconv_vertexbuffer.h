

#ifndef __MCONV_VERTEX_BUFFER_H__
#define __MCONV_VERTEX_BUFFER_H__


#include "mconv_node.h"
#include "mconv_vertex.h"


namespace mconv
{
    class VertexBuffer : public Node
    {
    public:
        VertexBuffer(const String &ID)
            : Node(ID)
        {

        }

        virtual ~VertexBuffer()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_VERTEX_BUFFER;
        }

        VertexAttributes    mAttributes;
        Vertices            mVertices;
    };

    class VertexBuffers : public Node
    {
    public:
        VertexBuffers(const String &ID)
            : Node(ID)
        {

        }

        virtual ~VertexBuffers()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_VERTEX_BUFFERS;
        }
    };
}

#endif  /*__MCONV_VERTEX_BUFFER_H__*/
