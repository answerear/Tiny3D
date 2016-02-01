
#ifndef __T3D_IMAGE_CODEC_PNG_H__
#define __T3D_IMAGE_CODEC_PNG_H__


#include "ImageCodec/T3DImageCodecBase.h"


namespace Tiny3D
{
    class ImageCodecPNG;

    T3D_DECLARE_SMART_PTR(ImageCodecPNG);

    class ImageCodecPNG : public ImageCodecBase
    {
    public:
        static ImageCodecPNGPtr create();

        virtual ~ImageCodecPNG();

        virtual FileType getFileType() const override;

        virtual bool encode(const String &name, const Image &image) override;
        virtual bool encode(DataStream &stream, const Image &image) override;
        virtual bool encode(uint8_t *&data, size_t &size, const Image &image) override;

        virtual bool decode(const String &name, Image &image) override;
        virtual bool decode(DataStream &stream, Image &image) override;
        virtual bool decode(const uint8_t *data, size_t size, Image &image) override;

    protected:
        ImageCodecPNG();
    };
}


#endif  /*__T3D_IMAGE_CODEC_PNG_H__*/

