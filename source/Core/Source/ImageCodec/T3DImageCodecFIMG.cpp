

#include "ImageCodec/T3DImageCodecFIMG.h"
#include <FreeImage.h>
#include <sstream>
#include "Misc/T3DString.h"
#include "Misc/T3DColor4.h"


namespace Tiny3D
{
    const size_t ImageCodecFIMG::MAX_SUPPORTED_FILE_TYPE = 37;

    void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
    {
        if (fif != FIF_UNKNOWN)
        {
            T3D_LOG_ERROR("%s Format", FreeImage_GetFormatFromFIF(fif));
        }

        T3D_LOG_ERROR(message);
    }

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

        FreeImage_SetOutputMessage(FreeImageErrorHandler);

        mFileTypeList.resize(MAX_SUPPORTED_FILE_TYPE);
        for (i = 0; i < mFileTypeList.size(); ++i)
        {
            mFileTypeList[i] = (ImageCodecBase::FileType)i;
        }

        return true;
    }

    bool ImageCodecFIMG::shutdown()
    {
        FreeImage_DeInitialise();

        return true;
    }

    bool ImageCodecFIMG::isSupportedType(uint8_t *data, size_t size, FileType &eFileType) const
    {
        bool ret = false;

        FIMEMORY* fiMem = FreeImage_OpenMemory(data, size);

        FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(fiMem, size);
        FreeImage_CloseMemory(fiMem);

        eFileType = (FileType)fif;

        if (fif != FIF_UNKNOWN)
        {
            ret = true;
        }

        return ret;
    }

    ImageCodecBase::FileType ImageCodecFIMG::getFileType() const
    {
        return E_FT_IMG;
    }

    bool ImageCodecFIMG::encode(uint8_t *&data, size_t &size, const Image &image)
    {
        return true;
    }

    bool ImageCodecFIMG::decode(uint8_t *data, size_t size, Image &image, FileType eFileType)
    {
        bool ret = false;
        FIMEMORY *stream = FreeImage_OpenMemory(data, size);

        FREE_IMAGE_FORMAT fif = (FREE_IMAGE_FORMAT)eFileType;
        FIBITMAP *dib = FreeImage_LoadFromMemory(fif, stream);

        if (dib == nullptr)
        {
            FreeImage_CloseMemory(stream);
            T3D_LOG_ERROR("Error decoding image !");
            return false;
        }

        uint32_t width = FreeImage_GetWidth(dib);
        uint32_t height = FreeImage_GetHeight(dib);
        int32_t bpp = FreeImage_GetBPP(dib);

        FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(dib);
        FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(dib);
        PixelFormat eFormat = E_PF_A8R8G8B8;

        switch (imageType)
        {
        case FIT_BITMAP:
            {
                if (colorType == FIC_MINISWHITE || colorType == FIC_MINISBLACK)
                {
                    FIBITMAP *newBitmap = FreeImage_ConvertToGreyscale(dib);
                    FreeImage_Unload(dib);
                    dib = newBitmap;
                    bpp = FreeImage_GetBPP(dib);
                }
                else if (bpp < 8 || colorType == FIC_PALETTE || colorType == FIC_CMYK)
                {
                    FIBITMAP *newBitmap = nullptr;
                    if (FreeImage_IsTransparent(dib))
                    {
                        // 把带透明通道，转成32位
                        newBitmap = FreeImage_ConvertTo32Bits(dib);
                    }
                    else
                    {
                        // 不带透明通道，直接转成24位
                        newBitmap = FreeImage_ConvertTo24Bits(dib);
                    }

                    FreeImage_Unload(dib);
                    dib = newBitmap;
                    bpp = FreeImage_GetBPP(dib);
                }

                ret = true;
            }
            break;
        default:
            break;
        }

        if (ret)
        {
            bool hasAlpha = false;
            bool isPreMulti = false;

            int32_t srcPitch = FreeImage_GetPitch(dib);
            uint8_t *src = FreeImage_GetBits(dib);

            int32_t dstPitch = srcPitch;
            size_t imageSize = dstPitch * height;
            uint8_t *dst = new uint8_t[imageSize];

            switch (bpp)
            {
            case 8:
                {
                    eFormat = E_PF_PALETTE8;
                }
                break;
            case 16:
                {
                    if (FreeImage_GetGreenMask(dib) == FI16_565_GREEN_MASK)
                    {
                        eFormat = E_PF_R5G6B5;
                    }
                    else
                    {
                        eFormat = E_PF_A1R5G5B5;
                    }
                }
                break;
            case 24:
                {
                    eFormat = E_PF_R8G8B8;
                }
                break;
            case 32:
                {
                    eFormat = E_PF_A8R8G8B8;
                    hasAlpha = true;
                }
                break;
            }

            int32_t y = 0;
            uint8_t *pDst = dst;

            if (eFileType == E_FT_DDS)
            {
                // DDS要翻转一次，因为DDS原点在左上角
                for (y = 0; y < height; ++y)
                {
                    uint8_t *pSrc = src + (height - y - 1) * srcPitch;
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
                    Color4::convert_B8G8R8A8toA8R8G8B8(pSrc, pDst, width);
#elif FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
                    memcpy(pDst, pSrc, dstPitch);
#endif
                    pDst += dstPitch;
                }
            }
            else
            {
                for (y = 0; y < height; ++y)
                {
                    uint8_t *pSrc = src + y * srcPitch;
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
                    Color4::convert_B8G8R8A8toA8R8G8B8(pSrc, pDst, width);
#elif FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
                    memcpy(pDst, pSrc, dstPitch);
#endif
                    pDst += dstPitch;
                }
            }

            setImageDimension(image, width, height, dstPitch);
            setImageInfo(image, bpp, hasAlpha, isPreMulti, eFormat);
            setImageData(image, dst, imageSize);
        }

        FreeImage_Unload(dib);
        FreeImage_CloseMemory(stream);

        return ret;
    }
}

