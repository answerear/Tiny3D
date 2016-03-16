

#ifndef __MCONV_VERTEX_H__
#define __MCONV_VERTEX_H__


#include "mconv_prerequisites.h"


namespace mconv
{
    class VertexAttribute
    {
    public:
        enum VertexType
        {
            E_VT_POSITION = 0,
            E_VT_TEXCOORD,
            E_VT_NORMAL,
            E_VT_TANGENT,
            E_VT_BINORMAL,
            E_VT_COLOR,
            E_VT_BLEND_WEIGHT,
            E_VT_BLEND_INDEX,
            E_MAX_VERTEX_TYPE,
        };

        enum DataType
        {
            E_VT_FLOAT = 0,
            E_VT_DOUBLE,
            E_VT_INT16,
            E_VT_INT32,
            E_VT_INT64,
            E_MAX_DATA_TYPE,
        };

        String toString(VertexType eVertexType) const;
        String toString(DataType eDataType) const;

        VertexType  mVertexType;
        int         mSize;
        DataType    mDataType;
    };

    typedef std::list<VertexAttribute>          VertexAttributes;
    typedef VertexAttributes::iterator          VertexAttributesItr;
    typedef VertexAttributes::const_iterator    VertexAttributesConstItr;

    class Vertex
    {
    public:
        FbxVector3          mPosition;
        VectorElements4     mColorElements;
        VectorElements2     mTexElements;
        VectorElements3     mNormalElements;
        VectorElements3     mBinormalElements;
        VectorElements3     mTangentElements;
        FbxVector4          mBlendWeight;
        FbxVector4          mBlendIndex;
    };

    typedef std::list<Vertex>           Vertices;
    typedef Vertices::iterator          VerticesItr;
    typedef Vertices::const_iterator    VerticesConstItr;
}


#endif  /*__MCONV_VERTEX_H__*/
