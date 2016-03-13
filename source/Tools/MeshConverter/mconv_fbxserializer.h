

#ifndef __MCONV_FBX_SERIALIZER_H__
#define __MCONV_FBX_SERIALIZER_H__


#include "mconv_serializer.h"


namespace mconv
{
    class FbxSerializer : public Serializer
    {
    public:
        FbxSerializer();
        virtual ~FbxSerializer();

        virtual bool load(const String &path, void *&pData) override;
        virtual bool save(const String &path, void *pData) override;

    protected:
        bool initFBXObjects();
        void destroyFBXObjects();

        FbxManager  *mFbxManager;
        FbxScene    *mFbxScene;
    };
}


#endif  /*__MCONV_FBX_SERIALIZER_H__*/
