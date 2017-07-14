

#include "ImageCodec/T3DImageCodecFIMG.h"


namespace Tiny3D
{
    ImageCodecFIMGPtr ImageCodecFIMG::create()
    {
        ImageCodecFIMGPtr codec = new ImageCodecFIMG();
        codec->release();
        return codec;
    }

    ImageCodecFIMG::ImageCodecFIMG()
    {

    }

    ImageCodecFIMG::~ImageCodecFIMG()
    {

    }

    ImageCodecBase::FileType ImageCodecFIMG::getFileType() const
    {
        return E_TYPE_DDS;
    }

    bool ImageCodecFIMG::encode(const String &name, const Image &image)
    {
        return true;
    }

    bool ImageCodecFIMG::encode(DataStream &stream, const Image &image)
    {
        return true;
    }

    bool ImageCodecFIMG::encode(uint8_t *&data, size_t &size, const Image &image)
    {
        return true;
    }

    bool ImageCodecFIMG::decode(const String &name, Image &image)
    {
        return true;
    }

    bool ImageCodecFIMG::decode(DataStream &stream, Image &image)
    {
        return true;
    }

    bool ImageCodecFIMG::decode(const uint8_t *data, size_t size, Image &image)
    {
        return true;
    }
}

