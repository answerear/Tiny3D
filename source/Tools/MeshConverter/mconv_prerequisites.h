/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

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

    #define T3D_BIN_MODEL_FILE_EXT              "t3b"
    #define T3D_TXT_MODEL_FILE_EXT              "t3t"

    #define T3D_BIN_MATERIAL_FILE_EXT           "t3b"
    #define T3D_TXT_MATERIAL_FILE_EXT           "t3t"

    #define T3D_SCENE_FILE_MAGIC                "TSCN"
    #define T3D_MODEL_FILE_MAGIC                "TMDL"
    #define T3D_MATERIAL_FILE_MAGIC             "TMTL"


    enum FileType
    {
        E_FILETYPE_AUTO     = 0,        /// 根据扩展名自动判断文件格式
        E_FILETYPE_FBX      = 0x10,     /// FBX格式
        E_FILETYPE_DAE      = 0x20,     /// DAE格式
        E_FILETYPE_T3B      = 0x40,     /// T3D二进制格式
        E_FILETYPE_T3T      = 0x41,     /// T3D文本XML格式
        E_FILETYPE_OGRE     = 0x50,     /// Ogre文件格式
        E_FILETYPE_T3D      = E_FILETYPE_T3B|E_FILETYPE_T3T,  /// T3D二进制和文本格式
    };

    enum BoundType
    {
        E_BT_SPHERE = 0,
        E_BT_AABB,
    };

    enum FileMode
    {
        E_FM_SHARE_VERTEX = 0,              /// fbx中有多个mesh的时候会自动合并成一个model，全部都在一个文件中，并且共享顶点缓存
        E_FM_ORIGINAL,                      /// 维持fbx中的原始结构，如果有多个mesh就多个mesh，只有一个mesh就一个mesh
    };

    typedef std::list<Vector2>              VectorElements2;
    typedef VectorElements2::iterator       VectorElements2Itr;
    typedef VectorElements2::const_iterator VectorElements2ConstItr;

    typedef std::list<Vector3>              VectorElements3;
    typedef VectorElements3::iterator       VectorElements3Itr;
    typedef VectorElements3::const_iterator VectorElements3ConstItr;

    typedef std::list<Vector4>              VectorElements4;
    typedef VectorElements4::iterator       VectorElements4Itr;
    typedef VectorElements4::const_iterator VectorElements4ConstItr;

    typedef std::list<int>                  Indices;
    typedef Indices::iterator               IndicesItr;
    typedef Indices::const_iterator         IndicesConstItr;

    typedef std::list<uint16_t>             BlendIndices;
    typedef BlendIndices::iterator          BlendIndicesItr;
    typedef BlendIndices::const_iterator    BlendIndicesConstItr;
}



#endif  /*__MCONV_PREREQUISITES_H__*/
