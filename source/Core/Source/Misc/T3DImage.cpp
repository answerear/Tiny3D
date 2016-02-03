

#include "Misc/T3DImage.h"
#include "ImageCodec/T3DImageCodec.h"
#include "Resource/T3DArchiveManager.h"


namespace Tiny3D
{
    const char * const Image::FILETYPE_BMP = "bmp";
    const char * const Image::FILETYPE_PNG = "png";
    const char * const Image::FILETYPE_TGA = "tga";
    const char * const Image::FILETYPE_JPG = "jpg";
    const char * const Image::FILETYPE_DDS = "dds";

    Image::Image()
        : mWidth(0)
        , mHeight(0)
        , mBPP(0)
        , mPitch(0)
        , mFormat(E_PF_A8R8G8B8)
        , mHasAlpha(false)
        , mIsPreMulti(false)
        , mData(nullptr)
        , mDataSize(0)
    {

    }

    Image::Image(int32_t width, int32_t height)
    {
        
    }

    Image::~Image()
    {
        T3D_SAFE_DELETE_ARRAY(mData);
    }

    bool Image::load(const String &name)
    {
        return T3D_IMAGE_CODEC.decode(name, *this);
    }

    bool Image::save(const String &name, const String &fileType /* = FILETYPE_PNG */) const
    {
        ImageCodecBase::FileType eType = ImageCodecBase::E_TYPE_PNG;

        size_t len = fileType.size() > 3 ? 3 : fileType.size();
        if (strnicmp(fileType.c_str(), FILETYPE_JPG, len) == 0)
        {
            eType = ImageCodecBase::E_TYPE_JPG;
        }
        else if (strnicmp(fileType.c_str(), FILETYPE_BMP, len) == 0)
        {
            eType = ImageCodecBase::E_TYPE_BMP;
        }
        else if (strnicmp(fileType.c_str(), FILETYPE_TGA, len) == 0)
        {
            eType = ImageCodecBase::E_TYPE_TGA;
        }
        else if (strnicmp(fileType.c_str(), FILETYPE_DDS, len) == 0)
        {
            eType = ImageCodecBase::E_TYPE_DDS;
        }

        return T3D_IMAGE_CODEC.encode(name, *this, eType);
    }

    bool Image::flip()
    {
        return true;
    }

    bool Image::mirror()
    {
        return true;
    }

    bool Image::fill(const Color4 &rkColor)
    {
        return true;
    }

    int32_t Image::compare(const Image &other, bool compareAlpha /* = true */) const
    {
        return 0;
    }

    void Image::copy(const Image &other)
    {

    }
}
