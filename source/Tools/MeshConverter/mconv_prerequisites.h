

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
#include <T3DVector2.h>
#include <T3DVector3.h>
#include <T3DVector4.h>
#include <T3DMatrix4.h>
#include <T3DQuaternion.h>


namespace mconv
{
    using namespace Tiny3D;

    #define T3D_MODEL_FILE_VER00000001          0x00000001
    #define T3D_MATERIAL_FILE_VER_00000001      0x00000001

    #define T3D_MODEL_FILE_VER_00000001_STR     "0.0.0.1"
    #define T3D_MATERIAL_FILE_VER_00000001_STR  "0.0.0.1"

    #define T3D_MODEL_FILE_VER_CUR              T3D_MODEL_FILE_VER00000001
    #define T3D_MODEL_FILE_VER_CUR_STR          T3D_MODEL_FILE_VER_00000001_STR

    #define T3D_MATERIAL_FILE_VER_CUR           T3D_MATERIAL_FILE_VER_00000001
    #define T3D_MATERIAL_FILE_VER_CUR_STR       T3D_MATERIAL_FILE_VER_00000001_STR

    #define T3D_BIN_MODEL_FILE_EXT              "tmb"
    #define T3D_TXT_MODEL_FILE_EXT              "tmt"

    #define T3D_BIN_MATERIAL_FILE_EXT           "mtb"
    #define T3D_TXT_MATERIAL_FILE_EXT           "mtt"

    enum FileType
    {
        E_FILETYPE_AUTO     = 0,        /// 根据扩展名自动判断文件格式
        E_FILETYPE_FBX      = 0x10,     /// FBX格式
        E_FILETYPE_DAE      = 0x20,     /// DAE格式
        E_FILETYPE_TMB      = 0x40,     /// T3D二进制格式
        E_FILETYPE_TMT      = 0x41,     /// T3D文本XML格式
        E_FILETYPE_OGRE     = 0x50,     /// Ogre文件格式
        E_FILETYPE_T3D      = E_FILETYPE_TMB|E_FILETYPE_TMT,  /// T3D二进制和文本格式
    };

    enum BoundType
    {
        E_BT_SPHERE = 0,
        E_BT_AABB,
    };

    enum FileMode
    {
        E_FM_SPLIT_MESH = 0,                /// fbx中有多个mesh的时候会自动按照mesh分离出多个文件
        E_FM_SHARE_VERTEX,                  /// fbx中有多个mesh的时候会自动合并成一个model，全部都在一个文件中，并且共享顶点缓存
        E_FM_MERGE_MESH,                    /// fbx中有多个mesh的时候会自动合并成一个model，全部都在一个文件中，每个mesh有其独立顶点缓存
    };

//     class FbxVector3 : public FbxDouble3
//     {
//     public:
//         FbxVector3()
//             : FbxDouble3()
//         {
// 
//         }
// 
//         FbxVector3(FbxDouble x, FbxDouble y, FbxDouble z)
//             : FbxDouble3(x, y, z)
//         {
// 
//         }
// 
//         FbxVector3(const FbxVector3 &other)
//         {
//             memcpy(mData, other.mData, sizeof(mData));
//         }
// 
//         FbxVector3 &operator =(const FbxVector3 &other)
//         {
//             memcpy(mData, other.mData, sizeof(mData));
//             return *this;
//         }
//     };

//     typedef std::list<FbxVector2>           VectorElements2;
    typedef std::list<Vector2>              VectorElements2;
    typedef VectorElements2::iterator       VectorElements2Itr;
    typedef VectorElements2::const_iterator VectorElements2ConstItr;

//     typedef std::list<FbxVector3>           VectorElements3;
    typedef std::list<Vector3>              VectorElements3;
    typedef VectorElements3::iterator       VectorElements3Itr;
    typedef VectorElements3::const_iterator VectorElements3ConstItr;

//     typedef std::list<FbxVector4>           VectorElements4;
    typedef std::list<Vector4>              VectorElements4;
    typedef VectorElements4::iterator       VectorElements4Itr;
    typedef VectorElements4::const_iterator VectorElements4ConstItr;

    typedef std::list<int>                  Indices;
    typedef Indices::iterator               IndicesItr;
    typedef Indices::const_iterator         IndicesConstItr;

    typedef std::list<uint16_t>             BlendIndices;
    typedef BlendIndices::iterator          BlendIndicesItr;
    typedef BlendIndices::const_iterator    BlendIndicesConstItr;

//     inline bool operator <(const FbxVector2 &v1, const FbxVector2 &v2)
//     {
//         return (memcmp(v1.Buffer(), v2.Buffer(), sizeof(v1.mData)) < 0);
//     }
// 
//     inline bool operator <(const FbxVector3 &v1, const FbxVector3 &v2)
//     {
//         return (memcmp(v1.Buffer(), v2.Buffer(), sizeof(v1.mData)) < 0);
//     }
// 
//     inline bool operator <(const FbxVector4 &v1, const FbxVector4 &v2)
//     {
//         return (memcmp(v1.Buffer(), v2.Buffer(), sizeof(v1.mData)) < 0);
//     }
}



#endif  /*__MCONV_PREREQUISITES_H__*/
