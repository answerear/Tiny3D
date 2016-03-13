

#include "mconv_t3dserializer.h"


namespace mconv
{
    T3DBinSerializer::T3DBinSerializer()
    {

    }

    T3DBinSerializer::~T3DBinSerializer()
    {

    }

    bool T3DBinSerializer::load(const String &path, void *&pData)
    {
        return true;
    }

    bool T3DBinSerializer::save(const String &path, void *pData)
    {
        return true;
    }

    T3DXMLSerializer::T3DXMLSerializer()
    {

    }

    T3DXMLSerializer::~T3DXMLSerializer()
    {

    }

    bool T3DXMLSerializer::load(const String &path, void *&pData)
    {
        return true;
    }

    bool T3DXMLSerializer::save(const String &path, void *pData)
    {
        return true;
    }

    T3DSerializer::T3DSerializer()
    {

    }

    T3DSerializer::~T3DSerializer()
    {

    }

    bool T3DSerializer::load(const String &path, void *&pData)
    {
        T3D_LOG_WARNING("Unable to load two different format files at the same time !");
        T3D_ASSERT(0);
        return true;
    }

    bool T3DSerializer::save(const String &path, void *pData)
    {
        return true;
    }
}
