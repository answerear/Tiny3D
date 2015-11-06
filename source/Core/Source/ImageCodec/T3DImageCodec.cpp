

#include "T3DImageCodec.h"


namespace Tiny3D
{
    const char * const ImageCodec::FORMAT_BMP = "bmp";
    const char * const ImageCodec::FORMAT_PNG = "png";
    const char * const ImageCodec::FORMAT_JPG = "jpg";
    const char * const ImageCodec::FORMAT_TGA = "tga";
    const char * const ImageCodec::FORMAT_DDS = "dds";

    ImageCodec::ImageCodec()
    {

    }

    ImageCodec::~ImageCodec()
    {

    }

    bool ImageCodec::isSupportedType(const String &name)
    {
        EType type = getType(name);
        return (type != E_TYPE_UNKNOWN && type < MAX_TYPE_SUPPORTED);
    }

    ImageCodec::EType ImageCodec::getType(const String &name)
    {
        EType type = E_TYPE_UNKNOWN;
        String::size_type pos = name.rfind('.');
        if (pos != String::npos)
        {
            String ext = name.substr(pos+1);

            if (stricmp(ext.c_str(), FORMAT_BMP) == 0)
            {
                // BMP
                type = E_TYPE_BMP;
            }
            else if (stricmp(ext.c_str(), FORMAT_PNG) == 0)
            {
                // PNG
                type = E_TYPE_PNG;
            }
            else if (stricmp(ext.c_str(), FORMAT_JPG) == 0)
            {
                // JPG
                type = E_TYPE_JPG;
            }
            else if (stricmp(ext.c_str(), FORMAT_TGA) == 0)
            {
                // TGA
                type = E_TYPE_TGA;
            }
            else if (stricmp(ext.c_str(), FORMAT_DDS) == 0)
            {
                // DDS
                type = E_TYPE_DDS;
            }
        }

        return type;
    }
}
