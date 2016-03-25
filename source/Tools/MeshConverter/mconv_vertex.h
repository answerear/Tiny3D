

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

        String vertexTypeStr() const;
        String dataTypeStr() const;

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
        Vertex()
            : mPosition(0.0, 0.0, 0.0)
            , mBlendWeight(-1.0, -1.0, -1.0, -1.0)
            , mBlendIndex(-1.0, -1.0, -1.0, -1.0)
            , mMaterialIdx(-1)
        {

        }

        bool operator <(const Vertex &other) const
        {
            bool ret = mPosition < other.mPosition;
            ret = ret && mTexElements < other.mTexElements;
            ret = ret && mNormalElements < other.mNormalElements;
            ret = ret && mBinormalElements < other.mNormalElements;
            ret = ret && mTangentElements < other.mTangentElements;
            ret = ret && mColorElements < other.mColorElements;
            ret = ret && mBlendWeight < other.mBlendWeight;
            ret = ret && mBlendIndex < other.mBlendIndex;
            ret = ret && mMaterialIdx < other.mMaterialIdx;
            return ret;
        }

        FbxVector3          mPosition;
        VectorElements2     mTexElements;
        VectorElements3     mNormalElements;
        VectorElements3     mBinormalElements;
        VectorElements3     mTangentElements;
        VectorElements4     mColorElements;
        FbxVector4          mBlendWeight;
        FbxVector4          mBlendIndex;
        int32_t             mMaterialIdx;
    };

    typedef std::list<Vertex>           Vertices;
    typedef Vertices::iterator          VerticesItr;
    typedef Vertices::const_iterator    VerticesConstItr;
}


#endif  /*__MCONV_VERTEX_H__*/
