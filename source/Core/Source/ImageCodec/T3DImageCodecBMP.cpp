

#include "ImageCodec/T3DImageCodecBMP.h"


namespace Tiny3D
{
    ImageCodecBMP::ImageCodecBMP()
    {

    }

    ImageCodecBMP::~ImageCodecBMP()
    {

    }

    ImageCodec::EType ImageCodecBMP::getType() const
    {
        return E_TYPE_BMP;
    }

    bool ImageCodecBMP::encode(const String &name, const Image &image)
    {
        return true;
    }

    bool ImageCodecBMP::encode(DataStream &stream, const Image &image)
    {
        return true;
    }

    bool ImageCodecBMP::encode(uint8_t *&data, size_t &size, const Image &image)
    {
        return true;
    }

    bool ImageCodecBMP::decode(const String &name, Image &image)
    {
        return true;
    }

    bool ImageCodecBMP::decode(DataStream &stream, Image &image)
    {
        return true;
    }

    bool ImageCodecBMP::decode(const uint8_t *data, size_t size, Image &image)
    {
        return true;
    }
}
