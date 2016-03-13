

#ifndef __MCONV_SERIALIZER_H__
#define __MCONV_SERIALIZER_H__


#include "mconv_prerequisites.h"


namespace mconv
{
    class Serializer
    {
    public:
        virtual ~Serializer()   {}

        virtual bool load(const String &path, void *&pData) = 0;
        virtual bool save(const String &path, void *pData) = 0;
    };
}


#endif  /*__MCONV_SERIALIZER_H__*/
