

#ifndef __MCONV_SETTING_H__
#define __MCONV_SETTING_H__


#include "mconv_prerequisites.h"


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

    class Settings
    {
    public:
        String  mSrcPath;
        String  mDstPath;

        FileType    mSrcType;
        FileType    mDstType;

        bool    mVerbose;
    };
}


#endif  /*__MCONV_SETTING_H__*/