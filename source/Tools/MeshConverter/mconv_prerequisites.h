

#ifndef __MCONV_PREREQUISITES_H__
#define __MCONV_PREREQUISITES_H__


#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <sstream>
#include <fbxsdk.h>

#include <T3DLog.h>
#include <T3DPlatform.h>


namespace mconv
{
    using namespace Tiny3D;

    enum FileType
    {
        E_FILETYPE_AUTO     = 0,        /// 根据扩展名自动判断文件格式
        E_FILETYPE_FBX      = 0x10,     /// FBX格式
        E_FILETYPE_DAE      = 0x20,     /// DAE格式
        E_FILETYPE_T3DB     = 0x40,     /// T3D二进制格式
        E_FILETYPE_T3DT     = 0x41,     /// T3D文本XML格式
        E_FILETYPE_T3D      = E_FILETYPE_T3DB|E_FILETYPE_T3DT,  /// T3D二进制和文本格式
    };

//     typedef FbxDouble3 FbxVector3;
    class FbxVector3 : public FbxDouble3
    {
    public:
        FbxVector3()
            : FbxDouble3()
        {

        }

        FbxVector3(FbxDouble x, FbxDouble y, FbxDouble z)
            : FbxDouble3(x, y, z)
        {

        }

        FbxVector3(const FbxVector3 &other)
        {
            memcpy(mData, other.mData, sizeof(mData));
        }

        FbxVector3 &operator =(const FbxVector3 &other)
        {
            memcpy(mData, other.mData, sizeof(mData));
            return *this;
        }
    };

    typedef std::list<FbxVector2>           VectorElements2;
    typedef VectorElements2::iterator       VectorElements2Itr;
    typedef VectorElements2::const_iterator VectorElements2ConstItr;

    typedef std::list<FbxVector3>           VectorElements3;
    typedef VectorElements3::iterator       VectorElements3Itr;
    typedef VectorElements3::const_iterator VectorElements3ConstItr;

    typedef std::list<FbxVector4>           VectorElements4;
    typedef VectorElements4::iterator       VectorElements4Itr;
    typedef VectorElements4::const_iterator VectorElements4ConstItr;

    typedef std::list<int>                  Indices;
    typedef Indices::iterator               IndicesItr;
    typedef Indices::const_iterator         IndicesConstItr;

    typedef std::list<uint16_t>             BlendIndices;
    typedef BlendIndices::iterator          BlendIndicesItr;
    typedef BlendIndices::const_iterator    BlendIndicesConstItr;

    inline bool operator <(const FbxVector2 &v1, const FbxVector2 &v2)
    {
        return (memcmp(v1.Buffer(), v2.Buffer(), sizeof(v1.mData)) < 0);
    }

    inline bool operator <(const FbxVector3 &v1, const FbxVector3 &v2)
    {
        return (memcmp(v1.Buffer(), v2.Buffer(), sizeof(v1.mData)) < 0);
    }

    inline bool operator <(const FbxVector4 &v1, const FbxVector4 &v2)
    {
        return (memcmp(v1.Buffer(), v2.Buffer(), sizeof(v1.mData)) < 0);
    }
}



#endif  /*__MCONV_PREREQUISITES_H__*/
