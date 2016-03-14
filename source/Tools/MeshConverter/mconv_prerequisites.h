

#ifndef __MCONV_PREREQUISITES_H__
#define __MCONV_PREREQUISITES_H__


#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>

#include <fbxsdk.h>

#include <T3DLog.h>
#include <T3DPlatform.h>


namespace mconv
{
    enum FileType
    {
        E_FILETYPE_AUTO     = 0,        /// 根据扩展名自动判断文件格式
        E_FILETYPE_FBX      = 0x10,     /// FBX格式
        E_FILETYPE_DAE      = 0x20,     /// DAE格式
        E_FILETYPE_T3DB     = 0x40,     /// T3D二进制格式
        E_FILETYPE_T3DT     = 0x41,     /// T3D文本XML格式
        E_FILETYPE_T3D      = E_FILETYPE_T3DB|E_FILETYPE_T3DT,  /// T3D二进制和文本格式
    };
}



#endif  /*__MCONV_PREREQUISITES_H__*/
