
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

        bool startup();
        bool shutdown();

        bool encode(const String &name, const Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_PNG);
        bool encode(DataStream &stream, const Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_PNG);
        bool encode(uint8_t *data, size_t size, const Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_PNG);

        bool decode(const String &name, Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_UNKNOWN);
        bool decode(DataStream &stream, Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_UNKNOWN);
        bool decode(uint8_t *data, size_t size, Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_UNKNOWN);

    protected:
        ImageCodecBasePtr getImageCodec(uint8_t *data, size_t size, ImageCodecBase::FileType &eFileType) const;
        ImageCodecBasePtr getImageCodec(ImageCodecBase::FileType eFileType) const;

        typedef std::vector<ImageCodecBasePtr>      ImageCodecList;
        typedef ImageCodecList::iterator            ImageCodecListItr;
        typedef ImageCodecList::const_iterator      ImageCodecListConstItr;
        typedef ImageCodecList::value_type          ImageCodecListValue;

        typedef std::map<ImageCodecBase::FileType, ImageCodecBasePtr>       ImageCodecMap;
        typedef ImageCodecMap::iterator                                     ImageCodecMapItr;
        typedef ImageCodecMap::const_iterator                               ImageCodecMapConstItr;
        typedef ImageCodecMap::value_type                                   ImageCodecMapValue;
        
        ImageCodecList  mCodecList;
        ImageCodecMap   mCodecMap;

        static const size_t MAX_NUMBER_OF_CODEC;
    };

    #define T3D_IMAGE_CODEC     (ImageCodec::getInstance())
}




#endif  /*__T3D_IMAGE_CODEC_H__*/
