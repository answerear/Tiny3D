

namespace Tiny3D
{
    inline VertexList::VertexFormat VertexList::getVertexFormat() const
    {
        return mVertexFormat;
    }

    inline uint32_t VertexList::getVertexSize() const
    {
        return mVertexSize;
    }

    inline uint32_t VertexList::getVertexCount() const
    {
        return mVertexCount;
    }

    inline char *VertexList::getVertices() const
    {
        return mVertices;
    }

    inline void VertexList::setVertices(uint32_t startIdx, uint32_t vertexCount, VertexP *vertices)
    {
        setVertices(startIdx, vertexCount, (char *)vertices);
    }

    inline void VertexList::setVertices(uint32_t startIdx, uint32_t vertexCount, VertexN *vertices)
    {
        setVertices(startIdx, vertexCount, (char *)vertices);
    }

    inline void VertexList::setVertices(uint32_t startIdx, uint32_t vertexCount, VertexT *vertices)
    {
        setVertices(startIdx, vertexCount, (char *)vertices);
    }

    inline void VertexList::setVertices(uint32_t startIdx, uint32_t vertexCount, VertexC *vertices)
    {
        setVertices(startIdx, vertexCount, (char *)vertices);
    }

    inline void VertexList::setVertices(uint32_t startIdx, uint32_t vertexCount, VertexNT *vertices)
    {
        setVertices(startIdx, vertexCount, (char *)vertices);
    }

    inline void VertexList::setVertices(uint32_t startIdx, uint32_t vertexCount, VertexNC *vertices)
    {
        setVertices(startIdx, vertexCount, (char *)vertices);
    }

    inline void VertexList::setVertices(uint32_t startIdx, uint32_t vertexCount, VertexTC *vertices)
    {
        setVertices(startIdx, vertexCount, (char *)vertices);
    }

    inline void VertexList::setVertices(uint32_t startIdx, uint32_t vertexCount, VertexNTC *vertices)
    {
        setVertices(startIdx, vertexCount, (char *)vertices);
    }
}
