

#ifndef __MCONV_SETTING_H__
#define __MCONV_SETTING_H__


#include "mconv_prerequisites.h"


namespace mconv
{
    class Settings
    {
    public:
        Settings()
            : mSrcType(E_FILETYPE_FBX)
            , mDstType(E_FILETYPE_T3D)
            , mBoundType(E_BT_AABB)
            , mFileMode(E_FM_SHARE_VERTEX)
            , mVerbose(true)
        {

        }

        String  mSrcPath;
        String  mDstPath;

        FileType    mSrcType;
        FileType    mDstType;

        BoundType   mBoundType;
        FileMode    mFileMode;

        bool    mVerbose;
    };
}


#endif  /*__MCONV_SETTING_H__*/