

#ifndef __T3D_IMAGE_CODEC_BASE_H__
#define __T3D_IMAGE_CODEC_BASE_H__


#include "Misc/T3DObject.h"
#include "Misc/T3DCommon.h"
#include "Misc/T3DSmartPtr.h"


namespace Tiny3D
{
    class ImageCodecBase : public Object
    {
    public:
        enum FileType
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

        static FileType getFileType(const String &name);

        virtual ~ImageCodecBase();

        virtual FileType getFileType() const = 0;

        virtual bool encode(const String &name, const Image &image) = 0;
        virtual bool encode(DataStream &stream, const Image &image) = 0;
        virtual bool encode(uint8_t *&data, size_t &size, const Image &image) = 0;

        virtual bool decode(const String &name, Image &image) = 0;
        virtual bool decode(DataStream &stream, Image &image) = 0;
        virtual bool decode(const uint8_t *data, size_t size, Image &image) = 0;

    protected:
        ImageCodecBase();

        void setImageData(Image &image, uint8_t *data, size_t size);
        void setImageDimension(Image &image, int32_t width, int32_t height, int32_t pitch);
        void setImageInfo(Image &image, int32_t bpp, bool hasAlpha, bool isPreMulti, PixelFormat eFormat);
    };

    T3D_DECLARE_SMART_PTR(ImageCodecBase);
}


#endif  /*__T3D_IMAGE_CODEC_BASE_H__*/
