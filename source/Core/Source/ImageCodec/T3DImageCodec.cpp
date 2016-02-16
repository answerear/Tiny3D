

#include "T3DImageCodec.h"
#include "T3DImageCodecBMP.h"
#include "T3DImageCodecPNG.h"
#include "T3DImageCodecJPG.h"
#include "T3DImageCodecTGA.h"
#include "T3DImageCodecDDS.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(ImageCodec);

    ImageCodec::ImageCodec()
        : mCodecList(5)
    {
        mCodecList[0] = ImageCodecBMP::create();
        mCodecList[1] = ImageCodecPNG::create();
        mCodecList[2] = ImageCodecJPG::create();
        mCodecList[3] = ImageCodecTGA::create();
        mCodecList[4] = ImageCodecDDS::create();
    }

    ImageCodec::~ImageCodec()
    {

    }

    bool ImageCodec::encode(const String &name, const Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_TYPE_PNG */)
    {
        bool ret = false;
        const ImageCodecBasePtr &codec = getImageCodec(eType);

        if (codec != nullptr)
        {
            ret = codec->encode(name, image);
        }

        return ret;
    }

    bool ImageCodec::encode(DataStream &stream, const Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_TYPE_PNG */)
    {
        bool ret =false;
        const ImageCodecBasePtr &codec = getImageCodec(eType);

        if (codec != nullptr)
        {
            ret = codec->encode(stream, image);
        }

        return ret;
    }

    bool ImageCodec::encode(uint8_t *data, size_t &size, const Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_TYPE_PNG */)
    {
        bool ret = false;
        const ImageCodecBasePtr &codec = getImageCodec(eType);

        if (codec != nullptr)
        {
            ret = codec->encode(data, size, image);
        }

        return ret;
    }

    bool ImageCodec::decode(const String &name, Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_TYPE_UNKNOWN */)
    {
        bool ret = false;
        ArchivePtr archive;

        if (T3D_ARCHIVE_MGR.getArchive(name, archive))
        {
            MemoryDataStream stream;
            if (archive->read(name, stream))
            {
                ret = decode(stream, image);
            }
        }

        return ret;
    }

    bool ImageCodec::decode(DataStream &stream, Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_TYPE_UNKNOWN */)
    {
        bool ret = false;

        uint8_t *data = nullptr;
        size_t size = 0;

        size = stream.size();
        data = new uint8_t[size];
        stream.read(data, size);
        ret = decode(data, size, image, eType);
        T3D_SAFE_DELETE_ARRAY(data);

        return ret;
    }

    bool ImageCodec::decode(uint8_t *data, size_t &size, Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_TYPE_UNKNOWN */)
    {
        bool ret = false;

        if (eType == ImageCodecBase::E_TYPE_UNKNOWN)
        {
            eType = getImageType(data, size);
        }

        const ImageCodecBasePtr &codec = getImageCodec(eType);
        if (codec != ImageCodecBasePtr::NULL_PTR)
        {
            ret = codec->decode(data, size, image);
        }

        return ret;
    }

    const ImageCodecBasePtr &ImageCodec::getImageCodec(ImageCodecBase::FileType eType) const
    {
        ImageCodecListConstItr itr = mCodecList.begin();

        while (itr != mCodecList.end())
        {
            if ((*itr)->getFileType() == eType)
            {
                return *itr;
                break;
            }
            ++itr;
        }

        return ImageCodecBasePtr::NULL_PTR;
    }

    ImageCodecBase::FileType ImageCodec::getImageType(uint8_t *data, size_t size) const
    {
        ImageCodecBase::FileType eType = ImageCodecBase::E_TYPE_UNKNOWN;

        if (size > 8)
        {
            if (data[0] == 0x89 && data[1] == 0x50
                && data[2] == 0x4E && data[3] == 0x47
                && data[4] == 0x0D && data[5] == 0x0A
                && data[6] == 0x1A && data[7] == 0x0A)
            {
                // PNG
                eType = ImageCodecBase::E_TYPE_PNG;
            }
            else if (data[0] == 0x42 && data[1] == 0x4d)
            {
                // BMP
                eType = ImageCodecBase::E_TYPE_BMP;
            }
            else if (data[0] == 0xff && data[1] == 0xd8)
            {
                // JPG
                eType = ImageCodecBase::E_TYPE_JPG;
            }
            else if (data[0] == 0x44 && data[1] == 0x44 
                && data[2] == 0x53 && data[3] == 0x20)
            {
                // DDS
                eType = ImageCodecBase::E_TYPE_DDS;
            }
            else if ((data[0] == 0x49 && data[1] == 0x49)
                || (data[0] == 0x4d && data[1] == 0x4d))
            {
                // TIFF
            }
            else if (data[0] == 0 && data[1] == 0)
            {
                // TGA
                eType = ImageCodecBase::E_TYPE_TGA;
            }
        }

        return eType;
    }
}
