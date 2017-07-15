

#ifndef __T3D_IMAGE_CODEC_FIMG_H__
#define __T3D_IMAGE_CODEC_FIMG_H__


#include "ImageCodec/T3DImageCodecBase.h"


namespace Tiny3D
{
    class ImageCodecFIMG;

    T3D_DECLARE_SMART_PTR(ImageCodecFIMG);

    class ImageCodecFIMG : public ImageCodecBase
    {
    public:
        typedef std::vector<ImageCodecBase::FileType>   FileTypeList;
        typedef FileTypeList::iterator                  FileTypeListItr;
        typedef FileTypeList::const_iterator            FileTypeListConstItr;

        static ImageCodecFIMGPtr create();

        virtual ~ImageCodecFIMG();

        const FileTypeList &getSupportedFileTypeList() const { return mFileTypeList; }

        virtual bool startup() override;
        virtual bool shutdown() override;

        virtual FileType getFileType() const override;

        virtual bool encode(const String &name, const Image &image) override;
        virtual bool encode(DataStream &stream, const Image &image) override;
        virtual bool encode(uint8_t *&data, size_t &size, const Image &image) override;

        virtual bool decode(const String &name, Image &image) override;
        virtual bool decode(DataStream &stream, Image &image) override;
        virtual bool decode(const uint8_t *data, size_t size, Image &image) override;

    protected:
        ImageCodecFIMG();

        FileTypeList        mFileTypeList;
    };
}


#endif  /*__T3D_IMAGE_CODEC_FIMG_H__*/
