

#ifndef __T3D_IMAGE_CODEC_H__
#define __T3D_IMAGE_CODEC_H__


#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class Image;

    class ImageCodec : public Object
    {
    public:
        enum EType
        {
            E_TYPE_UNKNOWN = 0,
            E_TYPE_BMP,
            E_TYPE_PNG,
            E_TYPE_JPG,
            E_TYPE_TGA,
            E_TYPE_DDS,
            MAX_TYPE_SUPPORTED,
        };

        static const char * const FORMAT_BMP;
        static const char * const FORMAT_PNG;
        static const char * const FORMAT_JPG;
        static const char * const FORMAT_TGA;
        static const char * const FORMAT_DDS;

        static bool isSupportedType(const String &name);

        static EType getType(const String &name);

        ImageCodec();
        virtual ~ImageCodec();

        virtual EType getType() const = 0;

        virtual bool encode(const String &name, const Image &image) = 0;
        virtual bool encode(DataStream &stream, const Image &image) = 0;
        virtual bool encode(uint8_t *&data, size_t &size, const Image &image) = 0;

        virtual bool decode(const String &name, Image &image) = 0;
        virtual bool decode(DataStream &stream, Image &image) = 0;
        virtual bool decode(const uint8_t *data, size_t size, Image &image) = 0;
    };
}


#endif  /*__T3D_IMAGE_CODEC_H__*/
