

#include "ImageCodec/T3DImageCodecDDS.h"


namespace Tiny3D
{
    ImageCodecDDSPtr ImageCodecDDS::create()
    {
        ImageCodecDDSPtr codec = new ImageCodecDDS();
        codec->release();
        return codec;
    }

    ImageCodecDDS::ImageCodecDDS()
    {

    }

    ImageCodecDDS::~ImageCodecDDS()
    {

    }

    ImageCodecBase::FileType ImageCodecDDS::getFileType() const
    {
        return E_TYPE_DDS;
    }

    bool ImageCodecDDS::encode(const String &name, const Image &image)
    {
        return true;
    }

    bool ImageCodecDDS::encode(DataStream &stream, const Image &image)
    {
        return true;
    }

    bool ImageCodecDDS::encode(uint8_t *&data, size_t &size, const Image &image)
    {
        return true;
    }

    bool ImageCodecDDS::decode(const String &name, Image &image)
    {
        return true;
    }

    bool ImageCodecDDS::decode(DataStream &stream, Image &image)
    {
        return true;
    }

    bool ImageCodecDDS::decode(const uint8_t *data, size_t size, Image &image)
    {
        return true;
    }
}

