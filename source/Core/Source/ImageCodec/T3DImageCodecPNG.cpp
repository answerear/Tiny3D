

#include "ImageCodec/T3DImageCodecPNG.h"


namespace Tiny3D
{
    ImageCodecPNG::ImageCodecPNG()
    {

    }

    ImageCodecPNG::~ImageCodecPNG()
    {

    }

    ImageCodec::EType ImageCodecPNG::getType() const
    {
        return E_TYPE_PNG;
    }

    bool ImageCodecPNG::encode(const String &name, const Image &image)
    {
        return true;
    }

    bool ImageCodecPNG::encode(DataStream &stream, const Image &image)
    {
        return true;
    }

    bool ImageCodecPNG::encode(uint8_t *&data, size_t &size, const Image &image)
    {
        return true;
    }

    bool ImageCodecPNG::decode(const String &name, Image &image)
    {
        return true;
    }

    bool ImageCodecPNG::decode(DataStream &stream, Image &image)
    {
        return true;
    }

    bool ImageCodecPNG::decode(const uint8_t *data, size_t size, Image &image)
    {
        return true;
    }
}
