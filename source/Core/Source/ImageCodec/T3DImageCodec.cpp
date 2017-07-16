

#include "T3DImageCodec.h"
#include "T3DImageCodecBMP.h"
#include "T3DImageCodecPNG.h"
#include "T3DImageCodecJPG.h"
#include "T3DImageCodecTGA.h"
#include "T3DImageCodecDDS.h"
#include "T3DImageCodecFIMG.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(ImageCodec);
    
    const size_t ImageCodec::MAX_NUMBER_OF_CODEC = 1;

    ImageCodec::ImageCodec()
        : mCodecList(MAX_NUMBER_OF_CODEC)
    {
        size_t i = 0;
        mCodecList[i++] = ImageCodecFIMG::create();
    }

    ImageCodec::~ImageCodec()
    {

    }

    bool ImageCodec::startup()
    {
        bool ret = true;

        for (auto i = mCodecList.begin(); i != mCodecList.end(); ++i)
        {
            ImageCodecBasePtr codec = *i;
            ret = ret && codec->startup();

            if (ret)
            {
                if (codec->getFileType() == ImageCodecBase::E_FT_IMG)
                {
                    ImageCodecFIMGPtr codecIMG = smart_pointer_cast<ImageCodecFIMG>(codec);
                    ImageCodecFIMG::FileTypeList fileTypeList = codecIMG->getSupportedFileTypeList();

                    for (auto itr = fileTypeList.begin(); itr != fileTypeList.end(); ++itr)
                    {
                        mCodecMap.insert(ImageCodecMapValue(*itr, codec));
                    }
                }
                else
                {
                    mCodecMap.insert(ImageCodecMapValue(codec->getFileType(), codec));
                }
            }
        }

        return ret;
    }

    bool ImageCodec::shutdown()
    {
        bool ret = true;
        size_t i = 0;
        for (i = 0; i < mCodecList.size(); ++i)
        {
            ret = ret && mCodecList[i]->shutdown();
        }

        return ret;
    }

    bool ImageCodec::encode(const String &name, const Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_PNG */)
    {
        bool ret = false;

        return ret;
    }

    bool ImageCodec::encode(DataStream &stream, const Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_PNG */)
    {
        bool ret =false;

        return ret;
    }

    bool ImageCodec::encode(uint8_t *data, size_t size, const Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_PNG */)
    {
        bool ret = false;
        const ImageCodecBasePtr &codec = getImageCodec(eType);

        if (codec != nullptr)
        {
            ret = codec->encode(data, size, image);
        }

        return ret;
    }

    bool ImageCodec::decode(const String &name, Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_UNKNOWN */)
    {
        bool ret = false;
        ArchivePtr archive;

        if (T3D_ARCHIVE_MGR.getArchive(name, archive))
        {
            MemoryDataStream stream;
            if (archive->read(name, stream))
            {
                ret = decode(stream, image, eType);
            }
        }

        return ret;
    }

    bool ImageCodec::decode(DataStream &stream, Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_UNKNOWN */)
    {
        bool ret = false;

        uint8_t *data = nullptr;
        size_t size = stream.read(data);
        ret = decode(data, size, image, eType);

        return ret;
    }

    bool ImageCodec::decode(uint8_t *data, size_t size, Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_UNKNOWN */)
    {
        bool ret = false;

        ImageCodecBasePtr codec = getImageCodec(data, size, eType);

        if (codec != ImageCodecBasePtr::NULL_PTR)
        {
            ret = codec->decode(data, size, image, eType);
        }

        return ret;
    }

    ImageCodecBasePtr ImageCodec::getImageCodec(uint8_t *data, size_t size, ImageCodecBase::FileType &eFileType) const
    {
        ImageCodecBasePtr codec;

        if (eFileType != ImageCodecBase::E_FT_UNKNOWN)
        {
            auto itr = mCodecMap.find(eFileType);
            if (itr != mCodecMap.end())
            {
                codec = itr->second;
            }
        }

        if (codec == nullptr)
        {
            for (auto i = mCodecList.begin(); i != mCodecList.end(); ++i)
            {
                if ((*i)->isSupportedType(data, size, eFileType))
                {
                    codec = *i;
                    break;
                }
            }
        }

        return codec;
    }

    ImageCodecBasePtr ImageCodec::getImageCodec(ImageCodecBase::FileType eFileType) const
    {
        ImageCodecBasePtr codec;
        auto itr = mCodecMap.find(eFileType);
        if (itr != mCodecMap.end())
        {
            codec = itr->second;
        }
        return codec;
    }
}
