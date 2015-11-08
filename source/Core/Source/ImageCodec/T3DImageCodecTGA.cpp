

#include "ImageCodec/T3DImageCodecTGA.h"


namespace Tiny3D
{
    ImageCodecTGA::ImageCodecTGA()
    {

    }

    ImageCodecTGA::~ImageCodecTGA()
    {

    }

    ImageCodec::EType ImageCodecTGA::getType() const
    {
        return E_TYPE_TGA;
    }

    bool ImageCodecTGA::encode(const String &name, const Image &image)
    {
        return true;
    }

    bool ImageCodecTGA::encode(DataStream &stream, const Image &image)
    {
        return true;
    }

    bool ImageCodecTGA::encode(uint8_t *&data, size_t &size, const Image &image)
    {
        return true;
    }

    bool ImageCodecTGA::decode(const String &name, Image &image)
    {
        return true;
    }

    bool ImageCodecTGA::decode(DataStream &stream, Image &image)
    {
        return true;
    }

    bool ImageCodecTGA::decode(const uint8_t *data, size_t size, Image &image)
    {
        return true;
    }
}
