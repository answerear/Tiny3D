

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
            E_FT_UNKNOWN = -1,
            E_FT_BMP = 0,
            E_FT_ICO = 1,
            E_FT_JPEG = 2,
            E_FT_JNG = 3,
            E_FT_KOALA = 4,
            E_FT_LBM = 5,
            E_FT_IFF = E_FT_LBM,
            E_FT_MNG = 6,
            E_FT_PBM = 7,
            E_FT_PBMRAW = 8,
            E_FT_PCD = 9,
            E_FT_PCX = 10,
            E_FT_PGM = 11,
            E_FT_PGMRAW = 12,
            E_FT_PNG = 13,
            E_FT_PPM = 14,
            E_FT_PPMRAW = 15,
            E_FT_RAS = 16,
            E_FT_TARGA = 17,
            E_FT_TIFF = 18,
            E_FT_WBMP = 19,
            E_FT_PSD = 20,
            E_FT_CUT = 21,
            E_FT_XBM = 22,
            E_FT_XPM = 23,
            E_FT_DDS = 24,
            E_FT_GIF = 25,
            E_FT_HDR = 26,
            E_FT_FAXG3 = 27,
            E_FT_SGI = 28,
            E_FT_EXR = 29,
            E_FT_J2K = 30,
            E_FT_JP2 = 31,
            E_FT_PFM = 32,
            E_FT_PICT = 33,
            E_FT_RAW = 34,
            E_FT_WEBP = 35,
            E_FT_JXR = 36,
            E_FT_IMG = 37,
            MAX_TYPE_SUPPORTED,
        };

//         static const char * const FORMAT_BMP;
//         static const char * const FORMAT_PNG;
//         static const char * const FORMAT_JPG;
//         static const char * const FORMAT_TGA;
//         static const char * const FORMAT_DDS;

//         static bool isSupportedType(const String &name);
// 
//         static FileType getFileType(const String &name);

        virtual ~ImageCodecBase();

        virtual bool startup() = 0;
        virtual bool shutdown() = 0;

        virtual bool isSupportedType() const = 0;
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
