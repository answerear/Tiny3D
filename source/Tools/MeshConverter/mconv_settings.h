

#ifndef __MCONV_SETTING_H__
#define __MCONV_SETTING_H__


#include "mconv_prerequisites.h"


namespace mconv
{
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