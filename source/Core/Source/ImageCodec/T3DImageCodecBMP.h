

#ifndef __T3D_IMAGE_CODEC_BMP_H__
#define __T3D_IMAGE_CODEC_BMP_H__


#include "ImageCodec/T3DImageCodecBase.h"


namespace Tiny3D
{
    class ImageCodecBMP;

    T3D_DECLARE_SMART_PTR(ImageCodecBMP);

    class ImageCodecBMP : public ImageCodecBase
    {
    public:
        static ImageCodecBMPPtr create();

        virtual ~ImageCodecBMP();

        virtual FileType getFileType() const override;

        virtual bool encode(const String &name, const Image &image) override;
        virtual bool encode(DataStream &stream, const Image &image) override;
        virtual bool encode(uint8_t *&data, size_t &size, const Image &image) override;

        virtual bool decode(const String &name, Image &image) override;
        virtual bool decode(DataStream &stream, Image &image) override;
        virtual bool decode(const uint8_t *data, size_t size, Image &image) override;

    protected:
        ImageCodecBMP();
    };
}


#endif  /*__T3D_IMAGE_CODEC_BMP_H__*/
