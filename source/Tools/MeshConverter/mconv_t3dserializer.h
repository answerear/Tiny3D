

#ifndef __MCONV_T3DSERIALIZER_H__
#define __MCONV_T3DSERIALIZER_H__


#include "mconv_serializer.h"


namespace mconv
{
    class Node;

    class T3DBinSerializer : public Serializer
    {
    public:
        T3DBinSerializer();
        virtual ~T3DBinSerializer();

        virtual bool load(const String &path, void *&pData) override;
        virtual bool save(const String &path, void *pData) override;
    };

    class T3DXMLSerializer : public Serializer
    {
    public:
        T3DXMLSerializer();
        virtual ~T3DXMLSerializer();

        virtual bool load(const String &path, void *&pData) override;
        virtual bool save(const String &path, void *pData) override;
    };

    class T3DSerializer : public Serializer
    {
    public:
        T3DSerializer();
        virtual ~T3DSerializer();

        virtual bool load(const String &path, void *&pData) override;
        virtual bool save(const String &path, void *pData) override;

    protected:
        T3DBinSerializer    *mBinSerializer;
        T3DXMLSerializer    *mXMLSerializer;
    };
}


#endif  /*__MCONV_T3DSERIALIZER_H__*/
