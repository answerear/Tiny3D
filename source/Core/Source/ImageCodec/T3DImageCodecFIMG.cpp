

#include "ImageCodec/T3DImageCodecFIMG.h"
#include <FreeImage.h>
#include <sstream>
#include "Misc/T3DString.h"


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

    bool ImageCodecFIMG::startup()
    {
        FreeImage_Initialise(FALSE);

        T3D_LOG_INFO("FreeImage Version : %s", FreeImage_GetVersion());
        T3D_LOG_INFO("%s", FreeImage_GetCopyrightMessage());

        std::stringstream ss;
        ss << "Supported formats : ";
        bool first = true;
        int i = 0;
        for (i = 0; i < FreeImage_GetFIFCount(); ++i)
        {
            String exts(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));
            if (!first)
            {
                ss << ",";
            }
            first = false;
            ss << exts;

            StringVector extensions = StringUtil::split(exts, ",");
        }

        T3D_LOG_INFO("%s", ss.str().c_str());

        return true;
    }

    bool ImageCodecFIMG::shutdown()
    {
        FreeImage_DeInitialise();

        return true;
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

