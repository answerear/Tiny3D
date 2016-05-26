

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

    bool Image::copyToScaling(void *dstData, int32_t dstWidth, int32_t dstHeight, PixelFormat dstFormat, int32_t dstPitch, bool needFlip /* = false */) const
    {
        bool ret = false;

        if (dstFormat == mFormat && dstWidth == mWidth && dstHeight == mHeight)
        {
            if (needFlip)
            {
                uint8_t *dst = (uint8_t *)dstData;
                uint8_t *src = mData;
                int32_t bpp = (mFormat == E_PF_A8R8G8B8 ? 32 : 24) / 8;
                const uint32_t bwidth = dstWidth * bpp;
                const uint32_t rest = dstPitch - bwidth;
                dst = dst + dstPitch * (dstHeight - 1);
                for (int32_t y = 0; y < dstHeight; ++y)
                {
                    // copy scanline
                    memcpy(dst, src, bwidth);
                    // clear pitch
                    memset(dst+bwidth, 0, rest);
                    dst -= dstPitch;
                    src += mPitch;
                }
                ret = true;
            }
            else
            {
                if (dstPitch == mPitch)
                {
                    memcpy(dstData, mData, mDataSize);
                    ret = true;
                }
                else
                {
                    uint8_t *dst = (uint8_t *)dstData;
                    uint8_t *src = mData;
                    int32_t bpp = (mFormat == E_PF_A8R8G8B8 ? 32 : 24) / 8;
                    const uint32_t bwidth = dstWidth * bpp;
                    const uint32_t rest = dstPitch - bwidth;
                    for (int32_t y = 0; y < dstHeight; ++y)
                    {
                        // copy scanline
                        memcpy(dst, src, bwidth);
                        // clear pitch
                        memset(dst+bwidth, 0, rest);
                        dst += dstPitch;
                        src += mPitch;
                    }
                    ret = true;
                }
            }
        }
        else
        {
            int32_t bpp = 3;
            if (dstFormat == E_PF_A8R8G8B8 || dstFormat == E_PF_A8B8G8R8 || dstFormat == E_PF_X8B8G8R8 || dstFormat == E_PF_X8R8G8B8)
                bpp = 4;
            const float sourceXStep = (float)mWidth / (float)dstWidth;
            const float sourceYStep = (float)mHeight / (float)dstHeight;
            int32_t yval=0, syval=0;
            float sy = 0.0f;
            uint8_t *dst = (uint8_t *)dstData;
            if (needFlip)
            {
                yval = dstPitch * (dstHeight - 1);
                for (int32_t y = 0; y < dstHeight; ++y)
                {
                    float sx = 0.0f;
                    for (int32_t x = 0; x < dstWidth; ++x)
                    {
                        convertPixel(mData+syval+int32_t(sx)*mBPP, mFormat, dst+yval+x*bpp, dstFormat);
                        sx += sourceXStep;
                    }
                    sy += sourceYStep;
                    syval = ((int32_t)sy) * mPitch;
                    yval -= dstPitch;
                }
            }
            else
            {
                for (int32_t y = 0; y < dstHeight; ++y)
                {
                    float sx = 0.0f;
                    for (int32_t x = 0; x < dstWidth; ++x)
                    {
                        convertPixel(mData+syval+int32_t(sx)*mBPP, mFormat, dst+yval+x*bpp, dstFormat);
                        sx += sourceXStep;
                    }
                    sy += sourceYStep;
                    syval = ((int32_t)sy) * mPitch;
                    yval += dstPitch;
                }
            }

            ret = true;
        }

        return ret;
    }

    int32_t Image::compare(const Image &other, bool compareAlpha /* = true */) const
    {
        return 0;
    }

    void Image::copy(const Image &other)
    {

    }

    void Image::convertPixel(void *srcPixel, PixelFormat srcFmt, void *dstPixel, PixelFormat dstFmt) const
    {
        switch (srcFmt)
        {
        case E_PF_R8G8B8:
            {
                uint8_t *src = (uint8_t *)srcPixel;
                uint8_t b = *src++;
                uint8_t g = *src++;
                uint8_t r = *src++;

                if (dstFmt == E_PF_A8R8G8B8 || dstFmt == E_PF_X8R8G8B8)
                {
                    uint8_t *dst = (uint8_t *)dstPixel;
                    *dst++ = b & 0xFF;
                    *dst++ = (g & 0xFF);
                    *dst++ = (r & 0xFF);
                    *dst++ = 0xFF;
                }
                else if (dstFmt == E_PF_A8B8G8R8 || dstFmt == E_PF_X8B8G8R8)
                {
                    uint8_t *dst = (uint8_t *)dstPixel;
                    *dst++ = (r & 0xFF);
                    *dst++ = (g & 0xFF);
                    *dst++ = (b & 0xFF);
                    *dst++ = 0xFF;
                }
            }
            break;
        case E_PF_A8R8G8B8:
            {

            }
            break;
        }
    }
}
