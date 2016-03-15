

#ifndef __MCONV_VERTEX_H__
#define __MCONV_VERTEX_H__


#include "mconv_prerequisites.h"


namespace mconv
{
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
}


#endif  /*__MCONV_VERTEX_H__*/
