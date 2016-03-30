

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

    class BlendInfo
    {
    public:
        BlendInfo()
            : mBlendIndex(-1)
            , mBlendWeight(0.0f)
        {

        }

        bool operator <(const BlendInfo &other) const
        {
            return mBlendWeight < other.mBlendWeight;
        }

        bool operator ==(const BlendInfo &other) const
        {
            return (mBlendWeight == other.mBlendWeight && mBlendIndex == other.mBlendIndex);
        }

        int     mBlendIndex;
        float   mBlendWeight;
    };

    typedef std::map<float, BlendInfo>      BlendInfoDict;
    typedef BlendInfoDict::iterator         BlendInfoDictItr;
    typedef BlendInfoDict::const_iterator   BlendInfoDictConstItr;
    typedef std::pair<float, BlendInfo>     BlendInfoValue;

    class Vertex
    {
    public:

        Vertex()
            : mCtrlPointIdx(0)
            , mPosition(0.0, 0.0, 0.0)
            , mMaterialIdx(-1)
        {

        }

        bool operator <(const Vertex &other) const
        {
            bool ret = mPosition < other.mPosition;
            ret = ret && (mTexElements.size() > 0 ? (mTexElements < other.mTexElements) : true);
            ret = ret && (mTexElements.size() > 0 ? (mNormalElements < other.mNormalElements) : true);
            ret = ret && (mBinormalElements.size() > 0 ? (mBinormalElements < other.mNormalElements) : true);
            ret = ret && (mTangentElements.size() > 0 ? (mTangentElements < other.mTangentElements) : true);
            ret = ret && (mColorElements.size() > 0 ? (mColorElements < other.mColorElements) : true);
            ret = ret && (mBlendInfo.size() > 0 ? (mBlendInfo < other.mBlendInfo) : true);
//             ret = ret && (mMaterialIdx < other.mMaterialIdx);
            return ret;
        }

        bool operator ==(const Vertex &other) const
        {
            bool ret = mPosition == other.mPosition;
            ret = ret && (mTexElements == other.mTexElements);
            ret = ret && (mNormalElements == other.mNormalElements);
            ret = ret && (mBinormalElements == other.mNormalElements);
            ret = ret && (mTangentElements == other.mTangentElements);
            ret = ret && (mColorElements == other.mColorElements);
            ret = ret && (mBlendInfo == other.mBlendInfo);
//             ret = ret && (mMaterialIdx == other.mMaterialIdx);
            return ret;
        }

        int                 mCtrlPointIdx;
        FbxVector3          mPosition;
        VectorElements2     mTexElements;
        VectorElements3     mNormalElements;
        VectorElements3     mBinormalElements;
        VectorElements3     mTangentElements;
        VectorElements4     mColorElements;
        BlendInfoDict       mBlendInfo;
        int                 mMaterialIdx;
    };

    typedef std::list<Vertex>           Vertices;
    typedef Vertices::iterator          VerticesItr;
    typedef Vertices::const_iterator    VerticesConstItr;
}


#endif  /*__MCONV_VERTEX_H__*/
