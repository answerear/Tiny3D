
#ifndef __T3D_IMAGE_CODEC_H__
#define __T3D_IMAGE_CODEC_H__


#include "ImageCodec/T3DImageCodecBase.h"


namespace Tiny3D
{
    class ImageCodec : public Singleton<ImageCodec>
    {
    public:
        ImageCodec();
        virtual ~ImageCodec();

        bool encode(const String &name, const Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_TYPE_PNG);
        bool encode(DataStream &stream, const Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_TYPE_PNG);
        bool encode(uint8_t *data, size_t &size, const Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_TYPE_PNG);

        bool decode(const String &name, Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_TYPE_UNKNOWN);
        bool decode(DataStream &stream, Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_TYPE_UNKNOWN);
        bool decode(uint8_t *data, size_t &size, Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_TYPE_UNKNOWN);

    protected:
        const ImageCodecBasePtr &getImageCodec(ImageCodecBase::FileType eFileType) const;

        ImageCodecBase::FileType getImageType(uint8_t *data, size_t size) const;

        typedef std::vector<ImageCodecBasePtr>  ImageCodecList;
        typedef ImageCodecList::iterator        ImageCodecListItr;
        typedef ImageCodecList::const_iterator  ImageCodecListConstItr;

        ImageCodecList  mCodecList;
    };

    #define T3D_IMAGE_CODEC     (ImageCodec::getInstance())
}




#endif  /*__T3D_IMAGE_CODEC_H__*/
