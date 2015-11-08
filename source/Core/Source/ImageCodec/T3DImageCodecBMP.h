

#ifndef __T3D_IMAGE_CODEC_BMP_H__
#define __T3D_IMAGE_CODEC_BMP_H__


#include "ImageCodec/T3DImageCodec.h"


namespace Tiny3D
{
    class ImageCodecBMP : public ImageCodec
    {
    public:
        ImageCodecBMP();
        virtual ~ImageCodecBMP();

        virtual EType getType() const;

        virtual bool encode(const String &name, const Image &image);
        virtual bool encode(DataStream &stream, const Image &image);
        virtual bool encode(uint8_t *&data, size_t &size, const Image &image);

        virtual bool decode(const String &name, Image &image);
        virtual bool decode(DataStream &stream, Image &image);
        virtual bool decode(const uint8_t *data, size_t size, Image &image);
    };
}


#endif  /*__T3D_IMAGE_CODEC_BMP_H__*/
